/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file createRoadConnection.h
 *
 * @brief file contains method for generating road connection in junction area
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de
 *
 */
extern settings setting;
/**
 * @brief function creates a new road connection 
 * 
 * @param r1                road at start point
 * @param r2                road at end point
 * @param r                 resulting road which is connection from r1 to r2
 * @param junc              current junction 
 * @param fromId            start lane Id
 * @param toId              end lane Id
 * @param laneMarkLeft      left roadmarking
 * @param laneMarkRight     right roadmarking
 * @return int              error code
 */
int createRoadConnection(road r1, road r2, road &r, junction &junc, int fromId, int toId, string laneMarkLeft, string laneMarkRight)
{
    laneSection lS;
    if (r.laneSections.size() == 0)
        r.laneSections.push_back(lS);

    r.junction = junc.id;
    r.predecessor.id = r1.id;
    r.successor.id = r2.id;
    r.predecessor.elementType = roadType;
    r.successor.elementType = roadType;
    r.predecessor.contactPoint = startType;
    r.successor.contactPoint = startType;
    
   
    // connect r1 with r2 at reference points
    double x1, y1, hdg1, x2, y2, hdg2, s1, s2;
    geometry g1, g2;
    laneSection lS1, lS2;

    // --- compute starting point ----------------------------------------------
    if (r1.predecessor.elementType == junctionType)
    {
        g1 = r1.geometries.front();
        lS1 = r1.laneSections.front();
        x1 = g1.x;
        y1 = g1.y;
        s1 = 0;
        hdg1 = g1.hdg + M_PI;

        fixAngle(g1.hdg);
        fixAngle(hdg1);
    }

    if (r1.successor.elementType == junctionType)
    {
        g1 = r1.geometries.back();
        lS1 = r1.laneSections.back();
        x1 = g1.x;
        y1 = g1.y;
        s1 = r1.length - lS1.s;
        hdg1 = g1.hdg;
        curve(g1.length, g1, x1, y1, hdg1, 1);

        fixAngle(hdg1);
        g1.hdg = hdg1;
    }

    // connection between starting road and current road
    connection con1;
    con1.id = junc.connections.size() + 1;
    con1.contactPoint = startType;
    con1.from = r1.id;
    con1.to = r.id;
    con1.fromLane = fromId;
    con1.toLane = sgn(toId);
    junc.connections.push_back(con1);

    // --- compute ending point ------------------------------------------------
    if (r2.predecessor.elementType == junctionType)
    {
        g2 = r2.geometries.front();
        lS2 = r2.laneSections.front();
        x2 = g2.x;
        y2 = g2.y;
        s2 = 0;
        hdg2 = g2.hdg;

        fixAngle(hdg2);
        g2.hdg = hdg2;
    }

    if (r2.successor.elementType == junctionType)
    {
        g2 = r2.geometries.back();
        lS2 = r2.laneSections.back();
        x2 = g2.x;
        y2 = g2.y;
        s2 = r2.length - lS2.s;
        hdg2 = g2.hdg;
        curve(g2.length, g2, x2, y2, hdg2, 1);

        g2.hdg = hdg2;
        fixAngle(g2.hdg);

        hdg2 = hdg2 + M_PI;
        fixAngle(hdg2);
    }

    // connection between current road and ending road
    connection con2;
    con2.id = junc.connections.size() + 1;
    con2.contactPoint = startType; //Switched this from endpoint to startpoint!
    con2.from = r2.id; //switched r to r2 here to be consistent with the user guide. 
    con2.to = r.id;    //switched r2 to r
    con2.fromLane = sgn(toId);
    con2.toLane = toId;
    junc.connections.push_back(con2); // might cause unintended linkage. 

    // --- correct laneOffset --------------------------------------------------
    double w1 = lS1.o.a + s1 * lS1.o.b + s1 * s1 * lS1.o.c + s1 * s1 * s1 * lS.o.d;
    double w2 = lS2.o.a + s2 * lS2.o.b + s2 * s2 * lS2.o.c + s2 * s2 * s2 * lS.o.d;

    x1 += cos(g1.hdg + M_PI / 2) * w1;
    y1 += sin(g1.hdg + M_PI / 2) * w1;

    x2 += cos(g2.hdg + M_PI / 2) * w2;
    y2 += sin(g2.hdg + M_PI / 2) * w2;

    // --- update start end endpoint according to lane position ----------------
    double phi1 = 0;
    double phi2 = 0;
    double t1 = findTOffset(lS1, fromId - sgn(fromId), 0);
    double t2 = findTOffset(lS2, toId - sgn(toId), 0);

    if (t1 > 0)
        phi1 = g1.hdg + M_PI / 2;
    if (t1 < 0)
        phi1 = g1.hdg - M_PI / 2;
    fixAngle(phi1);

    if (t2 > 0)
        phi2 = g2.hdg + M_PI / 2;
    if (t2 < 0)
        phi2 = g2.hdg - M_PI / 2;
    fixAngle(phi2);

    x1 += cos(phi1) * fabs(t1);
    y1 += sin(phi1) * fabs(t1);

    x2 += cos(phi2) * fabs(t2);
    y2 += sin(phi2) * fabs(t2);

    // --- compute connecting road ---------------------------------------------
    double a = hdg2 - hdg1;
    fixAngle(a);

    // simple line if angles are almost the same
    if (abs(a) < 0.1)
    {
        
        geometry g;
        g.s = 0;
        g.c = 0;
        g.c1 = 0;
        g.c2 = 0;
        g.x = x1;
        g.y = y1;
        g.hdg = atan2(y2 - y1, x2 - x1);

        g.length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        g.type = line;
        r.geometries.push_back(g);
        r.length = g.length;
    }
    // combine an arc with a line to connect both points
    //  -> maybe better a composite curve
    else
    {
        double m1 = tan(hdg1);
        double b1 = y1 - m1 * x1;

        double m2 = tan(hdg2);
        double b2 = y2 - m2 * x2;

        // intersection point of lines
        double ipX = (b2 - b1) / (m1 - m2);
        double ipY = m1 * (b2 - b1) / (m1 - m2) + b1;

        double d1 = sqrt(pow(ipX - x1, 2) + pow(ipY - y1, 2));
        double d2 = sqrt(pow(ipX - x2, 2) + pow(ipY - y2, 2));

        // additional line
        geometry g1;
        g1.s = 0;
        g1.c = 0;
        g1.c1 = 0;
        g1.c2 = 0;
        g1.type = line;
        g1.length = abs(d2 - d1);
        double minLength = setting.minConnectingRoadLength;  //min length for line segment. below that it will get disregarded

        // line at start
        if (d1 > d2)
        {
            g1.x = x1;
            g1.y = y1;
            g1.hdg = hdg1;
            
            x1 = g1.x + cos(hdg1) * (d1 - d2);
            y1 = g1.y + sin(hdg1) * (d1 - d2);
        }
        // line at end
        if (d1 < d2)
        {
            g1.x = x2 - cos(hdg2) * (d2 - d1);
            g1.y = y2 - sin(hdg2) * (d2 - d1);
            g1.hdg = hdg2;

            x2 = g1.x;
            y2 = g1.y;
        }

        // arc
        geometry g2;
        g2.s = 0;
        g2.c = 0;
        g2.c1 = 0;
        g2.c2 = 0;
        g2.x = x1;
        g2.y = y1;
        g2.hdg = hdg1;

        // compute curvature and length of arc
        double d = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        double R = (d / 2) / sin(a / 2);

        g2.c = 1 / R;
        g2.length = abs(R * a);
        g2.type = arc;
        
        // first line then arc
        if (d1 - d2 > minLength)
        {
            g2.s = g1.length;
            r.geometries.push_back(g1);
            r.geometries.push_back(g2);
        }
        // first arc then line
        else if (d2 - d1 > minLength)
        {
            g1.s = g2.length;
            r.geometries.push_back(g2);
            r.geometries.push_back(g1);
        }
        // only arc is necessary
        else
        {
            r.geometries.push_back(g2);
        }

        if (d1 != d2)
            r.length += g1.length;
        r.length += g2.length;
    }

    // --- lanemarkings in crossing section ------------------------------------
    if (createLaneConnection(r, lS1, lS2, fromId, toId, laneMarkLeft, laneMarkRight))
    {
        cerr << "Error in createLaneConnection" << endl;
        exit(0);
        return 1;
    }

    return 0;
}