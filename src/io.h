// file io.h

#include "pugixml.hpp"
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/validators/common/Grammar.hpp>

using namespace xercesc;

/**
 * @brief function which checks the input file against the corresponding xsd
 * 
 * @param file  input file
 * @return int  error code
 */
int validateInput (char* file)
{
    XMLPlatformUtils::Initialize();
	string tmp = string_format("%s/xml/input.xsd", PROJ_DIR);
	const char* schemaFilePath = tmp.c_str();
	const char* xmlFilePath = file;

	XercesDOMParser domParser;
    if (domParser.loadGrammar(schemaFilePath, Grammar::SchemaGrammarType) == NULL)
    {
        fprintf(stderr, "couldn't load schema\n");
        return 1;
    }
 
    domParser.setValidationScheme(XercesDOMParser::Val_Auto);
    domParser.setDoNamespaces(true);
    domParser.setDoSchema(true);
    domParser.setValidationConstraintFatal(true);
	
    domParser.parse(xmlFilePath);

    if (domParser.getErrorCount() == 0)
        printf("XML file validated against the schema successfully\n");
    else
    {
        printf("XML file doesn't conform to the schema\n");
        return 1;
    }

    return 0;
}

/**
 * @brief function which checks the output file against the corresponding xsd
 * 
 * @param file  output file
 * @return int  error code
 */
int validateOutput (string file)
{
    XMLPlatformUtils::Initialize();

    file.append(".xodr");
	const char* xmlFilePath = file.c_str();

	string tmp = string_format("%s/xml/output.xsd", PROJ_DIR);	
	const char* schemaFilePath = tmp.c_str();

	XercesDOMParser domParser;
    if (domParser.loadGrammar(schemaFilePath, Grammar::SchemaGrammarType) == NULL)
    {
        fprintf(stderr, "couldn't load schema\n");
        return 1;
    }
 
    domParser.setValidationScheme(XercesDOMParser::Val_Auto);
    domParser.setDoNamespaces(true);
    domParser.setDoSchema(true);
    domParser.setValidationConstraintFatal(true);
	
    domParser.parse(xmlFilePath);
    if (domParser.getErrorCount() == 0)
        printf("XML file validated against the schema successfully\n");
    else
    {
        printf("XML file doesn't conform to the schema\n");
        return 1;
    }

    return 0;
}

/**
 * @brief function parses the input xml file with the external tool pugi_xml
 *  input is then accessable in a pugi::xml_document tree structure 
 * 
 * @param doc   tree structure which contains the data of the xml input file
 * @param data  roadNetwork data which will be stored as openDrive format
 * @param file  xml input file
 * @return int  errorcode
 */
int parseXML(pugi::xml_document &doc, roadNetwork &data, char * file)
{   
    // TODO: only debugging purposes
    if (true)
    {  // TODO: use PROJ_DIR!
        data.file = "bin/test.xml";
        doc.load_file("bin/test.xml");
        return 0;
    }
    string f = file;
    data.file = f.substr(0,f.find(".xml"));

	if (doc.load_file(file)) 
    {
        return 0;
    }
    else 
    {
        cerr << "ERR: InputFile not found" << endl;
        return 1;
    }
}

/**
 * @brief function stores the generated structure of type roadNetwork as a openDrive format
 * 
 * @param doc   tree structure which contains the generated data in openDrive format
 * @param data  generated data by this tool
 * @return int  errorcode
 */
