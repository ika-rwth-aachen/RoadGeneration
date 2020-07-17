/**
 * @file tjunction.h
 *
 * @brief file contains method for generating t junction
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

extern settings setting;

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
    if ((string)node.attribute("type").value() == "MA")
        mode = 1;
    if ((string)node.attribute("type").value() == "3A")
        mode = 2;
    if (mode == 0)
    {
        cerr << "ERR: junction type is not defined correct." << endl;
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
        cerr << "ERR: intersection point is not defined correct.";
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
        cerr << "ERR: specified roads in intersection are not found.";
        return 1;
    }

    double sMain, sAdd1, sAdd2, sOffMain, sOffAdd1, sOffAdd2, phi1, phi2;

    // calculate offsets
    double sOffset = 0;
    if (cA)
        sOffset = stod(cA.attribute("gap").value(), &st);
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
    road help1;
    if (buildRoad(mainRoad, help1, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        cerr << "ERR: error in buildRoad" << endl;
        return 1;
    }

    road help2;
    if (buildRoad(additionalRoad1, help2, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        cerr << "ERR: error in buildRoad" << endl;
        return 1;
    }

    road help3;
    if (buildRoad(additionalRoad2, help3, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        cerr << "ERR: error in buildRoad" << endl;
        return 1;
    }

    // calculate width of mainRoad and addtionalRoad
    laneSection lS1 = help1.laneSections.front();
    double width1 = abs(findTOffset(lS1, findMinLaneId(lS1), 0)) + abs(findTOffset(lS1, findMaxLaneId(lS1), 0));

    laneSection lS2 = help2.laneSections.front();
    double width2 = abs(findTOffset(lS2, findMinLaneId(lS2), 0)) + abs(findTOffset(lS2, findMaxLaneId(lS2), 0));

    laneSection lS3 = help3.laneSections.front();
    double width3 = abs(findTOffset(lS3, findMinLaneId(lS3), 0)) + abs(findTOffset(lS3, findMaxLaneId(lS3), 0));

    // check offsets and adjust them if necessary (here: 4 is safty factor)
    double w1 = max(width2 / 2, width3 / 2) * 4;
    double w2 = max(width1 / 2, width3 / 2) * 4;
    double w3 = max(width1 / 2, width2 / 2) * 4;

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
        cerr << "!!! sOffset of at least one road was changed, due to feasible road structure !!!" << endl;
    }

    // calculate s and phi at intersection
    sMain = iP.attribute("s").as_double();

    if (mode >= 1)
    {
        tmpNode = iP.child("adRoad");
        if (!tmpNode)
        {
            cerr << "ERR: first 'adRoad' is missing." << endl;
            return 1;
        }
        sAdd1 = stod(tmpNode.attribute("s").value(), &st);
        phi1 = stod(tmpNode.attribute("angle").value(), &st);
    }

    if (mode >= 2)
    {
        if (!tmpNode.next_sibling("adRoad"))
        {
            cerr << "ERR: second 'adRoad' is missing." << endl;
            return 1;
        }
        sAdd2 = stod(tmpNode.next_sibling("adRoad").attribute("s").value(), &st);
        phi2 = stod(tmpNode.next_sibling("adRoad").attribute("angle").value(), &st);
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
    cout << "\t Generating Roads" << endl;
    laneSection lS;
    double t;

    road r1;
    r1.id = 100 * junc.id + 1;
    r1.junction = junc.id;
    r1.predecessor.elementType = junctionType;
    r1.predecessor.id = junc.id;
    if (mode == 1)
    {
        double phi = phi1;
        fixAngle(phi);

        // add street is left from road 1
        if (phi > 0)
            if (buildRoad(mainRoad, r1, sMain - sOffMain, 0, automaticWidening, sMain, iPx, iPy, iPhdg))
            {
                cerr << "ERR: error in buildRoad" << endl;
                return 1;
            }
        // add street is right from road 1
        if (phi < 0)
            if (buildRoad(mainRoad, r1, sMain - sOffMain, 0, automaticRestricted, sMain, iPx, iPy, iPhdg))
            {
                cerr << "ERR: error in buildRoad" << endl;
                return 1;
            }
    }
    if (mode == 2)
    {
        if (buildRoad(mainRoad, r1, sMain + sOffMain, INFINITY, automaticWidening, sMain, iPx, iPy, iPhdg))
            {
                cerr << "ERR: error in buildRoad" << endl;
                return 1;
            }
    }
    if (addObjects(mainRoad, r1, data))
    {
        cerr << "ERR: error in addObjects" << endl;
        return 1;
    }

    road r2;
    r2.id = 100 * junc.id + 2;
    r2.junction = junc.id;
    r2.predecessor.elementType = junctionType;
    r2.predecessor.id = junc.id;
    if (mode == 1)
    {
        double phi = phi1;
        fixAngle(phi);

        // add street is left from road 1
        if (phi > 0)
            if (buildRoad(mainRoad, r2, sMain + sOffMain, INFINITY, automaticRestricted, sMain, iPx, iPy, iPhdg))
            {
                cerr << "ERR: error in buildRoad" << endl;
                return 1;
            }
        // add street is right from road 1
        if (phi < 0)
            if (buildRoad(mainRoad, r2, sMain + sOffMain, INFINITY, automaticWidening, sMain, iPx, iPy, iPhdg))
            {
                cerr << "ERR: error in buildRoad" << endl;
                return 1;
            }
        if (addObjects(mainRoad, r2, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }
    if (mode == 2)
    {
        if (buildRoad(additionalRoad1, r2, sAdd1 + sOffAdd1, INFINITY, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad1, r2, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }

    road r3;
    r3.id = 100 * junc.id + 3;
    r3.junction = junc.id;
    r3.predecessor.elementType = junctionType;
    r3.predecessor.id = junc.id;
    if (mode == 1)
    {
        if (buildRoad(additionalRoad1, r3, sAdd1 + sOffAdd1, INFINITY, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad1, r3, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }
    if (mode == 2)
    {
        if (buildRoad(additionalRoad2, r3, sAdd2 + sOffAdd2, INFINITY, automaticWidening, sAdd2, iPx, iPy, iPhdg + phi2))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad2, r3, data))
        {
            cerr << "ERR: error in addObjects" << endl;
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
        string tmpType = addLane.attribute("type").value();
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

        string inputPos = "end";
        if (addLane.attribute("roadPos"))
            inputPos = addLane.attribute("roadPos").value();

        if (inputId == r1.inputId && inputPos == r1.inputPos)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r1, length, ds, type, verschwenkung, restricted);
        }

        if (inputId == r2.inputId && inputPos == r2.inputPos)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r2, length, ds, type, verschwenkung, restricted);
        }

        if (inputId == r3.inputId && inputPos == r3.inputPos)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r3, length, ds, type, verschwenkung, restricted);
        }
    }

    data.getRoads().push_back(r1);
    data.getRoads().push_back(r2);
    data.getRoads().push_back(r3);

    // --- generate connecting lanes -------------------------------------------
    cout << "\t Generate Connecting Lanes" << endl;

    // --- generate user-defined connecting lanes
    if (con && (string)con.attribute("type").value() == "single")
    {
        for (pugi::xml_node roadLink : con.children("roadLink"))
        {
            int fromId = roadLink.attribute("fromId").as_int();
            int toId = roadLink.attribute("toId").as_int();

            string fromPos = "end";
            if (roadLink.attribute("fromPos"))
                fromPos = roadLink.attribute("fromPos").value();

            string toPos = "end";
            if (roadLink.attribute("toPos"))
                toPos = roadLink.attribute("toPos").value();

            road r1, r2;
            for (int i = 0; i < data.getRoads().size(); i++)
            {
                road tmp = data.getRoads()[i];
                if (tmp.inputId == fromId && tmp.inputPos == fromPos)
                    r1 = tmp;
                if (tmp.inputId == toId && tmp.inputPos == toPos)
                    r2 = tmp;
            }
            if (r1.id == -1 || r2.id == -1)
            {
                cerr << "ERR: error in user-defined lane connecting:" << endl;
                cerr << "\t road to 'fromId' or 'toId' can not be found" << endl;
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

                string left = non;
                string right = non;

                if (laneLink.attribute("left"))
                    left = laneLink.attribute("left").value();

                if (laneLink.attribute("right"))
                    left = laneLink.attribute("right").value();

                road r;
                r.id = 100 * junc.id + data.getRoads().size() + 1;
                createRoadConnection(r1, r2, r, junc, from, to, left, right);
                data.getRoads().push_back(r);
            }
        }
    }
    // generate automatic connecting lanes
    else
    {
        // switch roads if necessary, so that
        if (sortRoads(r1, r2, r3))
        {
            cerr << "ERR: roads can not be sorted." << endl;
            return 1;
        }

        // generate connections
        int nCount = 1;
        int from, to, nF, nT;

        // 1) PART from R1 To R2 -> Right to Right (if exist)

        calcFromTo(r1, r2, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.id = 100 * junc.id + 50 + nCount;

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

            data.getRoads().push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 2) PART from L2 To L1 -> Left to Left (if exist)

        calcFromTo(r2, r1, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0)
                createRoadConnection(r2, r1, r, junc, from, to, bro, bro);
            else if (mode == 1 && phi1 < 0)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);
            else if (mode == 2)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);

            data.getRoads().push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 3) PART from R2 To R3 -> Right to Right (if exist)

        calcFromTo(r2, r3, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.id = 100 * junc.id + 50 + nCount;

            if (i == 0)
                createRoadConnection(r2, r3, r, junc, from, to, non, sol);
            else
                createRoadConnection(r2, r3, r, junc, from, to, non, non);

            data.getRoads().push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 4) PART from R3 To R2 -> Left to Left (if exist)

        calcFromTo(r3, r2, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r3, r2, r, junc, from, to, non, non);

            data.getRoads().push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 5) PART from R3 To R1 -> Right to Right (if exist)

        calcFromTo(r3, r1, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.id = 100 * junc.id + 50 + nCount;

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

            data.getRoads().push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 6) PART from L1 To L3 -> Left to Left (if exist)

        calcFromTo(r1, r3, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0)
                createRoadConnection(r1, r3, r, junc, from, to, bro, bro);
            else if (mode == 2)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);

            data.getRoads().push_back(r);

            from++;
            to--;
            nCount++;
        }
    }

    data.getJunctions().push_back(junc);

    return 0;
}