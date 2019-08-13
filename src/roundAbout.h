// tjunction.h

int roundAbout(pugi::xml_node &node, roadNetwork &data)
{
    // create junction
    junction junc;
    junc.id = node.attribute("id").as_int();

    // search for referenceId
    int refId = node.attribute("refId").as_int();
    pugi::xml_node mainRoad;

    for (pugi::xml_node road: node.children("road"))
    {
        if(road.attribute("id").as_int() == refId)
            mainRoad = road;
    }

    // iteration over all additonalRoads defined by separate intersectionPoints
    for (pugi::xml_node iP: node.children("intersectionPoint"))
    {
        // corresponding coupler
        pugi::xml_node coupler;
        for (pugi::xml_node c: node.children("coupler"))
        {
            if (c.attribute("id").as_int() == iP.attribute("id").as_int())
                coupler = c;
        }

        // find additionalRoad
        int adId = iP.child("adRoad").attribute("id").as_int();
        pugi::xml_node additionalRoad;
        for (pugi::xml_node road: node.children("road"))
        {
            if(road.attribute("id").as_int() == adId)
                additionalRoad = road;
        }

        if (!additionalRoad)
        {
            cout << "ERR: no corresponding roads are found." << endl;
            exit(0);
        }

        // calculate offsets
        double sOffset = coupler.child("couplerArea").attribute("sOffset").as_double();

        double sOffMain = sOffset;
        double sOffAdd = sOffset;

        for (pugi::xml_node sB: coupler.child("couplerArea").children("streetBorder"))
        {
            if (sB.attribute("id").as_int() == refId) 
                sOffMain = sB.attribute("sOffset").as_double();

            if (sB.attribute("id").as_int() == adId) 
                sOffAdd = sB.attribute("sOffset").as_double();
        }

        // calculate s and phi at intersection
        double sMain = iP.attribute("s").as_double();
        double sAdd = iP.child("adRoad").attribute("s").as_double();
        double phi = iP.child("adRoad").attribute("angle").as_double();


        // calculate intersectionPoint
        double R = mainRoad.child("referenceLine").child("geometry").attribute("R").as_double();

        double curPhi = sMain / (2*M_PI*R) * 2 * M_PI; 
        double iPx = R * cos(curPhi-M_PI/2);
        double iPy = R * sin(curPhi-M_PI/2); //TODO
        double iPhdg = curPhi;

        road r;
        r.id = 100*junc.id + iP.attribute("id").as_int();
        r.junction = junc.id;
        generateRoad(mainRoad, r, sMain, -sOffMain, 0, 0, iPx, iPy);        
        data.roads.push_back(r);

        road r2;
        r2.id = -(100*junc.id + iP.attribute("id").as_int());
        r2.junction = junc.id;
        generateRoad(additionalRoad, r2, sAdd, sOffAdd, 0, 0, iPx, iPy);        
        data.roads.push_back(r2);


    }


    /*
    // --- create connecting lanes ---------------------------------------------
    cout << "Generating Connecting Lanes" << endl;

    if((string)con.attribute("type").value() == "all")
    {   
        // switch roads if necessary, so that 
        phi1 = r2.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
        phi2 = r3.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
        phi3 = r4.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
        fixAngle(phi1);
        fixAngle(phi2);
        fixAngle(phi3);

        road tmpR2 = r2;
        road tmpR3 = r3;
        road tmpR4 = r4;

        if (phi1 < phi2 && phi1 < phi3)
        {
            r2 = tmpR2;
            if (phi2 < phi3)
            {
                r3 = tmpR3;
                r4 = tmpR4;
            }
            else {
                r3 = tmpR4;
                r4 = tmpR3;
            }
        }
        else if (phi2 < phi1 && phi2 < phi3)
        {
            r2 = tmpR3;
            if (phi1 < phi3)
            {
                r3 = tmpR2;
                r4 = tmpR4;
            }
            else {
                r3 = tmpR4;
                r4 = tmpR2;
            }
        }
        else if (phi3 < phi1 && phi3 < phi2)
        {
            r2 = tmpR4;
            if (phi1 < phi2)
            {
                r3 = tmpR2;
                r4 = tmpR3;
            }
            else {
                r3 = tmpR3;
                r4 = tmpR2;
            }
        }
        else 
        {
            cout << "ERR: angles at intersection point are not defined.";
            exit(0);
        }

        double phi;
        int from, to;

        // max and min id's of laneSections
        int max1 = findMaxLaneId(r1.laneSections.front());
        int min1 = findMinLaneId(r1.laneSections.front());

        int max2 = findMaxLaneId(r2.laneSections.front());
        int min2 = findMinLaneId(r2.laneSections.front());

        int max3 = findMaxLaneId(r3.laneSections.front());
        int min3 = findMinLaneId(r3.laneSections.front());

        int max4 = findMaxLaneId(r4.laneSections.front());
        int min4 = findMinLaneId(r4.laneSections.front());

        cout << "Road 5" << endl;
        road r5; 
        r5.id = 100*junc.id + 5;
        from = findMiddleLane(max1);
        to = findMiddleLane(min3);
        if (mode == 1 || mode == 2)
            createRoadConnection(r1,r3,r5,junc,from,to,d,d,d);
        if (mode == 3)
            createRoadConnection(r1,r3,r5,junc,from,to,n,n,n);
        data.roads.push_back(r5);

        cout << "Road 6" << endl;
        road r6; 
        r6.id = 100*junc.id + 6;
        from = findMiddleLane(max3);
        to = findMiddleLane(min1);
        if (mode == 1 || mode == 2)
        createRoadConnection(r3,r1,r6,junc,from,to,d,d,d);
        if (mode == 3)
        createRoadConnection(r3,r1,r6,junc,from,to,n,n,n);
        data.roads.push_back(r6);

        cout << "Road 15" << endl;
        road r15; 
        r15.id = 100*junc.id + 15;
        from = findMiddleLane(max2);
        to = findMiddleLane(min4);
        createRoadConnection(r2,r4,r15,junc,from,to,n,n,n);
        data.roads.push_back(r15);

        cout << "Road 16" << endl;
        road r16; 
        r16.id = 100*junc.id + 16;
        from = findMiddleLane(max4);
        to = findMiddleLane(min2);
        createRoadConnection(r4,r2,r16,junc,from,to,n,n,n);
        data.roads.push_back(r16);

        // --- more complicated junctions ---
        cout << "Road 7" << endl;
        road r7; 
        r7.id = 100*junc.id + 7;
        from = findRightLane(max1);
        to = findRightLane(min2);
        createRoadConnection(r1,r2,r7,junc,from,to,n,s,n);
        data.roads.push_back(r7);

        cout << "Road 8" << endl;
        road r8; 
        r8.id = 100*junc.id + 8;
        from = findLeftLane(min1);
        to = findLeftLane(max2);
        createRoadConnection(r1,r2,r8,junc,from,to,n,n,n);
        data.roads.push_back(r8);

        cout << "Road 9" << endl;
        road r9; 
        r9.id = 100*junc.id + 9;
        from = findRightLane(max2);
        to = findRightLane(min3);
        createRoadConnection(r2,r3,r9,junc,from,to,n,s,n);
        data.roads.push_back(r9);

        cout << "Road 10" << endl;
        road r10; 
        r10.id = 100*junc.id + 10;
        from = findLeftLane(min2);
        to = findLeftLane(max3);
        createRoadConnection(r2,r3,r10,junc,from,to,n,n,n);
        data.roads.push_back(r10);

        cout << "Road 11" << endl;
        road r11; 
        r11.id = 100*junc.id + 11;
        from = findRightLane(max3);
        to = findRightLane(min4);
        createRoadConnection(r3,r4,r11,junc,from,to,n,s,n);
        data.roads.push_back(r11);
        
        cout << "Road 12" << endl;
        road r12; 
        r12.id = 100*junc.id + 12;
        from = findLeftLane(min3);
        to = findLeftLane(max4);
        createRoadConnection(r3,r4,r12,junc,from,to,n,n,n);
        data.roads.push_back(r12);

        cout << "Road 13" << endl;
        road r13; 
        r13.id = 100*junc.id + 13;
        from = findRightLane(max4);
        to = findRightLane(min1);
        createRoadConnection(r4,r1,r13,junc,from,to,n,s,n);
        data.roads.push_back(r13);

        cout << "Road 14" << endl;
        road r14; 
        r14.id = 100*junc.id + 14;
        from = findLeftLane(min4);
        to = findLeftLane(max1);
        createRoadConnection(r4,r1,r14,junc,from,to,n,n,n);
        data.roads.push_back(r14);
    }
    // user defined lane connections
    else if((string)con.attribute("type").value() == "single")
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

            for (pugi::xml_node laneLink: roadLink.children("laneLink"))
            {
                int from = laneLink.attribute("fromId").as_int();
                int to = laneLink.attribute("toId").as_int();

                string left = n;
                string right = n;
                string middle = n;

                if (laneLink.attribute("left")) 
                    left = laneLink.attribute("left").value();

                if (laneLink.attribute("right")) 
                    right = laneLink.attribute("right").value();

                cout << "Road " << data.roads.size() + 1 << endl;
                road r;
                r.id = 100*junc.id + data.roads.size() + 1;
                createRoadConnection(r1,r2,r,junc,from,to,left,right,middle);
                data.roads.push_back(r);
            }
        }
    }
    */

    data.junctions.push_back(junc);     

    return 0;
}