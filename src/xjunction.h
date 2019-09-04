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
    pugi::xml_node addLanes = node.child("coupler").child("additionalLanes");


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
        generateRoad(mainRoad, r1, sMain-sOffMain, 0, 25, sMain, iPx, iPy, iPhdg);
    }
    if (mode == 3)
    {
        generateRoad(mainRoad, r1, sMain+sOffMain, INFINITY, 0, sMain,iPx, iPy, iPhdg);
    }
    addObjects(mainRoad,r1,data);
    addSignal(r1, data, 1, INFINITY, "1.000.001", "-");

    road r2;
    r2.id = 100*junc.id + 2;
    r2.junction = junc.id;
    r2.predecessor.elementId = junc.id;
    r2.predecessor.elementType = "junction";
    if (mode == 1)
    {
        generateRoad(additionalRoad1, r2, sAdd1-sOffAdd1, 0, 25, sAdd1, iPx, iPy, iPhdg+phi1);
    }
    if (mode == 2 || mode == 3)
    {
        generateRoad(additionalRoad1, r2, sAdd1+sOffAdd1, INFINITY, 0, sAdd1, iPx, iPy, iPhdg+phi1);
    }
    addObjects(additionalRoad1,r2,data);
    addSignal(r2, data, 1, INFINITY, "1.000.001", "-");

    road r3; 
    r3.id = 100*junc.id + 3;
    r3.junction = junc.id;
    r3.predecessor.elementId = junc.id;
    r3.predecessor.elementType = "junction";
    if (mode == 1 || mode == 2)
    {
        generateRoad(mainRoad, r3, sMain+sOffMain, INFINITY, 25, sMain,iPx, iPy,iPhdg);
        addObjects(mainRoad,r3,data);
    }
    if (mode == 3)
    {
        generateRoad(additionalRoad2, r3, sAdd2+sOffAdd2, INFINITY, 0, sAdd2, iPx, iPy, iPhdg+phi2);
        addObjects(additionalRoad2,r3,data);
    }
    addSignal(r3, data, 1, INFINITY, "1.000.001", "-");

    road r4; 
    r4.id = 100*junc.id + 4;
    r4.junction = junc.id;
    r4.predecessor.elementId = junc.id;
    r4.predecessor.elementType = "junction";
    if (mode == 1)
    {
        generateRoad(additionalRoad1, r4, sAdd1+sOffAdd1, INFINITY, 25,sAdd1, iPx, iPy, iPhdg+phi1);
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

    // add addtional lanes
    for (pugi::xml_node addLane: addLanes.children("additionalLane"))
    {
        int n = 1;
        if (addLane.attribute("amount"))
            n = addLane.attribute("amount").as_int();

        int type = addLane.attribute("type").as_int();
        int id = addLane.attribute("roadId").as_int();
        
        if (id == r1.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r1, 25, type);
        }

        if (id == r2.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r2, 25, type);
        }
        
        if (id == r3.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r3, 25, type);
        }

        if (id == r4.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r4, 25, type);
        }
    }

    data.roads.push_back(r1);
    data.roads.push_back(r2);
    data.roads.push_back(r3);
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

        // lane ID's of relevant laneSections
        int r1_F_L  = findLeftLane(r1.laneSections.front(), 1);
        int r1_F_MI = findInnerMiddleLane(r1.laneSections.front(), 1);
        int r1_F_MO = findOuterMiddleLane(r1.laneSections.front(), 1);
        int r1_F_R  = findRightLane(r1.laneSections.front(), 1);

        int r1_T_L  = findLeftLane(r1.laneSections.front(), -1);
        int r1_T_MI = findInnerMiddleLane(r1.laneSections.front(), -1);
        int r1_T_MO = findOuterMiddleLane(r1.laneSections.front(), -1);
        int r1_T_R  = findRightLane(r1.laneSections.front(), -1);

        int r2_F_L  = findLeftLane(r2.laneSections.front(), 1);
        int r2_F_MI = findInnerMiddleLane(r2.laneSections.front(), 1);
        int r2_F_MO = findOuterMiddleLane(r2.laneSections.front(), 1);
        int r2_F_R  = findRightLane(r2.laneSections.front(), 1);

        int r2_T_L  = findLeftLane(r2.laneSections.front(), -1);
        int r2_T_MI = findInnerMiddleLane(r2.laneSections.front(), -1);
        int r2_T_MO = findOuterMiddleLane(r2.laneSections.front(), -1);
        int r2_T_R  = findRightLane(r2.laneSections.front(), -1);

        int r3_F_L  = findLeftLane(r3.laneSections.front(), 1);
        int r3_F_MI = findInnerMiddleLane(r3.laneSections.front(), 1);
        int r3_F_MO = findOuterMiddleLane(r3.laneSections.front(), 1);
        int r3_F_R  = findRightLane(r3.laneSections.front(), 1);

        int r3_T_L  = findLeftLane(r3.laneSections.front(), -1);
        int r3_T_MI = findInnerMiddleLane(r3.laneSections.front(), -1);
        int r3_T_MO = findOuterMiddleLane(r3.laneSections.front(), -1);
        int r3_T_R  = findRightLane(r3.laneSections.front(), -1);

        int r4_F_L  = findLeftLane(r4.laneSections.front(), 1);
        int r4_F_MI = findInnerMiddleLane(r4.laneSections.front(), 1);
        int r4_F_MO = findOuterMiddleLane(r4.laneSections.front(), 1);
        int r4_F_R  = findRightLane(r4.laneSections.front(), 1);

        int r4_T_L  = findLeftLane(r4.laneSections.front(), -1);
        int r4_T_MI = findInnerMiddleLane(r4.laneSections.front(), -1);
        int r4_T_MO = findOuterMiddleLane(r4.laneSections.front(), -1);
        int r4_T_R  = findRightLane(r4.laneSections.front(), -1);

        // generate connections
        int nCount = 1;
        int from, to, n;



        // 1) PART from M1 To M3 -> Middle to Middle
        
        n = r1_F_MO - r1_F_MI + 1;
        from = r1_F_MI;
        to = r3_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (mode == 1 || mode == 2)
                createRoadConnection(r1,r3,r,junc,from,to,bro,bro,bro);
            if (mode == 3)
                createRoadConnection(r1,r3,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from ++;
            to--;
            nCount ++;
        }



        // 2) PART from M2 To M4 -> Middle to Middle
        
        n = r2_F_MO - r2_F_MI + 1;
        from = r2_F_MI;
        to = r4_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r2,r4,r,junc,from,to,non,non,non);            
            data.roads.push_back(r);

            from ++;
            to--;
            nCount ++;
        }



        // 3) PART from M3 To M1 -> Middle to Middle
        
        n = r3_F_MO - r3_F_MI + 1;
        from = r3_F_MI;
        to = r1_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (mode == 1 || mode == 2)
                createRoadConnection(r3,r1,r,junc,from,to,bro,bro,bro);
            if (mode == 3)
                createRoadConnection(r3,r1,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from ++;
            to--;
            nCount ++;
        }



        // 4) PART from M4 To M2 -> Middle to Middle
        
        n = r4_F_MO - r4_F_MI + 1;
        from = r4_F_MI;
        to = r2_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r4,r2,r,junc,from,to,non,non,non);            
            data.roads.push_back(r);

            from ++;
            to--;
            nCount ++;
        }



        // 5) PART from R1 To R2 -> Right to Right (if exist)
        
        if (r1_F_R != 0) 
        {
            n = r1_F_R - r1_F_MO;
            from = r1_F_R;
        }
        else
        {
            n = 1;
            from = r1_F_MO;
        }

        if (r2_T_R != 0) to = r2_T_R;
        else to = r2_T_MO;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (i == 0) 
                createRoadConnection(r1,r2,r,junc,from,to,non,sol,non);
            else if (i != 0) 
                createRoadConnection(r1,r2,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from --;
            to++;
            nCount ++;
        }



        // 6) PART from L2 To L1 -> Left to Left (if exist)
        
        if (r2_F_L != 0) 
        {
            n = r2_F_MI - r2_F_L;
            from = r2_F_L;
        }
        else
        {
            n = 1;
            from = r2_F_MI;
        }

        if (r1_T_L != 0) to = r1_T_L;
        else to = r1_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r2,r1,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }
         

        // 7) PART from R2 To R3 -> Right to Right (if exist)
        
        if (r2_F_R != 0) 
        {
            n = r2_F_R - r2_F_MO;
            from = r2_F_R;
        }
        else
        {
            n = 1;
            from = r2_F_MO;
        }

        if (r3_T_R != 0) to = r3_T_R;
        else to = r3_T_MO;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (i == 0) 
                createRoadConnection(r2,r3,r,junc,from,to,non,sol,bro);
            else if (i != 0) 
                createRoadConnection(r2,r3,r,junc,from,to,non,non,bro);
            
            data.roads.push_back(r);

            from --;
            to++;
            nCount ++;
        }



        // 8) PART from L3 To L2 -> Left to Left (if exist)
        
        if (r3_F_L != 0) 
        {
            n = r3_F_MI - r3_F_L;
            from = r3_F_L;
        }
        else
        {
            n = 1;
            from = r3_F_MI;
        }

        if (r2_T_L != 0) to = r2_T_L;
        else to = r2_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r3,r2,r,junc,from,to,non,non,non);            
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }


        // 9) PART from R3 To R4 -> Right to Right (if exist)
        
        if (r3_F_R != 0) 
        {
            n = r3_F_R - r3_F_MO;
            from = r3_F_R;
        }
        else
        {
            n = 1;
            from = r3_F_MO;
        }

        if (r4_T_R != 0) to = r4_T_R;
        else to = r4_T_MO;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (i == 0) 
                createRoadConnection(r3,r4,r,junc,from,to,non,sol,non);
            else if (i != 0) 
                createRoadConnection(r3,r4,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from --;
            to++;
            nCount ++;
        }



        // 10) PART from L4 To L3 -> Left to Left (if exist)
        
        if (r4_F_L != 0) 
        {
            n = r4_F_MI - r4_F_L;
            from = r4_F_L;
        }
        else
        {
            n = 1;
            from = r4_F_MI;
        }

        if (r3_T_L != 0) to = r3_T_L;
        else to = r3_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r4,r3,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }


        // 11) PART from R4 To R1 -> Right to Right (if exist)
        
        if (r4_F_R != 0) 
        {
            n = r4_F_R - r4_F_MO;
            from = r4_F_R;
        }
        else
        {
            n = 1;
            from = r4_F_MO;
        }

        if (r1_T_R != 0) to = r1_T_R;
        else to = r1_T_MO;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (i == 0) 
                createRoadConnection(r4,r1,r,junc,from,to,non,sol,non);
            else if (i != 0) 
                createRoadConnection(r4,r1,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from --;
            to++;
            nCount ++;
        }



        // 12) PART from L1 To L4 -> Left to Left (if exist)
        
        if (r1_F_L != 0) 
        {
            n = r1_F_MI - r1_F_L;
            from = r1_F_L;
        }
        else
        {
            n = 1;
            from = r1_F_MI;
        }

        if (r4_T_L != 0) to = r4_T_L;
        else to = r4_T_MI;
        
        for (int i = 0; i < n; i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r1,r4,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }
    }

    data.junctions.push_back(junc);     

    return 0;
}