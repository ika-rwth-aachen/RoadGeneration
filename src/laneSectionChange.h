// file laneSectionChange.h

/**
 * @brief function adds a laneSection with laneWiding  to a given lanesection set
 * 
 * @param secs      vector of all lanesections of a road
 * @param laneId    laneId of the lane where the laneWiding should be performed
 * @param s         position of lane widing
 * @param ds        length of lane widing
 * @return int      errorcode
 */
int addLaneWidening(vector<laneSection> &secs, int laneId, double s, double ds, bool busStop)
{ 
    std::vector<laneSection>::iterator it;

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size()-1; i++)
    {
        if (secs[i].s < s && secs[i+1].s >= s) {
            found = true;
            it = secs.begin() + i;
            break; 
        }
    }
    if (!found) {
        it = secs.begin() + secs.size()-1;
        i = secs.size()-1;
    }

    laneSection adLaneSec = *it;

    adLaneSec.id = it->id + 100;
    adLaneSec.s = s;

    // additional road in adLaneSec
    lane l;
    findLane(adLaneSec, l, laneId);
    double w = laneWidth(l,adLaneSec.s);
    l.w.d = - 2 * w / pow(ds,3);
    l.w.c = 3 * w / pow(ds,2);
    l.w.b = 0;
    l.w.a = 0;
    l.rm.type = "broken";

    // shift other lanes and add lane
    if (busStop)
    {
        l.type = "bus";
        l.rm.type = "solid";
        l.id = laneId + sgn(laneId);
        adLaneSec.lanes.push_back(l);  
    }
    else
    {
        shiftLanes(adLaneSec, laneId, 1);
        adLaneSec.lanes.push_back(l);  
    }

    // center line solid
    lane tmp;
    int id = findLane(adLaneSec, tmp, 0);
    adLaneSec.lanes[id].rm.type = "solid";

    it++;
    it = secs.insert(it, adLaneSec);
    it++;


    // adjust the sections after the laneWiding
    l.w.d = 0;
    l.w.c = 0;
    l.w.a = w;
    adLaneSec.lanes.back() = l;
    adLaneSec.s += ds;

    // center line broken
    id = findLane(adLaneSec, tmp, 0);
    adLaneSec.lanes[id].rm.type = "broken";

    it = secs.insert(it, adLaneSec);
    it++;
    i += 2;

    // shift all lanes in following lane sections
    for (; it != secs.end(); ++it)
    {
        if(!busStop)
        {
            shiftLanes(secs[i], laneId, 1);
            it->lanes.push_back(l); 
        }
    }

    return 0;
}

/**
 * @brief function adds a laneSection with laneDrop  to a given lanesection set
 * 
 * @param secs      vector of all lanesections of a road
 * @param laneId    laneId of the lane where the laneDrop should be performed
 * @param s         position of laneDrop
 * @param ds        length of laneDrop
 * @return int      errorcode
 */
int addLaneDrop(vector<laneSection> &secs, int laneId, double s, double ds, bool busStop)
{
    std::vector<laneSection>::iterator it;
    std::vector<lane>::iterator itt;

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size()-1; i++)
    {
        if (secs[i].s < s && secs[i+1].s >= s) {
            found = true;
            it = secs.begin() + i;
            break; 
        }
    }
    if (!found) {
        it = secs.begin() + secs.size()-1;
        i = secs.size()-1;
    }

    laneSection adLaneSec = *it;

    adLaneSec.id = it->id + 100;
    adLaneSec.s = s;

    // additional road in adLaneSec
    lane l;
    int id = findLane(adLaneSec, l, laneId);
    double w = laneWidth(l,adLaneSec.s);
    l.w.d = 2 * w / pow(ds,3);
    l.w.c = - 3 * w / pow(ds,2);
    l.w.b = 0;
    l.w.a = w;
    adLaneSec.lanes[id] = l;

    // center line solid
    id = findLane(adLaneSec, l, 0);
    adLaneSec.lanes[id].rm.type = "solid";

    it++;
    it = secs.insert(it, adLaneSec);
    it++;
    adLaneSec.s += ds;
    adLaneSec.lanes[id].rm.type = "broken";

    it = secs.insert(it, adLaneSec);

    if (busStop)
    {
        int id = findLane(*it, l, laneId);
        itt = it->lanes.begin() + id;
        it->lanes.erase(itt); 

        return 0;
    }

    // shift all lanes in following lane sections
    for (; it != secs.end(); ++it)
    {        
        int id = findLane(*it, l, laneId);

        shiftLanes(*it, laneId, -1);

        itt = it->lanes.begin() + id;
        it->lanes.erase(itt); 
    }

    return 0;
}
