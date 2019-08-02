// file createRoadConnections.h

int createRoadConnection(road r1, road r2, road &r, junction &junc)
{
    // connect r1 with r2   

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
        curve(g1.length,g1.c,g1.c1,g1.c2,x1,y1,hdg1);
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
        hdg2 = g2.hdg + M_PI; 
        fixAngle(hdg2);
        curve(g2.length,g2.c,g2.c1,g2.c2,x2,y2,hdg2);
    }

    connection con2;
    con2.id = junc.connections.size() + 1;
    con2.from = r.id;
    con2.to = r2.id;
    junc.connections.push_back(con2);

    // compute connecting road
    double d = sqrt(pow(x2-x1,2)+pow(y2-y1,2));
    double a = hdg2-hdg1;
    fixAngle(a);

    geometry g;

    g.s = 0;
    g.c = 0;
    g.c1 = 0;
    g.c2 = 0;
    g.x = x1;
    g.y = y1;
    g.hdg = hdg1;

    if (a == 0) 
    {
        g.length = d;
        g.type = 1;
    }
    else 
    {
        double R = (d / 2) / sin(a / 2);
        g.c = 1/R;
        g.length = abs(R * a);
        g.type = 2;
    }

    double xx = x1;
    double yy = y1; 
    double phiphi = hdg1;
    curve(g.length,g.c,g.c1,g.c2,xx,yy,phiphi);

    r.geometries.push_back(g);
    r.length = g.length;

    // add lanes // TODO: consider values in input file

    // TODO lanemarkings in crossing section
    laneSection laneSec;
    
    if (lS1.lanes.size() != lS2.lanes.size()) 
        cout << "ERR: not implemented" << endl; 

    for (int i = 0; i < lS1.lanes.size(); i++)
    {
        lane l;
        l.id = -ceil(lS1.lanes.size()/2) + i;
        l.rm.type = "none";
        l.w.a = (lS1.lanes[i].w.a + lS1.lanes[i].w.a) / 2;
        
        laneSec.lanes.push_back(l);
    }

    r.laneSections.push_back(laneSec);

    return 0;
}