/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file xjunction.h
 *
 * @brief file contains method for generating x junction
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

extern settings setting;

/**
 * @brief function generates the roads and junctions for a x junction which is specified in the input file
 *  
 * @param node  input data from the input file for the xjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int xjunction(const DOMElement* domNode, roadNetwork &data)
{
    // check type of the junction (here: M = mainroad, A = accessroad)
    int mode = 0;
    string type = readStrAttrFromNode(domNode, "type");
    if (type == "2M")
        mode = 1;
    if (type == "M2A")
        mode = 2;
    if (type == "4A")
        mode = 3;
    if (mode == 0)
    {
        cerr << "ERR: junction type is not defined correct." << endl;
        return 1;
    }

    // create segment
    data.nSegment++;
    junction junc;
    junc.id = readIntAttrFromNode(domNode, "id");
    int inputSegmentId = readIntAttrFromNode(domNode, "id");

    // automatic widening
    DOMElement* dummy = NULL; 
    DOMElement* automaticWidening = getChildWithName(domNode, "automaticWidening");


    // define intersection properties
    DOMElement* iP =  getChildWithName(domNode, "intersectionPoint");
    if (iP == NULL)
    {
        cerr << "ERR: intersection point is not defined correct.";
        return 1;
    }
    DOMElement* cA = getChildWithName(getChildWithName(domNode, "coupler"), "junctionArea");
    DOMElement* con = getChildWithName(getChildWithName(domNode, "coupler"), "connection");
    DOMElement* addLanes = getChildWithName(getChildWithName(domNode, "coupler"), "additionalLanes");


    // define junction roads
    DOMElement* refRoad = NULL;
    DOMElement* additionalRoad1 = NULL;
    DOMElement* additionalRoad2 = NULL;
    DOMElement* additionalRoad3 = NULL;

    DOMElement* tmpNode = getChildWithName(iP, "adRoad");
    DOMNodeList *roads = domNode->getElementsByTagName(X("road"));
    for (int i = 0; i < roads->getLength(); i ++)
    {
        DOMElement* road = (DOMElement*)roads->item(i);
        if(road->getNodeType() != 1) continue; //we have to check the node type. Type 1 is an element. The reason for the check is that line breaks are handled as
        // text nodes by xercesC.

        int roadID = readIntAttrFromNode(road, "id");
        if (roadID == readIntAttrFromNode(iP,"refRoad"))
            refRoad = road;

        if (mode >= 1 && roadID == readIntAttrFromNode(tmpNode,"id"))
            additionalRoad1 = road;

        if (mode >= 2 && roadID == readIntAttrFromNode(getNextSiblingWithTagName(tmpNode, "adRoad"), "id"))
            additionalRoad2 = road;

        if (mode >= 3 && roadID == readIntAttrFromNode(getNextSiblingWithTagName(getNextSiblingWithTagName(tmpNode, "adRoad"), "adRoad"), "id"))
            additionalRoad3 = road;
    }

    if (!refRoad || (mode >= 1 && !additionalRoad1) || (mode >= 2 && !additionalRoad2) || (mode >= 3 && !additionalRoad3))
    {
        cerr << "ERR: specified roads in intersection are not found." << endl;
        return 1;
    }

    double sMain, sAdd1, sAdd2, sAdd3, sOffMain, sOffAdd1, sOffAdd2, sOffAdd3, phi1, phi2, phi3;

    // calculate offsets
    double sOffset = 0;
    if (cA)
        sOffset = stod(readStrAttrFromNode(cA, "gap"), &st);

    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    sOffAdd3 = sOffset;


    
    for (DOMElement* it = cA->getFirstElementChild(); it != NULL ;it = it->getNextElementSibling())
    {
        if (readIntAttrFromNode(it, "id") == readIntAttrFromNode(refRoad, "id") )
            sOffMain = readDoubleAttrFromNode(it, "gap");

        if (readIntAttrFromNode(it, "id") == readIntAttrFromNode(additionalRoad1, "id"))
            sOffAdd1 = readDoubleAttrFromNode(it, "gap");

        if (readIntAttrFromNode(it, "id") == readIntAttrFromNode(additionalRoad2, "id"))
            sOffAdd2 = readDoubleAttrFromNode(it, "gap");

        if (readIntAttrFromNode(it, "id", true) == readIntAttrFromNode(additionalRoad3, "id", true))
            sOffAdd3 = readDoubleAttrFromNode(it, "gap");
       

    }

    // calculate helper roads
    road help1;
    if (buildRoad(refRoad, help1, 0, INFINITY, dummy, 0, 0, 0, 0))
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

    road help4;
    if (buildRoad(additionalRoad3, help4, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        cerr << "ERR: error in buildRoad" << endl;
        return 1;
    }

    // calculate width of refRoad and addtionalRoad
    laneSection lS1 = help1.laneSections.front();
    double width1 = abs(findTOffset(lS1, findMinLaneId(lS1), 0)) + abs(findTOffset(lS1, findMaxLaneId(lS1), 0));

    laneSection lS2 = help2.laneSections.front();
    double width2 = abs(findTOffset(lS2, findMinLaneId(lS2), 0)) + abs(findTOffset(lS2, findMaxLaneId(lS2), 0));

    laneSection lS3 = help3.laneSections.front();
    double width3 = abs(findTOffset(lS3, findMinLaneId(lS3), 0)) + abs(findTOffset(lS3, findMaxLaneId(lS3), 0));

    laneSection lS4 = help4.laneSections.front();
    double width4 = abs(findTOffset(lS4, findMinLaneId(lS4), 0)) + abs(findTOffset(lS4, findMaxLaneId(lS4), 0));

    // check offsets and adjust them if necessary (here: 4 is safty factor)
    double w1 = max(width2 / 2, max(width3 / 2, width4 / 2)) * 4;
    double w2 = max(width1 / 2, max(width3 / 2, width4 / 2)) * 4;
    double w3 = max(width1 / 2, max(width2 / 2, width4 / 2)) * 4;
    double w4 = max(width1 / 2, max(width2 / 2, width3 / 2)) * 4;

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
    if (sOffAdd3 < w4)
    {
        sOffAdd3 = w4;
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
        //tmpNode = tmpNode.next_sibling("adRoad");
        for(tmpNode = tmpNode->getNextElementSibling(); tmpNode != NULL && readNameFromNode(tmpNode) != "adRoad";tmpNode = tmpNode->getNextElementSibling());
        
        //some sanity checks---
        if (tmpNode == NULL)
        {
            cerr << "ERR: error in generating junction road (mode 1). AdRoad is missing in intersection point" << endl;
            return 1;
        }

        if(sAdd1 - sOffAdd1 <= 0)
        {
            cerr << "ERR: error in generating junction road. Intersection point is too close to add. road start" << endl;
            return 1;
        }

        if(sAdd1 >  help2.length)
        {
            cerr << "ERR: error in generating junction road. Intersection point is too close to add. road end " << endl;
            return 1;
        }

        if(sMain > help1.length)
        {
            cerr << "ERR: error in generating junction road. Intersection point is too close to main road end " << endl;
            return 1;
        }
        if(sMain <= sOffMain)
        {
            cerr << "ERR: error in generating junction road. Intersection point is too close to main road start " << endl;
            return 1;
        }

        //---------
    }
    if (mode >= 2)
    {
        if (tmpNode == NULL)
        {
            cerr << "ERR: second 'adRoad' is missing." << endl;
            return 1;
        }

        sAdd2 = readDoubleAttrFromNode(tmpNode, "s");
        phi2 = readDoubleAttrFromNode(tmpNode, "angle");
        for(tmpNode = tmpNode->getNextElementSibling(); tmpNode != NULL && readNameFromNode(tmpNode) != "adRoad";tmpNode = tmpNode->getNextElementSibling());


    }
    if (mode >= 3)
    {
        if (!tmpNode)
        {
            cerr << "ERR: third 'adRoad' is missing." << endl;
            return 1;
        }
        sAdd3 = readDoubleAttrFromNode(tmpNode, "s");
        phi3 = readDoubleAttrFromNode(tmpNode, "angle");
        for(tmpNode = tmpNode->getNextElementSibling(); tmpNode != NULL 
            && readNameFromNode(tmpNode) != "adRoad";tmpNode = tmpNode->getNextElementSibling());

    }

    // calculate coordinates of intersectionPoint
    double iPx = 0;
    double iPy = 0;
    double iPhdg = 0;

    // --- generate roads ------------------------------------------------------
    /*            |      |
                       | id:4 |
             __________|      |___________
    refRoad   id: 1               id: 3
             __________        ___________    
                       |      |
                       | id:2 |
                       |      |
    */
    if(!setting.silentMode)
        cout << "\t Generating Roads" << endl;

    road r1;
    r1.id = 100 * junc.id + 1;
    r1.junction = junc.id;
    r1.inputSegmentId = inputSegmentId;
    r1.isConnectingRoad = true;
    r1.predecessor.id = junc.id;
    r1.predecessor.elementType = junctionType;
    if (mode == 1 || mode == 2)
    {
        if (buildRoad(refRoad, r1, sMain - sOffMain, 0, automaticWidening, sMain, iPx, iPy, iPhdg))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
    }
    if (mode == 3)
    {
        if (buildRoad(refRoad, r1, sMain + sOffMain, INFINITY, automaticWidening, sMain, iPx, iPy, iPhdg))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
    }
    if (addObjects(refRoad, r1, data))
    {
        cerr << "ERR: error in addObjects" << endl;
        return 1;
    }

    road r2;
    r2.id = 100 * junc.id + 2;
    r2.junction = junc.id;
    r2.isConnectingRoad = true;
    r2.inputSegmentId = inputSegmentId;
    r2.predecessor.id = junc.id;
    r2.predecessor.elementType = junctionType;
    if (mode == 1)
    {
        
        if (buildRoad(additionalRoad1, r2, sAdd1 - sOffAdd1, 0, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
    }
    if (mode == 2 || mode == 3)
    {
        if (buildRoad(additionalRoad1, r2, sAdd1 + sOffAdd1, INFINITY, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
    }
    if (addObjects(additionalRoad1, r2, data))
    {
        cerr << "ERR: error in addObjects" << endl;
        return 1;
    }

    road r3;
    r3.id = 100 * junc.id + 3;
    r3.junction = junc.id;
    r3.predecessor.id = junc.id;
    r3.inputSegmentId = inputSegmentId;
    r3.isConnectingRoad = true;
    r3.predecessor.elementType = junctionType;
    if (mode == 1 || mode == 2)
    {
        if (buildRoad(refRoad, r3, sMain + sOffMain, INFINITY, automaticWidening, sMain, iPx, iPy, iPhdg))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(refRoad, r3, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }
    if (mode == 3)
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

    road r4;
    r4.id = 100 * junc.id + 4;
    r4.junction = junc.id;
    r4.predecessor.id = junc.id;
    r4.inputSegmentId = inputSegmentId;
    r4.isConnectingRoad = true;
    r4.predecessor.elementType = junctionType;
    if (mode == 1) //check here
    {
        if (buildRoad(additionalRoad1, r4, sAdd1 + sOffAdd1, INFINITY, automaticWidening, sAdd1, iPx, iPy, iPhdg + phi1))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad1, r4, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }
    if (mode == 2)
    {
        if (buildRoad(additionalRoad2, r4, sAdd2 + sOffAdd2, INFINITY, automaticWidening, sAdd2, iPx, iPy, iPhdg + phi2))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad2, r4, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }
    if (mode == 3)
    {
        if (buildRoad(additionalRoad3, r4, sAdd3 + sOffAdd3, INFINITY, automaticWidening, sAdd3, iPx, iPy, iPhdg + phi3))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad3, r4, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
    }

    // add addtional lanes
    if(addLanes != NULL){
        DOMNodeList *addLaneList = addLanes->getElementsByTagName(X("additionalLane"));
        for (int i = 0; i < addLaneList->getLength(); i ++)
        {
            DOMElement* addLane = (DOMElement*)addLaneList->item(i);
            if(addLane->getNodeType() != 1) continue; //we have to check the node type. Type 1 is an element. The reason for the check is that line breaks are handled as
            // text nodes by xercesC.
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

            if (inputId == r4.inputId && inputPos == r4.inputPos)
            {
                for (int i = 0; i < n; i++)
                    laneWideningJunction(r4, length, ds, type, verschwenkung, restricted);
            }
        }
    }

    data.roads.push_back(r1);
    data.roads.push_back(r2);
    data.roads.push_back(r3);
    data.roads.push_back(r4);

    // --- generate connecting lanes -------------------------------------------
    if(!setting.silentMode)
        cout << "\t Generate Connecting Lanes" << endl;

    // generate user-defined connecting lanes
    if (con != NULL && readStrAttrFromNode(con, "type") == "single")
    {
        DOMNodeList *roadLinkList = con->getElementsByTagName(X("roadLink"));
        for (int i = 0; i < roadLinkList->getLength(); i ++)
        {
            DOMElement* roadLink = (DOMElement*)roadLinkList->item(i);
            if(roadLink->getNodeType() != 1) continue; //we have to check the node type. Type 1 is an element. The reason for the check is that line breaks are handled as
            // text nodes by xercesC.

            int fromId = readIntAttrFromNode(roadLink, "fromId");
            int toId = readIntAttrFromNode(roadLink, "toId");

            string fromPos = "end";
            if (attributeExits(roadLink, "fromPos"))
                fromPos = readIntAttrFromNode(roadLink, "fromPos");

            string toPos = "end";
            if (attributeExits(roadLink, "toPos"))
                toPos = readIntAttrFromNode(roadLink, "toPos");

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

            DOMNodeList *laneLinkList = roadLink->getElementsByTagName(X("laneLink"));
            for (int i = 0; i < laneLinkList->getLength(); i ++)
            {
                DOMElement* laneLink = (DOMElement*)laneLinkList->item(i);
                if(laneLink->getNodeType() != 1) continue; //we have to check the node type. Type 1 is an element. The reason for the check is that line breaks are handled as
                // text nodes by xercesC.

                int from = readIntAttrFromNode(laneLink, "fromId");
                int to = readIntAttrFromNode(laneLink, "toId");

                // flip ids
                if (fromPos == "start")
                    from *= -1;
                if (toPos == "end")
                    to *= -1;

                string left = non;
                string right = non;

                if (attributeExits(laneLink, "left"))
                    left = readStrAttrFromNode(laneLink, "left");

                if (attributeExits(laneLink, "right"))
                    right = readStrAttrFromNode(laneLink, "right");;

                road r;
                r.id = 100 * junc.id + data.roads.size() + 1;
                r.inputSegmentId = inputSegmentId;
                createRoadConnection(r1, r2, r, junc, from, to, left, right);
                data.roads.push_back(r);
            }
        }
    }
    // generate automatic connecting lanes
    else
    {
        // switch roads if necessary, so that the angle to refRoad increases
        if (sortRoads(r1, r2, r3, r4))
        {
            cerr << "ERR: roads can not be sorted." << endl;
            return 1;
        }

        // generate connections
        int nCount = 1;
        int from, to, nF, nT;

        // 1) PART from M1 To M3 -> Middle to Middle

        calcFromTo(r1, r3, from, to, nF, nT, 0);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 || mode == 2)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);
            if (mode == 3)
                createRoadConnection(r1, r3, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 2) PART from M2 To M4 -> Middle to Middle

        calcFromTo(r2, r4, from, to, nF, nT, 0);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r2, r4, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 3) PART from M3 To M1 -> Middle to Middle

        calcFromTo(r3, r1, from, to, nF, nT, 0);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (mode == 1 || mode == 2)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);
            if (mode == 3)
                createRoadConnection(r3, r1, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 4) PART from M4 To M2 -> Middle to Middle

        calcFromTo(r4, r2, from, to, nF, nT, 0);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r4, r2, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 5) PART from R1 To R2 -> Right to Right (if exist)

        calcFromTo(r1, r2, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (i == 0)
                createRoadConnection(r1, r2, r, junc, from, to, non, sol);
            else
                createRoadConnection(r1, r2, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 6) PART from L2 To L1 -> Left to Left (if exist)

        calcFromTo(r2, r1, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r2, r1, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 7) PART from R2 To R3 -> Right to Right (if exist)

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

        // 8) PART from L3 To L2 -> Left to Left (if exist)

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

        // 9) PART from R3 To R4 -> Right to Right (if exist)

        calcFromTo(r3, r4, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (i == 0)
                createRoadConnection(r3, r4, r, junc, from, to, non, sol);
            else
                createRoadConnection(r3, r4, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 10) PART from L4 To L3 -> Left to Left (if exist)

        calcFromTo(r4, r3, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r4, r3, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }

        // 11) PART from R4 To R1 -> Right to Right (if exist)

        calcFromTo(r4, r1, from, to, nF, nT, -1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            if (i == 0)
                createRoadConnection(r4, r1, r, junc, from, to, non, sol);
            else
                createRoadConnection(r4, r1, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from--;
            to++;
            nCount++;
        }

        // 12) PART from L1 To L4 -> Left to Left (if exist)

        calcFromTo(r1, r4, from, to, nF, nT, 1);

        for (int i = 0; i < min(nF, nT); i++)
        {
            road r;
            r.inputSegmentId = inputSegmentId;
            r.id = 100 * junc.id + 50 + nCount;

            createRoadConnection(r1, r4, r, junc, from, to, non, non);

            data.roads.push_back(r);

            from++;
            to--;
            nCount++;
        }
    }
    data.junctions.push_back(junc);

    return 0;
}