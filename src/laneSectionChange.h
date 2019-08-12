// file laneSectionChange.h

int addLaneWidening(vector<laneSection> &secs, int laneId, double s, double ds)
{ 
    cout << "Begin LaneWidening" << endl;
    std::vector<laneSection>::iterator it;

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size()-1; i++)
    {
        if (secs[i].s < s && secs[i+1].s >= s) {
            found = true;
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

    shiftLanes(adLaneSec, laneId);
    adLaneSec.lanes.push_back(l);  

    it++;
    it = secs.insert(it, adLaneSec);
    it++;

    // part after laneWiding
    l.w.d = 0;
    l.w.c = 0;
    l.w.a = w;
    adLaneSec.lanes.back() = l;
    adLaneSec.s += ds;

    it = secs.insert(it, adLaneSec);
    it++;
    i += 2;

    // shift all lanes in following lane sections
    for (; it != secs.end(); ++it)
    {
        shiftLanes(secs[i], laneId);
        it->lanes.push_back(l); 
    }

    return 0;
}

int addLaneDrop(vector<laneSection> &secs, int laneId, double s, double ds)
{
    cout << "Begin LaneDrop" << endl;
    std::vector<laneSection>::iterator it;

    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < secs.size()-1; i++)
    {
        if (secs[i].s < s && secs[i+1].s >= s) {
            found = true;
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
    l.rm.type = "broken";
    adLaneSec.lanes[id] = l;

    it++;
    it = secs.insert(it, adLaneSec);
    it++;

    // part after laneDrop
    l.w.d = 0;
    l.w.c = 0;
    l.w.a = 0;
    adLaneSec.lanes[id] = l;
    adLaneSec.s += ds;

    it = secs.insert(it, adLaneSec);
    it++;
    i += 2;

    // shift all lanes in following lane sections
    for (; it != secs.end(); ++it)
    {        
        int id = findLane(*it, l, laneId);
        it->lanes[id] = l; 
    }

    return 0;
}
