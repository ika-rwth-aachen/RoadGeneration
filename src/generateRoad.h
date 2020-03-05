// file generateRoad.h

#include "curve.h"
#include "laneSectionChange.h"




int computeFirstLast(pugi::xml_node roadIn, int &foundfirst, int &foundlast, double &sStart, double &sEnd)
{
    int cc = 0;
    double s = 0;

    for (pugi::xml_node_iterator it = roadIn.child("referenceLine").begin(); it != roadIn.child("referenceLine").end(); ++it)
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

    return 0;
}

int generateGeometries(pugi::xml_node roadIn, road &r, double &sStart, double &sEnd)
{
    // search first and last relevant geometry
    int foundfirst = -1;
    int foundlast = -1;
    computeFirstLast(roadIn, foundfirst, foundlast, sStart, sEnd);

    // start values (starting point at origin)
    double s = 0;
    double x = 0;
    double y = 0;
    double hdg = 0;
    r.length = 0;

    int cc = 0;
    for (pugi::xml_node_iterator it = roadIn.child("referenceLine").begin(); it != roadIn.child("referenceLine").end(); ++it)
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
            cc++;         
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

            // normal calculation of full length
            curve(length, geo, x, y,hdg,1);

            // calculate new start point of geometry
            curve(sStart - s, geo, geo.x, geo.y, geo.hdg,1);

            // update first curvature
            geo.c1 += (sStart - s) * (geo.c2 - geo.c1) / length; 
            // update length
            geo.length = actuallength;
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
            // update s
            geo.s = s - sStart;         
        }

        // |-----sStart------sEnd--------|   
        if (cc == foundfirst && cc == foundlast)
        {
            actuallength = sEnd - sStart;

            // reset s to zero
            geo.s = 0;

            // calculate new start point of geometry
            curve(sStart - s, geo, geo.x, geo.y, geo.hdg,1);

            // update curvatures
            double c1 = geo.c1;
            double c2 = geo.c2;

            geo.c1 = c1 + (sStart - s) * (c2 - c1) / length; 
            geo.c2 = c1 + (sEnd   - s) * (c2 - c1) / length;

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
        r.length += actuallength;
        r.geometries.push_back(geo);

        // increase variables
        s += length;
        cc++;
    }
    return 0;
}

