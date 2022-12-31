/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * 
 * @file xml.cpp
 *
 * @brief file contains the import and export, as well as the validation, of xml files
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <algorithm>
#include "xmlParser.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace std;
using namespace xercesc;

extern settings setting;

/**
 * @brief function checks the input file against the corresponding input.xsd
 * 
 * @param file  input file
 * @return int  error code
 */
int validateInput(char *file, xmlTree &xmlInput)
{

    string schema = string_format("%s/xml/input.xsd", PROJ_DIR);
    const char *schema_file = schema.c_str();
    const char *xml_file = file;

    if (xmlInput.loadGrammar(schema_file) == NULL)
    {
        cerr << "ERR: couldn't load schema" << endl;
        return 1;
    }

    xmlInput.parseDocument(xml_file);

    if (xmlInput.getErrorCount() == 0){
        //if(!setting.silentMode)
        cout << "XML input file validated against the schema successfully" << endl;
    }
    else
    {
        cerr << "ERR: XML input file doesn't conform to the schema" << endl;
        return 1;
    }

    return 0;
}

/**
 * @brief function checks the output file against the corresponding output.xsd
 * 
 * @param data  output data
 * @return int  error code
 */
int validateOutput(roadNetwork &data)
{
    // setup file
    string file = data.outputFile;
    file.append(".xodr");
    const char *xml_file = file.c_str();

    XMLPlatformUtils::Initialize();

    string schema = string_format("%s/xml/output.xsd", PROJ_DIR);
    const char *schema_path = schema.c_str();

    // load output file
    XercesDOMParser domParser;
    if (domParser.loadGrammar(schema_path, Grammar::SchemaGrammarType) == NULL)
    {
        cerr << "ERR: couldn't load schema" << endl;
        return 1;
    }

    //create error handler from custom error handler class
    xercesc::ErrorHandler *handler = new ValidationErrorHandler();

    // check output file
    domParser.setValidationScheme(XercesDOMParser::Val_Always);
    domParser.setDoNamespaces(true);
    domParser.setErrorHandler(handler);
    domParser.setDoSchema(true);
    domParser.setValidationConstraintFatal(true);

    domParser.parse(xml_file);
    if (domParser.getErrorCount() == 0){
        if(!setting.silentMode)
            cout << "XML output file validated against the schema successfully" << endl;
    }
    else
    {
        cerr << "ERR: XML output file doesn't conform to the schema" << endl;
        return 1;
    }

    return 0;
}

/**
 * @brief helper function to append the link node to road node
 * 
 * @param road the road xml node 
 * @param sucessor the potential successor to the road segment
 * @param sucessor the potential predecessor to the road segment * 
 */
void appendLinkToNodeXercesC(nodeElement road, link &successor, link &predecessor)
{
    if (successor.id == -1 && predecessor.id == -1){
        return;
    }
    nodeElement link("link");
    link.appendToNode(road);

    if( predecessor.id != -1){
        nodeElement pre("predecessor");
        pre.appendToNode(link);
        pre.addAttribute("elementId", predecessor.id);
        pre.addAttribute("elementType", getLinkType(predecessor.elementType).c_str());
        if(predecessor.contactPoint != noneType)
            pre.addAttribute("contactPoint", getContactPointType(predecessor.contactPoint).c_str());
    }

    if( successor.id != -1){
        nodeElement suc("successor");
        suc.appendToNode(link);
        suc.addAttribute("elementId", successor.id);
        suc.addAttribute("elementType", getLinkType(successor.elementType));
        if(successor.contactPoint != noneType)
            suc.addAttribute("contactPoint", getContactPointType(successor.contactPoint));
    }
}


