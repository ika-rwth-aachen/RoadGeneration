// xjunction.h

/**
 * @brief function generates the roads and junctions for a x junction which is specified in the input file
 *  
 * @param node  input data from the input file for the xjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  errorcode
 */
int xjunction(pugi::xml_node &node, roadNetwork &data)
{
    // check type of the junction (M = mainroad, A = accessroad)
    int mode = 0; 
    if ((string)node.attribute("type").value() == "2M") mode = 1;
    if ((string)node.attribute("type").value() == "M2A")mode = 2;
    if ((string)node.attribute("type").value() == "4A") mode = 3;
    if (mode == 0) 
    {
        cerr << "ERR: junction type is not defined correct." << endl;
        return 1;
    }

    // create junction
    junction junc;
    junc.id = node.attribute("id").as_int();

    // define intersection properties
    pugi::xml_node iP = node.child("intersectionPoint");
    if (!iP)
    {
        cerr << "ERR: intersection point is not defined correct.";
        return 1;
    }
    pugi::xml_node cA = node.child("coupler").child("couplerArea");
    pugi::xml_node con = node.child("coupler").child("connection");
    

    // define junction roads
    pugi::xml_node mainRoad;
    pugi::xml_node additionalRoad1;
    pugi::xml_node additionalRoad2;
    pugi::xml_node additionalRoad3; 

    pugi::xml_node tmpNode = iP.child("adRoad");

    for (pugi::xml_node road: node.children("road"))
    {
        if(road.attribute("id").as_int() == iP.attribute("refId").as_int())
            mainRoad = road;

        if(mode >= 1 &&  road.attribute("id").as_int() == tmpNode.attribute("id").as_int())
            additionalRoad1 = road;

        if(mode >= 2 && road.attribute("id").as_int() == tmpNode.next_sibling("adRoad").attribute("id").as_int())
            additionalRoad2= road;

        if(mode >= 3 && road.attribute("id").as_int() == tmpNode.next_sibling("adRoad").next_sibling("adRoad").attribute("id").as_int())
            additionalRoad3 = road;
    }   

    if(!mainRoad || (mode >= 1 && !additionalRoad1) || (mode >= 2 && !additionalRoad2) || (mode >= 3 && !additionalRoad3)) 
    {
        cerr << "ERR: specified roads in intersection are not found.";
        return 1;
    }

    double sMain, sAdd1, sAdd2, sAdd3, sOffMain, sOffAdd1, sOffAdd2, sOffAdd3, phi1, phi2, phi3;

    // calculate offsets
    double sOffset = 0;
    if (cA) sOffset = stod(cA.attribute("sOffset").value(),&sz);
    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    sOffAdd3 = sOffset;
    for (pugi::xml_node_iterator sB = cA.begin(); sB != cA.end(); ++sB)
    {
        if (sB->attribute("id").as_int() == mainRoad.attribute("id").as_int()) sOffMain = sB->attribute("sOffset").as_double();
        
        if (sB->attribute("id").as_int() == additionalRoad1.attribute("id").as_int()) sOffAdd1 = sB->attribute("sOffset").as_double();
        
        if (sB->attribute("id").as_int() == additionalRoad2.attribute("id").as_int()) sOffAdd2 = sB->attribute("sOffset").as_double();
        
        if (sB->attribute("id").as_int() == additionalRoad3.attribute("id").as_int()) sOffAdd3 = sB->attribute("sOffset").as_double();
    }

    // calculate width of mainRoad and addtionalRoad
    road help1;
    generateRoad(mainRoad, help1, 0, INFINITY, 0, 0, 0, 0, 0); 

    road help2;
    generateRoad(additionalRoad1, help2, 0, INFINITY, 0, 0, 0, 0, 0); 

    road help3;
    generateRoad(additionalRoad2, help3, 0, INFINITY, 0, 0, 0, 0, 0);

    road help4;
    generateRoad(additionalRoad3, help4, 0, INFINITY, 0, 0, 0, 0, 0);
 
    laneSection lS1 = help1.laneSections.front();
    double width1 = abs(findTOffset(lS1, findMinLaneId(lS1), 0))                              + abs(findTOffset(lS1, findMaxLaneId(lS1), 0));

    laneSection lS2 = help2.laneSections.front();
    double width2 = abs(findTOffset(lS2, findMinLaneId(lS2), 0))                              + abs(findTOffset(lS2, findMaxLaneId(lS2), 0));

    laneSection lS3 = help3.laneSections.front();
    double width3 = abs(findTOffset(lS3, findMinLaneId(lS3), 0))                              + abs(findTOffset(lS3, findMaxLaneId(lS3), 0));

    laneSection lS4 = help4.laneSections.front();
    double width4 = abs(findTOffset(lS4, findMinLaneId(lS4), 0))                              + abs(findTOffset(lS4, findMaxLaneId(lS4), 0));
        
    // check offsets and adjust them if necessary
    double w1 = max(width2/2, max(width3/2, width4/2)) * 4;
    double w2 = max(width1/2, max(width3/2, width4/2)) * 4;
    double w3 = max(width1/2, max(width2/2, width4/2)) * 4;
    double w4 = max(width1/2, max(width2/2, width3/2)) * 4;

    bool changed = false;
    if (sOffMain < w1) {sOffMain = w1; changed = true;}
    if (sOffAdd1 < w2) {sOffAdd1 = w2; changed = true;} 
    if (sOffAdd2 < w3) {sOffAdd2 = w3; changed = true;} 
    if (sOffAdd3 < w4) {sOffAdd3 = w4; changed = true;} 

    if (changed)
    {
        cerr << "!!! sOffset of at least one road was changed, due to feasible road structure !!!" << endl;
    }

    // calculate s and phi at intersection
    sMain = iP.attribute("s").as_double();

    if(mode >= 1)
    {
        tmpNode = iP.child("adRoad");
        if(!tmpNode)
        {
            cerr << "ERR: first 'adRoad' is missing."<< endl;
            return 1;
        }
        sAdd1 = tmpNode.attribute("s").as_double();
        phi1 = tmpNode.attribute("angle").as_double();
        tmpNode = tmpNode.next_sibling("adRoad");
    }
    if(mode >= 2)
    {
        if(!tmpNode)
        {
            cerr << "ERR: second 'adRoad' is missing."<< endl;
            return 1;
        }
        sAdd2 = tmpNode.attribute("s").as_double();
        phi2 = tmpNode.attribute("angle").as_double();
        tmpNode = tmpNode.next_sibling("adRoad");
    }
    if(mode >= 3)
    {
        if(!tmpNode)
        {
            cerr << "ERR: third 'adRoad' is missing."<< endl;
            return 1;
        }
        sAdd3 = tmpNode.attribute("s").as_double();
        phi3 = tmpNode.attribute("angle").as_double();
        tmpNode = tmpNode.next_sibling("adRoad");
    }

    // calculate coordinates of intersectionPoint
    road helperRoad;
    generateRoad(mainRoad, helperRoad, sMain, INFINITY, 0, sMain, 0, 0, 0);
    double iPx = helperRoad.geometries.back().x;
    double iPy = helperRoad.geometries.back().y;
    double iPhdg = helperRoad.geometries.back().hdg;

    // --- generate roads ------------------------------------------------------
    /*            |      |
                       | id:4 |
             __________|      |___________
    mainRoad   id: 1               id: 3
             __________        ___________    
                       |      |
                       | id:2 |
                       |      |
    */
    cout << "\t Generating Roads" << endl;

    road r1;
    r1.id = 100*junc.id + 1;
    r1.junction = junc.id;
    r1.predecessor.elementId = junc.id;
    r1.predecessor.elementType = "junction";
    if (mode == 1 || mode == 2)
    {
        generateRoad(mainRoad, r1, sMain-sOffMain, 0, 50, sMain, iPx, iPy, iPhdg);
    }
    if (mode == 3)
    {
        generateRoad(mainRoad, r1, sMain+sOffMain, INFINITY, 0, sMain,iPx, iPy, iPhdg);
    }
    addObjects(mainRoad,r1,data);
    addSignal(r1, data, 1, INFINITY, "1.000.001", "-");
    data.roads.push_back(r1);

    road r2;
    r2.id = 100*junc.id + 2;
    r2.junction = junc.id;
    r2.predecessor.elementId = junc.id;
    r2.predecessor.elementType = "junction";
    if (mode == 1)
    {
        generateRoad(additionalRoad1, r2, sAdd1-sOffAdd1, 0, 50, sAdd1, iPx, iPy, iPhdg+phi1);
    }
    if (mode == 2 || mode == 3)
    {
        generateRoad(additionalRoad1, r2, sAdd1+sOffAdd1, INFINITY, 0, sAdd1, iPx, iPy, iPhdg+phi1);
    }
    addObjects(additionalRoad1,r2,data);
    addSignal(r2, data, 1, INFINITY, "1.000.001", "-");
    data.roads.push_back(r2);

    road r3; 
    r3.id = 100*junc.id + 3;
    r3.junction = junc.id;
    r3.predecessor.elementId = junc.id;
    r3.predecessor.elementType = "junction";
    if (mode == 1 || mode == 2)
    {
        generateRoad(mainRoad, r3, sMain+sOffMain, INFINITY, 50, sMain,iPx, iPy,iPhdg);
        addObjects(mainRoad,r3,data);
    }
    if (mode == 3)
    {
        generateRoad(additionalRoad2, r3, sAdd2+sOffAdd2, INFINITY, 0, sAdd2, iPx, iPy, iPhdg+phi2);
        addObjects(additionalRoad2,r3,data);
    }
    addSignal(r3, data, 1, INFINITY, "1.000.001", "-");
    data.roads.push_back(r3);

    road r4; 
    r4.id = 100*junc.id + 4;
    r4.junction = junc.id;
    r4.predecessor.elementId = junc.id;
    r4.predecessor.elementType = "junction";
    if (mode == 1)
    {
        generateRoad(additionalRoad1, r4, sAdd1+sOffAdd1, INFINITY, 50,sAdd1, iPx, iPy, iPhdg+phi1);
        addObjects(additionalRoad1,r4,data);
    }
    if (mode == 2)
    {
        generateRoad(additionalRoad2, r4, sAdd2+sOffAdd2, INFINITY, 0, sAdd2, iPx, iPy, iPhdg+phi2);
        addObjects(additionalRoad2,r4,data);
    }
    if (mode == 3)
    {
        generateRoad(additionalRoad3, r4, sAdd3+sOffAdd3, INFINITY, 0, sAdd3, iPx, iPy, iPhdg+phi3);
        addObjects(additionalRoad3,r4,data);
    }
    addSignal(r4, data, 1, INFINITY, "1.000.001", "-");
    data.roads.push_back(r4);

    // --- generate connecting lanes -------------------------------------------
    cout << "\t Generate Connecting Lanes" << endl;

    // --- generate user-defined connecting lanes
    if(con && (string)con.attribute("type").value() == "single")
    {
        for (pugi::xml_node roadLink: con.children("roadLink"))
        {
            int fromId = roadLink.attribute("fromId").as_int();
            int toId = roadLink.attribute("toId").as_int();

            road r1,r2;
            for (int i = 0; i < data.roads.size(); i++)
            {
                if (data.roads[i].id == fromId) r1 = data.roads[i];
                if (data.roads[i].id == toId)   r2 = data.roads[i];
            }
            if (r1.id == -1 || r2.id == -1)
            {
                cerr << "ERR: error in user-defined lane connecting:" << endl;
                cerr << "\t road to 'fromId' or 'toId' can not be found" << endl;
                return 1;
            }

            for (pugi::xml_node laneLink: roadLink.children("laneLink"))
            {
                int from = laneLink.attribute("fromId").as_int();
                int to = laneLink.attribute("toId").as_int();

                string left = non;
                string right = non;
                string middle = non;

                if (laneLink.attribute("left")) 
                    left = laneLink.attribute("left").value();

                if (laneLink.attribute("right")) 
                    right = laneLink.attribute("right").value();

                if (laneLink.attribute("middle")) 
                    middle = laneLink.attribute("middle").value();

                road r;
                r.id = 100*junc.id + data.roads.size() + 1;
                createRoadConnection(r1,r2,r,junc,from,to,left,right,middle);
                data.roads.push_back(r);
            }
        }
    }
    else
    {   
        // switch roads if necessary, so that the angle to mainRoad increases
        if(sortRoads(r1,r2,r3,r4))
        {
            cerr << "ERR: roads can not be sorted." << endl;
            return 1;
        }

        int from, to;

        // max and min id's of relevant laneSections
        int max1 = findMaxLaneId(r1.laneSections.front());
        int min1 = findMinLaneId(r1.laneSections.front());

        int max2 = findMaxLaneId(r2.laneSections.front());
        int min2 = findMinLaneId(r2.laneSections.front());

        int max3 = findMaxLaneId(r3.laneSections.front());
        int min3 = findMinLaneId(r3.laneSections.front());

        int max4 = findMaxLaneId(r4.laneSections.front());
        int min4 = findMinLaneId(r4.laneSections.front());

        road r5; 
        r5.id = 100*junc.id + 50 + 1;
        from = findMiddleLane(max1);
        to = findMiddleLane(min3);
        if (mode == 1 || mode == 2)
            createRoadConnection(r1,r3,r5,junc,from,to,bro,bro,bro);
        if (mode == 3)
            createRoadConnection(r1,r3,r5,junc,from,to,non,non,non);
        data.roads.push_back(r5);

        road r6; 
        r6.id = 100*junc.id + 50 + 2;
        from = findMiddleLane(max3);
        to = findMiddleLane(min1);
        if (mode == 1 || mode == 2)
        createRoadConnection(r3,r1,r6,junc,from,to,bro,bro,bro);
        if (mode == 3)
        createRoadConnection(r3,r1,r6,junc,from,to,non,non,non);
        data.roads.push_back(r6);

        road r7; 
        r7.id = 100*junc.id + 50 + 3;
        from = findMiddleLane(max2);
        to = findMiddleLane(min4);
        createRoadConnection(r2,r4,r7,junc,from,to,non,non,non);
        data.roads.push_back(r7);

        road r8; 
        r8.id = 100*junc.id + 50 + 4;
        from = findMiddleLane(max4);
        to = findMiddleLane(min2);
        createRoadConnection(r4,r2,r8,junc,from,to,non,non,non);
        data.roads.push_back(r8);

        // --- more complicated junctions ---
        road r9; 
        r9.id = 100*junc.id + 50 + 5;
        from = findRightLane(max1);
        to = findRightLane(min2);
        createRoadConnection(r1,r2,r9,junc,from,to,non,sol,non);
        data.roads.push_back(r9);

        road r10; 
        r10.id = 100*junc.id + 50 + 6;
        from = findLeftLane(min1);
        to = findLeftLane(max2);
        createRoadConnection(r1,r2,r10,junc,from,to,non,non,non);
        data.roads.push_back(r10);

        road r11; 
        r11.id = 100*junc.id + 50 + 7;
        from = findRightLane(max2);
        to = findRightLane(min3);
        createRoadConnection(r2,r3,r11,junc,from,to,non,sol,non);
        data.roads.push_back(r11);

        road r12; 
        r12.id = 100*junc.id + 50 + 8;
        from = findLeftLane(min2);
        to = findLeftLane(max3);
        createRoadConnection(r2,r3,r12,junc,from,to,non,non,non);
        data.roads.push_back(r12);

        road r13; 
        r13.id = 100*junc.id + 50 + 9;
        from = findRightLane(max3);
        to = findRightLane(min4);
        createRoadConnection(r3,r4,r13,junc,from,to,non,sol,non);
        data.roads.push_back(r13);
        
        road r14; 
        r14.id = 100*junc.id + 50 + 10;
        from = findLeftLane(min3);
        to = findLeftLane(max4);
        createRoadConnection(r3,r4,r14,junc,from,to,non,non,non);
        data.roads.push_back(r14);

        road r15; 
        r15.id = 100*junc.id + 50 + 11;
        from = findRightLane(max4);
        to = findRightLane(min1);
        createRoadConnection(r4,r1,r15,junc,from,to,non,sol,non);
        data.roads.push_back(r15);

        road r16; 
        r16.id = 100*junc.id + 50 + 12;
        from = findLeftLane(min4);
        to = findLeftLane(max1);
        createRoadConnection(r4,r1,r16,junc,from,to,non,non,non);
        data.roads.push_back(r16);
    }

    data.junctions.push_back(junc);     

    return 0;
}