/**
 * @file xml.cpp
 *
 * @brief file contains the import and export, as well as the validation, of xml files
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "pugixml.hpp"
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <algorithm>

using namespace xercesc;

extern settings setting;

/**
 * @brief function checks the input file against the corresponding input.xsd
 * 
 * @param file  input file
 * @return int  error code
 */
int validateInput(char *file)
{
    XMLPlatformUtils::Initialize();
    string schema = string_format("%s/xml/input.xsd", PROJ_DIR);
    const char *schema_file = schema.c_str();
    const char *xml_file = file;

    XercesDOMParser domParser;
    if (domParser.loadGrammar(schema_file, Grammar::SchemaGrammarType) == NULL)
    {
        cerr << "ERR: couldn't load schema" << endl;
        return 1;
    }

    domParser.setValidationScheme(XercesDOMParser::Val_Auto);
    domParser.setDoNamespaces(true);
    domParser.setDoSchema(true);
    domParser.setValidationConstraintFatal(true);

    domParser.parse(xml_file);

    if (domParser.getErrorCount() == 0)
        cout << "XML input file validated against the schema successfully" << endl;
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
int validateOutput(roadNetwork data)
{
    // setup file
    string file = data.file;
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

    // check output file
    domParser.setValidationScheme(XercesDOMParser::Val_Auto);
    domParser.setDoNamespaces(true);
    domParser.setDoSchema(true);
    domParser.setValidationConstraintFatal(true);

    domParser.parse(xml_file);
    if (domParser.getErrorCount() == 0)
        cout << "XML output file validated against the schema successfully" << endl;
    else
    {
        cerr << "ERR: XML output file doesn't conform to the schema" << endl;
        return 1;
    }

    return 0;
}

/**
 * @brief function parses the input xml file with the external tool pugi_xml
 *  input is then accessable in a pugi::xml_document tree structure 
 * 
 * @param doc   tree structure which contains the data of the xml input file
 * @param data  roadNetwork data which holds the input file name
 * @param file  xml input file
 * @return int  error code
 */
int parseXML(pugi::xml_document &doc, roadNetwork &data, char *file)
{
    // save file name in data
    string f = file;
    data.file = f.substr(0, f.find(".xml"));

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
 * @brief function stores the generated structure of type roadNetwork as OpenDrive format with the external tool pugi_xml
 * 
 * @param doc   tree structure which contains the generated output data in OpenDrive format
 * @param data  roadNetwork data filled by the road generation tool
 * @return int  error code
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
    header.append_attribute("revMajor") = setting.versionMajor;
    header.append_attribute("revMinor") = setting.versionMinor;
    header.append_attribute("north") = setting.north;
    header.append_attribute("south") = setting.south;
    header.append_attribute("west") = setting.west;
    header.append_attribute("east") = setting.east;

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    // uncomment that for allowing CI, due to same output files
    //header.append_attribute("date") = oss.str().c_str();

    // geoReference tag
    pugi::xml_node geoReference = header.append_child("geoReference");
    geoReference.append_child(pugi::node_cdata).set_value("+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs");

    // --- write roads ---------------------------------------------------------
    for (std::vector<road>::iterator it = data.roads.begin(); it != data.roads.end(); ++it)
    {
        pugi::xml_node road = root.append_child("road");

        road.append_attribute("id") = it->id;
        road.append_attribute("length") = it->length;
        road.append_attribute("junction") = it->junction;

        pugi::xml_node pre = road.append_child("link").append_child("predecessor");
        pre.append_attribute("elementId") = it->predecessor.id;
        pre.append_attribute("elementType") = getLinkType(it->predecessor.elementType).c_str();
        pre.append_attribute("contactPoint") = getContactPointType(it->predecessor.contactPoint).c_str();

        pugi::xml_node suc = road.child("link").append_child("successor");
        suc.append_attribute("elementId") = it->successor.id;
        suc.append_attribute("elementType") = getLinkType(it->successor.elementType).c_str();
        suc.append_attribute("contactPoint") = getContactPointType(it->successor.contactPoint).c_str();

        road.append_child("type").append_attribute("s") = "0";
        road.child("type").append_attribute("type") = it->type.c_str();

        // --- write geometries ------------------------------------------------
        pugi::xml_node planView = road.append_child("planView");

        for (std::vector<geometry>::iterator itt = it->geometries.begin(); itt != it->geometries.end(); ++itt)
        {
            pugi::xml_node geo = planView.append_child("geometry");

            geo.append_attribute("s") = tS(itt->s).c_str();
            geo.append_attribute("x") = tS(itt->x).c_str();
            geo.append_attribute("y") = tS(itt->y).c_str();
            geo.append_attribute("hdg") = tS(itt->hdg).c_str();
            geo.append_attribute("length") = tS(itt->length).c_str();

            if (itt->type == line)
            {
                geo.append_child("line");
            }
            if (itt->type == arc)
            {
                geo.append_child("arc").append_attribute("curvature") = tS(itt->c).c_str();
            }
            if (itt->type == spiral)
            {
                pugi::xml_node spiral = geo.append_child("spiral");
                spiral.append_attribute("curvStart") = tS(itt->c1).c_str();
                spiral.append_attribute("curvEnd") = tS(itt->c2).c_str();
            }
        }

        // --- write lanes -----------------------------------------------------
        pugi::xml_node lanes = road.append_child("lanes");

        for (std::vector<laneSection>::iterator itt = it->laneSections.begin(); itt != it->laneSections.end(); ++itt)
        {
            pugi::xml_node laneOffset = lanes.append_child("laneOffset");

            laneOffset.append_attribute("s") = tS(itt->s).c_str();
            laneOffset.append_attribute("a") = tS(itt->o.a).c_str();
            laneOffset.append_attribute("b") = tS(itt->o.b).c_str();
            laneOffset.append_attribute("c") = tS(itt->o.c).c_str();
            laneOffset.append_attribute("d") = tS(itt->o.d).c_str();
        }

        for (std::vector<laneSection>::iterator itt = it->laneSections.begin(); itt != it->laneSections.end(); ++itt)
        {
            pugi::xml_node laneSection = lanes.append_child("laneSection");
            laneSection.append_attribute("s") = tS(itt->s).c_str();

            pugi::xml_node left;
            pugi::xml_node center;
            pugi::xml_node right;

            if (findMaxLaneId(*itt) > 0)
                left = laneSection.append_child("left");
            center = laneSection.append_child("center");
            if (findMinLaneId(*itt) < 0)
                right = laneSection.append_child("right");

            std::sort(itt->lanes.begin(), itt->lanes.end(), compareLanes);
            for (std::vector<lane>::iterator ittt = itt->lanes.begin(); ittt != itt->lanes.end(); ++ittt)
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
                    pugi::xml_node link = lane.append_child("link");
                    if (ittt->preId != 0)
                    {
                        pugi::xml_node pre = link.append_child("predecessor");
                        pre.append_attribute("id") = ittt->preId;
                    }
                    if (ittt->sucId != 0)
                    {
                        pugi::xml_node suc = link.append_child("successor");
                        suc.append_attribute("id") = ittt->sucId;
                    }

                    pugi::xml_node width = lane.append_child("width");
                    width.append_attribute("sOffset") = tS(ittt->w.s).c_str();
                    width.append_attribute("a") = tS(ittt->w.a).c_str();
                    width.append_attribute("b") = tS(ittt->w.b).c_str();
                    width.append_attribute("c") = tS(ittt->w.c).c_str();
                    width.append_attribute("d") = tS(ittt->w.d).c_str();
                }

                pugi::xml_node roadmark = lane.append_child("roadMark");
                roadmark.append_attribute("sOffset") = tS(ittt->rm.s).c_str();
                roadmark.append_attribute("type") = ittt->rm.type.c_str();
                roadmark.append_attribute("weight") = ittt->rm.weight.c_str();
                roadmark.append_attribute("color") = ittt->rm.color.c_str();
                roadmark.append_attribute("width") = tS(ittt->rm.width).c_str();

                if (ittt->id != 0)
                {
                    pugi::xml_node material = lane.append_child("material");
                    material.append_attribute("sOffset") = tS(ittt->m.s).c_str();
                    material.append_attribute("surface") = ittt->m.surface.c_str();
                    material.append_attribute("friction") = tS(ittt->m.friction).c_str();
                    material.append_attribute("roughness") = tS(ittt->m.roughness).c_str();

                    pugi::xml_node speed = lane.append_child("speed");
                    speed.append_attribute("sOffset") = 0;
                    speed.append_attribute("max") = tS(ittt->speed).c_str();
                }
            }
        }

        // --- write objects ---------------------------------------------------
        pugi::xml_node objects = road.append_child("objects");

        std::sort(it->objects.begin(), it->objects.end(), compareObjects);
        for (std::vector<object>::iterator itt = it->objects.begin(); itt != it->objects.end(); ++itt)
        {
            object o = *itt;
            pugi::xml_node obj = objects.append_child("object");

            obj.append_attribute("type") = o.type.c_str();
            obj.append_attribute("name") = o.type.c_str();
            obj.append_attribute("dynamic") = "no";
            obj.append_attribute("id") = o.id;
            obj.append_attribute("s") = tS(o.s).c_str();
            obj.append_attribute("t") = tS(o.t).c_str();
            obj.append_attribute("zOffset") = tS(o.z).c_str();
            obj.append_attribute("hdg") = tS(o.hdg).c_str();
            obj.append_attribute("pitch") = 0;
            obj.append_attribute("roll") = 0;
            obj.append_attribute("validLength") = 0;
            obj.append_attribute("orientation") = o.orientation.c_str();
            obj.append_attribute("length") = tS(o.length).c_str();
            obj.append_attribute("width") = tS(o.width).c_str();
            obj.append_attribute("height") = tS(o.height).c_str();

            if (o.repeat)
            {
                obj = obj.append_child("repeat");
                obj.append_attribute("s") = tS(o.s).c_str();
                obj.append_attribute("length") = tS(o.len).c_str();
                obj.append_attribute("distance") = tS(o.distance).c_str();
                obj.append_attribute("tStart") = tS(o.t).c_str();
                obj.append_attribute("tEnd") = tS(o.t).c_str();
                obj.append_attribute("widthStart") = tS(o.width).c_str();
                obj.append_attribute("widthEnd") = tS(o.width).c_str();
                obj.append_attribute("heightStart") = tS(o.height).c_str();
                obj.append_attribute("heightEnd") = tS(o.height).c_str();
                obj.append_attribute("zOffsetStart") = tS(o.z).c_str();
                obj.append_attribute("zOffsetEnd") = tS(o.z).c_str();
                obj.append_attribute("lengthStart") = tS(o.length).c_str();
                obj.append_attribute("lengthEnd") = tS(o.length).c_str();
            }
        }

        // --- write signs ---------------------------------------------------

        // signs format is different in version 1.4
        if (setting.versionMajor >= 1 && setting.versionMinor >= 5)
        {
            pugi::xml_node signs = road.append_child("signals");

            std::sort(it->signs.begin(), it->signs.end(), compareSignals);
            for (std::vector<sign>::iterator itt = it->signs.begin(); itt != it->signs.end(); ++itt)
            {
                sign s = *itt;
                pugi::xml_node sig = signs.append_child("signal");
                //pugi::xml_node sig = objects.append_child("object");

                sig.append_attribute("id") = s.id;

                sig.append_attribute("name") = s.type.c_str();
                sig.append_attribute("type") = s.type.c_str();
                sig.append_attribute("subtype") = s.subtype.c_str();
                sig.append_attribute("country") = s.country.c_str();
                sig.append_attribute("s") = tS(s.s).c_str();
                sig.append_attribute("t") = tS(s.t).c_str();
                sig.append_attribute("zOffset") = tS(s.z).c_str();
                sig.append_attribute("orientation") = s.orientation.c_str();
                if (s.dynamic)
                    sig.append_attribute("dynamic") = "yes";
                else
                    sig.append_attribute("dynamic") = "no";
                sig.append_attribute("value") = tS(s.value).c_str();
                sig.append_attribute("width") = tS(s.width).c_str();
                sig.append_attribute("height") = tS(s.height).c_str();
            }
        }
    }

    // --- write controllers ---------------------------------------------------

    // controllers format is different in version 1.4
    if (setting.versionMajor >= 1 && setting.versionMinor >= 5)
    {
        for (std::vector<control>::iterator it = data.controller.begin(); it != data.controller.end(); ++it)
        {
            pugi::xml_node controller = root.append_child("controller");

            controller.append_attribute("id") = it->id;

            for (std::vector<sign>::iterator itt = it->signs.begin(); itt != it->signs.end(); ++itt)
            {
                pugi::xml_node con = controller.append_child("control");

                con.append_attribute("signalId") = itt->id;
            }
        }
    }

    // --- write junctions -----------------------------------------------------
    for (std::vector<junction>::iterator it = data.junctions.begin(); it != data.junctions.end(); ++it)
    {
        pugi::xml_node junc = root.append_child("junction");

        junc.append_attribute("id") = it->id;

        for (std::vector<connection>::iterator itt = it->connections.begin(); itt != it->connections.end(); ++itt)
        {
            pugi::xml_node con = junc.append_child("connection");

            con.append_attribute("id") = itt->id;
            con.append_attribute("incomingRoad") = itt->from;
            con.append_attribute("connectingRoad") = itt->to;
            con.append_attribute("contactPoint") = getContactPointType(itt->contactPoint).c_str();

            pugi::xml_node ll = con.append_child("laneLink");
            ll.append_attribute("from") = itt->fromLane;
            ll.append_attribute("to") = itt->toLane;
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

/**
 * @brief function for displaying the road generation header name
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