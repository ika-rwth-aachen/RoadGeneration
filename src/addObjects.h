// file addObjects.h
#include <cassert>

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
        
        // consider different cases
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
            if (o.t > 0) laneId = findMaxLaneId(lS)+1;
            if (o.t < 0) laneId = findMinLaneId(lS)-1;
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

            // find laneSection
            int i;
            for (i = r.laneSections.size()-1; i >= 0; i--)
                if (r.laneSections[i].s < o.s) break;

            lane l;
            laneSection lS = r.laneSections[i];
            int id = findLane(lS, l, obj.attribute("laneId").as_int());

            r.laneSections[i].lanes[id].type = "roadWorks";
        }

        if (type == "busStop")
        {
            // find laneSection
            int i;
            for (i = r.laneSections.size()-1; i >= 0; i--)
                if (r.laneSections[i].s < o.s) break;
            laneSection lS = r.laneSections[i];

            if (o.t > 0) 
            {
                int laneId = findMaxLaneId(lS);
                addLaneWidening(r.laneSections, laneId, o.s-12.5, 7.5,true);
                addLaneDrop (r.laneSections, laneId+1, o.s+5, 7.5, true);
            }
            if (o.t < 0) 
            {
                int laneId = findMinLaneId(lS);
                addLaneWidening(r.laneSections, laneId, o.s-12.5, 7.5,true);
                addLaneDrop (r.laneSections, laneId-1, o.s+5, 7.5, true);
            }
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