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
int addLaneWidening(vector<laneSection> &secs, int laneId, double s, double ds, bool addOuterLane)
{ 
    std::vector<laneSection>::iterator it;
    int i = 0;

    // search corresponding lane Section
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

    adLaneSec.id = it->id + 1;
    adLaneSec.s = s;

    // find current lane in adLaneSec
    lane l;
    int id = findLane(adLaneSec, l, laneId);

    // adjust new width
    double w = laneWidth(l,adLaneSec.s);
    l.w.d = - 2 * w / pow(ds,3);
    l.w.c = 3 * w / pow(ds,2);
    l.w.b = 0;
    l.w.a = 0;

    // shift other lanes and add addtional lane
    if (addOuterLane)
    {
        l.id = laneId + sgn(laneId);
        shiftLanes(adLaneSec, l.id, 1);

        adLaneSec.lanes.push_back(l);  
        adLaneSec.lanes[id].rm.type = "broken";
    }
    else
    {
        shiftLanes(adLaneSec, laneId, 1);

        l.rm.type = "broken";
        adLaneSec.lanes.push_back(l);  
    }

    // center line solid in laneWiding part
    lane tmp;
    id = findLane(adLaneSec, tmp, 0);
    adLaneSec.lanes[id].rm.type = "solid";

    it++;
    i++;
    it = secs.insert(it, adLaneSec);


    // --- adjust the section after the laneWiding ----------------------------
    l.w.d = 0;
    l.w.c = 0;
    l.w.a = w;
    adLaneSec.lanes.back() = l;

    adLaneSec.id++;
    adLaneSec.s += ds;

    // center line broken after laneWiding part
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

        if(addOuterLane)
        {
            shiftLanes(secs[i], laneId+sgn(laneId), 1);

            if (abs(l.id) <= abs(laneId + sgn(laneId))) l.rm.type = "broken";
            it->lanes.push_back(l); 
        }
        else
        {
            shiftLanes(secs[i], laneId, 1);
            if (abs(l.id) <= abs(laneId)) l.rm.type = "broken";
            it->lanes.push_back(l); 
        }
        i++;
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
int addLaneDrop(vector<laneSection> &secs, int laneId, double s, double ds)
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

    adLaneSec.id = it->id + 1;
    adLaneSec.s = s;

    // find current lane in adLaneSec
    lane l;
    int id = findLane(adLaneSec, l, laneId);

    // adjust new width
    double w = laneWidth(l,adLaneSec.s);
    l.w.d = 2 * w / pow(ds,3);
    l.w.c = - 3 * w / pow(ds,2);
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
