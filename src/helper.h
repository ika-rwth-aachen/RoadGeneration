// file helper.h

int fixAngle(double &a)
{
    a = fmod(a + 2*M_PI, 2 * M_PI);
    if (a > M_PI) a = a - 2 *M_PI; 

    return 0;
}

int findMinLaneId(laneSection sec)
{
    int min = 100;

    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id < min) min = sec.lanes[i].id;
    }
    return min;
}

int findMaxLaneId(laneSection sec)
{
    int max = -100;

    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id > max) max = sec.lanes[i].id;
    }
    return max;
}

bool isBoundary(laneSection sec, int id)
{
    bool res = false;

    if (id == findMaxLaneId(sec)) res = true;
    if (id == findMinLaneId(sec)) res = true;
    
    return res;
}

int findLane(laneSection sec, lane &l, int id)
{
    int i = -100;

    for (i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id == id) 
        {
            l = sec.lanes[i];
            break;
        }
    }
    return i;
}

double laneWidth(lane l, double s)
{
    return l.w.a + l.w.b * s + l.w.c * s * s + l.w.d * s * s * s;
}

double findTOffset(laneSection sec, int id, double s)
{
    double tOffset = 0;
    int dir = 0;
    int cur = 0;

    if (id < 0) dir = -1;
    if (id > 0) dir =  1;

    while (cur != id)
    {
        lane l;
        int err = findLane(sec, l, cur);
        s += l.w.s;
        tOffset += dir * laneWidth(l,s);
        cur += dir;
    }

    return tOffset;
}

int sgn(double d)
{
    int r = 0;
    if (d > 0) r = 1;
    if (d < 0) r = -1;

    return r;
}

int shiftLanes(laneSection &sec, int id)
{
    int dir = sgn(id);
    
    if (id > 0) id = findMaxLaneId(sec);
    else if (id < 0) id = findMinLaneId(sec);

    while(id != 0)
    {
        for (int i = 0; i < sec.lanes.size(); i++)
        {
            if (sec.lanes[i].id == id) 
            {
                sec.lanes[i].id += dir;
            }
        }
        id -= dir;
    }

    return 0;
}

