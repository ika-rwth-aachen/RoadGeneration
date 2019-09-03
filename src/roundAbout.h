// roundAbout.h

/**
 * @brief function generates the roads and junctions for a roundabout which is specified in the input file
 *  
 * @param node  input data from the input file for the roundAbout
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  errorcode
 */
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
    if (!mainRoad)
    {
        cerr << "ERR: mainRoad is not found.";
        return 1;
    }

    // store properties of mainRoad
    double length = mainRoad.child("referenceLine").child("geometry").attribute("length").as_double();
    double R = length / (2 * M_PI);
    mainRoad.child("referenceLine").child("geometry").append_attribute("R") = R;

    double sOld;
    road rOld;

    bool clockwise;
    if (R > 0) clockwise = false;
    if (R < 0) clockwise = true;
    if (abs(R) < 3)
    {
        cerr << "ERR: radius of reference road in a roundabout have to be larger than 3.";
        return 1;
    }

    // get coupler
    pugi::xml_node coupler = node.child("coupler");
    if (!coupler)
    {
        cerr << "ERR: coupler is not defined.";
        return 1;
    }

    // count intersectionPoints
    int nIp = 0;
    for (pugi::xml_node iP: node.children("intersectionPoint")) nIp++;
   
    int cc = 0;
    // iterate over all additonalRoads defined by separate intersectionPoints 
    // sMain of intersection points have to increase
    for (pugi::xml_node iP: node.children("intersectionPoint"))
    {   
        cc++;

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
            cerr << "ERR: specified road in intersection" << cc << " is not found.";
            return 1;
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

        // calculate width of mainRoad and addtionalRoad
        road helpMain;
        generateRoad(mainRoad, helpMain, 0, INFINITY, 0, 0, 0, 0, 0);  
        laneSection lSMain = helpMain.laneSections.front();
        double widthMain = abs(findTOffset(lSMain, findMinLaneId(lSMain), 0));
        
        road helpAdd;
        generateRoad(additionalRoad, helpAdd, 0, INFINITY, 0, 0, 0, 0, 0);  
        laneSection lSAdd = helpAdd.laneSections.front();
        double widthAdd = abs(findTOffset(lSAdd, findMinLaneId(lSAdd), 0)) +                      abs(findTOffset(lSAdd, findMaxLaneId(lSAdd), 0));
        
        // check offsets and adjust them if necessary
        bool (changed) = false;
        if (sOffMain < widthAdd/2 * 1.25) 
        {sOffMain = widthAdd/2 * 1.25; changed = true;}
        if (sOffAdd < widthMain   * 1.25) 
        {sOffAdd = widthMain * 1.25; changed = true;}        
        
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
        double sMain = iP.attribute("s").as_double() * length;
        double sAdd = iP.child("adRoad").attribute("s").as_double();
        double phi = iP.child("adRoad").attribute("angle").as_double();

        // calculate coordinates of intersectionPoint
        double curPhi = sMain / (2*M_PI*R) * 2 * M_PI; 
        double iPx = R * cos(curPhi-M_PI/2);
        double iPy = R * sin(curPhi-M_PI/2);
        double iPhdg = curPhi;
    
        // --- generate roads --------------------------------------------------
        cout << "\t Generating Roads" << endl;
            /*           
                    \___       ____/
                 id: 1         id: helper
               \_______        _______/    
                       |      |
                       | id:2 |
                       |      |
        */

        road r1;
        r1.id = 100*junc.id + cc * 10 + 1;
        r1.junction = junc.id;
        r1.successor.elementId = junc.id;
        r1.successor.elementType = "junction";
        if (cc == 1) sOld = sOffMain;
        generateRoad(mainRoad, r1, sOld, sMain-sOffMain, 0, sMain, iPx, iPy,iPhdg);        

        road r2;
        r2.id = 100*junc.id + cc * 10 + 2;
        r2.junction = junc.id;
        r2.predecessor.elementId = junc.id;
        r2.predecessor.elementType = "junction";
        generateRoad(additionalRoad, r2, sAdd+sOffAdd, INFINITY, 0, sAdd, iPx, iPy, iPhdg+phi); 
        addObjects(additionalRoad, r2, data);

        // add signal to outgoing roads
        addSignal(r2, data, 1, INFINITY, "1.000.001", "-");

        road helper;
        helper.id = r1.id + cc * 10 + 3; 
        helper.junction = junc.id;
        if (cc < nIp)
            generateRoad(mainRoad, helper, sMain+sOffMain, sMain+2*sOffMain, 0, sMain, iPx, iPy,iPhdg);  
        else // last segment
        {
            helper = rOld;
            helper.successor.elementType = "road";
            helper.successor.elementId = -101;
        }
        helper.predecessor.elementId = junc.id;
        helper.predecessor.elementType = "junction";

        // --- generate connecting lanes ---------------------------------------
        cout << "\t Generate Connecting Lanes" << endl;

        // max and min id's of laneSections
        int inner1, outer1, outer3, inner3, nLane;
        
        if (clockwise)
        {
            outer1 = findMaxLaneId(r1.laneSections.back());
            inner1 = sgn(outer1);

            outer3 = findMaxLaneId(helper.laneSections.back());
            inner3 = sgn(outer3);

            nLane = outer1 - inner1 + 1;
        } 
        if (!clockwise)
        {
            outer1 = findMinLaneId(r1.laneSections.back());
            inner1 = sgn(outer1);

            outer3 = findMinLaneId(helper.laneSections.back());
            inner3 = sgn(outer3);
            nLane = inner1 - outer1 + 1;
        } 

        int max2 = findMaxLaneId(r2.laneSections.front());
        int min2 = findMinLaneId(r2.laneSections.front());

        int from, to;

        road r3; 
        r3.id = 100*junc.id + cc * 10 + 4;
        from = inner1;
        to = inner3;
        if (clockwise)
            createRoadConnection(r1,helper,r3,junc,from,to,bro,sol,bro);
        if (!clockwise)
            createRoadConnection(r1,helper,r3,junc,from,to,sol,bro,bro);

        road r4; 
        r4.id = 100*junc.id + cc * 10 + 5;
        from = outer1;
        to = outer3;
        if (clockwise)
            createRoadConnection(r1,helper,r4,junc,from,to,bro,bro,bro);
        if (!clockwise)
            createRoadConnection(r1,helper,r4,junc,from,to,bro,bro,bro);

        road r5; 
        r5.id = 100*junc.id + cc * 10 + 6;
        if (clockwise){
            from = outer1;
            to = findLeftLane(max2);
            createRoadConnection(r1,r2,r5,junc,from,to,sol,non,non);
        }
        if (!clockwise){
            from = outer1;
            to = findRightLane(min2);
            createRoadConnection(r1,r2,r5,junc,from,to,non,sol,non);
        }

        road r6; 
        r6.id = 100*junc.id + cc * 10 + 7;
        if (clockwise){
            from = findLeftLane(min2);
            to = outer3;
            createRoadConnection(r2,helper,r6,junc,from,to,sol,non,non);
        }
        if (!clockwise){
            from = findRightLane(max2);
            to = outer3;
            createRoadConnection(r2,helper,r6,junc,from,to,non,sol,non);
        }

        // adjust precessor of first element, due to loop
        r1.predecessor.elementId = junc.id;
        r1.predecessor.elementType = "junction";

        data.roads.push_back(r1);
        data.roads.push_back(r2);
        data.roads.push_back(r3);
        if (nLane > 1) data.roads.push_back(r4);
        data.roads.push_back(r5);
        data.roads.push_back(r6);

        // update for next step
        sOld = sMain + sOffMain;
        if (cc == 1) rOld = r1;
    }

    data.junctions.push_back(junc);     

    return 0;
}