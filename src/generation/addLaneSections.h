/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file addLaneSections.h
 *
 * @brief file contains methodology for lanesection changes such as lanedrop, lane widening or restricted lanes
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

/**
 * @brief function adds a laneSection with laneWideing to a given lanesection set
 * 
 * @param secs          vector of all lanesections of a road
 * @param side          determines the road's side of the widening
 * @param s             position of lane widening
 * @param ds            length of lane widening
 * @param addouterLane  specifies if additional lane is on the outer side or not
 * @return int          error code
 */
int addLaneWidening(vector<laneSection> &secs, int addLaneId, double s, double ds, bool addOuterLane)
{
    std::vector<laneSection>::iterator it;
    int i = 0;


    // search corresponding lane Section
    bool found = false;
    int n = secs.size();
    for (i = n - 1; i >= 0; i--)
    {
        if (secs[i].s <= s)
        {
            found = true;
            it = secs.begin() + i;
            break;
        }
    }
    if (!found)
    {
        it = secs.begin() + secs.size() - 1;
        i = secs.size() - 1;
    }

    laneSection adLaneSec = *it;

    int max_lane_id = 0;
    for(lane l: adLaneSec.lanes)
    {
        if(addLaneId * l.id > max_lane_id)
            max_lane_id = addLaneId * l.id > max_lane_id;
    }

    int laneId = 0;

   
    //laneId = sgn(addLaneId);
    laneId = addLaneId; //changed this so the user can define the exact position where a new lane should get added!

    if (laneId == 0 || abs(laneId) >= 100)
    {
        cerr << "ERR: lane widening can not be performed" << endl;
        return 1;
    }

    adLaneSec.id = it->id + 1;
    adLaneSec.s = s;

    // find current lane in adLaneSec
    lane l;
    l.id = laneId;
    int id = findLane(adLaneSec, l, laneId);

    // adjust new width
    double w = laneWidth(l, 0);
    l.w.d = -2 * w / pow(ds, 3);
    l.w.c = 3 * w / pow(ds, 2);
    l.w.b = 0;
    l.w.a = 0;

    // shift other lanes and add additional lane
    shiftLanes(adLaneSec, laneId, 1);

    l.rm.type = "broken";
    adLaneSec.lanes.push_back(l);

    // center line solid in laneWidening part
    lane tmp;
    id = findLane(adLaneSec, tmp, 0);
    adLaneSec.lanes[id].rm.type = "solid";

    it++;
    i++;
    it = secs.insert(it, adLaneSec);

    // --- adjust the section after the laneWidening ---------------------------
    l.w.d = 0;
    l.w.c = 0;
    l.w.a = w;
    adLaneSec.lanes.back() = l;

    adLaneSec.id++;
    adLaneSec.s += ds;

    // center line broken after laneWidening part
    id = findLane(adLaneSec, tmp, 0);
    adLaneSec.lanes[id].rm.type = "broken";

    it++;
    i++;
    it = secs.insert(it, adLaneSec);

    it++;
    i++;

    // --- shift all lanes in following lane sections --------------------------
    for (; it != secs.end(); ++it)
    {
        secs[i].id += 2;

            shiftLanes(secs[i], laneId, 1);
            if (abs(l.id) <= abs(laneId))
                l.rm.type = "broken";
            it->lanes.push_back(l);
        i++;
    }

    return 0;
}

/**
 * @brief function adds a laneSection with laneDrop to a given lanesection set
 * 
 * @param secs      vector of all lanesections of a road
 * @param side      determines the road's side of the drop
 * @param s         position of laneDrop
 * @param ds        length of laneDrop
 * @return int      error code
 */
