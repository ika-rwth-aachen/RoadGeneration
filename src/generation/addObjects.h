/**
 * @file addObjects.h
 *
 * @brief file contains methodology for object creation
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

/**
 * @brief function adds a traffic island to the lane structure
 * 
 * @param o         object data
 * @param r         road data
 * @return int      error code
 */
int addTrafficIsland(object o, road &r)
{
    /*  design of a trafficIsland
     *          __________
     *      ___/    __    \___
     *             |  |
     *      ___    |__|    ___
     *         \__________/
     */

    r.objects.push_back(o);

    // find laneSection
    int i;
    laneSection lS;
    for (i = r.laneSections.size() - 1; i >= 0; i--)
        if (r.laneSections[i].s < o.s)
        {
            lS = r.laneSections[i];
            break;
        }

    lane l;
    findLane(lS, l, 1);

    double w = o.width / 2;
    double ds = o.length / 2;

    laneSection sec;

    // --- part 1 (opening)
    sec = r.laneSections[i];
    sec.s = o.s - o.length;

    l.w.d = -2 * w / pow(ds, 3);
    l.w.c = 3 * w / pow(ds, 2);
    l.w.b = 0;
    l.w.a = 0;
    l.rm.type = "none";
    l.type = "driving";

    shiftLanes(sec, 1, 1);
    shiftLanes(sec, -1, 1);

    l.id = 1;
    sec.lanes.push_back(l);
    l.id = -1;
    sec.lanes.push_back(l);

    r.laneSections.push_back(sec);

    // --- part 2 (straight)
    sec = r.laneSections[i];
    sec.s = o.s - o.length / 2;

    l.w.d = 0;
    l.w.c = 0;
    l.w.b = 0;
    l.w.a = w;
    l.rm.type = "none";
    l.type = "none";

    shiftLanes(sec, 1, 1);
    shiftLanes(sec, -1, 1);

    l.id = 1;
    sec.lanes.push_back(l);
    l.id = -1;
    sec.lanes.push_back(l);

    r.laneSections.push_back(sec);

    // --- part 3 (closing)
    sec = r.laneSections[i];
    sec.s = o.s + o.length / 2;

    l.w.d = 2 * w / pow(ds, 3);
    l.w.c = -3 * w / pow(ds, 2);
    l.w.b = 0;
    l.w.a = w;
    l.rm.type = "none";
    l.type = "driving";

    shiftLanes(sec, 1, 1);
    shiftLanes(sec, -1, 1);

    l.id = 1;
    sec.lanes.push_back(l);
    l.id = -1;
    sec.lanes.push_back(l);

    r.laneSections.push_back(sec);

    // --- part 4 (after trafficIsland)
    sec = r.laneSections[i];
    sec.s = o.s + o.length;

    r.laneSections.push_back(sec);

    return 0;
}

/**
 * @brief function generates a roadwork
 * 
 * @param o         object containing length and position of roadwork   
 * @param r         road which contains the roadwork
 * @param laneId    lane which contains the roadwork
 * @return int      error code
 */
int addRoadWork(object o, road &r, int laneId)
{
    std::vector<laneSection>::iterator it;

    // find lane section of starting point
    int i = 0;
    bool found = false;
    for (i = 0; i < r.laneSections.size() - 1; i++)
    {
        if (r.laneSections[i].s <= o.s && r.laneSections[i + 1].s > o.s)
        {
            found = true;
            it = r.laneSections.begin() + i;
            break;
        }
    }
    if (!found)
    {
        it = r.laneSections.begin() + r.laneSections.size() - 1;
        i = r.laneSections.size() - 1;
    }

    // if laneSection does not end before end of roadwork
    if (i == r.laneSections.size() - 1 || r.laneSections[i + 1].s - it->s > o.len)
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
        cerr << "Err: Length of roadwork is longer than  laneSection." << endl;
        return 1;
    }

    // shift remaining sections
    i++;
    for (; i < r.laneSections.size(); i++)
    {
        r.laneSections[i].id++;
    }
    return 0;
}

/**
 * @brief function adds parking splot objects
 * 
 * @param o         object data
 * @param r         road data
 * @return int      error code
 */
int addParking(object o, road &r)
{
    // find laneSection
    int i;
    for (i = r.laneSections.size() - 1; i >= 0; i--)
        if (r.laneSections[i].s < o.s)
            break;

    laneSection lS = r.laneSections[i];

    int laneId;
    if (o.t > 0)
        laneId = findMaxLaneId(lS);
    if (o.t < 0)
        laneId = findMinLaneId(lS);
    double t = findTOffset(lS, laneId, o.s);
    fixAngle(o.hdg);

    if (abs(o.hdg) > M_PI / 4)
    {
        o.t = t + sgn(laneId) * abs(o.length / (2 * sin(o.hdg)));
        o.distance = abs(o.width / sin(o.hdg));
    }
    if (abs(o.hdg) < M_PI / 4)
    {
        o.t = t + sgn(laneId) * abs(o.width / (2 * cos(o.hdg)));
        o.distance = abs(o.length / cos(o.hdg));
    }
    r.objects.push_back(o);

    return 0;
}

/**
 * @brief function adds a bus top
 * 
 * @param o     objct data
 * @param r     road data
 * @return int  error code
 */
