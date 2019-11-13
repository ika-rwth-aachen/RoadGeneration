// tjunction.h

/**
 * @brief function generates the roads and junctions for a t junction which is specified in the input file
 *  
 * @param node  input data from the input file for the tjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  errorcode
 */
int tjunction(pugi::xml_node &node, roadNetwork &data)
{
    // check type of the junction (M = mainroad, A = accessroad)
    int mode = 0; 
    if ((string)node.attribute("type").value() == "MA") mode = 1;
    if ((string)node.attribute("type").value() == "3A")  mode = 2;
    if (mode == 0) 
    {
        cerr << "ERR: junction type is not defined correct." << endl;
        return 1;
    }

    // create junction
    data.nSegment++;
    junction junc;
    junc.id = node.attribute("id").as_int();

    // automatic widing
    double autoWiding = 25;
    if (node.attribute("automaticWiding"))
        autoWiding = node.attribute("automaticWiding").as_double();

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

    pugi::xml_node tmpNode = iP.child("adRoad");

    for (pugi::xml_node road: node.children("road"))
    {
        if(road.attribute("id").as_int() == iP.attribute("refId").as_int())
            mainRoad = road;

        if(mode >= 1 &&  road.attribute("id").as_int() == tmpNode.attribute("id").as_int())
            additionalRoad1 = road;

        if(mode >= 2 && road.attribute("id").as_int() == tmpNode.next_sibling("adRoad").attribute("id").as_int())
            additionalRoad2= road;
    }   

    if(!mainRoad || !additionalRoad1 || (mode == 2 && !additionalRoad2)) 
    {
        cerr << "ERR: specified roads in intersection are not found.";
        return 1;
    }

    double sMain, sAdd1, sAdd2, sOffMain, sOffAdd1, sOffAdd2, phi1, phi2;

    // calculate offsets
    double sOffset = 0;
    if (cA) sOffset = stod(cA.attribute("sOffset").value(),&sz);
    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    for (pugi::xml_node_iterator sB = cA.begin(); sB != cA.end(); ++sB)
    {
        if (sB->attribute("id").as_int() == mainRoad.attribute("id").as_int()) sOffMain = sB->attribute("sOffset").as_double();
        
        if (sB->attribute("id").as_int() == additionalRoad1.attribute("id").as_int()) sOffAdd1 = sB->attribute("sOffset").as_double();
        
        if (sB->attribute("id").as_int() == additionalRoad2.attribute("id").as_int()) sOffAdd2 = sB->attribute("sOffset").as_double();
    }
    
     // calculate width of mainRoad and addtionalRoad
    road help1;
    generateRoad(mainRoad, help1, 0, INFINITY, 0, 0, 0, 0, 0); 

    road help2;
    generateRoad(additionalRoad1, help2, 0, INFINITY, 0, 0, 0, 0, 0); 

    road help3;
    generateRoad(additionalRoad2, help3, 0, INFINITY, 0, 0, 0, 0, 0);

 
    laneSection lS1 = help1.laneSections.front();
    double width1 = abs(findTOffset(lS1, findMinLaneId(lS1), 0))                              + abs(findTOffset(lS1, findMaxLaneId(lS1), 0));

    laneSection lS2 = help2.laneSections.front();
    double width2 = abs(findTOffset(lS2, findMinLaneId(lS2), 0))                              + abs(findTOffset(lS2, findMaxLaneId(lS2), 0));

    laneSection lS3 = help3.laneSections.front();
    double width3 = abs(findTOffset(lS3, findMinLaneId(lS3), 0))                              + abs(findTOffset(lS3, findMaxLaneId(lS3), 0));
        
    // check offsets and adjust them if necessary
    double w1 = max(width2/2, width3/2) * 4;
    double w2 = max(width1/2, width3/2) * 4;
    double w3 = max(width1/2, width2/2) * 4;

    bool changed = false;
    if (sOffMain < w1) {sOffMain = w1; changed = true;}
    if (sOffAdd1 < w2) {sOffAdd1 = w2; changed = true;} 
    if (sOffAdd2 < w3) {sOffAdd2 = w3; changed = true;} 

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
        sAdd1 = stod(tmpNode.attribute("s").value(),&sz);
        phi1 = stod(tmpNode.attribute("angle").value(),&sz);
    }

    if(mode >= 2)
    {   
        if(!tmpNode.next_sibling("adRoad"))
        {
            cerr << "ERR: second 'adRoad' is missing."<< endl;
            return 1;
        }
        sAdd2 = stod(tmpNode.next_sibling("adRoad").attribute("s").value(),&sz);
        phi2 = stod(tmpNode.next_sibling("adRoad").attribute("angle").value(),&sz);
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
    r1.id = 100*junc.id + 1;
    r1.junction = junc.id;
    r1.predecessor.elementType = "junction";
    r1.predecessor.elementId = junc.id;
    if (mode == 1)
    {
        double phi = phi1; 
        fixAngle(phi);

        // add street is left from road 1
        if (phi > 0) 
            generateRoad(mainRoad, r1, sMain-sOffMain, 0, autoWiding, sMain, iPx, iPy, iPhdg);
        // add street is right from road 1
        if (phi < 0) 
            generateRoad(mainRoad, r1, sMain-sOffMain, 0, -autoWiding, sMain, iPx, iPy, iPhdg);
    }
    if (mode == 2)
    {
        generateRoad(mainRoad, r1, sMain+sOffMain, INFINITY, autoWiding, sMain, iPx, iPy, iPhdg);
    }
    addObjects(mainRoad,r1,data);

    road r2;
    r2.id = 100*junc.id + 2;
    r2.junction = junc.id;
    r2.predecessor.elementType = "junction";
    r2.predecessor.elementId = junc.id;
    if (mode == 1)
    {
        double phi = phi1; 
        fixAngle(phi);

        // add street is left from road 1
        if (phi > 0) 
            generateRoad(mainRoad, r2, sMain+sOffMain, INFINITY, -autoWiding, sMain, iPx, iPy, iPhdg);
        // add street is right from road 1
        if (phi < 0) 
            generateRoad(mainRoad, r2, sMain+sOffMain, INFINITY, autoWiding, sMain, iPx, iPy, iPhdg);
        addObjects(mainRoad,r2,data);
    }
    if (mode == 2)
    {
        generateRoad(additionalRoad1, r2, sAdd1 + sOffAdd1, INFINITY, autoWiding, sAdd1,iPx, iPy, iPhdg+phi1);
        addObjects(additionalRoad1,r2,data);
    }

    road r3; 
    r3.id = 100*junc.id + 3;
    r3.junction = junc.id;
    r3.predecessor.elementType = "junction";
    r3.predecessor.elementId = junc.id;
    if (mode == 1)
    {
        generateRoad(additionalRoad1, r3, sAdd1 + sOffAdd1, INFINITY, autoWiding, sAdd1, iPx, iPy, iPhdg+phi1);
        addObjects(additionalRoad1,r3,data);
    }
    if (mode == 2)
    {
        generateRoad(additionalRoad2, r3, sAdd2 + sOffAdd2, INFINITY, autoWiding, sAdd2, iPx, iPy, iPhdg+phi2);
        addObjects(additionalRoad2,r3,data);
    }

    // add addtional lanes
    for (pugi::xml_node addLane: addLanes.children("additionalLane"))
    {
        int n = 1;
        if (addLane.attribute("amount"))
            n = addLane.attribute("amount").as_int();
        bool verschwenkung = true;
        if (addLane.attribute("verschwenkung"))
            verschwenkung = addLane.attribute("verschwenkung").as_bool();

        double length = 25;
        if (addLane.attribute("length"))
            length = addLane.attribute("length").as_double();

        int type;
        string tmpType = addLane.attribute("type").value();
        if (tmpType == "left") type = 1;
        if (tmpType == "right") type = -1;

        int id = addLane.attribute("roadId").as_int();
        
        if (id == r1.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r1, length, type, verschwenkung);
        }

        if (id == r2.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r2, length, type, verschwenkung);
        }
        
        if (id == r3.id)
        {
            for (int i = 0; i < n; i++)
                laneWideningJunction(r3, length, type, verschwenkung);
        }
    }
    addSignal(r1, data, 1, INFINITY, "1.000.001", -1);
    addSignal(r2, data, 1, INFINITY, "1.000.001", -1);
    addSignal(r3, data, 1, INFINITY, "1.000.001", -1);

    data.roads.push_back(r1);
    data.roads.push_back(r2);
    data.roads.push_back(r3);

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
                    left = laneLink.attribute("right").value();

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
        // switch roads if necessary, so that 
        if(sortRoads(r1,r2,r3))
        {
            cerr << "ERR: roads can not be sorted." << endl;
            return 1;
        }
        
        // generate connections
        int nCount = 1;
        int from, to, nF, nT;

        // 1) PART from R1 To R2 -> Right to Right (if exist)
        
        calcFromTo(r1,r2,from,to,nF,nT,-1);

        for (int i = 0; i < min(nF,nT); i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0 && i == 0) 
                createRoadConnection(r1,r2,r,junc,from,to,bro,sol,bro);
            else if (mode == 1 && phi1 > 0 && i != 0) 
                createRoadConnection(r1,r2,r,junc,from,to,bro,bro,bro);
            else if (mode == 1 && phi1 < 0 && i == 0) 
                createRoadConnection(r1,r2,r,junc,from,to,non,sol,non);
            else if (mode == 1 && phi1 < 0 && i != 0) 
                createRoadConnection(r1,r2,r,junc,from,to,non,non,non);
            else if (mode == 2 && i == 0) 
                createRoadConnection(r1,r2,r,junc,from,to,non,sol,non);
            else if (mode == 2 && i != 0) 
                createRoadConnection(r1,r2,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from --;
            to++;
            nCount ++;
        }

        // 2) PART from L2 To L1 -> Left to Left (if exist)
        
        calcFromTo(r2,r1,from,to,nF,nT,1);
        
        for (int i = 0; i < min(nF,nT); i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0) 
                createRoadConnection(r2,r1,r,junc,from,to,bro,bro,bro);
            else if (mode == 1 && phi1 < 0) 
                createRoadConnection(r2,r1,r,junc,from,to,non,non,non);
            else if (mode == 2) 
                createRoadConnection(r2,r1,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }
         
        // 3) PART from R2 To R3 -> Right to Right (if exist)
        
        calcFromTo(r2,r3,from,to,nF,nT,-1);

        for (int i = 0; i < min(nF,nT); i++)
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

        // 4) PART from R3 To R2 -> Left to Left (if exist)
        
        calcFromTo(r3,r2,from,to,nF,nT,1);
                
        for (int i = 0; i < min(nF,nT); i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            createRoadConnection(r3,r2,r,junc,from,to,non,non,non);
            
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }

        // 5) PART from R3 To R1 -> Right to Right (if exist)
        
        calcFromTo(r3,r1,from,to,nF,nT,-1);

        for (int i = 0; i < min(nF,nT); i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0 && i == 0) 
                createRoadConnection(r3,r1,r,junc,from,to,non,sol,non);
            else if (mode == 1 && phi1 > 0 && i != 0) 
                createRoadConnection(r3,r1,r,junc,from,to,non,non,non);
            else if (mode == 1 && phi1 < 0 && i == 0) 
                createRoadConnection(r3,r1,r,junc,from,to,bro,sol,bro);
            else if (mode == 1 && phi1 < 0 && i != 0) 
                createRoadConnection(r3,r1,r,junc,from,to,bro,bro,bro);
            else if (mode == 2 && i == 0) 
                createRoadConnection(r3,r1,r,junc,from,to,non,sol,non);
            else if (mode == 2 && i != 0) 
                createRoadConnection(r3,r1,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from --;
            to++;
            nCount ++;
        }

        // 6) PART from L1 To L3 -> Left to Left (if exist)
        
        calcFromTo(r1,r3,from,to,nF,nT,1);
        
        for (int i = 0; i < min(nF,nT); i++)
        {
            road r;
            r.id = 100*junc.id + 50 + nCount;

            if (mode == 1 && phi1 > 0) 
                createRoadConnection(r1,r3,r,junc,from,to,non,non,bro);
            else if (mode == 1 && phi1 < 0) 
                createRoadConnection(r1,r3,r,junc,from,to,bro,bro,non);
            else if (mode == 2) 
                createRoadConnection(r1,r3,r,junc,from,to,non,non,non);
            
            data.roads.push_back(r);

            from++;
            to--;
            nCount ++;
        }
    }

    data.junctions.push_back(junc);     

    return 0;
}