// file laneSectionChange.h

/**
 * @brief function adds a laneSection with laneWiding  to a given lanesection set in s direction
 * 
 * @param secs      vector of all lanesections of a road
 * @param laneId    laneId of the lane where the laneWiding should be performed
 * @param s         position of lane widing
 * @param ds        length of lane widing
 * @param addouterLane  specifies if additional lane is on the outer side or not
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

    // shift other lanes and add additional lane
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
 * @brief function adds a laneSection with laneDrop  to a given lanesection set in s direction
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



/**
 * @brief function adds a laneSection with restricted area to a given lanesection set in s direction
 * 
 * @param secs      vector of all lanesections of a road
 * @param laneId    laneId of the lane where the laneDrop should be performed
 * @param s         position of laneDrop
 * @param ds        length of laneDrop
 * @param sign      id of signNumber
 * @return int      errorcode
 */
int addRestrictedArea(vector<laneSection> &secs, int laneId, double s1, double s2, double ds1, double ds2, int sign)
{
    std::vector<laneSection>::iterator it;
    std::vector<lane>::iterator itt;

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size()-1; i++)
    {
        if (secs[i].s <= s1 && secs[i+1].s > s1) {
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
    double w = laneWidth(l,0);
    l.w.a = w - a;
    l.w.b = - b; 
    l.w.c = - c; 
    l.w.d = - d; 

    // type
    l.type = "restricted";
    l.id = sgn(laneId) + laneId;
    adLaneSec.lanes.push_back(l);

    secs[i] = adLaneSec;

    // --- Section 2 -----------------------------------------
    adLaneSec.id++;
    adLaneSec.s = s2;

    // shift coordinates of droppingLane
    id = findLane(adLaneSec, l, laneId);
    dx = s1 + ds1 - s2;
    l.w.d = d;
    l.w.c = 3 * dx * d + c;
    l.w.b = 3 * pow(dx,2) * d + 2 * dx * c + b;
    l.w.a = pow(dx,3) * d + pow(dx,2) * c + pow(dx,1) * b + a;
    adLaneSec.lanes[id] = l;

    // addtional lane: original polynom of additional lane - droppingLane
    id = findLane(adLaneSec, lTmp, laneId + sgn(laneId));
    lTmp.w.a = w - l.w.a;
    lTmp.w.b = 0 - l.w.b;
    lTmp.w.c = -3 * w / pow(ds2,2) - l.w.c;
    lTmp.w.d = 2 * w / pow(ds2,3) - l.w.d;
    
    // type
    lTmp.type = "restricted";
    adLaneSec.lanes[id] = lTmp;

    it++;
    secs.insert(it, adLaneSec);

    // --- Section 3 -----------------------------------------
    adLaneSec.id++;
    adLaneSec.s = s1 + ds1;

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
    a = w; b = 0; c = -3 * w / pow(ds2,2); d = 2 * w / pow(ds2,3);
    
    // shift original polynom
    l.w.a = pow(dx,3) * d + pow(dx,2) * c + pow(dx,1) * b + a;
    l.w.b = 3 * pow(dx,2) * d + 2 * dx * c + b;
    l.w.c = 3 * dx * d + c;
    l.w.d = d;

    // type
    l.type = "restricted";
    adLaneSec.lanes[id] = l;

    it++;
    secs.insert(it, adLaneSec);

    // --- Section 4 -----------------------------------------
    adLaneSec.id++;
    adLaneSec.s = s2 + ds2;

    // remove additional lane 
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
    if(laneId > 0) id = findMaxLaneId(adLaneSec);
    if(laneId < 0) id = findMinLaneId(adLaneSec);
    id = findLane(adLaneSec, l, id);
    adLaneSec.lanes[id].rm.type = "solid";

    // --- make center line dashed ---------------------------------------------
    id = findLane(adLaneSec, l, 0);
    adLaneSec.lanes[id].rm.type = "broken";

    it++;
    it = secs.insert(it, adLaneSec);

    it++; 
    secs.erase(it);

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
 * @brief function creates laneWidenings in Junction areas (in opposite s direction)
 * 
 * @param r 
 * @param sLaneWidening 
 * @param turn 
 * @return int 
 */
int laneWideningJunction(road &r, int sLaneWidening, int turn, bool verschwenkung)
{
    /*
        sLaneWidening > 0 -> laneWidening
        sLaneWidening < 0 -> restricted Area

        turn = 1  -> left lane
        turn = -1 -> right lane

        verschwenkung   -> original lane stays constant, additional lane with 3rd order polynom
        !verschwenkung  -> original lane splits in two new lanes (without marking), which increases to 0.75 of original lane (no offset shift)
    */

    // if restricted area, always on left side (turn = 1)
    if (sLaneWidening < 0) turn = 1;

    vector<laneSection>::iterator it = r.laneSections.begin();

    laneSection adLaneSec = *it;
    adLaneSec.s = 0;
    adLaneSec.id = 1;

    // additional lane
    lane l;
    int laneId = 0;

    
    if (turn ==  1) 
    {
        laneId = findLeftLane(*it,1);
        if (laneId == 0) laneId = findInnerMiddleLane(*it,1);
    }
    if (turn == -1) 
    {
        laneId = findRightLane(*it,1);
        if (laneId == 0) laneId = findOuterMiddleLane(*it,1);
    }

    int id = findLane(adLaneSec, l, laneId);
    double w = laneWidth(l,0);

    l.w.a = w;
    
    if (sLaneWidening > 0 && turn == 1)
    { 
        l.rm.type = "broken";
        l.type = "driving";
        l.turn = 1;
    }
    if (sLaneWidening > 0 && turn == -1)
    { 
        adLaneSec.lanes[id].rm.type = "broken";
        l.rm.type = "solid";
        l.type = "driving";
        l.turn = -1;
    }
    if (sLaneWidening < 0)
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
        shiftLanes(adLaneSec,laneId,1);
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
        adLaneSec.o.a = -abs(w)/2 + it->o.a;

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
    if(verschwenkung)
    {
        l.w.d = 2 * w / pow(20,3);
        l.w.c = - 3 * w / pow(20,2);
        l.w.b = 0;
    }
    else{
        l.w.a = 0.75 * w;
        l.w.b = - 0.0125 * w;
        l.w.c = 0;
        l.w.d = 0;

        id = findLane(adLaneSec, lTmp, laneId + turn);
        adLaneSec.lanes[id].w = l.w;

        if (turn ==  1) l.rm.type = "none";
        if (turn == -1) adLaneSec.lanes[id].rm.type = "none";
    }

    if (turn == 1 && verschwenkung)
    {
        adLaneSec.o.a = - abs(w)/2 + it->o.a;
        adLaneSec.o.b = 0;
        adLaneSec.o.c = 3 * abs(w)/2 / pow(20,2);
        adLaneSec.o.d = - 2 * abs(w)/2 / pow(20,3);
    }

    adLaneSec.id++;
    adLaneSec.s = abs(sLaneWidening);
    id = findLane(adLaneSec, lTmp, laneId);
    adLaneSec.lanes[id] = l;

    // update lane links
    for (int i = 0; i < adLaneSec.lanes.size(); i++)
    {
        int id = adLaneSec.lanes[i].id;
        adLaneSec.lanes[i].preId = id;

        lane tmpLane;
        int idL = findLane(*it, tmpLane, id);

        if (idL == -1)
        {
            cout << idL << endl;
            cout << id << endl;
            cout << turn << endl;
            
        }

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
        it->s += 20 + abs(sLaneWidening);
    }
    return 0;
}