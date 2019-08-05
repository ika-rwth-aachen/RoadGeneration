// tjunction.h

#define all 0 
#define n "none" 
#define d "broken" 
#define s "solid" 

int tjunction(pugi::xml_node &node, roadNetwork &data)
{
    // check type (all roads start in intersection point)
    int mode = 0; 
    if ((string)node.attribute("type").value() == "MA") mode = 1;
    if ((string)node.attribute("type").value() == "3A")  mode = 2;
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
        cout << "ERR: no corresponding roads are found" << endl;

    double sMain, sAdd1, sAdd2, sOffMain, sOffAdd1, sOffAdd2, phi1, phi2;

    // calculate offsets
    double sOffset = stod(cA.attribute("sOffset").value(),&sz);
    sOffMain = sOffset;
    sOffAdd1 = sOffset;
    sOffAdd2 = sOffset;
    for (pugi::xml_node_iterator sB = cA.begin(); sB != cA.end(); ++sB)
    {
        if (sB->attribute("id").as_int() == mainRoad.attribute("id").as_int()) sOffMain = stod(sB->attribute("sOffset").value(),&sz);
        
        if (sB->attribute("id").as_int() == additionalRoad1.attribute("id").as_int()) sOffAdd1 = stod(sB->attribute("sOffset").value(),&sz);
        
        if (sB->attribute("id").as_int() == additionalRoad2.attribute("id").as_int()) sOffAdd2 = stod(sB->attribute("sOffset").value(),&sz);
    }
    
    // calculate s and phi at intersection
    sMain = stod(iP.attribute("s").value(),&sz);

    if(mode >= 1)
    {
        tmpNode = iP.child("adRoad");
        sAdd1 = stod(tmpNode.attribute("s").value(),&sz);
        phi1 = stod(tmpNode.attribute("angle").value(),&sz);
    }

    if(mode >= 2)
    {
        sAdd2 = stod(tmpNode.next_sibling("adRoad").attribute("s").value(),&sz);
        phi2 = stod(tmpNode.next_sibling("adRoad").attribute("angle").value(),&sz);
    }

    // calculate intersectionPoint
    road helperRoad;
    generateRoad(mainRoad, helperRoad, sMain, 0, 0, 0, 0, 0);
    double iPx = helperRoad.geometries.back().x;
    double iPy = helperRoad.geometries.back().y;
    double iPhdg = helperRoad.geometries.back().hdg;

    cout << "Generating Roads" << endl;

    cout << "Road 1" << endl;
    road r1;
    r1.id = 100*junc.id + 1;
    r1.junction = junc.id;
    if (mode == 1)
    {
        r1.predecessor.elementId =mainRoad.attribute("idStart").as_int();
        r1.successor.elementId = 100*junc.id + 4;
        generateRoad(mainRoad, r1, sMain, -sOffMain, 50, 0, 0, 0);
    }
    if (mode == 2)
    {
        r1.predecessor.elementId = 100*junc.id + 4;
        r1.successor.elementId = mainRoad.attribute("idEnd").as_int();
        generateRoad(mainRoad, r1, sMain, sOffMain, 0, 0, 0, 0);
    }
    data.roads.push_back(r1);

    cout << "Road 2" << endl;
    road r2;
    r2.id = 100*junc.id + 2;
    r2.junction = junc.id;
    if (mode == 1)
    {
        r2.predecessor.elementId = 100*junc.id + 4;
        r2.successor.elementId = mainRoad.attribute("idEnd").as_int();
        generateRoad(mainRoad, r2, sMain, sOffMain, 50, 0, 0, 0);
    }
    if (mode == 2)
    {
        r2.predecessor.elementId = 100*junc.id + 4;
        r2.successor.elementId = additionalRoad1.attribute("idEnd").as_int();
        generateRoad(additionalRoad1, r2, sAdd1, sOffAdd1, 0, iPhdg+phi1, iPx, iPy);
    }
    data.roads.push_back(r2);

    cout << "Road 3" << endl;
    road r3; 
    r3.id = 100*junc.id + 3;
    r3.junction = junc.id;
    if (mode == 1)
    {
        r3.predecessor.elementId = 100*junc.id + 6;
        r3.successor.elementId = additionalRoad1.attribute("idEnd").as_int();
        generateRoad(additionalRoad1, r3, sAdd1, sOffAdd1, 0, iPhdg+phi1, iPx, iPy);
    }
    if (mode == 2)
    {
        r3.predecessor.elementId = 100*junc.id + 6;
        r3.successor.elementId = additionalRoad2.attribute("idEnd").as_int();
        generateRoad(additionalRoad2, r3, sAdd2, sOffAdd2, 0, iPhdg+phi2, iPx, iPy);
    }
    data.roads.push_back(r3);

    cout << "Generating Connecting Roads" << endl;
    
    if((string)con.attribute("type").value() == "all")
    {
        double phi;

        cout << "Road 4" << endl;
        road r4; 
        r4.id = 100*junc.id + 4;

        phi = r2.geometries.front().hdg - r1.geometries.front().hdg;
        fixAngle(phi);
        fixAngle(phi1);
        if (mode == 1 && phi1 > 0) 
            createRoadConnection(r1,r2,r4,junc,all,all,d,s,d);
        if (mode == 1 && phi1 < 0) 
            createRoadConnection(r1,r2,r4,junc,all,all,s,d,d);
        if (mode == 2 && phi < 0) 
            createRoadConnection(r1,r2,r4,junc,all,all,n,s,n);
        if (mode == 2 && phi > 0) 
            createRoadConnection(r1,r2,r4,junc,all,all,s,n,n);
        data.roads.push_back(r4);

        cout << "Road 5" << endl;
        road r5; 
        r5.id = 100*junc.id + 5;
        phi = r3.geometries.front().hdg - r2.geometries.front().hdg - M_PI;
        fixAngle(phi);
        if (phi < 0) createRoadConnection(r2,r3,r5,junc,all,all,n,s,n);
        if (phi > 0) createRoadConnection(r2,r3,r5,junc,all,all,s,n,n);
        data.roads.push_back(r5);

        cout << "Road 6" << endl;
        road r6; 
        r6.id = 100*junc.id + 6;
        phi = r1.geometries.front().hdg - r3.geometries.front().hdg;
        fixAngle(phi);
        if (phi < 0) createRoadConnection(r3,r1,r6,junc,all,all,n,s,n);
        if (phi > 0) createRoadConnection(r3,r1,r6,junc,all,all,s,n,n);
        data.roads.push_back(r6);
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
                    left = laneLink.attribute("right").value();

                if (laneLink.attribute("middle")) 
                    middle = laneLink.attribute("middle").value();

                cout << "Road " << data.roads.size() + 1 << endl;
                road r;
                r.id = 100*junc.id + data.roads.size() + 1;
                createRoadConnection(r1,r2,r,junc,from,to,left,right,middle);
                data.roads.push_back(r);
            }
        }
    }

    data.junctions.push_back(junc);     

    return 0;
}