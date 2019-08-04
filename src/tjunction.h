// tjunction.h

int tjunction(pugi::xml_node &node, roadNetwork &data)
{
    // check type (all roads start in intersection point)
    int mode = 0; 
    if ((string)node.attribute("type").value() == "M1A") mode = 1;
    if ((string)node.attribute("type").value() == "3A")  mode = 2;
    if (mode == 0) cout << "ERR:" << endl;

    // create junction
    junction junc;
    junc.id = node.attribute("id").as_int();

    pugi::xml_node iP = node.child("intersectionPoint");
    pugi::xml_node cA = node.child("coupler").child("couplerArea");

    // define junction roads
    pugi::xml_node referenceRoad;
    pugi::xml_node additionalRoad1;
    pugi::xml_node additionalRoad2; 

    pugi::xml_node tmpNode = iP.child("adRoad");

    for (pugi::xml_node road: node.children("road"))
    {
        if(road.attribute("id").as_int() == iP.attribute("refId").as_int())
            referenceRoad = road;

        if(mode >= 1 &&  road.attribute("id").as_int() == tmpNode.attribute("id").as_int())
            additionalRoad1 = road;

        if(mode >= 2 && road.attribute("id").as_int() == tmpNode.next_sibling("adRoad").attribute("id").as_int())
            additionalRoad2= road;
    }   

    if(!referenceRoad || !additionalRoad1 || (mode == 2 && !additionalRoad2)) 
        cout << "ERR: no corresponding roads are found" << endl;

    // cut original roads
    double sOffset = stod(cA.attribute("sOffset").value(),&sz);

    double sMain, sAccess1, sAccess2, phi1, phi2;
    
    sMain = stod(iP.attribute("s").value(),&sz);

    if(mode >= 1)
    {
        tmpNode = iP.child("adRoad");
        sAccess1 = stod(tmpNode.attribute("s").value(),&sz);
        phi1 = stod(tmpNode.attribute("angle").value(),&sz);
    }

    if(mode >= 2)
    {
        sAccess2 = stod(tmpNode.next_sibling("adRoad").attribute("s").value(),&sz);
        phi2 = stod(tmpNode.next_sibling("adRoad").attribute("angle").value(),&sz);
    }

    // calculate intersectionPoint
    road helperRoad;
    generateRoad(referenceRoad, helperRoad, sMain, 0, 0, 0, 0);
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
        r1.predecessor.elementId =referenceRoad.attribute("idStart").as_int();
        r1.successor.elementId = 100*junc.id + 4;
        generateRoad(referenceRoad, r1, sMain, -sOffset, 0, 0, 0);
    }
    if (mode == 2)
    {
        r1.predecessor.elementId = 100*junc.id + 4;
        r1.successor.elementId = referenceRoad.attribute("idEnd").as_int();
        generateRoad(referenceRoad, r1, sMain, sOffset, 0, 0, 0);
    }
    data.roads.push_back(r1);


    cout << "Road 2" << endl;
    road r2;
    r2.id = 100*junc.id + 2;
    r2.junction = junc.id;
    if (mode == 1)
    {
        r2.predecessor.elementId = 100*junc.id + 4;
        r2.successor.elementId = referenceRoad.attribute("idEnd").as_int();
        generateRoad(referenceRoad, r2, sMain, sOffset, 0, 0, 0);
    }
    if (mode == 2)
    {
        r2.predecessor.elementId = 100*junc.id + 4;
        r2.successor.elementId = additionalRoad1.attribute("idEnd").as_int();
        generateRoad(additionalRoad1, r2, sAccess1, sOffset, iPhdg+phi1, iPx, iPy);
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
        generateRoad(additionalRoad1, r3, sAccess1, sOffset, iPhdg+phi1, iPx, iPy);
    }
    if (mode == 2)
    {
        r3.predecessor.elementId = 100*junc.id + 6;
        r3.successor.elementId = additionalRoad2.attribute("idEnd").as_int();
        generateRoad(additionalRoad2, r3, sAccess2, sOffset, iPhdg+phi2, iPx, iPy);
    }
    data.roads.push_back(r3);

    cout << "Generating Connecting Roads" << endl;
    
    cout << "Road 4" << endl;
    road r4; 
    r4.id = 100*junc.id + 4;
    r4.junction = junc.id;
    r4.predecessor.elementId = r1.id;
    r4.successor.elementId   = r2.id;
    if (mode == 1) 
    {
        int laneId;
        if (phi1 > M_PI) laneId = r1.laneSections.back().lanes.back().id;
        if (phi1 < M_PI) laneId = r1.laneSections.back().lanes.front().id;
        createRoadConnection(r1,r2,r4,junc,2,laneId);
    }
    if (mode == 2) 
    {
        createRoadConnection(r1,r2,r4,junc,2,0);        
    }
    data.roads.push_back(r4);

    cout << "Road 5" << endl;
    road r5; 
    r5.id = 100*junc.id + 5;
    r5.junction = junc.id;
    r5.predecessor.elementId = r3.id;
    r5.successor.elementId   = r2.id;
    createRoadConnection(r3,r2,r5,junc,2,0);     
    data.roads.push_back(r5);

    cout << "Road 6" << endl;
    road r6; 
    r6.id = 100*junc.id + 6;
    r6.junction = junc.id;
    r6.predecessor.elementId = r1.id;
    r6.successor.elementId   = r3.id;
    createRoadConnection(r1,r3,r6,junc,2,0);
    data.roads.push_back(r6);

    data.junctions.push_back(junc);     

    return 0;
}