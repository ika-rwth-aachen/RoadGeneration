// file helper.h

/**
 * @brief function fixes the input angle in the range of [-M_PI, M_PI]
 * 
 * @param a     angle which should be fixed
 * @return int  errorcode
 */
int fixAngle(double &a)
{
    a = fmod(a + 2*M_PI, 2 * M_PI);
    if (a > M_PI) a = a - 2 *M_PI; 

    return 0;
}

/**
 * @brief function find the minimum laneId of current lanesection
 * 
 * @param sec   lanesection for which the laneId should be computed
 * @return int  minimum laneId -> if not found value 100 is stored
 */
int findMinLaneId(laneSection sec)
{
    int min = 100;

    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id < min) min = sec.lanes[i].id;
    }
    return min;
}

/**
 * @brief function find the maximum laneId of current lanesection
 * 
 * @param sec   lanesection for which the laneId should be computed
 * @return int  maximum laneId -> if not found value -100 is stored
 */
int findMaxLaneId(laneSection sec)
{
    int max = -100;

    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id > max) max = sec.lanes[i].id;
    }
    return max;
}

/**
 * @brief function determines of given laneId is at the boundary of the current lanesection
 * 
 * @param sec       lanesection for which the boundary should be proofed
 * @param id        laneid for which the boundary should be proofed
 * @return true     if the lane is a boundary-lane
 * @return false    if the lane is not a boundary-lane
 */
bool isBoundary(laneSection sec, int id)
{
    bool res = false;

    if (id == findMaxLaneId(sec)) res = true;
    if (id == findMinLaneId(sec)) res = true;
    
    return res;
}

/**
 * @brief function returns the lane with the given id
 * 
 * @param sec   lanesection in which the lane is stored
 * @param l     lane which has the laneId id
 * @param id    laneId of the lane to find
 * @return int  position in laneSection vector of the lane to find
 */
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

/**
 * @brief function determines lanewidth of the given lane at positon s
 * 
 * @param l         lane for which the lanewidth should be computed
 * @param s         position where the lanewidth should be computed
 * @return double   computed lanewidth
 */
double laneWidth(lane l, double s)
{
    return l.w.a + l.w.b * s + l.w.c * s * s + l.w.d * s * s * s;
}

/**
 * @brief function computes the tOfset of a given lane inside of a lanesection at position s
 * 
 * @param sec       lanesection for which the tOffset should be computed
 * @param id        laneId of the lane with the tOffset
 * @param s         s position of the tOffset
 * @return double   computed tOffset
 */
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

/**
 * @brief signum function
 * 
 * @param d         input 
 * @return int      -1, 0, 1
 */
int sgn(double d)
{
    int r = 0;
    if (d > 0) r = 1;
    if (d < 0) r = -1;

    return r;
}

/**
 * @brief function shifts all lanes in given lanesection from lane with laneId
 * 
 * @param sec   lanesection which should be shifted
 * @param id    lane which is the start of the shift; all outer lanes are shifted one
 * @return int  errorcode 
 */
int shiftLanes(laneSection &sec, int id, int dir2)
{
    int dir = sgn(id) * sgn(dir2);
    
    if (id > 0 && dir2 > 0) id = findMaxLaneId(sec);
    else if (id < 0 && dir2 > 0) id = findMinLaneId(sec);

    if (id > 0 && dir2 < 0) id = sgn(id);
    else if (id < 0 && dir2 < 0) id = sgn(id);

    int search = 0;
    if (dir2 < 0 && id > 0) search = findMaxLaneId(sec)+sgn(id);
    if (dir2 < 0 && id < 0) search = findMinLaneId(sec)+sgn(id);
    while(id != search)
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

/**
 * @brief function finds the id of the left lane
 * 
 * @param i     input laneId
 * @return int  laneId of left lane
 */
int findLeftLane(int i){
    if (i > 0)
        return 1;
    else if (i < 0)
        return -1;
    else 
        return 0;
}

/**
 * @brief function finds the id of the right lane
 * 
 * @param i     input laneId
 * @return int  laneId of right lane
 */
int findRightLane(int i){
    if (i > 0)
        return i;
    else if (i < 0)
        return i;
    else 
        return 0;
}

/**
 * @brief function finds the id of the middle lane
 * 
 * @param i     input laneId
 * @return int  laneId of middle lane
 */
int findMiddleLane(int i){
    if (i > 0)
        return floor(i/2+1);
    else if (i < 0)
        return ceil(i/2-1);
    else 
        return 0;
}

/**
 * @brief function sorts the roads r2,r3,r4 to their corresponding angle
 * 
 * @param r1    reference road with reference angle
 * @param r2    first additional road
 * @param r3    second additional road
 * @param r4    third additional road
 * @return int  errorcode
 */
int sortRoads(road r1, road &r2, road &r3, road &r4)
{
    double phi1 = r2.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    double phi2 = r3.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    double phi3 = r4.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
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
        cerr << "ERR: angles at intersection point are not defined correct.";
        return 1;
    }
    return 0;
}

/**
 * @brief function sorts the roads r2,r3 to their corresponding angle
 * 
 * @param r1    reference road with reference angle
 * @param r2    first additional road
 * @param r3    second additional road
 * @return int  errorcode
 */
int sortRoads(road r1, road &r2, road &r3)
{
    double phi1 = r2.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    double phi2 = r3.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    fixAngle(phi1);
    fixAngle(phi2);

    road tmpR2 = r2;
    road tmpR3 = r3;

    if (phi1 < phi2)
    {
        r2 = tmpR2;
        r3 = tmpR3;
    }
    else if (phi2 < phi1)
    {
        r2 = tmpR3;
        r3 = tmpR2;
    }
    else 
    {
        cerr << "ERR: angles at intersection point are not defined correct.";
        return 1;
    }
    return 0;
}

int computeIP(double x1,double y1,double phi1,double x2,double y2,double phi2,int &type, int &type1, int &type2,double &iPx,double &iPy)
{
    double t2 = (y1-y2+tan(phi1)*(x2-x1)) / (sin(phi2)-tan(phi1)*cos(phi2));
    double t1 = (x2 - x1 + cos(phi2) * t2) / cos(phi1);

    if (t1 > 0)  type1 = 1;
    if (t1 == 0) type1 = 0;
    if (t1 < 0)  type1 = -1;

    if (t2 > 0)  type2 = 1;
    if (t2 == 0) type2 = 0;
    if (t2 < 0)  type2 = -1;

        
    iPx = x1 + t1 * cos(phi1);
    iPy = y1 + t1 * sin(phi1);

    return 0;
}