int addBusStop(object o, road &r)
{
    // find starting laneSection
    int i;
    for (i = r.laneSections.size() - 1; i >= 0; i--)
        if (r.laneSections[i].s < o.s)
            break;
    laneSection lS = r.laneSections[i];

    int side;
    if (o.t > 0)
        side = 1;
    if (o.t < 0)
        side = -1;

    double length = setting.busStop.length;
    double widening = setting.busStop.widening;

    if (addLaneWidening(r.laneSections, side, o.s - length / 2 - widening, widening, true)))
        {
            cerr << "Err: error in addLaneWidening" << endl;
            return 1;
        }
    if (addLaneDrop(r.laneSections, side, o.s + length / 2, widening))
    {
        cerr << "Err: error in addLaneDrop" << endl;
        return 1;
    }

    return 0;
}

/**
 * @brief function gets the object position
 * 
 * @param node      object input data
 * @param o         object data
 * @return int      error code
 */
int getPosition(pugi::xml_node node, object &o)
{
    // read position of objects in st coordinates
    if (node.child("relativePosition"))
    {
        pugi::xml_node position = node.child("relativePosition");
        o.s = position.attribute("s").as_double();
        o.t = position.attribute("t").as_double();
        o.hdg = position.attribute("hdg").as_double();
    }
    if (node.child("repeatPosition"))
    {
        pugi::xml_node position = node.child("repeatPosition");
        o.s = position.attribute("s").as_double();
        o.t = position.attribute("t").as_double();
        o.hdg = position.attribute("hdg").as_double();

        o.repeat = true;
        o.len = position.attribute("length").as_double();
    }

    return 0;
}

/**
 * @brief function creates objects
 * 
 * @param inRoad    road input data from input file
 * @param r         road data 
 * @param data      roadNetwork structure where the generated roads and junctions are stored
 * @return int      error code
 */
int addObjects(pugi::xml_node inRoad, road &r, roadNetwork &data)
{
    for (pugi::xml_node obj : inRoad.child("objects").children())
    {
        std::string type = obj.name();
        object o;

        o.id = obj.attribute("id").as_double();

        // save position in object
        getPosition(obj, o);

        // --- consider different object cases ---------------------------------
        if (type == "parkingSpace")
        {
            o.type = type;
            o.length = obj.attribute("length").as_double();
            o.width = obj.attribute("width").as_double();
            o.height = 4;

            addParking(o, r);
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
            addBusStop(o, r);
        }

        if (type == "trafficIsland")
        {
            o.type = type;
            o.s = obj.attribute("s").as_double();
            o.t = 0;
            o.length = obj.attribute("length").as_double();
            o.width = obj.attribute("width").as_double();

            addTrafficIsland(o, r);
        }

        //--- Traffic Rule -----------------------------------------------------
        if (type != "trafficRule")
            continue;

        control c;
        c.id = obj.attribute("id").as_int();

        for (pugi::xml_node ob : obj.children())
        {
            sign s;
            data.nSignal++;
            s.id = data.nSignal;
            s.id = ob.attribute("id").as_int();
            s.type = ob.attribute("type").value();
            s.value = ob.attribute("value").as_double();
            s.dynamic = ob.attribute("dynamic").as_bool();
            s.s = ob.child("relativePosition").attribute("s").as_double();
            s.t = ob.child("relativePosition").attribute("t").as_double();
            s.z = ob.child("relativePosition").attribute("z").as_double();

            r.signs.push_back(s);

            if (s.dynamic)
                c.signs.push_back(s);
        }
        data.controller.push_back(c);
    }
    return 0;
}

/**
 * @brief function creates a signal which is automatically generated
 * 
 * @param r             road which contains the signal
 * @param data          roadNetwork data, which holds the controls
 * @param s             s position of signal
 * @param t             t position of signal
 * @param type          type of signal
 * @param subtype       subtype of signal
 * @param controller    controllerof signal
 * @return int          error code
 */
int addSignal(road &r, roadNetwork &data, double s, double t, string type, string subtype, int controller)
{
    if (controller == -1)
        controller = 1000 + r.junction * 100;

    laneSection lS = r.laneSections.front();

    if (t == INFINITY)
        t = findTOffset(lS, findMaxLaneId(lS), s) + 1;
    if (t == -INFINITY)
        t = findTOffset(lS, findMinLaneId(lS), s) - 1;
    t += r.laneSections.front().o.a;

    // search controller for given junction
    bool found = false;
    int i;
    for (i = 0; i < data.controller.size(); i++)
    {
        if (data.controller[i].id == controller)
        {
            found = true;
            break;
        }
    }
    // if controller does not exist -> create one
    if (!found)
    {
        control c;
        c.id = controller;
        data.controller.push_back(c);
        i = data.controller.size() - 1;
    }

    sign sig;
    data.nSignal++;
    sig.id = data.nSignal;
    sig.s = s;
    sig.t = t;
    sig.z = 0;
    sig.type = type;
    sig.subtype = subtype;
    sig.height = 2;
    sig.width = 0.5;
    sig.rule = 1000;

    // basic traffic light
    if (type == "1000001")
    {
        sig.dynamic = true;
        sig.orientation = "+";
        sig.value = 0;
        sig.z = 4;
    }

    // left traffic light
    if (type == "1000011")
    {
        sig.dynamic = true;
        sig.orientation = "+";
        sig.value = 0;
        sig.z = 4;
    }

    // TODO add more

    r.signs.push_back(sig);
    data.controller[i].signs.push_back(sig);

    return 0;
}
