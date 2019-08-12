// file generateRoad.h

#include "curve.h"
#include "laneSectionChange.h"

int generateRoad(pugi::xml_node geos, road &r, double s0, double sOffset, double sLaneWidening, double phi0, double x0, double y0)
{
    // mode = 1 -> save part before s0
    // mode = 2 -> save part after  s0
    int mode;
    if (sOffset < 0) mode = 1;
    if (sOffset >= 0) mode = 2;

    double dx, dy;

    double s = 0;
    double x = 0;
    double y = 0;
    double hdg = phi0;

    bool save = true;

    for (pugi::xml_node_iterator it = geos.child("referenceLine").begin(); it != geos.child("referenceLine").end(); ++it)
    {
        int type;
        double c = 0, c1 = 0, c2 = 0;
        double R = 0, R1 = 0, R2 = 0;

        if((string)it->attribute("type").value() == "line") type = 1;
        if((string)it->attribute("type").value() == "arc") type = 2;
        if((string)it->attribute("type").value() == "spiral") type = 3;

        double length = stod(it->attribute("length").value(),&sz);
        if (it->attribute("R"))  R = stod(it->attribute("R").value(),&sz);
        if (it->attribute("Rs")) R1 = stod(it->attribute("Rs").value(),&sz);
        if (it->attribute("Re")) R2 = stod(it->attribute("Re").value(),&sz);

        if (R != 0) c = 1/R; 
        if (R1 != 0) c1 = 1 / R1; 
        if (R2 != 0) c2 = 1 / R2; 

        double actualLength = length;
        bool complicatedCut = false;

        if (mode == 1)
        {
            if (s + length > s0 + sOffset) 
            {
                complicatedCut = true;
                actualLength = s0 + sOffset - s;
            }
            if (actualLength <= 0) break;
        } 
        if (mode == 2)
        {
            if (s + length < s0 + sOffset) save = false;
            
            if (s < s0 + sOffset && s + length > s0 + sOffset) 
            {
                save = true;
                complicatedCut = true;
                actualLength = s + length - s0 - sOffset;
            }
        } 

        // create new geometry
        geometry geo;
        geo.s = s;
        geo.x = x;
        geo.y = y;
        geo.hdg = hdg;
        geo.length = length;
        geo.c = c;
        geo.c1 = c1;
        geo.c2 = c2;  
        geo.type = type;

        if(!complicatedCut)
            curve(actualLength, geo, x, y, hdg,1);

        // if complicated cut (cut inside of current geometry)
        if(complicatedCut)
        {
            if (mode == 1)
            {
                curve(actualLength - sOffset, geo, x, y, hdg,1);
                dx = x0 - x;
                dy = y0 - y;

                // update second curvature
                geo.c2 = geo.c1 + (actualLength) * (geo.c2 - geo.c1) / length;
                // update length
                geo.length = actualLength;
            }
            if (mode == 2)
            {
                geo.s = 0;
                s = 0;
                curve(length-actualLength, geo, geo.x, geo.y, geo.hdg,1);

                double tmpX = x;
                double tmpY = y;
                double tmpHdg = hdg;
                curve(length-actualLength-sOffset, geo, tmpX, tmpY,tmpHdg,1);
                dx = x0 - tmpX;
                dy = y0 - tmpY;
                
                curve(length, geo, x, y,hdg,1);

                // update first curvature
                geo.c1 += (s0 + sOffset) * (geo.c2 - geo.c1) / length; 
                // update length
                geo.length = actualLength;
            }
        }
        if (save) 
        {
            r.length = s + actualLength;
            r.geometries.push_back(geo);
        }

        s += length;
    }

    // shift geometries -> endpoint is at x0 / y0
    for (int i = 0; i < r.geometries.size(); i++)
    {
        r.geometries[i].x += dx;
        r.geometries[i].y += dy;
    }

    // flip geometries (convention: all roads point away from junction)
    if (mode == 1)
    {
        road rNew = r;
        rNew.geometries.clear();

        for (int i = r.geometries.size()-1; i >= 0; i--)
        {
            geometry g = r.geometries[i];

            curve(g.length,g,g.x,g.y,g.hdg,1);

            g.hdg += M_PI;
            fixAngle(g.hdg);

            double c1 = g.c1;
            double c2 = g.c2;

            if (g.type == 2) g.c *= -1;
            if (g.type == 3) 
            {
                g.c1 = -c2;
                g.c2 = -c1;
            }
            rNew.geometries.push_back(g);
        }
        r.geometries.clear();
        r.geometries = rNew.geometries;
    }

    // add lanes to road
    for (pugi::xml_node_iterator it = geos.child("lanes").begin(); it != geos.child("lanes").end(); ++it)
    {
        if ((string)it->name() != "laneSection") continue;

        laneSection laneSec;
        laneSec.id = it->attribute("id").as_int();
        laneSec.s = stod(it->attribute("s").value(),&sz);

        for (pugi::xml_node_iterator itt = it->begin(); itt != it->end(); ++itt)
        {
            lane l; 

            l.id = itt->attribute("id").as_int();
            if (mode == 1) l.id *= -1;  // flip lanes for mode 1

            l.type = itt->attribute("type").value();
        
            pugi::xml_node w = itt->child("laneWidth");
            if (w)
            {
            l.w.s = stod(w.attribute("sOffset").value(),&sz);
            l.w.a = stod(w.attribute("w").value(),&sz);
            }

            pugi::xml_node rm = itt->child("roadMark");
            if (rm)
            {
            l.rm.s = stod(rm.attribute("sOffset").value(),&sz);
            l.rm.type = rm.attribute("type").value();
            l.rm.color = rm.attribute("color").value();
            l.rm.width = stod(rm.attribute("width").value(),&sz);
            }

            laneSec.lanes.push_back(l);
        }
        r.laneSections.push_back(laneSec);
    }

    // consider lanedrops and lanewidenings
    for (pugi::xml_node_iterator itt = geos.child("lanes").begin(); itt != geos.child("lanes").end(); ++itt)
    {   
        if ((string)itt->name() == "laneWidening")
        {
            int lane = itt->attribute("lane").as_int();
            double s = itt->attribute("sOffset").as_double();
            double ds = itt->attribute("ds1").as_double();

            // only perform drop if not close to junction
            if (s < sLaneWidening + 50) continue;
            if (s > r.length) continue;       
            addLaneWidening(r.laneSections, lane, s, ds);
        }
        if ((string)itt->name() == "laneDrop")
        {
            int lane = itt->attribute("lane").as_int();
            double s = itt->attribute("sOffset").as_double();
            double ds = itt->attribute("ds1").as_double();

            // only perform drop if not close to junction
            if (s < sLaneWidening + 50) continue;       
            if (s > r.length) continue;       
            addLaneDrop(r.laneSections, lane, s, ds);                
        }
    }

    // add laneWidening at junction
    if (sLaneWidening > 0) 
    {
        r.laneSections.front().s = sLaneWidening + 50;

        laneSection adLaneSec = r.laneSections.front();
        adLaneSec.s = 0;
        adLaneSec.id++;

        // additional lane
        lane l;
        findLane(adLaneSec, l, 1);
        double w = laneWidth(l,0);
        l.w.a = w;
        l.rm.type = "broken";
        shiftLanes(adLaneSec,1);
        adLaneSec.lanes.push_back(l);   

        // solid center line
        lane lTmp;
        int id = findLane(adLaneSec, lTmp, 0);
        adLaneSec.lanes[id].rm.type = "solid";

        vector<laneSection>::iterator it = r.laneSections.begin();
        it = r.laneSections.insert(it, adLaneSec);
        it++;

        // widening part
        l.w.d = 2 * w / pow(50,3);
        l.w.c = - 3 * w / pow(50,2);
        l.w.b = 0;

        adLaneSec.id++;
        adLaneSec.s = sLaneWidening;
        id = findLane(adLaneSec, lTmp, 1);
        adLaneSec.lanes[id] = l;
        it = r.laneSections.insert(it, adLaneSec);
    }

    return 0;
}

