/**
 * @file tjunction.cpp
 *
 * @brief file contains method for generating t junction
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "junctionWrapper.h"
#include "interface.h"
#include "roadNetwork.h"
#include "settings.h"
#include "helper.h"
#include "addLaneSections.h"

#include <stdio.h>
#include <math.h>

extern settings setting;
extern std::string::size_type st;

/**
 * @brief function generates the roads and junctions for a t junction which is specified in the input file
 *  
 * @param node  input data from the input file for the tjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int tjunction(pugi::xml_node &node, RoadNetwork &data)
{
    // check type of the junction (here: M = mainroad, A = accessroad)
    int mode = 0;
    if ((std::string)node.attribute("type").value() == "MA")
        mode = 1;
    if ((std::string)node.attribute("type").value() == "3A")
        mode = 2;
    if (mode == 0)
    {
        std::cerr << "ERR: junction type is not defined correct." << std::endl;
        return 1;
    }

    // create segment
    data.nSegment++;
    junction junc;
    junc.id = node.attribute("id").as_int();

    // automatic widening
    pugi::xml_node dummy;
    pugi::xml_node automaticWidening = node.child("automaticWidening");

    // create automatic restricted node based on defined automatic widening node
    pugi::xml_node automaticRestricted = node.child("automaticWidening");
    automaticRestricted.append_attribute("restricted") = true;

    // define intersection properties
    pugi::xml_node iP = node.child("intersectionPoint");
    if (!iP)
    {
        std::cerr << "ERR: intersection point is not defined correct.";
        return 1;
    }
    pugi::xml_node cA = node.child("coupler").child("junctionArea");
    pugi::xml_node con = node.child("coupler").child("connection");
    pugi::xml_node addLanes = node.child("coupler").child("additionalLanes");

    // define junction roads
    pugi::xml_node mainRoad;
    pugi::xml_node additionalRoad1;
    pugi::xml_node additionalRoad2;

    pugi::xml_node tmpNode = iP.child("adRoad");

    for (pugi::xml_node road : node.children("road"))
    {
        if (road.attribute("id").as_int() == iP.attribute("refRoad").as_int())
            mainRoad = road;

        if (mode >= 1 && road.attribute("id").as_int() == tmpNode.attribute("id").as_int())
            additionalRoad1 = road;

        if (mode >= 2 && road.attribute("id").as_int() == tmpNode.next_sibling("adRoad").attribute("id").as_int())
            additionalRoad2 = road;
    }

    if (!mainRoad || !additionalRoad1 || (mode == 2 && !additionalRoad2))
    {
        std::cerr << "ERR: specified roads in intersection are not found.";
        return 1;
    }

    double sMain, sAdd1, sAdd2, sOffMain, sOffAdd1, sOffAdd2, phi1, phi2;

    // calculate offsets
    double sOffset = 0;
    if (cA)
        sOffset = std::stod(cA.attribute("gap").value(), &st);
    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    for (pugi::xml_node_iterator sB = cA.begin(); sB != cA.end(); ++sB)
    {
        if (sB->attribute("id").as_int() == mainRoad.attribute("id").as_int())
            sOffMain = sB->attribute("gap").as_double();

        if (sB->attribute("id").as_int() == additionalRoad1.attribute("id").as_int())
            sOffAdd1 = sB->attribute("gap").as_double();

        if (sB->attribute("id").as_int() == additionalRoad2.attribute("id").as_int())
            sOffAdd2 = sB->attribute("gap").as_double();
    }

    // calculate helper roads
    Road help1;
    if (help1.buildRoad(mainRoad, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        std::cerr << "ERR: error in buildRoad" << std::endl;
        return 1;
    }

    Road help2;
    if (help2.buildRoad(additionalRoad1, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        std::cerr << "ERR: error in buildRoad" << std::endl;
        return 1;
    }

    Road help3;
    if (help3.buildRoad(additionalRoad2, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        std::cerr << "ERR: error in buildRoad" << std::endl;
        return 1;
    }

    // calculate width of mainRoad and addtionalRoad
    laneSection lS1 = help1.getLaneSections().front();
    double width1 = abs(findTOffset(lS1, findMinLaneId(lS1), 0)) + abs(findTOffset(lS1, findMaxLaneId(lS1), 0));

    laneSection lS2 = help2.getLaneSections().front();
    double width2 = abs(findTOffset(lS2, findMinLaneId(lS2), 0)) + abs(findTOffset(lS2, findMaxLaneId(lS2), 0));

    laneSection lS3 = help3.getLaneSections().front();
    double width3 = abs(findTOffset(lS3, findMinLaneId(lS3), 0)) + abs(findTOffset(lS3, findMaxLaneId(lS3), 0));

    // check offsets and adjust them if necessary (here: 4 is safty factor)
    double w1 = std::max(width2 / 2, width3 / 2) * 4;
    double w2 = std::max(width1 / 2, width3 / 2) * 4;
    double w3 = std::max(width1 / 2, width2 / 2) * 4;

    bool changed = false;
    if (sOffMain < w1)
    {
        sOffMain = w1;
        changed = true;
    }
    if (sOffAdd1 < w2)
    {
        sOffAdd1 = w2;
        changed = true;
    }
    if (sOffAdd2 < w3)
    {
        sOffAdd2 = w3;
        changed = true;
    }

    if (changed)
    {
        std::cerr << "!!! sOffset of at least one road was changed, due to feasible road structure !!!" << std::endl;
    }

    // calculate s and phi at intersection
    sMain = iP.attribute("s").as_double();

    if (mode >= 1)
    {
        tmpNode = iP.child("adRoad");
        if (!tmpNode)
        {
            std::cerr << "ERR: first 'adRoad' is missing." << std::endl;
            return 1;
        }
        sAdd1 = std::stod(tmpNode.attribute("s").value(), &st);
        phi1 = std::stod(tmpNode.attribute("angle").value(), &st);
    }

    if (mode >= 2)
    {
        if (!tmpNode.next_sibling("adRoad"))
        {
            std::cerr << "ERR: second 'adRoad' is missing." << std::endl;
            return 1;
        }
        sAdd2 = std::stod(tmpNode.next_sibling("adRoad").attribute("s").value(), &st);
        phi2 = std::stod(tmpNode.next_sibling("adRoad").attribute("angle").value(), &st);
    }

    // set coordinates of intersectionPoint
    double iPx = 0;
    double iPy = 0;
    double iPhdg = 0;

    // --- generate roads ------------------------------------------------------
    /*            |      |
                  | id:3 |
        __________|      |___________
        id: 1               id: 2
        __________        ___________    
     */
    std::cout << "\t Generating Roads" << std::endl;
    laneSection lS;
    double t;

    Road r1;
    r1.setID( 100 * junc.id + 1);
    r1.setJunction( junc.id);
    r1.getPredecessor().elementType = junctionType;
    r1.getPredecessor().id = junc.id;
    if (mode == 1)
    {
        double phi = phi1;
        fixAngle(phi);

        // add street is left from road 1
        if (phi > 0)
            if (r1.buildRoad(mainRoad, sMain - sOffMain, 0, automaticWidening, sMain, iPx, iPy, iPhdg))
            {
                std::cerr << "ERR: error in buildRoad" << std::endl;
                return 1;
            }
        // add street is right from road 1
        if (phi < 0)
            if (r1.buildRoad(mainRoad, sMain - sOffMain, 0, automaticRestricted, sMain, iPx, iPy, iPhdg))
            {
                std::cerr << "ERR: error in buildRoad" << std::endl;
                return 1;
            }
    }
    if (mode == 2)
    {
        if (r1.buildRoad(mainRoad, sMain + sOffMain, INFINITY, automaticWidening, sMain, iPx, iPy, iPhdg))
            {
                std::cerr << "ERR: error in buildRoad" << std::endl;
                return 1;
            }
    }
    if (addObjects(mainRoad, r1, data))
    {
        std::cerr << "ERR: error in addObjects" << std::endl;
        return 1;
    }

    Road r2;
    r2.setID( 100 * junc.id + 2);
    r2.setJunction(junc.id);
    r2.getPredecessor().elementType = junctionType;
    r2.getPredecessor().id = junc.id;
    if (mode == 1)
    {
        double phi = phi1;
        fixAngle(phi);

        // add street is left from road 1
        if (phi > 0)
            if (r2.buildRoad(mainRoad, sMain + sOffMain, INFINITY, automaticRestricted, sMain, iPx, iPy, iPhdg))
            {
                std::cerr << "ERR: error in buildRoad" << std::endl;
                return 1;
            }
        // add street is right from road 1
        if (phi < 0)
            if (r2.buildRoad(mainRoad, sMain + sOffMain, INFINITY, automaticWidening, sMain, iPx, iPy, iPhdg))
            {
                std::cerr << "ERR: error in buildRoad" << std::endl;
                return 1;
            }
        if (addObjects(mainRoad, r2, data))
        {
            std::cerr << "ERR: error in addObjects" << std::endl;
            return 1;
        }
    }
    if (mode == 2)
    {
        if (r2.buildRoad(additionalRoad1, sAdd1 + sOffAdd1, INFINITY, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }
        if (addObjects(additionalRoad1, r2, data))
        {
            std::cerr << "ERR: error in addObjects" << std::endl;
            return 1;
        }
    }

    Road r3;
    r3.setID(100 * junc.id + 3);
    r3.setJunction(junc.id);
    r3.getPredecessor().elementType = junctionType;
    r3.getPredecessor().id = junc.id;
    if (mode == 1)
    {
        if (r3.buildRoad(additionalRoad1, sAdd1 + sOffAdd1, INFINITY, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }
        if (addObjects(additionalRoad1, r3, data))
        {
            std::cerr << "ERR: error in addObjects" << std::endl;
            return 1;
        }
    }
    if (mode == 2)
    {
        if (r3.buildRoad(additionalRoad2, sAdd2 + sOffAdd2, INFINITY, automaticWidening, sAdd2, iPx, iPy, iPhdg + phi2))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }
        if (addObjects(additionalRoad2, r3, data))
        {
            std::cerr << "ERR: error in addObjects" << std::endl;
            return 1;
        }
    }

    // add addtional lanes
    for (pugi::xml_node addLane : addLanes.children("additionalLane"))
    {
        int n = 1;
        if (addLane.attribute("amount"))
            n = addLane.attribute("amount").as_int();

        bool verschwenkung = true;
        if (addLane.attribute("verschwenkung"))
            verschwenkung = addLane.attribute("verschwenkung").as_bool();

        double length = setting.laneChange.s;
        if (addLane.attribute("length"))
            length = addLane.attribute("length").as_double();

        double ds = setting.laneChange.ds;
        if (addLane.attribute("ds"))
            length = addLane.attribute("ds").as_double();

        int type;
        std::string tmpType = addLane.attribute("type").value();
        if (tmpType == "left")
            type = 1;
        if (tmpType == "right")
            type = -1;

        if (tmpType == "leftRestricted")
            type = 1;
        if (tmpType == "rightRestricted")
            type = -1;

        bool restricted = false;
        if (tmpType == "leftRestricted" || tmpType == "rightRestricted")
            restricted = true;

        int inputId = addLane.attribute("roadId").as_int();

        std::string inputPos = "end";
        if (addLane.attribute("roadPos"))
            inputPos = addLane.attribute("roadPos").value();

        if (inputId == r1.getInputID() && inputPos == r1.getInputPos())
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r1, length, ds, type, verschwenkung, restricted);
        }

        if (inputId == r2.getInputID() && inputPos == r2.getInputPos())
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r2, length, ds, type, verschwenkung, restricted);
        }

        if (inputId == r3.getInputID() && inputPos == r3.getInputPos())
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r3, length, ds, type, verschwenkung, restricted);
        }
    }

    data.pushRoad(r1);
    data.pushRoad(r2);
    data.pushRoad(r3);

    // --- generate connecting lanes -------------------------------------------
    std::cout << "\t Generate Connecting Lanes" << std::endl;

    // --- generate user-defined connecting lanes
    if (con && (std::string)con.attribute("type").value() == "single")
    {
        for (pugi::xml_node roadLink : con.children("roadLink"))
        {
            int fromId = roadLink.attribute("fromId").as_int();
            int toId = roadLink.attribute("toId").as_int();

            std::string fromPos = "end";
            if (roadLink.attribute("fromPos"))
                fromPos = roadLink.attribute("fromPos").value();

            std::string toPos = "end";
            if (roadLink.attribute("toPos"))
                toPos = roadLink.attribute("toPos").value();

            Road r1, r2;
            for (int i = 0; i < data.getRoads().size(); i++)
            {
                Road tmp = data.getRoads()[i];
                if (tmp.getInputID() == fromId && tmp.getInputPos() == fromPos)
                    r1 = tmp;
                if (tmp.getInputID() == toId && tmp.getInputPos() == toPos)
                    r2 = tmp;
            }
            if (r1.getID() == -1 || r2.getID() == -1)
            {
                std::cerr << "ERR: error in user-defined lane connecting:" << std::endl;
                std::cerr << "\t road to 'fromId' or 'toId' can not be found" << std::endl;
                return 1;
            }

            for (pugi::xml_node laneLink : roadLink.children("laneLink"))
            {
                int from = laneLink.attribute("fromId").as_int();
                int to = laneLink.attribute("toId").as_int();

                // flip ids
                if (fromPos == "start")
                    from *= -1;
                if (toPos == "end")
                    to *= -1;

                std::string left = non;
                std::string right = non;

                if (laneLink.attribute("left"))
                    left = laneLink.attribute("left").value();

                if (laneLink.attribute("right"))
                    left = laneLink.attribute("right").value();

                Road r;
                r.setID( 100 * junc.id + data.getRoads().size() + 1);
                createRoadConnection(r1, r2, r, junc, from, to, left, right);
                data.pushRoad(r);
            }
        }
    }
    // generate automatic connecting lanes
    else
    {
        // switch roads if necessary, so that
        if (sortRoads(r1, r2, r3))
        {
            std::cerr << "ERR: roads can not be sorted." << std::endl;
            return 1;
        }

        // generate connections
        int nCount = 1;
        int from, to, nF, nT;

        // 1) PART from R1 To R2 -> Right to Right (if exist)

        calcFromTo(r1, r2, from, to, nF, nT, -1);

        for (int i = 0; i < std::min(nF, nT); i++)
        {
            Road r;
            r.setID(100 * junc.id + 50 + nCount);

            if (mode == 1 && phi1 > 0 && i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, bro, sol);
            else if (mode == 1 && phi1 > 0 && i != 0)
                createRoadConnection(r1, r2, r, junc, from, to, bro, bro);
            else if (mode == 1 && phi1 < 0 && i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, sol);
            else if (mode == 1 && phi1 < 0 && i != 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, non);
            else if (mode == 2 && i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, sol);
            else if (mode == 2 && i != 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, non);

            data.pushRoad(r);

            from--;
            to++;
            nCount++;
        }

        // 2) PART from L2 To L1 -> Left to Left (if exist)

        calcFromTo(r2, r1, from, to, nF, nT, 1);

        for (int i = 0; i < std::min(nF, nT); i++)
        {
            Road r;
            r.setID(100 * junc.id + 50 + nCount);

            if (mode == 1 && phi1 > 0)
                createRoadConnection(r2, r1, r, junc, from, to, bro, bro);
            else if (mode == 1 && phi1 < 0)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);
            else if (mode == 2)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);

            data.pushRoad(r);

            from++;
            to--;
            nCount++;
        }

        // 3) PART from R2 To R3 -> Right to Right (if exist)

        calcFromTo(r2, r3, from, to, nF, nT, -1);

        for (int i = 0; i < std::min(nF, nT); i++)
        {
            Road r;
            r.setID(100 * junc.id + 50 + nCount);

            if (i == 0)
                createRoadConnection(r2, r3, r, junc, from, to, non, sol);
            else
                createRoadConnection(r2, r3, r, junc, from, to, non, non);

            data.pushRoad(r);

            from--;
            to++;
            nCount++;
        }

        // 4) PART from R3 To R2 -> Left to Left (if exist)

        calcFromTo(r3, r2, from, to, nF, nT, 1);

        for (int i = 0; i < std::min(nF, nT); i++)
        {
            Road r;
            r.setID( 100 * junc.id + 50 + nCount);

            createRoadConnection(r3, r2, r, junc, from, to, non, non);

            data.pushRoad(r);

            from++;
            to--;
            nCount++;
        }

        // 5) PART from R3 To R1 -> Right to Right (if exist)

        calcFromTo(r3, r1, from, to, nF, nT, -1);

        for (int i = 0; i < std::min(nF, nT); i++)
        {
            Road r;
            r.setID( 100 * junc.id + 50 + nCount);

            if (mode == 1 && phi1 > 0 && i == 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, sol);
            else if (mode == 1 && phi1 > 0 && i != 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0 && i == 0)
                createRoadConnection(r3, r1, r, junc, from, to, bro, sol);
            else if (mode == 1 && phi1 < 0 && i != 0)
                createRoadConnection(r3, r1, r, junc, from, to, bro, bro);
            else if (mode == 2 && i == 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, sol);
            else if (mode == 2 && i != 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);

            data.pushRoad(r);

            from--;
            to++;
            nCount++;
        }

        // 6) PART from L1 To L3 -> Left to Left (if exist)

        calcFromTo(r1, r3, from, to, nF, nT, 1);

        for (int i = 0; i < std::min(nF, nT); i++)
        {
            Road r;
            r.setID(100 * junc.id + 50 + nCount);

            if (mode == 1 && phi1 > 0)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0)
                createRoadConnection(r1, r3, r, junc, from, to, bro, bro);
            else if (mode == 2)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);

            data.pushRoad(r);

            from++;
            to--;
            nCount++;
        }
    }

    data.pushJunction(junc);

    return 0;
}
