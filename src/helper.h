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

lane findLane(laneSection sec, int id)
{
    lane l;
    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id == id) 
        {
            l = sec.lanes[i];
            break;
        }
    }
    return l;
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
        lane l = findLane(sec,cur);
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