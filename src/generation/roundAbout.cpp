/**
 * @file roundAbout.cpp
 *
 * @brief file contains method for generating roundabout
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "roundAbout.h"
#include "helper.h"
#include "buildRoad.h"
#include "addObjects.h"
#include "createRoadConnection.h"

/**
 * @brief function generates the roads and junctions for a roundabout which is specified in the input file
 *  
 * @param node  input data from the input file for the roundAbout
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int roundAbout(pugi::xml_node &node, RoadNetwork &data)
{
    // create segment
    data.nSegment++;
    junction junc;
    junc.id = node.attribute("id").as_int();

    pugi::xml_node dummy;

    pugi::xml_node circleRoad = node.child("circle");
    int refId = circleRoad.attribute("id").as_int();
    if (!circleRoad)
    {
        std::cerr << "ERR: circleRoad is not found.";
        return 1;
    }

    // store properties of circleRoad
    double length = circleRoad.child("referenceLine").child("circle").attribute("length").as_double();
    double R = length / (2 * M_PI);
    circleRoad.child("referenceLine").child("circle").append_attribute("R") = R;

    double sOld;
    Road rOld;

    bool clockwise;
    if (R > 0)
        clockwise = false;
    if (R < 0)
        clockwise = true;
    if (abs(R) < 3)
    {
        std::cerr << "ERR: radius of reference road in a roundabout have to be larger than 3.";
        return 1;
    }

    // get coupler
    pugi::xml_node cA = node.child("coupler").child("junctionArea");

    // count intersectionPoints
    int nIp = 0;
    for (pugi::xml_node iP : node.children("intersectionPoint"))
        nIp++;

    int cc = 0;
    // iterate over all additonalRoads defined by separate intersectionPoints
    // sMain of intersection points have to increase
    for (pugi::xml_node iP : node.children("intersectionPoint"))
    {
        cc++;

        // find additionalRoad
        int adId = iP.child("adRoad").attribute("id").as_int();
        pugi::xml_node additionalRoad;
        for (pugi::xml_node road : node.children("road"))
        {
            if (road.attribute("id").as_int() == adId)
                additionalRoad = road;
        }

        if (!additionalRoad)
        {
            std::cerr << "ERR: specified road in intersection" << cc << " is not found.";
            return 1;
        }

        // calculate offsets
        double sOffset = 0;
        if (cA)
            sOffset = cA.attribute("gap").as_double();

        double sOffMain = sOffset;
        double sOffAdd = sOffset;
        for (pugi::xml_node sB : cA.children("roadGap"))
        {
            if (sB.attribute("id").as_int() == refId)
                sOffMain = sB.attribute("gap").as_double();

            if (sB.attribute("id").as_int() == adId)
                sOffAdd = sB.attribute("gap").as_double();
        }

        // calculate width of circleRoad and addtionalRoad
        Road helpMain;
        Road helpAdd;
        if (buildRoad(circleRoad, helpMain, 0, INFINITY, dummy, 0, 0, 0, 0))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }
        if (buildRoad(additionalRoad, helpAdd, 0, INFINITY, dummy, 0, 0, 0, 0))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }

        laneSection lSMain = helpMain.getLaneSections().front();
        double widthMain = abs(findTOffset(lSMain, findMinLaneId(lSMain), 0));

        laneSection lSAdd = helpAdd.getLaneSections().front();
        double widthAdd = abs(findTOffset(lSAdd, findMinLaneId(lSAdd), 0)) + abs(findTOffset(lSAdd, findMaxLaneId(lSAdd), 0));

        // check offsets and adjust them if necessary (2 and 4 are safty factor)
        bool(changed) = false;
        if (sOffMain < widthAdd / 2 * 4)
        {
            sOffMain = widthAdd / 2 * 4;
            changed = true;
        }
        if (sOffAdd < widthMain * 2)
        {
            sOffAdd = widthMain * 2;
            changed = true;
        }

        if (changed)
        {
            std::cerr << "!!! sOffset of at least one road was changed, due to feasible road structure !!!" << std::endl;
        }

        if (sOffMain * 2 > length / nIp)
        {
            std::cerr << "Length of roundabout is too short, overlapping roads." << std::endl;
            return 1;
        }

        // calculate s and phi at intersection
        double sMain = iP.attribute("s").as_double();
        double sAdd = iP.child("adRoad").attribute("s").as_double();
        double phi = iP.child("adRoad").attribute("angle").as_double();

        // calculate coordinates of intersectionPoint
        double curPhi = sMain / (2 * M_PI * R) * 2 * M_PI;
        double iPx = R * cos(curPhi - M_PI / 2);
        double iPy = R * sin(curPhi - M_PI / 2);
        double iPhdg = curPhi;

        // --- generate roads --------------------------------------------------
        std::cout << "\t Generating Roads" << std::endl;
        /*           
                    \___       ____/
                 id: 1         id: helper
               \_______        _______/    
                       |      |
                       | id:2 |
                       |      |
        */
        int nCount = 1;

        Road r1;
        r1.setID(100 * junc.id + cc * 10 + nCount);
        r1.setJunction(junc.id);
        r1.getSuccessor().id = junc.id;
        r1.getSuccessor().elementType = junctionType;
        r1.getSuccessor().contactPoint = startType;
        if (cc == 1)
            sOld = sOffMain;

        if (buildRoad(circleRoad, r1, sOld, sMain - sOffMain, dummy, sMain, iPx, iPy, iPhdg))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }
        nCount++;

        Road r2;
        r2.setID(100 * junc.id + cc * 10 + nCount);
        r2.setJunction(junc.id);
        r2.getPredecessor().id = junc.id;
        r2.getPredecessor().elementType = junctionType;
        r2.getPredecessor().contactPoint = startType;
        if (buildRoad(additionalRoad, r2, sAdd + sOffAdd, INFINITY, dummy, sAdd, iPx, iPy, iPhdg + phi))
        {
            std::cerr << "ERR: error in buildRoad" << std::endl;
            return 1;
        }
        if (addObjects(additionalRoad, r2, data))
        {
            std::cerr << "ERR: error in addObjects" << std::endl;
            return 1;
        }
        nCount++;

        // add signal to outgoing roads
        //addSignal(r2, data, 1, INFINITY, "1.000.001", "-", -1);

        Road helper;
        helper.setID(100 * junc.id + cc * 10 + nCount);
        helper.setJunction(junc.id);
        if (cc < nIp)
        {
            if (buildRoad(circleRoad, helper, sMain + sOffMain, sMain + 2 * sOffMain, dummy, sMain, iPx, iPy, iPhdg))
            {
                std::cerr << "ERR: error in buildRoad" << std::endl;
                return 1;
            }
        }
        else // last segment
        {
            helper = rOld;
            helper.getSuccessor().elementType = roadType;
            helper.getSuccessor().id = -101;
            helper.getSuccessor().contactPoint = startType;
        }
        helper.getPredecessor().id = junc.id;
        helper.getPredecessor().elementType = junctionType;
        helper.getPredecessor().contactPoint = startType;

        // --- generate connecting lanes ---------------------------------------
        std::cout << "\t Generate Connecting Lanes" << std::endl;

        // max and min id's of laneSections
        int inner1, outer1, outer2, inner2, nLane;

        if (clockwise)
        {
            outer1 = findOuterMiddleLane(r1.getLaneSections().back(), 1);
            inner1 = findInnerMiddleLane(r1.getLaneSections().back(), 1);

            outer2 = findOuterMiddleLane(helper.getLaneSections().back(), 1);
            inner2 = findInnerMiddleLane(helper.getLaneSections().back(), 1);

            nLane = outer1 - inner1 + 1;
        }
        if (!clockwise)
        {
            outer1 = findOuterMiddleLane(r1.getLaneSections().back(), -1);
            inner1 = findInnerMiddleLane(r1.getLaneSections().back(), -1);

            outer2 = findOuterMiddleLane(helper.getLaneSections().back(), -1);
            inner2 = findInnerMiddleLane(helper.getLaneSections().back(), -1);
            nLane = inner1 - outer1 + 1;
        }

        int r2_F_L = findLeftLane(r2.getLaneSections().front(), 1);
        int r2_F_MI = findInnerMiddleLane(r2.getLaneSections().front(), 1);
        int r2_F_MO = findOuterMiddleLane(r2.getLaneSections().front(), 1);
        int r2_F_R = findRightLane(r2.getLaneSections().front(), 1);

        int r2_T_L = findLeftLane(r2.getLaneSections().front(), -1);
        int r2_T_MI = findInnerMiddleLane(r2.getLaneSections().front(), -1);
        int r2_T_MO = findOuterMiddleLane(r2.getLaneSections().front(), -1);
        int r2_T_R = findRightLane(r2.getLaneSections().front(), -1);

        // generate connections
        int from, to;

        // 1) PART from R1 to HELPER
        from = inner1;
        to = inner2;

        for (int i = 0; i < nLane; i++)
        {
            Road r;
            r.setID(100 * junc.id + cc * 10 + nCount);

            if (clockwise)
            {
                if (i == 0)
                    createRoadConnection(r1, helper, r, junc, from, to, non, sol);
                if (i != 0)
                    createRoadConnection(r1, helper, r, junc, from, to, bro, non);
                from++;
                to++;
            }
            if (!clockwise)
            {
                if (i == 0)
                    createRoadConnection(r1, helper, r, junc, from, to, sol, non);
                if (i != 0)
                    createRoadConnection(r1, helper, r, junc, from, to, bro, non);
                from--;
                to--;
            }

            data.pushRoad(r);
            nCount++;
        }

        Road r5;
        r5.setID(100 * junc.id + cc * 10 + nCount);
        if (clockwise)
        {
            from = outer1;
            if (r2_F_R != 0)
                to = r2_F_R;
            else
                to = r2_F_MO;

            createRoadConnection(r1, r2, r5, junc, from, to, sol, non);
        }
        if (!clockwise)
        {
            from = outer1;
            if (r2_T_R != 0)
                to = r2_T_R;
            else
                to = r2_T_MO;

            createRoadConnection(r1, r2, r5, junc, from, to, non, sol);
        }
        nCount++;

        Road r6;
        r6.setID(100 * junc.id + cc * 10 + nCount);
        if (clockwise)
        {
            if (r2_T_R != 0)
                from = r2_T_R;
            else
                from = r2_T_MO;
            to = outer2;
            createRoadConnection(r2, helper, r6, junc, from, to, sol, non);
        }
        if (!clockwise)
        {
            if (r2_F_R != 0)
                from = r2_F_R;
            else
                from = r2_F_MO;
            to = outer2;
            createRoadConnection(r2, helper, r6, junc, from, to, non, sol);
        }
        nCount++;

        // adjust precessor of first element, due to loop
        r1.getPredecessor().id = junc.id;
        r1.getPredecessor().elementType = junctionType;
        r1.getPredecessor().contactPoint = startType;

        data.pushRoad(r1);
        data.pushRoad(r2);
        data.pushRoad(r5);
        data.pushRoad(r6);

        // update for next step
        sOld = sMain + sOffMain;
        if (cc == 1)
            rOld = r1;
    }

    data.pushJunction(junc);

    return 0;
}