int shiftGeometries(road &r, double sStart, double sEnd, double s0, double x0, double y0, double phi0)
{
    /* -> s0 is located at x0, y0, phi0 
     * s0case 0: s0 lies between sStart and sEnd
     * s0case 1: s0 before sStart 
     * s0case 2: s0 after sEnd
     * -> road geometry segment at  the end is also considered at s = s0 
     *  (only valid for small offsets where the current geometry is valid)
     */

    // calculate x, y, hdg at s0
    double x, y, hdg, dphi;

    int Case = 0; 
    if (s0 < sStart) Case = 1;
    if (s0 > sEnd)   Case = 2;

    if (Case == 0 || Case == 2)
    {
        for (int i = r.geometries.size()-1; i >= 0; i--)
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
    else if (Case == 1)
    {
        // take first geometry and flip
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
    return 0;
}

void flipGeometries(road &r)
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

        //flip s
        g.s = r.length - g.length - g.s;

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

int addLanes(pugi::xml_node roadIn, road &r, int mode)
{
    double desWidth = 3.5;
    if ((string)roadIn.attribute("type").value() == "motorway") desWidth = 4.0;
    if ((string)roadIn.attribute("type").value() == "access") desWidth = 3.0;

    // --- add basic laneSection to road ---------------------------------------
    laneSection laneSec;
    laneSec.id = 1;
    laneSec.s = 0;

    lane l1;
    l1.id = 1;
    l1.speed = 50;
    l1.w.a = desWidth;
    l1.preId = 1;
    l1.sucId = 1;
    laneSec.lanes.push_back(l1);
    
    lane l2;
    l2.id = 0;
    l2.rm.type = "broken";
    l2.w.a = 0.0;
    laneSec.lanes.push_back(l2);
    
    lane l3;
    l3.id = -1;
    l3.speed = 50;
    l3.w.a = desWidth;
    l3.preId = -1;
    l3.sucId = -1;
    laneSec.lanes.push_back(l3);
        
    r.laneSections.push_back(laneSec);

    // --- add user defined laneSection to road --------------------------------
    for (pugi::xml_node_iterator it = roadIn.child("lanes").begin(); it != roadIn.child("lanes").end(); ++it)
    {
        if ((string)it->name() != "laneSection") continue;

        laneSection laneSec;
        laneSec.id = it->attribute("id").as_int();

        if (laneSec.id == 1) laneSec = r.laneSections.back();

        laneSec.s = it->attribute("s").as_double();

        for (pugi::xml_node_iterator itt = it->begin(); itt != it->end(); ++itt)
        {
            lane l; 

            l.id = itt->attribute("id").as_int();
            l.preId = l.id;
            l.sucId = l.id;

            // flip lanes for mode 1
            if (mode == 2) l.id *= -1;  

            l.type = itt->attribute("type").value();

            if (l.id == 0) l.w.a = 0.0;

            if (itt->attribute("width"))
                l.w.a = itt->attribute("width").as_double();
            else if(l.type == "access") l.w.a = 3.0;
            else if(l.type == "motorway") l.w.a = 4.0;
        
            pugi::xml_node rm = itt->child("roadMark");
            if (rm)
            {
                l.rm.type = rm.attribute("type").value();
                if (rm.attribute("color"))
                    l.rm.color = rm.attribute("color").value();
                if (rm.attribute("width"))
                    l.rm.width = rm.attribute("width").as_double();
            }

            pugi::xml_node m = itt->child("material");
            if (m)
            {
                l.m.surface = m.attribute("surface").value();
                l.m.friction = m.attribute("friction").as_double();
                l.m.roughness = m.attribute("roughness").as_double();
            }

            if (r.type == "main") l.speed = 50;
            if (r.type == "access") l.speed = 50;
            if (r.type == "motorway") l.speed = 130;

            lane ltmp;
            int id = findLane(laneSec,ltmp,l.id);
            if (id >= 0) laneSec.lanes[id] = l;
            else laneSec.lanes.push_back(l);

            if (l.type == "delete")
            {
                int id = findLane(laneSec,l,l.id);
                laneSec.lanes.erase(laneSec.lanes.begin() + id);
            }
        }
        if (laneSec.id == 1) r.laneSections.back() = laneSec;
        else r.laneSections.push_back(laneSec);
    }
    
    return 0;
}

int addLaneSectionChanges(pugi::xml_node roadIn, road &r, double sJunction)
{

    // --- user defined lanedrops or lanewidenings -----------------------------
    //      -> have to be defined in increasing s order !!!

    for (pugi::xml_node_iterator itt = roadIn.child("lanes").begin(); itt != roadIn.child("lanes").end(); ++itt)
    {   
        if ((string)itt->name() == "laneWidening")
        {
            int lane = itt->attribute("lane").as_int();
            double s = itt->attribute("sOffset").as_double();
            double ds = itt->attribute("ds1").as_double();

            // only perform drop if not close to junction
            if (s < sJunction + 25) continue;
            if (s > r.length) continue;  
                 
            addLaneWidening(r.laneSections, lane, s, ds, false);
        }
        if ((string)itt->name() == "laneDrop")
        {
            int lane = itt->attribute("lane").as_int();
            double s = itt->attribute("sOffset").as_double();
            double ds = itt->attribute("ds1").as_double();

            // only perform drop if not close to junction
            if (s < sJunction + 25) continue;       
            if (s > r.length) continue;       
            addLaneDrop(r.laneSections, lane, s, ds);  

            //restricted area
            if (itt->child("restrictedArea"))
            {
                int signNumber = itt->child("restrictedArea").attribute("signNumber").as_int();
                double s2 = itt->child("restrictedArea").attribute("sOffset").as_int();
                double ds2 = itt->child("restrictedArea").attribute("ds2").as_int();
                addRestrictedArea(r.laneSections, lane, s, s2, ds, ds2, signNumber);
            }              
        }
    }

    // --- automatic generated laneWiding --------------------------------------
    if (sJunction != 0) laneWideningJunction(r,  sJunction, 1, true);

    return 0;
}

/**
 * @brief function generate a road based on the input data from xml file
 * 
 * @param road              geometry data from the input file
 * @param r                 resulting road
 * @param sStart            starting s position
 * @param sEnd              ending s position
 * @param sJunction         position of laneWiding (always on lane 1 - roads points away from junction)
 * @param s0                position of s where x0, y0, phi0 should hold
 * @param x0                x0 should hold at s0 for the resulting road
 * @param y0                y0 should hold at s0 for the resulting road
 * @param phi0              phi0 should hold at s0 for the resulting road
 * @return int              errorcode
 */
int generateRoad(pugi::xml_node roadIn, road &r, double sStart, double sEnd, double sJunction, double s0, double x0, double y0, double phi0)
{
    r.type = roadIn.attribute("type").value();
    if (r.type == "access") sJunction = 0;

    // save geometry data from sStart - sEnd 
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

    // generate geometries
    generateGeometries(roadIn, r, sStart, sEnd);

    // shift geometries
    shiftGeometries(r, sStart, sEnd, s0, x0, y0, phi0);

    // flip geometries (convention: all roads point away from junction)
    if (mode == 2) flipGeometries(r);

    // add lanes
    addLanes(roadIn, r, mode);

    // add lane section changes
    addLaneSectionChanges(roadIn, r, sJunction);

    return 0;
}