int createXMLXercesC(roadNetwork &data)
{

    init("OpenDRIVE");

    DOMElement *root = getRootElement();

    nodeElement header("header");

    header.addAttribute("revMajor", to_string(setting.versionMajor).c_str());
    header.addAttribute("revMinor", to_string(setting.versionMinor).c_str());
    header.addAttribute("north", to_string(setting.north).c_str());
    header.addAttribute("south", to_string(setting.south).c_str());
    header.addAttribute("west", to_string(setting.west).c_str());
    header.addAttribute("east", to_string(setting.east).c_str());

    header.appendToNode(root);

     // geoReference tag
    nodeElement geoReference("geoReference");

    DOMCDATASection *cdata;
    generateCDATA("+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs", &cdata);
    geoReference.domelement->appendChild(cdata);
    geoReference.appendToNode(root);

     // --- write roads ---------------------------------------------------------
    for (std::vector<road>::iterator it = data.roads.begin(); it != data.roads.end(); ++it)
    {

        nodeElement road("road");
        road.appendToNode(root);
        road.addAttribute("id", it->id);
        road.addAttribute("length", it->length);
       
        //it has to be checked if it is a connecting road, since the junction attribute is missused as the original ID for connecting roads.
        road.addAttribute("junction" ,(it->isConnectingRoad) ? -1 : it->junction);
        appendLinkToNodeXercesC(road, it->successor, it->predecessor);

        nodeElement type("type");
        type.addAttribute("s", 0);
        type.addAttribute("type", it->type);
        type.appendToNode(road);


        // --- write geometries ------------------------------------------------
        nodeElement planView("planView");
        planView.appendToNode(road);

        for (std::vector<geometry>::iterator itt = it->geometries.begin(); itt != it->geometries.end(); ++itt)
        {
            nodeElement geo("geometry");
            geo.appendToNode(planView);

            geo.addAttribute("s", itt->s);
            geo.addAttribute("x", itt->x);
            geo.addAttribute("y", itt->y);
            geo.addAttribute("hdg", itt->hdg);
            geo.addAttribute("length", itt->length);

            if (itt->type == line)
            {
                nodeElement line("line");
                line.appendToNode(geo);
            }
            if (itt->type == arc)
            {
                nodeElement arc("arc");
                arc.addAttribute("curvature", itt->c);
                arc.appendToNode(geo);
            }
            if (itt->type == spiral)
            {
                nodeElement arc("spiral");
                arc.addAttribute("curvStart", itt->c1);
                arc.addAttribute("curvEnd", itt->c2);
                arc.appendToNode(geo);

            }
        }

        // --- write lanes -----------------------------------------------------
        nodeElement lanes("lanes");
        lanes.appendToNode(road);

        for (std::vector<laneSection>::iterator itt = it->laneSections.begin(); itt != it->laneSections.end(); ++itt)
        {
            nodeElement laneOffset("laneOffset");
            laneOffset.appendToNode(lanes);

            laneOffset.addAttribute("s", itt->s);
            laneOffset.addAttribute("a", itt->o.a);
            laneOffset.addAttribute("b", itt->o.b);
            laneOffset.addAttribute("c", itt->o.c);
            laneOffset.addAttribute("d", itt->o.d);
        }

        for (std::vector<laneSection>::iterator itt = it->laneSections.begin(); itt != it->laneSections.end(); ++itt)
        {
            nodeElement laneSection("laneSection");
            laneSection.appendToNode(lanes);
            laneSection.addAttribute("s", itt->s);

            nodeElement left("left");
            nodeElement center("center");
            nodeElement right("right");

            if (findMaxLaneId(*itt) > 0)
                left.appendToNode(laneSection);
            center.appendToNode(laneSection);
            if (findMinLaneId(*itt) < 0)
                right.appendToNode(laneSection);

            std::sort(itt->lanes.begin(), itt->lanes.end(), compareLanes);
            for (std::vector<lane>::iterator ittt = itt->lanes.begin(); ittt != itt->lanes.end(); ++ittt)
            {
                nodeElement lane("lane");

                if (ittt->id > 0)
                {
                    lane.appendToNode(left);
                }
                if (ittt->id < 0)
                {
                    lane.appendToNode(right);
                }
                if (ittt->id == 0)
                {
                    lane.appendToNode(center);
                }

                lane.addAttribute("id", ittt->id);
                lane.addAttribute("type", ittt->type);

                if (ittt->id != 0)
                {
                    nodeElement link("link");
                    link.appendToNode(lane);
                    if (ittt->preId != 0)
                    {
                        nodeElement pre ("predecessor");
                        pre.appendToNode(link);
                        pre.addAttribute("id", ittt->preId);
                    }
                    if (ittt->sucId != 0)
                    {
                        nodeElement suc("successor");
                        suc.appendToNode(link);
                        suc.addAttribute("id", ittt->sucId);
                    }

                    nodeElement width ("width");
                    width.appendToNode(lane);
                    width.addAttribute("sOffset", ittt->w.s);
                    width.addAttribute("a", ittt->w.a);
                    width.addAttribute("b", ittt->w.b);
                    width.addAttribute("c", ittt->w.c);
                    width.addAttribute("d", ittt->w.d);
                    
                }

                nodeElement roadmark = ("roadMark");
                roadmark.appendToNode(lane);
                roadmark.addAttribute("sOffset", ittt->rm.s);
                roadmark.addAttribute("type", ittt->rm.type.c_str());
                roadmark.addAttribute("weight", ittt->rm.weight.c_str());
                roadmark.addAttribute("color", ittt->rm.color.c_str());
                roadmark.addAttribute("width", (ittt->rm.width));

                if (ittt->id != 0)
                {
                    nodeElement material("material");
                    material.appendToNode(lane);
                    material.addAttribute("sOffset", ittt->m.s);
                    material.addAttribute("surface", ittt->m.surface);
                    material.addAttribute("friction", ittt->m.friction);
                    material.addAttribute("roughness", ittt->m.roughness);

                    nodeElement speed("speed");
                    speed.appendToNode(lane);
                    speed.addAttribute("sOffset", 0);
                    speed.addAttribute("max", ittt->speed);
                }
            }
        }
    

        //write objects
        nodeElement objects("objects");
        objects.appendToNode(road);

        std::sort(it->objects.begin(), it->objects.end(), compareObjects);
        for (std::vector<object>::iterator itt = it->objects.begin(); itt != it->objects.end(); ++itt)
        {
            object o = *itt;
            nodeElement obj("object");
            obj.appendToNode(objects);
            obj.addAttribute("type", o.type);
            obj.addAttribute("name", o.type);
            obj.addAttribute("dynamic", "no");
            obj.addAttribute("id", o.id);
            obj.addAttribute("s", (o.s));
            obj.addAttribute("t", (o.t));
            obj.addAttribute("zOffset", (o.z));
            obj.addAttribute("hdg", (o.hdg));
            obj.addAttribute("pitch", 0);
            obj.addAttribute("roll", 0);
            obj.addAttribute("validLength", 0);
            obj.addAttribute("orientation", o.orientation);
            obj.addAttribute("length", o.length);
            obj.addAttribute("width", o.width);
            obj.addAttribute("height", o.height);

            if (o.repeat)
            {
                nodeElement objj("repeat");
                objj.appendToNode(obj);
                objj.addAttribute("s", o.s);
                objj.addAttribute("length", o.len);
                objj.addAttribute("distance", o.distance);
                objj.addAttribute("tStart", o.t);
                objj.addAttribute("tEnd", o.t);
                objj.addAttribute("widthStart", o.width);
                objj.addAttribute("widthEnd", o.width);
                objj.addAttribute("heightStart", o.height);
                objj.addAttribute("heightEnd", o.height);
                objj.addAttribute("zOffsetStart", o.z);
                objj.addAttribute("zOffsetEnd", o.z);
                objj.addAttribute("lengthStart", o.length);
                objj.addAttribute("lengthEnd", o.length);
            }
        }

        // --- write signs ---------------------------------------------------

        // signs format is different in version 1.4
        if (setting.versionMajor >= 1 && setting.versionMinor >= 5)
        {
            nodeElement signs("signals", road);

            std::sort(it->signs.begin(), it->signs.end(), compareSignals);
            for (std::vector<sign>::iterator itt = it->signs.begin(); itt != it->signs.end(); ++itt)
            {
                sign s = *itt;
                nodeElement sig("signal", signs);
                //pugi::xml_node sig = objects.append_child("object");

                sig.addAttribute("id", s.id);
                sig.addAttribute("name", s.type);
                sig.addAttribute("type", s.type);
                sig.addAttribute("subtype", s.subtype);
                sig.addAttribute("country", s.country);
                sig.addAttribute("s", s.s);
                sig.addAttribute("t", s.t);
                sig.addAttribute("zOffset", s.z);
                sig.addAttribute("orientation", s.orientation);
                if (s.dynamic)
                    sig.addAttribute("dynamic", "yes");
                else
                    sig.addAttribute("dynamic", "no");
                sig.addAttribute("value", s.value);
                sig.addAttribute("width", s.width);
                sig.addAttribute("height", s.height);
            }
        }
    }

    // --- write controllers ---------------------------------------------------

    // controllers format is different in version 1.4
    if (setting.versionMajor >= 1 && setting.versionMinor >= 5)
    {
        for (std::vector<control>::iterator it = data.controller.begin(); it != data.controller.end(); ++it)
        {
            nodeElement controller("controller");
            controller.appendToNode(root);
            controller.addAttribute("id", it->id);

            for (std::vector<sign>::iterator itt = it->signs.begin(); itt != it->signs.end(); ++itt)
            {
                nodeElement con("control", controller);
                con.addAttribute("signalId", itt->id);
            }
        }
    }

    // --- write junctions -----------------------------------------------------
    for (std::vector<junction>::iterator it = data.junctions.begin(); it != data.junctions.end(); ++it)
    {
        nodeElement junc("junction");
        junc.appendToNode(root);
        junc.addAttribute("id", it->id);

        for (std::vector<connection>::iterator itt = it->connections.begin(); itt != it->connections.end(); ++itt)
        {
            nodeElement con("connection", junc);

            con.addAttribute("id", itt->id);
            con.addAttribute("incomingRoad", itt->from);
            con.addAttribute("connectingRoad", itt->to);
            con.addAttribute("contactPoint", getContactPointType(itt->contactPoint));

            nodeElement ll("laneLink", con);
            ll.addAttribute("from", itt->fromLane);
            ll.addAttribute("to", itt->toLane);
        }
    }
    // --- write junction groups -----------------------------------------------------

    for (std::vector<junctionGroup>::iterator it = data.juncGroups.begin(); it != data.juncGroups.end(); ++it)
    {

        nodeElement juncGroup("junctionGroup");
        juncGroup.appendToNode(root);
        juncGroup.addAttribute("id", it->id);
        juncGroup.addAttribute("name", it->name);
        juncGroup.addAttribute("type", it->type);


        for (std::vector<int>::iterator itt = it->juncIds.begin(); itt != it->juncIds.end(); ++itt)
        {
            nodeElement juncRef("junctionReference", juncGroup);
            juncRef.addAttribute("junction", *itt);
        }
    }


    serialize((data.outputFile + ".xodr").c_str());

    return 0;
}

/**
 * @brief function for displaying the road generation logo
 * 
 */
int printLogo()
{
    cout << "|‾\\  /‾\\  |‾‾| |‾\\       /‾‾  |‾‾  |\\  | |‾‾ |‾\\ |‾‾| ‾|‾ |  /‾\\  |\\  |" << endl;
    cout << "|_/ |   | |--| |  |  -  |  _  |--  | | | |-- |_/ |--|  |  | |   | | | |" << endl;
    cout << "| \\  \\_/  |  | |_/       \\_/  |__  |  \\| |__ | \\ |  |  |  |  \\_/  |  \\|" << endl;
    cout << "=======================================================================" << endl;

    return 0;
}