int addLaneDrop(vector<laneSection> &secs, int side, double s, double ds)
{
    std::vector<laneSection>::iterator it;
    std::vector<lane>::iterator itt;

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    int n = secs.size();
    for (i = n - 1; i >= 0; i--)
    {
        if (secs[i].s <= s)
        {
            found = true;
            it = secs.begin() + i;
            break;
        }
    }
    if (!found)
    {
        it = secs.begin() + secs.size() - 1;
        i = secs.size() - 1;
    }

    laneSection adLaneSec = *it;

    int laneId = 0;
    if (side > 0)
        laneId = findMaxLaneId(adLaneSec);
    if (side < 0)
        laneId = findMinLaneId(adLaneSec);

    if (laneId == 0 || abs(laneId) >= 100)
    {
        cerr << "ERR: lane drop can not be performed" << endl;
    }

    adLaneSec.id = it->id + 1;
    adLaneSec.s = s;

    // find current lane in adLaneSec
    lane l;
    int id = findLane(adLaneSec, l, laneId);
    if (id == -1)
        return 1;

    // adjust new width
    double w = laneWidth(l, adLaneSec.s);
    l.w.d = 2 * w / pow(ds, 3);
    l.w.c = -3 * w / pow(ds, 2);
    l.w.b = 0;
    l.w.a = w;

    adLaneSec.lanes[id] = l;

    // center line solid in laneDropping part
    id = findLane(adLaneSec, l, 0);
    adLaneSec.lanes[id].rm.type = "solid";

    it++;
    it = secs.insert(it, adLaneSec);

    // --- adjust the section after the laneDropping
    adLaneSec.id++;
    adLaneSec.s += ds;
    adLaneSec.lanes[id].rm.type = "broken";

    // remove lane
    id = findLane(adLaneSec, l, laneId);
    int idTmp = findLane(adLaneSec, l, laneId - sgn(laneId));

    shiftLanes(adLaneSec, laneId, -1);

    // adjust the correct lane type
    adLaneSec.lanes[idTmp].rm.type = adLaneSec.lanes[id].rm.type;

    itt = adLaneSec.lanes.begin() + id;
    adLaneSec.lanes.erase(itt);

    it++;
    it = secs.insert(it, adLaneSec);

    it++;

    // --- shift all lanes in following lane sections --------------------------
    for (; it != secs.end(); ++it)
    {
        int id = findLane(*it, l, laneId);

        shiftLanes(*it, laneId, -1);

        itt = it->lanes.begin() + id;
        it->lanes.erase(itt);
    }

    return 0;
}

/**
 * @brief function adds a laneSection with restricted area after a lane widening
 * 
 * @param secs      vector of all lanesections of a road
 * @param side      determines the road's side of the widening
 * @param s         position of lane widening
 * @param ds1       length of lane widening
 * @param ds2       length of restricted areay (always lager than ds1)
 * @return int      error code
 */
int addRestrictedAreaWidening(vector<laneSection> &secs, int side, double s, double ds1, double ds2)
{
    std::vector<laneSection>::iterator it;
    std::vector<lane>::iterator itt;

    if (ds1 >= ds2)
    {
        cerr << "ERR: length in restricted area is to short." << endl;
        return 1;
    }

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size() - 1; i++)
    {
        if (secs[i].s <= s && secs[i + 1].s > s)
        {
            found = true;
            it = secs.begin() + i;
            break;
        }
    }
    if (!found)
    {
        it = secs.begin() + secs.size() - 1;
        i = secs.size() - 1;
    }

    laneSection adLaneSec = *it;

    int laneId = 0;
    if (side > 0)
        laneId = findMaxLaneId(adLaneSec);
    if (side < 0)
        laneId = findMinLaneId(adLaneSec);

    if (laneId == 0 || abs(laneId) >= 100)
    {
        cerr << "ERR: restricted area widening can not be performed" << endl;
    }

    lane l, lTmp;
    int id;
    double dx;

    // skip if previous part is too small
    if (s - secs[i - 1].s < ds2 - ds1)
        return 1;

    // --- Section 1 -----------------------------------------
    id = findLane(adLaneSec, l, sgn(side));

    double a = l.w.a;
    double b = l.w.b;
    double c = l.w.c;
    double d = l.w.d;

    // addtional lane: w - droppingLane
    double w = laneWidth(l, ds1);
    l.w.a = w - a;
    l.w.b = -b;
    l.w.c = -c;
    l.w.d = -d;

    // type
    l.type = "restricted";
    l.rm.type = "solid";
    l.rm.color = "white";
    l.id = sgn(side) + laneId;
    adLaneSec.lanes.push_back(l);

    secs[i] = adLaneSec;

    // --- Section 2 -----------------------------------------
    adLaneSec.s = s - (ds2 - ds1);

    // zero droppingLane
    id = findLane(adLaneSec, l, sgn(side));
    l.w.a = 0;
    l.w.b = 0;
    l.w.c = 0;
    l.w.d = 0;
    adLaneSec.lanes[id] = l;

    // additional lane: shifted coordinates of original polynom
    id = findLane(adLaneSec, l, laneId + sgn(side));

    // polynom
    w = laneWidth(l, 0);
    l.w.d = -2 * w / pow(ds2 - ds1, 3);
    l.w.c = 3 * w / pow(ds2 - ds1, 2);
    l.w.b = 0;
    l.w.a = 0;

    // type
    l.type = "restricted";
    l.rm.type = "solid";
    l.rm.color = "white";
    adLaneSec.lanes[id] = l;

    secs.insert(it, adLaneSec);

    return 0;
}

/**
 * @brief function adds a laneSection with restricted area after a lane drop
 * 
 * @param secs      vector of all lanesections of a road
 * @param side      determines the road's side of the drop
 * @param s         position of lane drop
 * @param ds1       length of lane drop
 * @param ds2       length of restricted areay (always lager than ds1)
 * @return int      error code
 */
