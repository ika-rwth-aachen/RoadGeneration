// file generateRoad.h

#include "curve.h"

int generateRoad(pugi::xml_node geos, road &r, int mode, double s0, double phi0, double x0, double y0)
{
    double s = 0;
    double x = x0;
    double y = y0;
    double hdg = phi0;

    bool save = true;

    for (pugi::xml_node_iterator it = geos.child("referenceLine").begin(); it != geos.child("referenceLine").end(); ++it)
    {
        int type;
        double c = 0;
        double c1 = 0;
        double c2 = 0;

        if((string)it->attribute("type").value() == "line") type = 1;
        if((string)it->attribute("type").value() == "arc") type = 2;
        if((string)it->attribute("type").value() == "spiral") type = 3;

        double length = stod(it->attribute("length").value(),&sz);
        if (it->attribute("R")) c = 1 / stod(it->attribute("R").value(),&sz);
        if (it->attribute("Rs")) c1 = 1 / stod(it->attribute("Rs").value(),&sz);
        if (it->attribute("Re")) c2 = 1 / stod(it->attribute("Re").value(),&sz);

        double actualLength = length;
        bool complicatedCut = false;

        if (mode == 1)
        {
            if (s + length > s0) actualLength = s0 - s;
            if (actualLength <= 0) break;
        } 
        if (mode == 2)
        {
            if (s + length < s0) save = false;
            
            if (s < s0 && s + length > s0) 
            {
                save = true;
                complicatedCut = true;
                actualLength = s + length - s0;
            }
        } 
        
        // create new geometry
        geometry geo;
        geo.s = s;
        geo.x = x;
        geo.y = y;
        geo.hdg = hdg;
        geo.length = actualLength;
        geo.c = c;
        geo.c1 = c1;
        geo.c2 = c2;  
        geo.type = type;

        curve(length, c, c1, c2, x, y, hdg);

        // if complicated cut, fix s x y of geometry afterwards
        if(complicatedCut)
        {
            geo.s = 0;
            s = 0;
            curve(length-actualLength, c, c1, c2, geo.x, geo.y, geo.hdg);
        }
        if (save) 
        {
            r.length = s + actualLength;
            r.geometries.push_back(geo);
        }
        s += length;
    }

    for (pugi::xml_node_iterator it = geos.child("lanes").begin(); it != geos.child("lanes").end(); ++it)
    {
        laneSection laneSec;
        laneSec.id = it->attribute("id").as_int();
        laneSec.s = stod(it->attribute("s").value(),&sz);

        for (pugi::xml_node_iterator itt = it->begin(); itt != it->end(); ++itt)
        {
            lane l; 

            l.id = itt->attribute("laneId").as_int();
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

    /*
    // add lanes // TODO: consider values in input file
    laneSection laneSec;
    
    lane llane;
    llane.id = 1;
    laneSec.lanes.push_back(llane);

    lane clane;
    clane.id = 0;
    clane.rm.type = "broken";
    clane.w.a = 0;
    laneSec.lanes.push_back(clane);

    lane rlane;
    rlane.id = -1;
    laneSec.lanes.push_back(rlane);

    r.laneSections.push_back(laneSec);*/

    return 0;
}

