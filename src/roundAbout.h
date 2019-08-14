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

    double R = mainRoad.child("referenceLine").child("geometry").attribute("R").as_double();
    double sOld = node.last_child().child("couplerArea").attribute("sOffset").as_double();
    road rOld;

    bool clockwise;
    if (R > 0) clockwise = false;
    if (R < 0) clockwise = true;
    if (R == 0) exit(0);

    // count intersectionPoints
    int cc = 0;
    for (pugi::xml_node iP: node.children("intersectionPoint")) cc++;
    int nIp = cc;

    cc = 0;
    // iteration over all additonalRoads defined by separate intersectionPoints
    for (pugi::xml_node iP: node.children("intersectionPoint"))
    {   
        cc++;

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
        double iPy = R * sin(curPhi-M_PI/2);
        double iPhdg = curPhi;

        road r1;
        r1.id = 100*junc.id + cc * 10 + 1;
        r1.junction = junc.id;
        r1.successor.elementId = junc.id;
        r1.successor.elementType = "junction";
        generateRoad(mainRoad, r1, sOld, sMain-sOffMain, 0, sMain, iPx, iPy,iPhdg);        

        road r2;
        r2.id = 100*junc.id + cc * 10 + 2;
        r2.junction = junc.id;
        r2.predecessor.elementId = junc.id;
        r2.predecessor.elementType = "junction";
        generateRoad(additionalRoad, r2, sAdd+sOffAdd, INFINITY, 0, sAdd, iPx, iPy, iPhdg+phi);        

        road helper;
        helper.id = r1.id + cc * 10 + 3; 
        helper.junction = junc.id;
        if (cc < nIp)
            generateRoad(mainRoad, helper, sMain+sOffMain, sMain+2*sOffMain, 0, sMain, iPx, iPy,iPhdg);  
        else 
        {
            helper = rOld;
            helper.successor.elementType = "road";
            helper.successor.elementId = -101;
        }
        helper.predecessor.elementId = junc.id;
        helper.predecessor.elementType = "junction";

        // --- add connecting lanes --------------------------------------------

        // max and min id's of laneSections
        int max1 = findMaxLaneId(r1.laneSections.back());
        int min1 = findMinLaneId(r1.laneSections.back());

        int max2 = findMaxLaneId(r2.laneSections.front());
        int min2 = findMinLaneId(r2.laneSections.front());

        int max3 = findMaxLaneId(helper.laneSections.front());
        int min3 = findMinLaneId(helper.laneSections.front());

        int from, to;

        road r3; 
        r3.id = 100*junc.id + cc * 10 + 4;
        from = findRightLane(max1);
        to = findRightLane(max3);
        if (clockwise)
            createRoadConnection(r1,helper,r3,junc,from,to,d,d,d);
        if (!clockwise)
            createRoadConnection(r1,helper,r3,junc,from,to,s,d,d);

        road r4; 
        r4.id = 100*junc.id + cc * 10 + 5;
        from = findRightLane(min1);
        to = findRightLane(min3);
        if (clockwise)
            createRoadConnection(r1,helper,r4,junc,from,to,d,s,d);
        if (!clockwise)
            createRoadConnection(r1,helper,r4,junc,from,to,d,d,d);

        road r5; 
        r5.id = 100*junc.id + cc * 10 + 6;
        if (clockwise){
            from = findLeftLane(max1);
            to = findLeftLane(max2);
            createRoadConnection(r1,r2,r5,junc,from,to,s,n,n);
        }
        if (!clockwise){
            from = findRightLane(min1);
            to = findRightLane(min2);
            createRoadConnection(r1,r2,r5,junc,from,to,n,s,n);
        }

        road r6; 
        r6.id = 100*junc.id + cc * 10 + 7;
        if (clockwise){
            from = findLeftLane(min2);
            to = findLeftLane(max3);
            createRoadConnection(r2,helper,r6,junc,from,to,s,n,n);
        }
        if (!clockwise){
            from = findRightLane(max2);
            to = findRightLane(min3);
            createRoadConnection(r2,helper,r6,junc,from,to,n,s,n);
        }

        // adjust precessor of first element
        r1.predecessor.elementId = junc.id;
        r1.predecessor.elementType = "junction";

        data.roads.push_back(r1);
        data.roads.push_back(r2);
        data.roads.push_back(r3);
        data.roads.push_back(r4);
        data.roads.push_back(r5);
        data.roads.push_back(r6);

        // update for next step
        sOld = sMain + sOffMain;
        if (cc == 1) rOld = r1;
        
    }
    data.junctions.push_back(junc);     

    return 0;
}