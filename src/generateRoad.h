// file generateRoad.h

#include "curve.h"
#include "laneSectionChange.h"

/**
 * @brief function generate a road based on the input data from xml file
 * 
 * @param geos              geometry data from the input file
 * @param r                 resulting road 
 * @param sStart            starting s position
 * @param sEnd              ending s position
 * @param sLaneWidening     position of laneWiding (always on lane 1 - roads points away from junction)
 * @param s0                position of s where x0, y0, phi0 should hold
 * @param x0                x0 should hold at s0 for the resulting road
 * @param y0                y0 should hold at s0 for the resulting road
 * @param phi0              phi0 should hold at s0 for the resulting road
 * @return int              errorcode
 */
int generateRoad(pugi::xml_node geos, road &r, double sStart, double sEnd, double sLaneWidening, double s0, double x0, double y0, double phi0)
{
    // save geometry data from sStart - sEnd 
    //  -> s0 is located at x0, y0, phi0 
    // mode = 1 -> in s direction
    // mode = 2 -> in opposite s direction
    int mode;

    if (sEnd >= sStart)
    {   
        mode = 1;
    }
    else if (sEnd < sStart)
    {   
        // switch sStart and sEnd but store this with mode = 2
        double tmp = sStart;
        sStart = sEnd;
        sEnd = tmp;
        mode = 2;
    }

    // s0case 0: s0 lies between sStart and sEnd
    // s0case 1: s0 before sStart 
    // s0case 2: s0 after sEnd
    // -> road geometry at end is also considered for s0 
    // (only valid for small offsets where the current geometry is valid)
    int s0case = 0;
    if (s0 < sStart) s0case = 1;
    if (s0 > sEnd)   s0case = 2;
    
    int foundfirst = -1;
    int foundlast = -1;

    // search first and last relevant geometry
    int cc = 0;
    double s = 0;

    for (pugi::xml_node_iterator it = geos.child("referenceLine").begin(); it != geos.child("referenceLine").end(); ++it)
    {
        double length = it->attribute("length").as_double();
        
        if (s + length > sStart && foundfirst == -1) foundfirst = cc;
        
        if (s + length >= sEnd  && foundlast == -1) foundlast = cc;

        cc++;
        s += length;
    }

    // set sEnd to last containing s value if it's set to inf
    if (sEnd == INFINITY) 
    {
        foundlast = cc - 1;
        sEnd = s;
    }

    // start values (starting point at origin)
    s = 0;
    double x = 0;
    double y = 0;
    double hdg = 0;

    cc = 0;
    for (pugi::xml_node_iterator it = geos.child("referenceLine").begin(); it != geos.child("referenceLine").end(); ++it)
    {
        int type;
        double c = 0, c1 = 0, c2 = 0;
        double R = 0, R1 = 0, R2 = 0;

        // define type
        if((string)it->attribute("type").value() == "line") type = 1;
        if((string)it->attribute("type").value() == "arc") type = 2;
        if((string)it->attribute("type").value() == "spiral") type = 3;

        double length = it->attribute("length").as_double();

        // define radi and curvatures
        if (it->attribute("R"))  R = it->attribute("R").as_double();
        if (it->attribute("Rs")) R1 = it->attribute("Rs").as_double();
        if (it->attribute("Re")) R2 = it->attribute("Re").as_double();
        if (R != 0) c = 1/R; 
        if (R1 != 0) c1 = 1 / R1; 
        if (R2 != 0) c2 = 1 / R2; 
           
        // create new geometry struct
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

        // actual length can vary if a cut has to be perfomed
        double actuallength = length;

        if (cc < foundfirst)
        {
            // calculate new position and increase s but do not save this
            curve(length, geo, x, y, hdg,1);
            s += length;            
            continue;
        } 
        if (cc > foundlast)
        {
            // do nothing and break the loop at the end
            break;
        }

        // 4 different cases has to be observed: |----| is the current geometry

        // |-------sStart--------|  sEnd
        if (cc == foundfirst && cc != foundlast)
        {
            actuallength = length - (sStart-s);

            // reset s to zero
            geo.s = 0;
            s = 0;

            // calculate new start point of geometry
            curve(sStart - s, geo, geo.x, geo.y, geo.hdg,1);

            // update first curvature
            geo.c1 += (sStart - s) * (geo.c2 - geo.c1) / length; 
            // update length
            geo.length = actuallength;

            // normal calculation of full length
            curve(length, geo, x, y,hdg,1);
        }

        // sStart   |-------sEnd--------|
        if (cc != foundfirst && cc == foundlast)
        {
            actuallength = sEnd - s;
            curve(actuallength, geo, x, y, hdg, 1);

            // update second curvature
            geo.c2 = geo.c1 + (actuallength) * (geo.c2 - geo.c1) / length;
            // update length
            geo.length = actuallength;            
        }

        // |-----sStart------sEnd--------|   
        if (cc == foundfirst && cc == foundlast)
        {
            actuallength = sEnd - sStart;

            // reset s to zero
            geo.s = 0;
            s = 0;

            // calculate new start point of geometry
            curve(sStart - s, geo, geo.x, geo.y, geo.hdg,1);

            // update first curvature
            geo.c1 += (sStart - s) * (geo.c2 - geo.c1) / length; 
            // update length
            geo.length = actuallength;            
        }
        //   sStart |------------| sEnd
        if (cc != foundfirst && cc != foundlast)
        {
            curve(length, geo, x, y, hdg,1);           
        }
        
        // update road
        fixAngle(geo.hdg);
        r.length = s + actuallength;
        r.geometries.push_back(geo);

        // increase variables
        s += length;
        cc++;;
    }

    // calculate x, y, phi at s0
    double dphi;
    
    if (s0case == 0 || s0case == 2)
    {
        for (int i = 0; i < r.geometries.size(); i++)
        {
            geometry g = r.geometries[i];

            if (s0 >= g.s)
            {
                x = g.x;
                y = g.y;
                hdg = g.hdg;
                curve(s0-sStart-g.s,g,x,y,hdg,1);

                dphi = phi0-hdg;
                break;
            }
        }
    }
    else if (s0case == 1)
    {
        geometry g = r.geometries.front();

        // update angle and curvatures
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

        x = g.x;
        y = g.y;
        hdg = g.hdg;

        curve(sStart-s0,g,x,y,hdg,1);
        dphi = phi0-hdg+M_PI;
    }

    // shift x,y,phi according to x0, y0, phi0
    for (int i = 0; i < r.geometries.size(); i++)
    {
        // shift in origin
        double xtemp = r.geometries[i].x - x;
        double ytemp = r.geometries[i].y - y;

        // rotate with dphi
        r.geometries[i].hdg += dphi;
        r.geometries[i].x = xtemp * cos(dphi) - ytemp * sin(dphi);
        r.geometries[i].y = xtemp * sin(dphi) + ytemp * cos(dphi);

        // shift back with x0, y0 
        r.geometries[i].x += x0;
        r.geometries[i].y += y0;

    }

    // flip geometries (convention: all roads point away from junction)
    if (mode == 2)
    {
        road rNew = r;
        rNew.geometries.clear();

        for (int i = r.geometries.size()-1; i >= 0; i--)
        {
            geometry g = r.geometries[i];

            curve(g.length,g,g.x,g.y,g.hdg,1);

            // flip angles and curvature
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

    // --- add lanes to road ---------------------------------------------------
    for (pugi::xml_node_iterator it = geos.child("lanes").begin(); it != geos.child("lanes").end(); ++it)
    {
        if ((string)it->name() != "laneSection") continue;

        laneSection laneSec;
        laneSec.id = it->attribute("id").as_int();
        laneSec.s = it->attribute("s").as_double();

        for (pugi::xml_node_iterator itt = it->begin(); itt != it->end(); ++itt)
        {
            lane l; 

            l.id = itt->attribute("id").as_int();

            // flip lanes for mode 1
            if (mode == 1) l.id *= -1;  

            l.type = itt->attribute("type").value();
        
            pugi::xml_node w = itt->child("laneWidth");
            if (w)
            {
                l.w.s = w.attribute("sOffset").as_double();
                l.w.a = w.attribute("w").as_double();
            }

            pugi::xml_node rm = itt->child("roadMark");
            if (rm)
            {
                l.rm.s = rm.attribute("sOffset").as_double();
                l.rm.type = rm.attribute("type").value();
                l.rm.color = rm.attribute("color").value();
                l.rm.width = rm.attribute("width").as_double();
            }

            laneSec.lanes.push_back(l);
        }
        r.laneSections.push_back(laneSec);
    }

    // consider lanedrops and lanewidenings 
    //      -> have to be defined in increasing s order
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
            addLaneWidening(r.laneSections, lane, s, ds, false);
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

    // add laneWidening before junction
    //     sLaneWidening is distance from junction to point where the lane drops
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
        shiftLanes(adLaneSec,1,1);
        adLaneSec.lanes.push_back(l);   

        // solid center line
        lane lTmp;
        int id = findLane(adLaneSec, lTmp, 0);
        adLaneSec.lanes[id].rm.type = "solid";

        vector<laneSection>::iterator it = r.laneSections.begin();
        it = r.laneSections.insert(it, adLaneSec);
        it++;

        // widening lane
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

