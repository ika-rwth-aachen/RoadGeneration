/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file buildRoad.h
 *
 * @brief file contains method for building up a road
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "../utils/curve.h"
#include "addLaneSections.h"

extern settings setting;

/**
 * @brief function computes first and last considered geometry in s interval 
 * 
 * @param roadIn        road input data
 * @param foundfirst    id of the first considerd geometry
 * @param foundlast     id of the last considerd geometry
 * @param sStart        start of the s interval
 * @param sEnd          end of the s interval
 * @return int          error code
 */
int computeFirstLast(DOMElement* roadIn, int &foundfirst, int &foundlast, double &sStart, double &sEnd)
{
    int cc = 0;
    double s = 0;

    if (roadIn != NULL) {
        DOMNodeList* referenceLines = getChildWithName(roadIn, "referenceLine")->getChildNodes();

        for (int i = 0; i < referenceLines->getLength(); i++)
        {
            DOMElement* it = (DOMElement*)referenceLines->item(i);
            if(it->getNodeType() != 1) //1 is an Element Node https://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html
            {
                continue;
            }

            double length = readDoubleAttrFromNode(it, "length");

            if (s + length > sStart && foundfirst == -1)
                foundfirst = cc;

            if (s + length >= sEnd && foundlast == -1)
                foundlast = cc;

            cc++;
            s += length;

        }
    }

    // set sEnd to last containing s value if it's set to inf
    if (sEnd == INFINITY)
    {
        foundlast = cc - 1;
        sEnd = s;
    }

    return 0;
}

/**
 * @brief function genetares the geometries of the reference line based on an s intervall
 * 
 * @param roadIn    road input data
 * @param r         road data containing the reference line information
 * @param sStart    start of the s interval
 * @param sEnd      end of the s interval
 * @return int      error code
 */
