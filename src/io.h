// file io.h

int parseXML(pugi::xml_document &doc)
{   
	//if (doc.load_file("../xml/test.xml")) return 0;
	if (doc.load_file("test.xml")) return 0;
    else return -1;

}

int createXML(pugi::xml_document &doc, roadNetwork data)
{   
    pugi::xml_node root = doc.append_child("OpenDRIVE");

    for (std::vector<road>::iterator it = data.roads.begin() ; it != data.roads.end(); ++it)
    {
        pugi::xml_node road = root.append_child("road");
        
        road.append_attribute("length") = it->length;
        road.append_attribute("id") = it->id;
        road.append_attribute("junction") = it->junction;

    
        pugi::xml_node pre = road.append_child("link").append_child("predecessor");
        pre.append_attribute("elementType") = it->predecessor.elementType.c_str();
        pre.append_attribute("elementId") = it->predecessor.elementId;
        pre.append_attribute("contactPoint") = it->predecessor.contactPoint.c_str();
        
        pugi::xml_node suc = road.child("link").append_child("successor");
        suc.append_attribute("elementType") = it->successor.elementType.c_str();
        suc.append_attribute("elementId") = it->successor.elementId;
        suc.append_attribute("contactPoint") = it->successor.contactPoint.c_str();
        
        road.append_child("type").append_attribute("s") = "0";
        road.child("type").append_attribute("type") = "town";

        pugi::xml_node planView = road.append_child("planView");

        for (std::vector<geometry>::iterator itt = it->geometries.begin() ; itt != it->geometries.end(); ++itt)
        {
            pugi::xml_node geo = planView.append_child("geometry");

            geo.append_attribute("s") = itt->s;
            geo.append_attribute("x") = itt->x;
            geo.append_attribute("y") = itt->y;
            geo.append_attribute("hdg") = itt->hdg;
            geo.append_attribute("length") = itt->length;

            if (itt->type == 1)
            {
                geo.append_child("line");
            }
            if (itt->type == 2)
            {
                geo.append_child("arc").append_attribute("curvature") = itt->c;
            }
            if (itt->type == 3)
            {
                pugi::xml_node spiral = geo.append_child("spiral");
                spiral.append_attribute("curvStart") = itt->c1;
                spiral.append_attribute("curvEnd") = itt->c2;
            }
        }
        
        
        pugi::xml_node lanes = road.append_child("lanes");

        for (std::vector<laneSection>::iterator itt = it->laneSections.begin() ; itt != it->laneSections.end(); ++itt)
        {
            pugi::xml_node laneSection = lanes.append_child("laneSection");
            laneSection.append_attribute("s") = itt->s;

            for (std::vector<lane>::iterator ittt = itt->lanes.begin() ; ittt != itt->lanes.end(); ++ittt)
            {
                pugi::xml_node lane;
                if (ittt->id > 0)  
                lane = laneSection.append_child("left").append_child("lane");
                if (ittt->id < 0)  
                lane = laneSection.append_child("right").append_child("lane");
                if (ittt->id == 0) 
                lane = laneSection.append_child("center").append_child("lane");
                
                lane.append_attribute("id") = ittt->id;
                lane.append_attribute("type") = ittt->type.c_str();
                lane.append_attribute("level") = ittt->level;

                /*pugi::xml_node link = lane.append_child("link");

                link.append_child("predecessor").append_attribute("id") = ittt->predecessor;
                link.append_child("successor").append_attribute("id") = ittt->successor;*/

                pugi::xml_node width = lane.append_child("width");

                width.append_attribute("sOffset") = ittt->w.s;
                width.append_attribute("a") = ittt->w.a;
                width.append_attribute("b") = ittt->w.b;
                width.append_attribute("c") = ittt->w.c;
                width.append_attribute("s") = ittt->w.d;

                pugi::xml_node roadmark = lane.append_child("roadMark");

                roadmark.append_attribute("sOffset") = ittt->rm.s;
                roadmark.append_attribute("type") = ittt->rm.type.c_str();
                roadmark.append_attribute("weight") = ittt->rm.weight.c_str();
                roadmark.append_attribute("color") = ittt->rm.color.c_str();
                roadmark.append_attribute("width") = ittt->rm.width;
            }
        }

/*
        pugi::xml_node laneSection = road.append_child("lanes").append_child("laneSection");
        laneSection.append_attribute("s") = 0;

        pugi::xml_node lane = laneSection.append_child("center").append_child("lane");

        lane.append_attribute("id") = 0;
        lane.append_attribute("type") = "driving";
        lane.append_attribute("level") = 0;

        pugi::xml_node width = lane.append_child("width");

        width.append_attribute("sOffset") = 0;
        width.append_attribute("a") = 3.5;
        width.append_attribute("b") = 0;
        width.append_attribute("c") = 0;
        width.append_attribute("d") = 0;

        pugi::xml_node roadMark = lane.append_child("roadMark");
        roadMark.append_attribute("sOffset") = 0;
        roadMark.append_attribute("type") = "solid";
        roadMark.append_attribute("weight") = "standard";
        roadMark.append_attribute("color") = "white";
        roadMark.append_attribute("width") = "0.2";*/
    }
        

    for (std::vector<junction>::iterator it = data.junctions.begin() ; it != data.junctions.end(); ++it)
    {
        pugi::xml_node junc = root.append_child("junction");

        junc.append_attribute("id") = it->id;

        for (std::vector<connection>::iterator itt = it->connections.begin() ; itt != it->connections.end(); ++itt)
        {
            pugi::xml_node con = junc.append_child("connection");

            con.append_attribute("id") = itt->id;
            con.append_attribute("incomingRoad") = itt->from;
            con.append_attribute("connetingRoad") = itt->to;
            con.append_attribute("contactPoint") = itt->contactPoint.c_str();
        }
    }

    // TODO remove
    doc.print(std::cout);

    //if (doc.save_file("../xml/output.xodr")) return 0;
    if (doc.save_file("output.xodr")) return 0;
    else return -1;
}