/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file tjunction.h
 *
 * @brief file contains method for generating t junction
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
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
int tjunction(const DOMElement* node, roadNetwork &data)
{
    // check type of the junction (here: M = mainroad, A = accessroad)
    int mode = 0;
    if (readStrAttrFromNode(node, "type") == "MA")
        mode = 1;
    if (readStrAttrFromNode(node, "type")  == "3A")
        mode = 2;
    if (mode == 0)
    {
        cerr << "ERR: junction type is not defined correctly." << endl;
        return 1;
    }

    // create segment
    data.nSegment++;
    junction junc;
    junc.id = readIntAttrFromNode(node, "id");
    int inputSegmentId = readIntAttrFromNode(node, "id");

    // automatic widening
    DOMElement* dummy = NULL;
    DOMElement* automaticWidening = getChildWithName(node ,"automaticWidening");

    // create automatic restricted node based on defined automatic widening node
    DOMElement* automaticRestricted = getChildWithName(node ,"automaticWidening");
    if(automaticRestricted != NULL)
        automaticRestricted->setAttribute(X("restricted"), X("true"));
    //automaticRestricted.append_attribute("restricted") = true;

    // define intersection properties
    DOMElement* iP = getChildWithName(node, "intersectionPoint");
    if (iP == NULL)
    {
        cerr << "ERR: intersection point is not defined correct." << endl;
        return 1;
    }
    DOMElement* cA = getChildWithName(getChildWithName(node, "coupler"), "junctionArea");
    DOMElement* con = getChildWithName(getChildWithName(node, "coupler"), "connection");
    DOMElement* addLanes = getChildWithName(getChildWithName(node, "coupler"), "additionalLanes");

    // define junction roads
    DOMElement* mainRoad = NULL;
    DOMElement* additionalRoad1 = NULL;
    DOMElement* additionalRoad2 = NULL;

    DOMElement* tmpNode = getChildWithName(iP, "adRoad");

    DOMNodeList *roadList = node->getElementsByTagName(X("road"));
    for (int i = 0; i < roadList->getLength(); i ++)
    {
        DOMElement* road = (DOMElement*)roadList->item(i);
        if(road->getNodeType() != 1) continue; //check the node type. Type 1 is an element. The reason for the check is that line breaks are handled as
        // text nodes by xercesC.
        int currentRoadId = readIntAttrFromNode(road, "id");

        if (currentRoadId == readIntAttrFromNode(iP, "refRoad"))
            mainRoad = road;

        if (mode >= 1 && currentRoadId == readIntAttrFromNode(tmpNode, "id"))
            additionalRoad1 = road;

        if (mode >= 2 && currentRoadId == readIntAttrFromNode(getNextSiblingWithTagName(tmpNode, "adRoad"), "id"))
            additionalRoad2 = road;
    }

    if (mainRoad == NULL || additionalRoad1 == NULL || (mode == 2 && additionalRoad2 == NULL))
    {
        cerr << "ERR: specified roads in intersection are not found.";
        return 1;
    }

    double sMain, sAdd1, sAdd2, sOffMain, sOffAdd1, sOffAdd2, phi1, phi2;

    // calculate offsets
    double sOffset = 0;
    if (cA)
        sOffset = readDoubleAttrFromNode(cA, "gap");
    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    

    if(cA != NULL) // there might be no coupler provided
    {
        for (DOMElement *sB = cA->getFirstElementChild();sB != NULL; sB = sB->getNextElementSibling())
        {
            if (readIntAttrFromNode(sB, "id") == readIntAttrFromNode(mainRoad, "id"))
                sOffMain = readDoubleAttrFromNode(sB, "gap");

            if (additionalRoad1 != NULL && readIntAttrFromNode(sB, "id") == readIntAttrFromNode(additionalRoad1, "id"))
                sOffAdd1 = readDoubleAttrFromNode(sB, "gap");

            if (additionalRoad2 != NULL && readIntAttrFromNode(sB, "id") == readIntAttrFromNode(additionalRoad2, "id"))
                sOffAdd2 = readDoubleAttrFromNode(sB, "gap");
        }
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
    sMain = readDoubleAttrFromNode(iP, "s");

    if (mode >= 1)
    {
        tmpNode = getChildWithName(iP, "adRoad");
        if (tmpNode == NULL)
        {
            cerr << "ERR: first 'adRoad' is missing." << endl;
            return 1;
        }
        sAdd1 = readDoubleAttrFromNode(tmpNode, "s");
        phi1 = readDoubleAttrFromNode(tmpNode, "angle");
    }

    if (mode >= 2)
    {
        for(tmpNode = tmpNode->getNextElementSibling(); tmpNode != NULL 
            && readNameFromNode(tmpNode) != "adRoad";tmpNode = tmpNode->getNextElementSibling());

        if (tmpNode == NULL)
        {
            cerr << "ERR: second 'adRoad' is missing." << endl;
            return 1;
        }
        sAdd2 = readDoubleAttrFromNode(tmpNode, "s");
        phi2 = readDoubleAttrFromNode(tmpNode, "angle");
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
    if(!setting.silentMode)
        cout << "\tGenerating Roads" << endl;
    laneSection lS;
    double t;

    road r1;
    r1.id = 100 * junc.id + 1;
    r1.junction = junc.id;
    r1.inputSegmentId = inputSegmentId;
    r1.isConnectingRoad = true;  //this is a fix for the junction bug
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
    r2.inputSegmentId = inputSegmentId;
    r2.isConnectingRoad = true; //this is a fix for the junction bug
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
    r3.inputSegmentId = inputSegmentId;
    r3.isConnectingRoad = true; //this is a fix for the junction bug
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
    if(addLanes != NULL)
    {
        for (DOMElement *addLane = addLanes->getFirstElementChild();addLane != NULL; addLane = addLane->getNextElementSibling())
        {
            if(readNameFromNode(addLane) != "additionalLane") continue;
            int n = 1;

            if (attributeExits(addLane, "amount"))
                n = readIntAttrFromNode(addLane, "amount");

            bool verschwenkung = true;
            if (attributeExits(addLane, "verschwenkung"))
                verschwenkung = readBoolAttrFromNode(addLane, "verschwenkung");
                
            double length = setting.laneChange.s;
            if (attributeExits(addLane, "length"))
                length = readDoubleAttrFromNode(addLane, "length");

            double ds = setting.laneChange.ds;
            if (attributeExits(addLane, "ds"))
                length = readDoubleAttrFromNode(addLane, "ds");

            int type;
            string tmpType = readStrAttrFromNode(addLane, "type");
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

            int inputId = readIntAttrFromNode(addLane, "roadId");

            string inputPos = "end";
            if (attributeExits(addLane, "roadPos"))
                inputPos = readStrAttrFromNode(addLane, "roadPos");

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
    }

    //correct lane links 
    //set the correct lane link id for the predecessor
       for(laneSection &ls: r1.laneSections)
    {
        for(lane &l: ls.lanes)
        {
            l.preId = -1; //the junction connecting road always use lane -1
        }
    }
    
    for(laneSection &ls: r2.laneSections)
    {
        for(lane &l: ls.lanes)
        {
            l.preId = -1; //the junction connecting road always use lane -1
        }
    }
    
    for(laneSection &ls: r3.laneSections)
    {
        for(lane &l: ls.lanes)
        {
            l.preId = -1; //the junction connecting road always use lane -1
        }
    }
    //end correcting lane links
    
    data.roads.push_back(r1);
    data.roads.push_back(r2);
    data.roads.push_back(r3);

    // --- generate connecting lanes -------------------------------------------
    if(!setting.silentMode)
        cout << "\t Generate Connecting Lanes" << endl;

    // --- generate user-defined connecting lanes
    if (con != NULL && readStrAttrFromNode(con, "type") == "single")
    {
        for (DOMElement* roadLink = con->getFirstElementChild(); roadLink != NULL; roadLink = roadLink->getNextElementSibling())
        {
            if (readNameFromNode(roadLink) != "roadLink") continue;

            int fromId = readIntAttrFromNode(roadLink, "fromId");
            int toId = readIntAttrFromNode(roadLink, "toId");

            string fromPos = "end";
            if (attributeExits(roadLink, "fromPos"))
                fromPos = readStrAttrFromNode(roadLink, "fromPos");

            string toPos = "end";
            if (attributeExits(roadLink,"toPos"))
                toPos = readStrAttrFromNode(roadLink, "toPos");

            road r1, r2;
            for (int i = 0; i < data.roads.size(); i++)
            {
                road tmp = data.roads[i];
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

            for (DOMElement* laneLink = roadLink->getFirstElementChild(); laneLink != NULL; laneLink = laneLink->getNextElementSibling())
            {
                if(readNameFromNode(laneLink) != "laneLink") continue;
                int from = readIntAttrFromNode(laneLink, "fromId");
                int to = readIntAttrFromNode(laneLink, "toId");

                // flip ids
                if (fromPos == "start")
                    from *= -1;
                if (toPos == "end")
                    to *= -1;

                string left = sol;
                string right = sol;

                if (attributeExits(laneLink, "left"))
                    left = readStrAttrFromNode(laneLink, "left");

                if (attributeExits(laneLink, "right"))
                    right = readStrAttrFromNode(laneLink, "right");
                    //left = readStrAttrFromNode(laneLink, "right");  The line above was like this. i changed it to this since i think it was a bug.

                road r;
                r.inputSegmentId = inputSegmentId;
                r.id = 100 * junc.id + data.roads.size() + 1;
                createRoadConnection(r1, r2, r, junc, from, to, left, right);
                data.roads.push_back(r);
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
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0 && i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, sol);
            else if (mode == 1 && phi1 > 0 && i != 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0 && i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, sol);
            else if (mode == 1 && phi1 < 0 && i != 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, non);
            else if (mode == 2 && i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, sol);
            else if (mode == 2 && i != 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 2) PART from L2 To L1 -> Left to Left (if exist)

        calcFromTo(r2, r1, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);
            else if (mode == 2)
                createRoadConnection(r2, r1, r, junc, from, to, non, non);


            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 3) PART from R2 To R3 -> Right to Right (if exist)

        calcFromTo(r2, r3, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (i == 0)
                createRoadConnection(r2, r3, r, junc, from, to, non, sol);
            else
                createRoadConnection(r2, r3, r, junc, from, to, non, non);


            data.roads.push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 4) PART from R3 To R2 -> Left to Left (if exist)

        calcFromTo(r3, r2, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r3, r2, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 5) PART from R3 To R1 -> Right to Right (if exist)

        calcFromTo(r3, r1, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0 && i == 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, sol);
            else if (mode == 1 && phi1 > 0 && i != 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0 && i == 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, sol);
            else if (mode == 1 && phi1 < 0 && i != 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);
            else if (mode == 2 && i == 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, sol);
            else if (mode == 2 && i != 0)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 6) PART from L1 To L3 -> Left to Left (if exist)

        calcFromTo(r1, r3, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);
            else if (mode == 1 && phi1 < 0)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);
            else if (mode == 2)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }
    }

    data.junctions.push_back(junc);


    return 0;
}