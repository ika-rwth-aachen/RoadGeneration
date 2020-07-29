#ifndef XML
#define XML

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
#include "roadNetwork.h"



/**
 * @brief function checks the input file against the corresponding input.xsd
 * 
 * @param file  input file
 * @return int  error code
 */
int validateInput(char *file);

/**
 * @brief function checks the output file against the corresponding output.xsd
 * 
 * @param data  output data
 * @return int  error code
 */
int validateOutput(RoadNetwork data);
/**
 * @brief function parses the input xml file with the external tool pugi_xml
 *  input is then accessable in a pugi::xml_document tree structure 
 * 
 * @param doc   tree structure which contains the data of the xml input file
 * @param data  roadNetwork data which holds the input file name
 * @param file  xml input file
 * @return int  error code
 */
int parseXML(pugi::xml_document &doc, RoadNetwork &data, char *file);

/**
 * @brief function stores the generated structure of type roadNetwork as OpenDrive format with the external tool pugi_xml
 * 
 * @param doc   tree structure which contains the generated output data in OpenDrive format
 * @param data  roadNetwork data filled by the road generation tool
 * @return int  error code
 */
int createXML(pugi::xml_document &doc, RoadNetwork data);

/**
 * @brief function for displaying the road generation header name
 * 
 */
int printLogo();

#endif