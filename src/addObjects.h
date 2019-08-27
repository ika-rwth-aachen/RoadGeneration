// file addObjects.h

/**
 * @brief function generates a roadwork
 * 
 * @param o         object containing length and position of roadwork   
 * @param r         road which contains the roadwork
 * @param laneId    lane which contains the roadwork
 * @return int      errorcode
 */
int addRoadWork(object o, road &r, int laneId)
{
    std::vector<laneSection>::iterator it;
    
    // search corresponding lane Section
    int i = 0;
    bool found = false;
    for (i = 0; i < r.laneSections.size()-1; i++)
    {
        if (r.laneSections[i].s <= o.s && r.laneSections[i+1].s > o.s) {
            found = true;
            it = r.laneSections.begin() + i;
            break; 
        }
    }
    if (!found) {
        it = r.laneSections.begin() + r.laneSections.size()-1;
        i = r.laneSections.size()-1;
    }

    if (i == r.laneSections.size()-1 || r.laneSections[i+1].s-it->s > o.len)
    {
        laneSection adLaneSec = *it;
        adLaneSec.id++;
        adLaneSec.s = o.s;

        it++;
        i++;
        r.laneSections.insert(it, adLaneSec);

        lane l;
        int id = findLane(r.laneSections[i], l, laneId);
        r.laneSections[i].lanes[id].type = "roadWorks";

        adLaneSec.id++;
        adLaneSec.s += o.len;

        it++;
        i++;
        r.laneSections.insert(it, adLaneSec);
    }
    else
    {
        cerr << "Length of roadwork is longer than  laneSection." << endl;
        return 1;
    }
    
    // shift remaining sections
    i++;
    for (; i < r.laneSections.size(); i++)
    {
        r.laneSections[i].id++;
    }
}

/**
 * @brief function creates objects like parking spots, traffic signs or markings
 * 
 * @param inRoad    road data from input file
 * @param r         resulting road with additional objects
 * @param data      roadNetwork structure where the generated roads and junctions are stored
 * @return int      errorcode
 */