int addRestrictedAreaDrop(vector<laneSection> &secs, int side, double s, double ds1, double ds2)
{
    std::vector<laneSection>::iterator it;
    std::vector<lane>::iterator itt;

    if (ds1 >= ds2)
    {
        cerr << "ERR: length in restricted area is to short." << endl;
        return 1;
    }

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size() - 1; i++)
    {
        if (secs[i].s <= s && secs[i + 1].s > s)
        {
            found = true;
            it = secs.begin() + i;
            break;
        }
    }
    if (!found)
    {
        it = secs.begin() + secs.size() - 1;
        i = secs.size() - 1;
    }

    laneSection adLaneSec = *it;

    int laneId = 0;
    if (side > 0)
        laneId = findMaxLaneId(adLaneSec);
    if (side < 0)
        laneId = findMinLaneId(adLaneSec);

    if (laneId == 0 || abs(laneId) >= 100)
    {
        cerr << "ERR: restriced area drop can not be performed" << endl;
    }

    lane l, lTmp;
    int id;
    double dx;

    // --- Section 1 -----------------------------------------
    id = findLane(adLaneSec, l, laneId);

    double a = l.w.a;
    double b = l.w.b;
    double c = l.w.c;
    double d = l.w.d;

    // addtional lane: w - droppingLane
    double w = laneWidth(l, 0);
    l.w.a = w - a;
    l.w.b = -b;
    l.w.c = -c;
    l.w.d = -d;

    // type
    l.type = "restricted";
    l.id = sgn(laneId) + laneId;
    adLaneSec.lanes.push_back(l);

    secs[i] = adLaneSec;

    // --- Section 2 -----------------------------------------
    adLaneSec.id++;
    adLaneSec.s = s + ds1;

    // zero droppingLane
    id = findLane(adLaneSec, l, laneId);
    l.w.a = 0;
    l.w.b = 0;
    l.w.c = 0;
    l.w.d = 0;
    adLaneSec.lanes[id] = l;

    // additional lane: shifted coordinates of original polynom
    id = findLane(adLaneSec, l, laneId + sgn(laneId));

    // original polynoam

    w = laneWidth(l, ds1);
    l.w.d = 2 * w / pow(ds2 - ds1, 3);
    l.w.c = -3 * w / pow(ds2 - ds1, 2);
    l.w.b = 0;
    l.w.a = w;

    // type
    l.type = "restricted";
    adLaneSec.lanes[id] = l;

    it++;
    secs.insert(it, adLaneSec);

    // --- Section 3 -----------------------------------------
    adLaneSec.id++;
    adLaneSec.s = s + ds2;

    // remove restricted lane
    id = findLane(adLaneSec, l, laneId + sgn(laneId));
    shiftLanes(adLaneSec, laneId + sgn(laneId), -1);
    itt = adLaneSec.lanes.begin() + id;
    adLaneSec.lanes.erase(itt);

    // remove droppingLane
    id = findLane(adLaneSec, l, laneId);
    shiftLanes(adLaneSec, laneId, -1);
    itt = adLaneSec.lanes.begin() + id;
    adLaneSec.lanes.erase(itt);

    // --- make outer boundary solid -------------------------------------------
    if (laneId > 0)
        id = findMaxLaneId(adLaneSec);
    if (laneId < 0)
        id = findMinLaneId(adLaneSec);
    id = findLane(adLaneSec, l, id);
    adLaneSec.lanes[id].rm.type = "solid";

    // --- make center line dashed ---------------------------------------------
    id = findLane(adLaneSec, l, 0);
    adLaneSec.lanes[id].rm.type = "broken";

    it++;
    it = secs.insert(it, adLaneSec);

    it++;
    secs.erase(it);

    return 0;
}

/**
 * @brief function creates laneWidenings in junction areas (in opposite s direction), for additional turning lanes 
 * 
 * @param r                 road data
 * @param s                 start of widening
 * @param ds                 length of widening
 * @param turn              1 = left lane turn, -1 = right lane turn
 * @param verschwenkung     determines if a 'verschwenkung' exists
 * @param restricted        determines if widening is a restricted area
 * @return int              error code
 */
