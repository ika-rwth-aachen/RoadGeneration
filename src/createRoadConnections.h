// file createRoadConnections.h

int createRoadConnection(road r1, road r2, road &r, junction &junc, int fromId, int toId, string laneMarkLeft, string laneMarkRight, string laneMarkMiddle)
{
    laneSection lS;

    r.junction = junc.id;
    r.predecessor.elementId = r1.id;
    r.successor.elementId   = r2.id;
    if (r.laneSections.size() == 0) r.laneSections.push_back(lS);

    // connect r1 with r2 at reference points  
    double x1,y1,hdg1,x2,y2,hdg2;
    geometry g1, g2;
    laneSection lS1, lS2;

    // compute starting point
    if (r1.predecessor.elementId > 100) 
    {
        g1 = r1.geometries.front(); 
        lS1 = r1.laneSections.front();
        x1 = g1.x; 
        y1 = g1.y; 
        hdg1 = g1.hdg + M_PI; 
        fixAngle(hdg1);
    }

    if (r1.successor.elementId > 100) 
    {
        g1 = r1.geometries.back(); 
        lS1 = r1.laneSections.back();
        x1 = g1.x; 
        y1 = g1.y; 
        hdg1 = g1.hdg; 
        curve(g1.length,g1,x1,y1,hdg1,1);
    }

    connection con1;
    con1.id = junc.connections.size() + 1;
    con1.from = r1.id;
    con1.to = r.id;
    junc.connections.push_back(con1);

    // compute ending point
    if (r2.predecessor.elementId > 100)
    {
        g2 = r2.geometries.front(); 
        lS2 = r2.laneSections.front();
        x2 = g2.x; 
        y2 = g2.y; 
        hdg2 = g2.hdg; 
    }

    if (r2.successor.elementId > 100) 
    {
        g2 = r2.geometries.back(); 
        lS2 = r2.laneSections.back();
        x2 = g2.x; 
        y2 = g2.y; 
        hdg2 = g2.hdg; 
        curve(g2.length,g2,x2,y2,hdg2,1);

        hdg2 = hdg2 + M_PI; 
        fixAngle(hdg2);
    }

    connection con2;
    con2.id = junc.connections.size() + 1;
    con2.from = r.id;
    con2.to = r2.id;
    junc.connections.push_back(con2);

    // --- compute connecting road ---------------------------------------------
    double a = hdg2-hdg1;
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
        g.hdg = atan2(y2-y1,x2-x1);

        g.length = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
        g.type = 1;

        r.geometries.push_back(g);
        r.length = g.length;
    }
    // combine an arc with a line to connect both points
    else 
    {
        double m1 = tan(hdg1);
        double b1 = y1 - m1 * x1;

        double m2 = tan(hdg2);
        double b2 = y2 - m2 * x2;

        // intersection point of lines
        double ipX = (b2-b1)/(m1-m2);
        double ipY = m1*(b2-b1)/(m1-m2) + b1;

        double d1 = sqrt(pow(ipX-x1,2)+pow(ipY-y1,2));
        double d2 = sqrt(pow(ipX-x2,2)+pow(ipY-y2,2));

        // additional line
        geometry g1;
        g1.s = 0;
        g1.c = 0;
        g1.c1 = 0;
        g1.c2 = 0;
        g1.type = 1;

        // line at start
        if (d1 > d2)
        {
            g1.x = x1;
            g1.y = y1;
            g1.hdg = hdg1;
            g1.length = d1 - d2;

            x1 = g1.x + cos(hdg1) * (d1-d2);
            y1 = g1.y + sin(hdg1) * (d1-d2);
        }
        // line at end
        if (d1 < d2)
        {
            g1.x = x2 - cos(hdg2) * (d2-d1);
            g1.y = y2 - sin(hdg2) * (d2-d1);
            g1.hdg = hdg2;
            g1.length = d2 - d1;

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

        double d = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
        double R = (d / 2) / sin(a / 2);

        g2.c = 1/R;
        g2.length = abs(R * a);
        g2.type = 2;

        if (d1 > d2) 
        {
            g2.s = g1.length; 
            r.geometries.push_back(g1);    
            r.geometries.push_back(g2);    

        }
        else if (d1 < d2) 
        {
            g1.s = g2.length;
            r.geometries.push_back(g2);    
            r.geometries.push_back(g1);    
        }
        else
        {
            r.geometries.push_back(g2);    
        }
        
        if (d1 != d2) r.length += g1.length;
        r.length += g2.length;
    }

    // --- lanemarkings in crossing section ------------------------------------
    laneSection laneSec;

    string left = laneMarkLeft;
    string right = laneMarkRight;
    createLaneConnection(r,lS1,lS2,fromId,toId,left,right);
               
    return 0;
}