// file createLaneConnection.h

int createLaneConnection(road &r, laneSection lS1, laneSection lS2, int from, int to, string left, string right)
{
       
    if (from == 0 || to == 0) {
        cout << "ERR: cannot connect lane Id 0." << endl;
    }
    
    int dir = sgn(to);

    lane l1;
    lane l2;
    findLane(lS1, l1, from);
    findLane(lS2, l2, to);

    double tOffSet1 = abs(findTOffset(lS1,from,0));
    double tOffSet2 = abs(findTOffset(lS2,to,0));

    double w1 = laneWidth(l1,0);
    double w2 = laneWidth(l2,0);

    // check if center already contained in laneSection
    bool foundCenter = false;
    for (int i = 0; i < r.laneSections.back().lanes.size(); i++)
    {
        if (r.laneSections.back().lanes[i].id == 0) 
        {
            foundCenter = true;
            r.laneSections.back().lanes[i].rm.type = "none";
            break;
        }
    }
    
    lane center;
    center.id = 0;
    center.type = "none";
    center.w.a = 0;

    lane helpLane;
    helpLane.id = dir;
    helpLane.type = "none";
    
    lane newLane;
    newLane.id = 2 * dir;

    if (dir > 0)
    {
        center.rm.type = "none";
        helpLane.rm.type = right;
        newLane.rm.type = left;
    }
    else if (dir < 0)
    {
        center.rm.type = "none";
        helpLane.rm.type = left;
        newLane.rm.type = right;
    }

    // width of helplane
    helpLane.w.d = -2 * (tOffSet2 - tOffSet1) / pow(r.length,3);
    helpLane.w.c =  3 * (tOffSet2 - tOffSet1) / pow(r.length,2);
    helpLane.w.b = 0;
    helpLane.w.a = tOffSet1;

    // width of newLane
    newLane.w.d = -2 * (w2 - w1) / pow(r.length,3);
    newLane.w.c =  3 * (w2 - w1) / pow(r.length,2);
    newLane.w.b = 0;
    newLane.w.a = w1;

    if (!foundCenter) 
    {
        r.laneSections.back().lanes.push_back(center);
    }

    r.laneSections.back().lanes.push_back(helpLane);
    
    r.laneSections.back().lanes.push_back(newLane);

    return 0;
}