/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file roundAbout.h
 *
 * @brief file contains method for generating roundabout
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

extern settings setting;


/**
 * @brief translates the input segment id and the index of the junction inside the roundabout to an absolute junction id
 * 
 * @param juncGroupId input id of the segment. Equals the id of the junction group
 * @param juncIdx index of the junction inside the roundabout. Starts with 0 and counting up
 * @return int resulting id of the junction in the output file
 */
int juncGroupIdToJuncId(int juncGroupId, int juncIdx)
{
    return juncGroupId * 10000 + juncIdx * 100;
}

/**
 * @brief function generates the roads and junctions for a roundabout which is specified in the input file
 *  
 * @param node  input data from the input file for the roundAbout
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int roundAbout(const DOMElement* node, roadNetwork &data)
{
    // create segment
    data.nSegment++;
    vector<junction> junctions;
    junctionGroup juncGroup;

    juncGroup.id = readIntAttrFromNode(node, "id"); 
    juncGroup.name = "jg" + to_string(juncGroup.id);

    int inputSegmentId = readIntAttrFromNode(node, "id");

    DOMElement* dummy = NULL;
    DOMElement* circleRoad = getChildWithName(node, "circle");
    int refId = readIntAttrFromNode(circleRoad, "id");

    if (!circleRoad)
    {
        cerr << "ERR: circleRoad is not found.";
        return 1;
    }

    // store properties of circleRoad
    double length = readDoubleAttrFromNode(getChildWithName(getChildWithName(circleRoad, "referenceLine"), "circle"),"length") ;
    double R = length / (2 * M_PI);
    getChildWithName(getChildWithName(circleRoad, "referenceLine"), "circle")->setAttribute(X("R"), X(to_string(R).c_str()));
    

    double sOld;
    road rOld;

    bool clockwise;
    if (R > 0)
        clockwise = false;
    if (R < 0)
        clockwise = true;
    if (abs(R) < 3)
    {
        cerr << "ERR: radius of reference road in a roundabout have to be larger than 3.";
        return 1;
    }

    // get coupler
    //pugi::xml_node cA = node.child("coupler").child("junctionArea");
    DOMElement* cA = getChildWithName(getChildWithName(node, "coupler"), "junctionArea");

    // count intersectionPoints
    int nIp = 0;
    for (DOMElement* child = node->getFirstElementChild(); child != NULL; child = child->getNextElementSibling())
    {
        if(readNameFromNode(child) == "intersectionPoint")
        {
            nIp++;
        }
    }

    

    //generate all junctions first for easier linking
    int cc = 0;
    for (DOMElement* iP = node->getFirstElementChild(); iP != NULL; iP = iP->getNextElementSibling())
    {
        if(readNameFromNode(iP) != "intersectionPoint")
        {
            continue;
        }
        int adId = stoi(readAttributeFromChildren(iP, "adRoad", "id"));
        junction junc;
        junc.id = juncGroupIdToJuncId(juncGroup.id, adId);
        cc++;
        junctions.push_back(junc);
        juncGroup.juncIds.push_back(junc.id);
        
    }

    cc = 0;
    // iterate over all additonalRoads defined by separate intersectionPoints
    // sMain of intersection points have to increase
    for (DOMElement* iP = node->getFirstElementChild(); iP != NULL; iP = iP->getNextElementSibling())
    {

        if(readNameFromNode(iP) != "intersectionPoint")
        {
            continue;
        }
        junction &junc = junctions[cc];
        cc++;

        // find additionalRoad
        int adId = stoi(readAttributeFromChildren(iP, "adRoad", "id"));
        
        DOMElement* additionalRoad;
        for (DOMElement* road = node->getFirstElementChild(); road != NULL; road = road->getNextElementSibling())
        {
            if(readNameFromNode(road) != "road")
            {
                continue;
            }
            if (readIntAttrFromNode(road, "id") == adId)
                additionalRoad = road;
        }

        if (additionalRoad == NULL)
        {
            cerr << "ERR: specified road in intersection" << cc << " is not found.";
            return 1;
        }

        // calculate offsets
        double sOffset = 0;
        if (cA)
            sOffset = readDoubleAttrFromNode(cA, "gap");

        double sOffMain = sOffset;
        double sOffAdd = sOffset;
        for (DOMElement* sB = cA->getFirstElementChild(); sB != NULL; sB = sB->getNextElementSibling())
        {
            if(readNameFromNode(sB) != "roadGap")
            {
                continue;
            }

            if (readIntAttrFromNode(sB, "id") == refId)
                sOffMain = readDoubleAttrFromNode(sB, "gap");

            if (readIntAttrFromNode(sB, "id") == adId)
                sOffAdd = readDoubleAttrFromNode(sB, "gap");
        }

        

        // calculate width of circleRoad and addtionalRoad
        road helpMain;
        road helpAdd;
        if (buildRoad(circleRoad, helpMain, 0, INFINITY, dummy, 0, 0, 0, 0))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (buildRoad(additionalRoad, helpAdd, 0, INFINITY, dummy, 0, 0, 0, 0))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }

        laneSection lSMain = helpMain.laneSections.front();
        double widthMain = abs(findTOffset(lSMain, findMinLaneId(lSMain), 0));

        laneSection lSAdd = helpAdd.laneSections.front();
        double widthAdd = abs(findTOffset(lSAdd, findMinLaneId(lSAdd), 0)) + abs(findTOffset(lSAdd, findMaxLaneId(lSAdd), 0));

        // check offsets and adjust them if necessary (2 and 4 are safety factors)
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
            cerr << "!!! sOffset of at least one road was changed, due to feasible road structure !!!" << endl;
        }

        if (sOffMain * 2 > length / nIp)
        {
            cerr << "Length of roundabout is too short, overlapping roads." << endl;
            return 1;
        }

        // calculate s and phi at intersection
        double sMain = readDoubleAttrFromNode(iP, "s");
        double sAdd = readDoubleAttrFromNode(getChildWithName(iP, "adRoad"), "s");
        double phi = readDoubleAttrFromNode(getChildWithName(iP, "adRoad"), "angle");

        // calculate coordinates of intersectionPoint
        double curPhi = sMain / (2 * M_PI * R) * 2 * M_PI;
        double iPx = R * cos(curPhi - M_PI / 2);
        double iPy = R * sin(curPhi - M_PI / 2);
        double iPhdg = curPhi;

        // --- generate roads --------------------------------------------------
        if(!setting.silentMode)
            cout << "\t Generating Roads for Roundabout" << endl;
        /*           
                    \___       ____/
                 id: 1         id: helper
               \_______        _______/    
                       |      |
                       | id:2 |
                       |      |
        */
        int nCount = 1;

        road r1;
        r1.id = junc.id + nCount;
        r1.junction = -1;
        r1.inputSegmentId = inputSegmentId;
        r1.roundAboutInputSegment = juncGroup.id;
        r1.isConnectingRoad = true;
        r1.successor.id = junctions[(cc - 1+ junctions.size()) % junctions.size()].id;
        r1.successor.elementType = junctionType;
        r1.successor.contactPoint = startType;

        r1.predecessor.id = junctions[(cc - 2 + junctions.size()) % junctions.size()].id;
        r1.predecessor.elementType = junctionType;
        r1.predecessor.contactPoint = endType;


        if (cc == 1)
            sOld = sOffMain;

        if (buildRoad(circleRoad, r1, sOld, sMain - sOffMain, dummy, sMain, iPx, iPy, iPhdg))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        nCount++;

        road r2;
        r2.inputId = adId;
        r2.roundAboutInputSegment = juncGroup.id;
        r2.inputSegmentId = inputSegmentId;

        r2.id = junc.id + nCount;
        r2.junction = juncGroup.id; //storing the junction like this is a workaround for the problem with the id namespace. It needs to be this way
        //so there wont be a problem in linking and closing the road network
        r2.predecessor.id = junc.id;
        r2.predecessor.elementType = junctionType;
        r2.predecessor.contactPoint = startType;

        r2.isConnectingRoad = true;
        if (buildRoad(additionalRoad, r2, sAdd + sOffAdd, INFINITY, dummy, sAdd, iPx, iPy, iPhdg + phi))
        {
            cerr << "ERR: error in buildRoad" << endl;
            return 1;
        }
        if (addObjects(additionalRoad, r2, data))
        {
            cerr << "ERR: error in addObjects" << endl;
            return 1;
        }
        nCount++;

        // add signal to outgoing roads
        //addSignal(r2, data, 1, INFINITY, "1.000.001", "-", -1);

        road helper;
        helper.id = junctions[(cc)%nIp].id + nCount - 2 ;
        helper.junction = junc.id;
        if (cc < nIp)
        {
            if (buildRoad(circleRoad, helper, sMain + sOffMain, sMain + 2 * sOffMain, dummy, sMain, iPx, iPy, iPhdg))
            {
                cerr << "ERR: error in buildRoad" << endl;
                return 1;
            }
        }
        else // last segment
        {
            helper = rOld;
            helper.successor.elementType = roadType;
            helper.successor.id = -101;
            helper.successor.contactPoint = startType;
        }

  

        helper.predecessor.id = junc.id;
        helper.predecessor.elementType = junctionType;
        helper.predecessor.contactPoint = startType;

        // --- generate connecting lanes ---------------------------------------
        if(!setting.silentMode)
            cout << "\t Generate Connecting Lanes" << endl;

        // max and min id's of laneSections
        int inner1, outer1, outer2, inner2, nLane;

        if (clockwise)
        {
            outer1 = findOuterMiddleLane(r1.laneSections.back(), 1);
            inner1 = findInnerMiddleLane(r1.laneSections.back(), 1);

            outer2 = findOuterMiddleLane(helper.laneSections.back(), 1);
            inner2 = findInnerMiddleLane(helper.laneSections.back(), 1);

            nLane = outer1 - inner1 + 1;
        }
        if (!clockwise)
        {
            outer1 = findOuterMiddleLane(r1.laneSections.back(), -1);
            inner1 = findInnerMiddleLane(r1.laneSections.back(), -1);

            outer2 = findOuterMiddleLane(helper.laneSections.back(), -1);
            inner2 = findInnerMiddleLane(helper.laneSections.back(), -1);
            nLane = inner1 - outer1 + 1;
        }

        int r2_F_L = findLeftLane(r2.laneSections.front(), 1);
        int r2_F_MI = findInnerMiddleLane(r2.laneSections.front(), 1);
        int r2_F_MO = findOuterMiddleLane(r2.laneSections.front(), 1);
        int r2_F_R = findRightLane(r2.laneSections.front(), 1);

        int r2_T_L = findLeftLane(r2.laneSections.front(), -1);
        int r2_T_MI = findInnerMiddleLane(r2.laneSections.front(), -1);
        int r2_T_MO = findOuterMiddleLane(r2.laneSections.front(), -1);
        int r2_T_R = findRightLane(r2.laneSections.front(), -1);

        // generate connections
        int from, to;

        // 1) PART from R1 to HELPER
        from = inner1;
        to = inner2;

        for (int i = 0; i < nLane; i++)
        {
            road r;
            r.roundAboutInputSegment = juncGroup.id;
            r.inputSegmentId = inputSegmentId;
            r.id = junc.id + nCount;

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

            data.roads.push_back(r); //these are the connecting roads in the roundabout 
            nCount++;
        }

        road r5;
        r5.id = junc.id  + nCount;
        r5.inputSegmentId = inputSegmentId;

        r5.roundAboutInputSegment = juncGroup.id;

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

        road r6;
        r6.roundAboutInputSegment = juncGroup.id;
        r6.inputSegmentId = inputSegmentId;
        r6.id = junc.id + nCount;
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

        data.roads.push_back(r1);
        data.roads.push_back(r2);
        data.roads.push_back(r5);
        data.roads.push_back(r6);

        // update for next step
        sOld = sMain + sOffMain;
        if (cc == 1)
            rOld = r1;
        
    }

    for(auto &j: junctions)
        data.junctions.push_back(j);
    data.juncGroups.push_back(juncGroup);


    return 0;
}