int laneWideningJunction(road &r, double s, double ds, int turn, bool verschwenkung, bool restricted)
{
    /*
        s > 0 -> laneWidening
        s < 0 -> restricted Area

        turn = 1  -> left lane
        turn = -1 -> right lane

        verschwenkung   -> original lane stays constant, additional lane with 3rd order polynom
        !verschwenkung  -> original lane splits in two new lanes (without marking), which increases to 0.75 of original lane (no offset shift)
    */

    // if restricted area, always on left side (turn = 1)
    if (restricted)
        turn = 1;

    vector<laneSection>::iterator it = r.laneSections.begin();

    laneSection adLaneSec = *it;
    adLaneSec.s = 0;
    adLaneSec.id = 1;

    // additional lane
    lane l;
    int laneId = 0;

    if (turn == 1)
    {
        laneId = findLeftLane(*it, 1);
        if (laneId == 0)
            laneId = findInnerMiddleLane(*it, 1);
    }
    if (turn == -1)
    {
        laneId = findRightLane(*it, 1);
        if (laneId == 0)
            laneId = findOuterMiddleLane(*it, 1);
    }

    int id = findLane(adLaneSec, l, laneId);
    double w = laneWidth(l, 0);

    l.w.a = w;

    if (s > 0 && turn == 1)
    {
        l.rm.type = "broken";
        l.type = "driving";
        l.turnLeft = true;
        l.turnStraight = false;
    }
    if (s > 0 && turn == -1)
    {
        adLaneSec.lanes[id].rm.type = "broken";
        l.rm.type = "solid";
        l.type = "driving";
        l.turnRight = true;
        l.turnStraight = false;
    }
    if (restricted)
    {
        l.rm.type = "solid";
        l.type = "restricted";
    }

    if (!verschwenkung)
    {
        l.w.a *= 0.75;
        adLaneSec.lanes[id].w.a = l.w.a;
    }

    if (turn == 1)
    {
        shiftLanes(adLaneSec, laneId, 1);
        adLaneSec.lanes.push_back(l);
    }
    if (turn == -1)
    {
        laneId += sgn(laneId);
        l.id = laneId;
        adLaneSec.lanes.push_back(l);
    }

    // solid center line
    lane lTmp;
    id = findLane(adLaneSec, lTmp, 0);
    adLaneSec.lanes[id].rm.type = "solid";

    // laneOffset
    if (turn == 1 && verschwenkung)
        adLaneSec.o.a = -abs(w) / 2 + it->o.a;

    // update lane links
    for (int i = 0; i < adLaneSec.lanes.size(); i++)
    {
        adLaneSec.lanes[i].preId = 0;
        adLaneSec.lanes[i].sucId = adLaneSec.lanes[i].id;
    }

    // insert lane section
    it = r.laneSections.insert(it, adLaneSec);
    it++;

    // ---------------------------------------------------------------------
    // widening lane
    if (verschwenkung)
    {
        l.w.d = 2 * w / pow(ds, 3);
        l.w.c = -3 * w / pow(ds, 2);
        l.w.b = 0;
    }
    else
    {
        l.w.a = 0.75 * w;
        l.w.b = -0.25 / ds * w;
        l.w.c = 0;
        l.w.d = 0;

        id = findLane(adLaneSec, lTmp, laneId + turn);
        adLaneSec.lanes[id].w = l.w;

        if (turn == 1)
            l.rm.type = "none";
        if (turn == -1)
            adLaneSec.lanes[id].rm.type = "none";
    }

    if (turn == 1 && verschwenkung)
    {
        adLaneSec.o.a = -abs(w) / 2 + it->o.a;
        adLaneSec.o.b = 0;
        adLaneSec.o.c = 3 * abs(w) / 2 / pow(ds, 2);
        adLaneSec.o.d = -2 * abs(w) / 2 / pow(ds, 3);
    }

    adLaneSec.id++;
    adLaneSec.s = s;
    id = findLane(adLaneSec, lTmp, laneId);
    adLaneSec.lanes[id] = l;

    // update lane links
    for (int i = 0; i < adLaneSec.lanes.size(); i++)
    {
        int id = adLaneSec.lanes[i].id;
        adLaneSec.lanes[i].preId = id;

        lane tmpLane;
        int idL = findLane(*it, tmpLane, id);

        if (sgn(id) == sgn(laneId))
        {
            if (turn == 1)
            {
                adLaneSec.lanes[i].sucId = id - sgn(id);
                if (idL != -1)
                    it->lanes[idL].preId = id + sgn(id);
            }
            if (turn == -1)
            {
                adLaneSec.lanes[i].sucId = id;
                if (idL != -1)
                    it->lanes[idL].preId = id;
            }
            if (turn == -1 && id == laneId)
            {
                adLaneSec.lanes[i].sucId = 0;
            }
        }
        else
        {
            adLaneSec.lanes[i].sucId = id;
            it->lanes[idL].preId = id;
        }
    }

    // update lane links in next lane section
    it = r.laneSections.insert(it, adLaneSec);
    it++;

    // --- shift all lanes in following lane sections --------------------------
    for (; it != r.laneSections.end(); ++it)
    {
        it->id += 2;
        it->s += s + ds;
    }
    return 0;
}