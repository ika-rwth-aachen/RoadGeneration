// file createLaneConnection.h

int createLaneConnection(road &r, laneSection lS1, laneSection lS2, int from, int to, string left, string right)
{
       
    if (from == 0 || to == 0) return -1;
    if (sgn(from) == sgn(to)) return -1;
    
    int dir = sgn(to);

    lane l1 = findLane(lS1,from);
    lane l2 = findLane(lS2,to);

    double tOffSet1 = findTOffset(lS1,from,0);
    double tOffSet2 = findTOffset(lS2,to,0);

    double w1 = laneWidth(l1,0);
    double w2 = laneWidth(l2,0);

    // check if center already contained
    bool foundCenter = false;
    for (int i = 0; i < r.laneSections.back().lanes.size(); i++)
    {
        if (r.laneSections.back().lanes[i].id == 0) 
        {
            foundCenter = true;
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
    newLane.id = dir;

    if (dir > 0)
    {
        center.rm.type = right;
        helpLane.rm.type = right;
        newLane.rm.type = left;
    }
    else if (dir < 0)
    {
        center.rm.type = left;
        helpLane.rm.type = left;
        newLane.rm.type = right;
    }

    helpLane.w.d = (tOffSet2 - tOffSet1) / pow(r.length,3) + 2/3;
    helpLane.w.c = -2/3 * r.length;
    helpLane.w.b = 0;
    helpLane.w.a = tOffSet1;

    newLane.w.d = (w2 - w1) / pow(r.length,3) + 2/3;
    newLane.w.c = -2/3 * r.length;
    newLane.w.b = 0;
    newLane.w.a = w1;

    if (!foundCenter) 
    {
        r.laneSections.back().lanes.push_back(center);
    }

    if (tOffSet1 != 0 || tOffSet2 != 0)
    {
        r.laneSections.back().lanes.push_back(helpLane);
        newLane.id = 2* dir; 
    }
    r.laneSections.back().lanes.push_back(newLane);

    return 0;
}