// file io.h

/**
 * @brief function parses the input xml file with the external tool pugi_xml
 *  input in then accessable in a pugi::xml_document tree structure 
 * 
 * @param doc   tree structure which contains the data of the xml input file
 * @param data  roadNetwork data which will be stored as openDrive format
 * @param file  xml input file
 * @return int  errorcode
 */
int parseXML(pugi::xml_document &doc, roadNetwork &data, char * file)
{   
    if (false)
    {
        data.file = "bin/all.xml";
        doc.load_file("bin/all.xml");
        return 0;
    }

    data.file = file;
	if (doc.load_file(file)) 
    {
        return 0;
    }
    else {
        cerr << "ERR: InputFile not found" << endl;
        return 1;
    }
}

/**
 * @brief function stores the generated structure of type roadNetwork as a openDrive format
 * 
 * @param doc   tree structure which contains the generated data in openDrive format
 * @param data  generated data by the tool
 * @return int  errorcode
 */
int createXML(pugi::xml_document &doc, roadNetwork data)
{   
    pugi::xml_node root = doc.append_child("OpenDRIVE");

    root.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
    root.append_attribute("xsi:noNamespaceSchemaLocation") = "../xml/output.xsd";
    
    // write header
    pugi::xml_node header = root.append_child("header");
    header.append_attribute("revMajor") = 1;
    header.append_attribute("revMinor") = 5;

    // write roads
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

        // write geometries
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

        // write lanes        
        for (std::vector<laneSection>::iterator itt = it->laneSections.begin() ; itt != it->laneSections.end(); ++itt)
        {
            pugi::xml_node laneSection = lanes.append_child("laneSection");
            laneSection.append_attribute("s") = itt->s;

            pugi::xml_node left;
            pugi::xml_node center;
            pugi::xml_node right;

            if(findMaxLaneId(*itt) > 0) left = laneSection.append_child("left");
            center = laneSection.append_child("center");
            if(findMinLaneId(*itt) < 0) right = laneSection.append_child("right");

            for (std::vector<lane>::iterator ittt = itt->lanes.begin() ; ittt != itt->lanes.end(); ++ittt)
            {
                pugi::xml_node lane;

                if (ittt->id > 0)
                {
                    lane = left.append_child("lane");
                }  
                if (ittt->id < 0)  
                {
                    lane = right.append_child("lane");
                } 
                if (ittt->id == 0) 
                {
                    lane = center.append_child("lane");
                }
                
                lane.append_attribute("id") = ittt->id;
                lane.append_attribute("type") = ittt->type.c_str();

                //pugi::xml_node link = lane.append_child("link");

                if (ittt->id != 0)
                {
                    pugi::xml_node width = lane.append_child("width");
                    width.append_attribute("sOffset") = ittt->w.s;
                    width.append_attribute("a") = ittt->w.a;
                    width.append_attribute("b") = ittt->w.b;
                    width.append_attribute("c") = ittt->w.c;
                    width.append_attribute("d") = ittt->w.d;
                }
                pugi::xml_node roadmark = lane.append_child("roadMark");

                roadmark.append_attribute("sOffset") = ittt->rm.s;
                roadmark.append_attribute("type") = ittt->rm.type.c_str();
                roadmark.append_attribute("weight") = ittt->rm.weight.c_str();
                roadmark.append_attribute("color") = ittt->rm.color.c_str();
                roadmark.append_attribute("width") = ittt->rm.width;
            }
        }
    }
        
    // write junctions
    for (std::vector<junction>::iterator it = data.junctions.begin() ; it != data.junctions.end(); ++it)
    {
        pugi::xml_node junc = root.append_child("junction");

        junc.append_attribute("id") = it->id;

        for (std::vector<connection>::iterator itt = it->connections.begin() ; itt != it->connections.end(); ++itt)
        {
            pugi::xml_node con = junc.append_child("connection");

            con.append_attribute("id") = itt->id;
            con.append_attribute("incomingRoad") = itt->from;
            con.append_attribute("connectingRoad") = itt->to;
            con.append_attribute("contactPoint") = itt->contactPoint.c_str();

            pugi::xml_node lL = con.append_child("laneLink");
            lL.append_attribute("from") = itt->fromLane;
            lL.append_attribute("to") = itt->toLane;
        }
    }

    // PRINT: doc.print(std::cout);

    // write doc to file
    string file = data.file.substr(0,data.file.find(".xml"));
    file.append(".xodr");

    if (doc.save_file(file.c_str())) 
    {
        return 0;
    }
    else {
        cerr << "ERR: file could not be saved." << endl;
        return 1;
    }
}