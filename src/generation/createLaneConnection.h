/**
 * @file createLaneConnection.h
 *
 * @brief file contains method for generating lane connection in junction area
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

extern settings setting;

/**
 * @brief function creates a new lane for a connecting road
 * 
 * @param r         connecting road
 * @param lS1       lanesection of adjacent road at start 
 * @param lS2       lanesection of adjacent road at end
 * @param from      start lane Id
 * @param to        end lane Id
 * @param left      left roadmarking 
 * @param right     right roadmarking
 * @return int      error code
 */
int createLaneConnection(road &r, laneSection lS1, laneSection lS2, int from, int to, string left, string right)
{
    if (from == 0 || to == 0)
    {
        cerr << "ERR: cannot connect lane Id 0." << endl;
        return 1;
    }

    if (from > findMaxLaneId(lS1) || from < findMinLaneId(lS1))
    {
        cerr << "ERR: cannot connect lane Id from:" << from << endl;
        return 1;
    }

    if (to > findMaxLaneId(lS2) || to < findMinLaneId(lS2))
    {
        cerr << "ERR: cannot connect lane Id to:" << to << endl;
        return 1;
    }

    int dir = sgn(to);

    lane l1;
    lane l2;
    findLane(lS1, l1, from);
    findLane(lS2, l2, to);

    // calculate tOffsets
    double tOffSet1 = abs(findTOffset(lS1, from - sgn(from), 0));
    double tOffSet2 = abs(findTOffset(lS2, to - sgn(to), 0));

    // calculate widths
    double w1 = laneWidth(l1, 0);
    double w2 = laneWidth(l2, 0);

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

    lane newLane;
    newLane.id = dir;
    newLane.speed = setting.speed.access;
    newLane.preId = from;
    newLane.sucId = to;

    if (dir > 0)
    {
        center.rm.type = right;
        newLane.rm.type = left;
    }
    else if (dir < 0)
    {
        center.rm.type = left;
        newLane.rm.type = right;
    }

    // width of newLane
    newLane.w.d = -2 * (w2 - w1) / pow(r.length, 3);
    newLane.w.c = 3 * (w2 - w1) / pow(r.length, 2);
    newLane.w.b = 0;
    newLane.w.a = w1;

    // add lanes to laneSection
    if (!foundCenter)
    {
        r.laneSections.back().lanes.push_back(center);
    }

    r.laneSections.back().lanes.push_back(newLane);

    return 0;
}