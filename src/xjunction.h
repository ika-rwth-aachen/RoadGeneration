// xjunction.h

int xjunction(pugi::xml_node &node, roadNetwork &data)
{
    // check type (all roads start in intersection point)
    int mode = 0; 
    if ((string)node.attribute("type").value() == "2M") mode = 1;
    if ((string)node.attribute("type").value() == "M2A")mode = 2;
    if ((string)node.attribute("type").value() == "4A") mode = 3;
    if (mode == 0) 
    {
        cout << "ERR: wrong type" << endl;
        exit(0);
    }

    // create junction
    junction junc;
    junc.id = node.attribute("id").as_int();

    pugi::xml_node iP = node.child("intersectionPoint");
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

        if(mode >= 3 && road.attribute("id").as_int() == tmpNode.next_sibling("adRoad").attribute("id").as_int())
            additionalRoad3 = road;
    }   

    if(!mainRoad || (mode >= 1 && !additionalRoad1) || (mode >= 2 && !additionalRoad2) || (mode >= 3 && !additionalRoad3)) 
        cout << "ERR: no corresponding roads are found" << endl;

    double sMain, sAdd1, sAdd2, sAdd3, sOffMain, sOffAdd1, sOffAdd2, sOffAdd3, phi1, phi2, phi3;

    // calculate offsets
    double sOffset = stod(cA.attribute("sOffset").value(),&sz);
    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    sOffAdd3 = sOffset;
    for (pugi::xml_node_iterator sB = cA.begin(); sB != cA.end(); ++sB)
    {
        if (sB->attribute("id").as_int() == mainRoad.attribute("id").as_int()) sOffMain = stod(sB->attribute("sOffset").value(),&sz);
        
        if (sB->attribute("id").as_int() == additionalRoad1.attribute("id").as_int()) sOffAdd1 = stod(sB->attribute("sOffset").value(),&sz);
        
        if (sB->attribute("id").as_int() == additionalRoad2.attribute("id").as_int()) sOffAdd2 = stod(sB->attribute("sOffset").value(),&sz);
        
        if (sB->attribute("id").as_int() == additionalRoad3.attribute("id").as_int()) sOffAdd3 = stod(sB->attribute("sOffset").value(),&sz);
    }

    // calculate s and phi at intersection
    sMain = stod(iP.attribute("s").value(),&sz);

    if(mode >= 1)
    {
        tmpNode = iP.child("adRoad");
        sAdd1 = stod(tmpNode.attribute("s").value(),&sz);
        phi1 = stod(tmpNode.attribute("angle").value(),&sz);
        tmpNode = tmpNode.next_sibling("adRoad");
    }
    if(mode >= 2)
    {
        sAdd2 = stod(tmpNode.attribute("s").value(),&sz);
        phi2 = stod(tmpNode.attribute("angle").value(),&sz);
        tmpNode = tmpNode.next_sibling("adRoad");
    }
    if(mode >= 3)
    {
        sAdd3 = stod(tmpNode.attribute("s").value(),&sz);
        phi3 = stod(tmpNode.attribute("angle").value(),&sz);
        tmpNode = tmpNode.next_sibling("adRoad");
    }

    // calculate intersectionPoint
    road helperRoad;
    generateRoad(mainRoad, helperRoad, sMain, 0, 0, 0, 0);
    double iPx = helperRoad.geometries.back().x;
    double iPy = helperRoad.geometries.back().y;
    double iPhdg = helperRoad.geometries.back().hdg;

    cout << "Generating Roads" << endl;

    cout << "Road 1" << endl;
    road r1;
    r1.id = 100*junc.id + 1;
    r1.junction = junc.id;
    if (mode == 1 || mode == 2)
    {
        r1.predecessor.elementId =mainRoad.attribute("idStart").as_int();
        r1.successor.elementId = 100*junc.id + 5;
        generateRoad(mainRoad, r1, sMain, -sOffMain, 0, 0, 0);
    }
    if (mode == 3)
    {
        r1.predecessor.elementId = 100*junc.id + 5;
        r1.successor.elementId = mainRoad.attribute("idEnd").as_int();
        generateRoad(mainRoad, r1, sMain, sOffMain, 0, 0, 0);
    }
    data.roads.push_back(r1);

    cout << "Road 2" << endl;
    road r2;
    r2.id = 100*junc.id + 2;
    r2.junction = junc.id;
    if (mode == 1)
    {
        r2.predecessor.elementId = additionalRoad1.attribute("idStart").as_int();
        r2.successor.elementId = 100*junc.id + 6;
        generateRoad(additionalRoad1, r2, sAdd1, -sOffAdd1, iPhdg+phi1, iPx, iPy);
    }
    if (mode == 2 || mode == 3)
    {
        r2.predecessor.elementId = 100*junc.id + 6;
        r2.successor.elementId = additionalRoad1.attribute("idEnd").as_int();
        generateRoad(additionalRoad1, r2, sAdd1, sOffAdd1, iPhdg+phi1, iPx, iPy);
    }
    data.roads.push_back(r2);

    cout << "Road 3" << endl;
    road r3; 
    r3.id = 100*junc.id + 3;
    r3.junction = junc.id;
    if (mode == 1 || mode == 2)
    {
        r3.predecessor.elementId = 100*junc.id + 5;
        r3.successor.elementId = mainRoad.attribute("idEnd").as_int();
        generateRoad(mainRoad, r3, sMain, sOffMain, 0, 0, 0);
    }
    if (mode == 3)
    {
        r3.predecessor.elementId = 100*junc.id + 5;
        r3.successor.elementId = additionalRoad2.attribute("idEnd").as_int();
        generateRoad(additionalRoad2, r3, sAdd2, sOffAdd2, iPhdg+phi2, iPx, iPy);
    }
    data.roads.push_back(r3);

    cout << "Road 4" << endl;
    road r4; 
    r4.id = 100*junc.id + 4;
    r4.junction = junc.id;
    if (mode == 1)
    {
        r4.predecessor.elementId = 100*junc.id + 6;
        r4.successor.elementId = additionalRoad1.attribute("idEnd").as_int();
        generateRoad(additionalRoad1, r4, sAdd1, sOffAdd1, iPhdg+phi1, iPx, iPy);
    }
    if (mode == 2)
    {
        r4.predecessor.elementId = 100*junc.id + 6;
        r4.successor.elementId = additionalRoad2.attribute("idEnd").as_int();
        generateRoad(additionalRoad2, r4, sAdd2, sOffAdd2, iPhdg+phi2, iPx, iPy);
    }
    if (mode == 3)
    {
        r4.predecessor.elementId = 100*junc.id + 6;
        r4.successor.elementId = additionalRoad3.attribute("idEnd").as_int();
        generateRoad(additionalRoad3, r4, sAdd3, sOffAdd3, iPhdg+phi3, iPx, iPy);
    }
    data.roads.push_back(r4);

    // create connecting lanes
    cout << "Create Connecting Lanes" << endl;

    if((string)con.attribute("type").value() == "all")
    {
        cout << "Road 5" << endl;
        road r5; 
        r5.id = 100*junc.id + 5;
        r5.junction = junc.id;
        r5.predecessor.elementId = r1.id;
        r5.successor.elementId   = r3.id;
        createRoadConnection(r1,r3,r5,junc,100,1,100);
        data.roads.push_back(r5);

        cout << "Road 6" << endl;
        road r6; 
        r6.id = 100*junc.id + 6;
        r6.junction = junc.id;
        r6.predecessor.elementId = r2.id;
        r6.successor.elementId   = r4.id;
        createRoadConnection(r2,r4,r6,junc,100,0,0);
        data.roads.push_back(r6);

        cout << "Road 7" << endl;
        road r7; 
        r7.id = 100*junc.id + 7;
        r7.junction = junc.id;
        r7.predecessor.elementId = r1.id;
        r7.successor.elementId   = r2.id;
        createRoadConnection(r1,r2,r7,junc,100,2,0);
        data.roads.push_back(r7);

        cout << "Road 8" << endl;
        road r8; 
        r8.id = 100*junc.id + 8;
        r8.junction = junc.id;
        r8.predecessor.elementId = r2.id;
        r8.successor.elementId   = r3.id;
        createRoadConnection(r2,r3,r8,junc,100,2,0);
        data.roads.push_back(r8);

        cout << "Road 9" << endl;
        road r9; 
        r9.id = 100*junc.id + 9;
        r9.junction = junc.id;
        r9.predecessor.elementId = r3.id;
        r9.successor.elementId   = r4.id;
        createRoadConnection(r3,r4,r9,junc,100,2,0);
        data.roads.push_back(r9);

        cout << "Road 10" << endl;
        road r10; 
        r10.id = 100*junc.id + 10;
        r10.junction = junc.id;
        r10.predecessor.elementId = r4.id;
        r10.successor.elementId   = r1.id;
        createRoadConnection(r4,r1,r10,junc,100,2,0);
        data.roads.push_back(r10);
    }
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

            cout << data.roads[0].geometries.size() << endl;
            for (pugi::xml_node laneLink: roadLink.children("laneLink"))
            {
                int laneId = laneLink.attribute("laneId").as_int();

                cout << "Road " << data.roads.size() + 1 << endl;
                road r;
                r.id = 100*junc.id + data.roads.size() + 1;
                r.junction = junc.id;
                r.predecessor.elementId = fromId;
                r.successor.elementId   = toId;
                createRoadConnection(r1,r2,r,junc,laneId,0,0);
                data.roads.push_back(r);
            }
        }
    }

    data.junctions.push_back(junc);     

    return 0;
}