int addObjects(pugi::xml_node inRoad, road &r, roadNetwork &data)
{
    for (pugi::xml_node obj: inRoad.child("objects").children())
    {
        std::string type = obj.name();
        object o;

        o.id = obj.attribute("id").as_double();

        if (obj.child("absolutePosition")) 
        {
            pugi::xml_node position = obj.child("absolutePosition");
            cerr << "Absolute Position is not implemented." << endl;
            return 1;
        }
        if (obj.child("relativePosition")) 
        {
            pugi::xml_node position = obj.child("relativePosition");
            o.s = position.attribute("s").as_double();
            o.t = position.attribute("t").as_double();
            o.hdg = position.attribute("hdg").as_double();
        }
        if (obj.child("repeatPosition")) 
        {
            pugi::xml_node position = obj.child("repeatPosition");
            o.s = position.attribute("s").as_double();
            o.t = position.attribute("t").as_double();
            o.hdg = position.attribute("hdg").as_double();
                        
            o.repeat = true;
            o.len = position.attribute("length").as_double();
        }
        
        // --- consider different object cases ---------------------------------
        if (type == "parkingSpace")
        {

            o.type = type;
            o.length = obj.attribute("length").as_double();
            o.width = obj.attribute("width").as_double();
            o.height = 4;

            // find laneSection
            int i;
            for (i = r.laneSections.size()-1; i >= 0; i--)
                if (r.laneSections[i].s < o.s) break;

            laneSection lS = r.laneSections[i];

            int laneId;
            if (o.t > 0) laneId = findMaxLaneId(lS);
            if (o.t < 0) laneId = findMinLaneId(lS);
            double t = findTOffset(lS,laneId,o.s);
            fixAngle(o.hdg);

            if (abs(o.hdg) > M_PI/4)
            {
                o.t = t + sgn(laneId) * abs(o.length / (2 * sin(o.hdg)));
                o.distance = abs(o.width / sin(o.hdg));
            }
            if (abs(o.hdg) < M_PI/4)
            {
                o.t = t + sgn(laneId) * abs(o.width / (2 * cos(o.hdg)));
                o.distance = abs(o.length / cos(o.hdg));
            }
            r.objects.push_back(o);
        }

        if (type == "streetLamp")
        {
            o.type = type;
            o.distance = 20;
            r.objects.push_back(o);
        }

        if (type == "roadWork")
        {
            o.s = obj.attribute("s").as_double();
            o.len = obj.attribute("length").as_double();
            int laneId = obj.attribute("laneId").as_int();

            addRoadWork(o, r, laneId);
        }

        if (type == "busStop")
        {
            // find laneSection
            int i;
            for (i = r.laneSections.size()-1; i >= 0; i--)
                if (r.laneSections[i].s < o.s) break;
            laneSection lS = r.laneSections[i];

            int laneId;
            if (o.t > 0) laneId = findMaxLaneId(lS);
            if (o.t < 0) laneId = findMinLaneId(lS);

            addLaneWidening(r.laneSections, laneId, o.s-12.5, 7.5, true);
            addLaneDrop (r.laneSections, laneId+sgn(laneId), o.s+5, 7.5);

            lane l;
            int id; 
            id = findLane(r.laneSections[i+1],l,laneId+sgn(laneId));
            r.laneSections[i+1].lanes[id].type = "bus";
            id = findLane(r.laneSections[i+2],l,laneId+sgn(laneId));
            r.laneSections[i+2].lanes[id].type = "bus";
            id = findLane(r.laneSections[i+3],l,laneId+sgn(laneId));
            r.laneSections[i+3].lanes[id].type = "bus";
        }

        if (type == "trafficIsland")
        {
            o.type = type;
            o.s = obj.attribute("s").as_double();
            o.t = 0;
            o.length = obj.attribute("length").as_double();
            o.width = obj.attribute("width").as_double();
            r.objects.push_back(o);
        }

        if (type == "marking")
        {
            // TODO: depends on type
        }

        //--- SIGNALS ----------------------------------------------------------
        if (type != "trafficRule") continue;

        control c;
        c.id = obj.attribute("id").as_int();

        for (pugi::xml_node ob: obj.children())
        {
            signal s; 
            s.rule = c.id;
            s.id = ob.attribute("id").as_int();
            s.type = ob.attribute("type").value();
            s.value = ob.attribute("value").as_double();
            s.dynamic = ob.attribute("dynamic").as_bool();
            s.height = 2;
            s.width = 0.4;
            s.s = ob.child("relativePosition").attribute("s").as_double();
            s.t = ob.child("relativePosition").attribute("t").as_double();
            s.z = ob.child("relativePosition").attribute("z").as_double();

            r.signals.push_back(s);

            if (s.dynamic) c.signals.push_back(s);
        }
        data.controller.push_back(c);
    }
    return 0;
}

int addSignal(road &r, roadNetwork &data, double s, double t, string type, string ori)
{
    bool found = false;
    int i; 
    for (i = 0; i < data.controller.size(); i++)
    {
        if (data.controller[i].id == 1000) 
        {
            found = true; 
            break;
        }
    }
    if (!found) 
    {
        control c; 
        c.id = 1000;
        data.controller.push_back(c);
        i = data.controller.size()-1;
    }

    signal sig; 
    sig.id = data.controller[i].signals.size();
    sig.s = s;
    sig.t = t;
    sig.z = 0;
    sig.type = type;
    sig.height = 2;
    sig.width = 0.5;
    sig.orientation = ori;
    sig.rule = 1000;

    if (type == "100.0.0.1")
    {
        sig.dynamic = true;
        sig.value = 0;
    }

    r.signals.push_back(sig);
    data.controller[i].signals.push_back(sig);

    return 0;
}