int generateGeometries(DOMElement* roadIn, road &r, double &sStart, double &sEnd)
{
    // search first and last relevant geometry
    int foundfirst = -1;
    int foundlast = -1;
    computeFirstLast(roadIn, foundfirst, foundlast, sStart, sEnd);

    // start values (starting point at origin)
    int cc = 0;
    r.length = 0;

    double s = 0;
    double x = 0;
    double y = 0;
    double hdg = 0;
    if(roadIn == NULL) return 0;

    DOMNodeList* referenceLines =  getChildWithName(roadIn, "referenceLine")->getChildNodes();
    for (int i = 0; i < referenceLines->getLength(); i++)
    {
        DOMElement* it = (DOMElement*)referenceLines->item(i);
        if(it->getNodeType() != 1) continue;
        
        geometryType type;
        double c = 0, c1 = 0, c2 = 0;
        double R = 0, R1 = 0, R2 = 0;

        // define type
        if (readNameFromNode(it) == "line")
            type = line;
        if (readNameFromNode(it) == "spiral")
            type = spiral;
        if (readNameFromNode(it) == "arc")
            type = arc;
        if (readNameFromNode(it) == "circle")
            type = arc;

        double length = readDoubleAttrFromNode(it, "length");

        // define radi and curvatures
        if (type == spiral)
        {
            R1 = readDoubleAttrFromNode(it, "Rs");
            R2 = readDoubleAttrFromNode(it, "Re");
            if (R1 != 0)
                c1 = 1 / R1;
            if (R2 != 0)
                c2 = 1 / R2;
        }
        if (type == arc)
        {
            R = readDoubleAttrFromNode(it, "R");
            if (R != 0)
                c = 1 / R;
        }

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
            curve(length, geo, x, y, hdg, 1);
            s += length;
            cc++;
            continue;
        }
        if (cc > foundlast)
        {
            // do nothing and break the loop at the end
            break;
        }

        // --- different cases has to be observed: |---| is the current geometry

        // |-------sStart--------|  sEnd
        if (cc == foundfirst && cc != foundlast)
        {
            actuallength = length - (sStart - s);

            // reset s to zero
            geo.s = 0;

            // normal calculation of full length
            curve(length, geo, x, y, hdg, 1);

            // calculate new start point of geometry
            curve(sStart - s, geo, geo.x, geo.y, geo.hdg, 1);

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
            curve(sStart - s, geo, geo.x, geo.y, geo.hdg, 1);

            // update curvatures
            double c1 = geo.c1;
            double c2 = geo.c2;

            geo.c1 = c1 + (sStart - s) * (c2 - c1) / length;
            geo.c2 = c1 + (sEnd - s) * (c2 - c1) / length;

            // update length
            geo.length = actuallength;
        }
        //   sStart |------------| sEnd
        if (cc != foundfirst && cc != foundlast)
        {
            curve(length, geo, x, y, hdg, 1);
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

/**
 * @brief function shift the geometries of the reference line
 * 
 * @param r         road input data 
 * @param sStart    start of s interval 
 * @param sEnd      end of s interval 
 * @param s0        s0 is origin of reference line
 * @param x0        global x position of origin
 * @param y0        global y position of origin
 * @param phi0      global angle of origin
 * @return int      error code
 */
int shiftGeometries(road &r, double sStart, double sEnd, double s0, double x0, double y0, double phi0)
{
    /* -> s0 is located at x0, y0, phi0 
     * s0case 0: s0 lies between sStart and sEnd
     * s0case 1: s0 before sStart 
     * s0case 2: s0 after sEnd
     * -> if s0 is outside s interval the ending geometry is used for the gap
     */

    // calculate x, y, hdg at s0
    double x, y, hdg, dphi;

    int Case = 0;
    if (s0 < sStart)
        Case = 1;
    if (s0 > sEnd)
        Case = 2;

    if (Case == 0 || Case == 2)
    {
        for (int i = r.geometries.size() - 1; i >= 0; i--)
        {
            geometry g = r.geometries[i];

            if (s0 >= g.s)
            {
                x = g.x;
                y = g.y;
                hdg = g.hdg;
                curve(s0 - sStart - g.s, g, x, y, hdg, 1);

                dphi = phi0 - hdg;
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

        if (g.type == 2)
            g.c *= -1;
        if (g.type == 3)
        {
            g.c1 = -c2;
            g.c2 = -c1;
        }

        x = g.x;
        y = g.y;
        hdg = g.hdg;

        curve(sStart - s0, g, x, y, hdg, 1);
        dphi = phi0 - hdg + M_PI;
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

/**
 * @brief function flips geometries of reference line
 * 
 * @param r     road data
 * @int         error code
 */
int flipGeometries(road &r)
{
    road rNew = r;
    rNew.geometries.clear();

    for (int i = r.geometries.size() - 1; i >= 0; i--)
    {
        geometry g = r.geometries[i];

        curve(g.length, g, g.x, g.y, g.hdg, 1);

        // flip angles and curvature
        g.hdg += M_PI;
        fixAngle(g.hdg);

        //flip s
        g.s = r.length - g.length - g.s;

        double c1 = g.c1;
        double c2 = g.c2;
        if (g.type == arc)
            g.c *= -1;
        if (g.type == spiral)
        {
            g.c1 = -c2;
            g.c2 = -c1;
        }
        rNew.geometries.push_back(g);
    }
    r.geometries.clear();
    r.geometries = rNew.geometries;

    return 0;
}

/**
 * @brief function adds lanes to the road structure
 * 
 * @param roadIn    road input data
 * @param r         road data
 * @param mode      defines the mode (flipped or not)
 * @return int      error code
 */
int addLanes(DOMElement* roadIn, road &r, int mode)
{
    double desWidth = setting.width.standard;
    double desSpeed = setting.speed.standard;

    if (r.classification == "main")
    {
        desWidth = setting.width.main;
        desSpeed = setting.speed.main;
    }
    else if (r.classification == "access")
    {
        desWidth = setting.width.access;
        desSpeed = setting.speed.access;
    }

    // --- add basic laneSection to road ---------------------------------------
    laneSection laneSec;
    laneSec.id = 1;
    laneSec.s = 0;

    lane l1;
    l1.id = 1;
    l1.speed = desSpeed;
    l1.w.a = desWidth;
    l1.preId = 1;
    l1.sucId = 1;
    laneSec.lanes.push_back(l1);

    lane l2;
    l2.id = 0;
    l2.rm.type = "broken";
    l2.w.a = 0;
    l2.turnStraight = false;
    laneSec.lanes.push_back(l2);

    lane l3;
    l3.id = -1;
    l3.speed = desSpeed;
    l3.w.a = desWidth;
    l3.preId = -1;
    l3.sucId = -1;
    laneSec.lanes.push_back(l3);

    r.laneSections.push_back(laneSec);

    // --- add user defined laneSection to road --------------------------------
    if (roadIn != NULL)
    {
        DOMElement* lanes = getChildWithName(roadIn, "lanes");

        if(lanes != NULL)
        {
            for (DOMElement *itt = lanes->getFirstElementChild();itt != NULL; itt = itt->getNextElementSibling())
            {
                if (readNameFromNode(itt) != "lane")
                    continue;

                lane l;
                l.id = readIntAttrFromNode(itt, "id");
                l.preId = l.id;
                l.sucId = l.id;

                // flip lanes for mode 1
                if (mode == 2)
                    l.id *= -1;

                if (!readStrAttrFromNode(itt, "type").empty())
                    l.type = readStrAttrFromNode(itt, "type");

                l.w.a = desWidth;
                if (!readStrAttrFromNode(itt, "width").empty())
                    l.w.a = readDoubleAttrFromNode(itt, "width");
                if (l.id == 0)
                    l.w.a = 0.0;

                l.speed = desSpeed;
                if (!readStrAttrFromNode(itt, "speed").empty())
                    l.speed = readDoubleAttrFromNode(itt, "speed");

                DOMElement* rm = getChildWithName(itt, "roadMark");
                if (rm)
                {
                    if (!readStrAttrFromNode(rm, "type").empty())
                        l.rm.type = readStrAttrFromNode(rm, "type");
                    if (!readStrAttrFromNode(rm, "color").empty())
                        l.rm.color = readStrAttrFromNode(rm, "color");
                    if (!readStrAttrFromNode(rm, "width").empty())
                        l.rm.width = readDoubleAttrFromNode(rm, "width");
                }

                DOMElement* m = getChildWithName(itt,"material");
                if (m)
                {
                    if (!readStrAttrFromNode(m, "surface").empty())
                        l.m.surface = readStrAttrFromNode(m, "surface");
                    if (!readStrAttrFromNode(m, "friction").empty())
                        l.m.friction = readDoubleAttrFromNode(m, "friction");
                    if (!readStrAttrFromNode(m, "roughness").empty())
                        l.m.roughness = readDoubleAttrFromNode(m, "roughness");
                }

                lane tmp;
                int id = findLane(laneSec, tmp, l.id);
                if (id >= 0)
                    laneSec.lanes[id] = l;
                else
                    laneSec.lanes.push_back(l);

                if (l.type == "delete")
                {
                    int id = findLane(laneSec, l, l.id);
                    laneSec.lanes.erase(laneSec.lanes.begin() + id);
                }
            }
        }
    }
    r.laneSections.front() = laneSec;

    return 0;
}

/**
 * @brief function adds additional lane sections for changing lane structure
 *  
 * @param roadIn                road input data
 * @param r                     road data
 * @param automaticWidening     automatic widing input data
 * @return int                  error code
 */
int addLaneSectionChanges(DOMElement* roadIn, road &r, DOMElement* automaticWidening)
{
    // --- user defined lanedrops or lanewidenings -----------------------------
    //      -> have to be defined in increasing s order, because the lane changes are concatenated in s direction

    if(roadIn == NULL) return 0; //leave if the road is null. might cause errros 

    DOMNodeList* referenceLines = roadIn->getElementsByTagName(X("lanes"));
    for (int i = 0; i < referenceLines->getLength(); i++)
    {
        DOMElement* itt = (DOMElement*)referenceLines->item(i);
        if(itt->getNodeType() != 1) continue;

        if (getChildWithName(itt,"laneWidening"))
        {
            int side = readIntAttrFromNode(itt, "side");

            if (side == 0)
            {
                cerr << "ERR: laneWidening with side = 0" << endl;
                return 1;
            }

            double s = readDoubleAttrFromNode(itt, "s");

            double ds = setting.laneChange.ds;
            if (attributeExits(itt,"length"))
                ds = readDoubleAttrFromNode(itt, "length");

            // only perform drop if on road length
            if (s > r.length)
                continue;

            if (addLaneWidening(r.laneSections, side, s, ds, false))
            {
                cerr << "ERR: error in addLaneWidening";
                return 1;
            }

            //restricted area
            if (getChildWithName(itt, "restrictedArea") != NULL)
            {
                double ds2 = setting.laneChange.ds;
                if (attributeExits(getChildWithName(itt, "restrictedArea"),"length"))
                    ds2 = readIntAttrFromNode(getChildWithName(itt, "restrictedArea"),"length");

                if (addRestrictedAreaWidening(r.laneSections, side, s, ds, ds2))
                {
                    cerr << "ERR: error in addRestrictedAreaWidening" << endl;
                    return 1;
                }
            }
        }
        if (readNameFromNode(itt) == "laneDrop")
        {
            int side = readIntAttrFromNode(itt, "side");

            if (side == 0)
            {
                cerr << "ERR: laneWidening with side = 0" << endl;
                return 1;
            }

            double s =  readDoubleAttrFromNode(itt, "s");

            double ds = setting.laneChange.ds;
            if (attributeExits(itt, "length"))
                ds =  readDoubleAttrFromNode(itt, "length");

            // only perform drop if on road length
            if (s > r.length)
                continue;

            if (addLaneDrop(r.laneSections, side, s, ds))
            {
                cerr << "ERR: error in addLaneDrop";
                return 1;
            }

            //restricted area
            if (getChildWithName(itt, "restrictedArea") != NULL)
            {
                double ds2 = setting.laneChange.ds;
                if (attributeExits(getChildWithName(itt, "restrictedArea"),"length"))
                    ds2 = readIntAttrFromNode(getChildWithName(itt, "restrictedArea"),"length");

                if (addRestrictedAreaDrop(r.laneSections, side, s, ds, ds2))
                {
                    cerr << "ERR: error in addRestrictedAreaDrop";
                    return 1;
                }
            }
        }
    }

    // --- automatic generated laneWidening --------------------------------------

    double widening_s = setting.laneChange.s;
    double widening_ds = setting.laneChange.ds;
    string active = "main";

    if (automaticWidening != NULL)
    {
        cout << "in automatic widening" << endl;
        if (attributeExits(automaticWidening,"active"))
        {
            active = readStrAttrFromNode(automaticWidening, "active");

            if (attributeExits(automaticWidening,"length"))
                widening_s = readDoubleAttrFromNode(automaticWidening, "length");

            if (attributeExits(automaticWidening,"ds"))
                widening_ds = readDoubleAttrFromNode(automaticWidening, "ds");
        }

        bool restricted = false;
        if (readBoolAttrFromNode(automaticWidening, "restricted"))
            restricted = true;

        if (active == "all")
        {
            if (laneWideningJunction(r, widening_s, widening_ds, 1, true, restricted))
            {
                cerr << "ERR: error in laneWideningJunction";
                return 1;
            }
        }
        else if (active == "main" && r.classification == "main")
        {

            if (laneWideningJunction(r, widening_s, widening_ds, 1, true, restricted))
            {
                cerr << "ERR: error in laneWideningJunction";
                return 1;
            }
        }
        else if (active == "access" && r.classification == "access")
        {
            if (laneWideningJunction(r, widening_s, widening_ds, 1, true, restricted))
            {
                cerr << "ERR: error in laneWideningJunction";
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief function builds a road based on the input data
 * 
 * @param road              road input data
 * @param r                 road data
 * @param sStart            starting s position
 * @param sEnd              ending s position
 * @param automaticWidening automaticWiding input data
 * @param s0                position of s where x0, y0, phi0 should be reached
 * @param x0                reference x position
 * @param y0                reference y position
 * @param phi0              reference angle
 * @return int              error code
 */
int buildRoad(DOMElement* roadIn, road &r, double sStart, double sEnd, DOMElement* automaticWidening, double s0, double x0, double y0, double phi0)
{
    
    if(roadIn != NULL)
        r.classification = readStrAttrFromNode(roadIn,"classification");
    r.inputId = readIntAttrFromNode(roadIn, "id", true);

    // save geometry data from sStart - sEnd
    // mode = 1 -> in s direction
    // mode = 2 -> in opposite s direction
    int mode;

    if (sEnd >= sStart)
    {
        mode = 1;
        r.inputPos = "end";
    }
    else if (sEnd < sStart)
    {
        // switch sStart and sEnd but store this with mode = 2
        double tmp = sStart;
        sStart = sEnd;
        sEnd = tmp;
        mode = 2;
        r.inputPos = "start";
    }

    // generate geometries
    if (generateGeometries(roadIn, r, sStart, sEnd))
    {
        cerr << "ERR: error in generateGeometries";
        return 1;
    }

    // shift geometries
    if (shiftGeometries(r, sStart, sEnd, s0, x0, y0, phi0))
    {
        cerr << "ERR: error in shiftGeometries";
        return 1;
    }

    // flip geometries (convention: all roads point away from junction)
    if (mode == 2)
    {
        if (flipGeometries(r))
        {
            cerr << "ERR: error in flipGeometries";
            return 1;
        }
    }
    // add lanes
    if (addLanes(roadIn, r, mode))
    {
        cerr << "ERR: error in addLanes";
        return 1;
    }

    // // add lane section changes
    if (addLaneSectionChanges(roadIn, r, automaticWidening))
    {
        cerr << "ERR: error in addLaneSectionChanges";
        return 1;
    }

    return 0;
}