int createXML(pugi::xml_document &doc, roadNetwork data)
{   
    pugi::xml_node root = doc.append_child("OpenDRIVE");

    // specify location of .xsd file
    if (false)
    {
        root.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
	    string tmp = string_format("%s/xml/output.xsd", PROJ_DIR);
        root.append_attribute("xsi:noNamespaceSchemaLocation") = tmp.c_str();
    }
    
    // write header
    pugi::xml_node header = root.append_child("header");
    header.append_attribute("revMajor") = data.versionMajor;
    header.append_attribute("revMinor") = data.versionMinor;

    // --- write roads ---------------------------------------------------------
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
        road.child("type").append_attribute("type") = it->type.c_str();


        // --- write geometries ------------------------------------------------
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
        

        // --- write lanes -----------------------------------------------------
        pugi::xml_node lanes = road.append_child("lanes");

        for (std::vector<laneSection>::iterator itt = it->laneSections.begin() ; itt != it->laneSections.end(); ++itt)
        {
            pugi::xml_node laneOffset = lanes.append_child("laneOffset");

            laneOffset.append_attribute("s") = itt->s;
            laneOffset.append_attribute("a") = itt->o.a;
            laneOffset.append_attribute("b") = itt->o.b;
            laneOffset.append_attribute("c") = itt->o.c;
            laneOffset.append_attribute("d") = itt->o.d;
        }

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

                if (ittt->id != 0)
                {
                    pugi::xml_node material = lane.append_child("material");
                    material.append_attribute("sOffset") = ittt->m.s;
                    material.append_attribute("surface") = ittt->m.surface.c_str();
                    material.append_attribute("friction") = ittt->m.friction;
                    material.append_attribute("roughness") = ittt->m.roughness;
                
                    pugi::xml_node speed = lane.append_child("speed");
                    speed.append_attribute("sOffset") = 0;
                    speed.append_attribute("max") = ittt->speed;
                }
            }
        }


        // --- write objects ---------------------------------------------------
        pugi::xml_node objects = road.append_child("objects");

        for (std::vector<object>::iterator itt = it->objects.begin() ; itt != it->objects.end(); ++itt)
        {
            object o = *itt;
            pugi::xml_node obj = objects.append_child("object");
            
            obj.append_attribute("type") = o.type.c_str();
            obj.append_attribute("name") = o.type.c_str();
            obj.append_attribute("dynamic") = "no";
            obj.append_attribute("id") = o.id;
            obj.append_attribute("s") = o.s;
            obj.append_attribute("t") = o.t;
            obj.append_attribute("zOffset") = o.z;
            obj.append_attribute("hdg") = o.hdg;
            obj.append_attribute("pitch") = 0;
            obj.append_attribute("roll") = 0;
            obj.append_attribute("validLength") = 0;
            obj.append_attribute("orientation") = o.orientation.c_str();
            obj.append_attribute("length") = o.length;
            obj.append_attribute("width") = o.width;
            obj.append_attribute("height") = o.height;

            if (o.repeat)
            {
                obj = obj.append_child("repeat");
                obj.append_attribute("s") = o.s;
                obj.append_attribute("length") = o.len;
                obj.append_attribute("distance") = o.distance;
                obj.append_attribute("tStart") = o.t;
                obj.append_attribute("tEnd") = o.t;
                obj.append_attribute("widthStart") = o.width;
                obj.append_attribute("widthEnd") = o.width;
                obj.append_attribute("heightStart") = o.height;
                obj.append_attribute("heightEnd") = o.height;
                obj.append_attribute("zOffsetStart") = o.z;
                obj.append_attribute("zOffsetEnd") = o.z;
                obj.append_attribute("lengthStart") = o.length;
                obj.append_attribute("lengthEnd") = o.length;  
            }
        }


        // --- write signals ---------------------------------------------------
    
        // signals format is different in version 1.4
        if (data.versionMajor >= 1 && data.versionMinor >= 5)
        {
            pugi::xml_node signals = road.append_child("signals");

            for (std::vector<Signal>::iterator itt = it->signals.begin() ; itt != it->signals.end(); ++itt)
            {
                Signal s = *itt;
                pugi::xml_node sig = signals.append_child("signal");
                //pugi::xml_node sig = objects.append_child("object");
                
                sig.append_attribute("id") = s.id;
                
                sig.append_attribute("name") = s.type.c_str();
                sig.append_attribute("type") = s.type.c_str();
                
                sig.append_attribute("subtype") = "-";
                sig.append_attribute("s") = s.s;
                sig.append_attribute("t") = s.t;
                sig.append_attribute("zOffset") = s.z;
                sig.append_attribute("orientation") = s.orientation.c_str();
                if (s.dynamic) sig.append_attribute("dynamic") = "yes";
                else sig.append_attribute("dynamic") = "no";
                sig.append_attribute("value") = s.value;
                sig.append_attribute("width") = s.width;
                sig.append_attribute("height") = s.height;
            }
        }
    }
        
    // --- write controllers ---------------------------------------------------
    
    // controllers format is different in version 1.4
    if (data.versionMajor >= 1 && data.versionMinor >= 5)
    {        
        for (std::vector<control>::iterator it = data.controller.begin() ; it != data.controller.end(); ++it)
        {
            pugi::xml_node controller = root.append_child("controller");

            controller.append_attribute("id") = it->id;

            for (std::vector<Signal>::iterator itt = it->signals.begin() ; itt != it->signals.end(); ++itt)
            {
                pugi::xml_node con = controller.append_child("control");

                con.append_attribute("signalId") = itt->id;
            }
        }
    }

    // --- write junctions -----------------------------------------------------
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

    // --- write doc structure to file -----------------------------------------
    string file = data.file;
    file.append(".xodr");

    if (doc.save_file(file.c_str())) 
    {
        return 0;
    }
    else 
    {
        cerr << "ERR: file could not be saved." << endl;
        return 1;
    }
}