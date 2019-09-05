#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

std::string::size_type sz;

using namespace std;

#include "pugixml.hpp"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "interface.h"
#include "helper.h"
#include "io.h"
#include "buildSegments.h"
#include "linkSegments.h"
#include "closeRoadNetwork.h"

using namespace xercesc;

/**
 * @brief main function for the tool 'roadGeneration'
 * an input xml file with given structure (defined in input.xsd) is converted 
 * into a valid openDrive format (defined in output.xsd)
 * 
 * @param argc amount of parameter calls
 * @param argv argv[1] is the specified input file <file>.xml which will be converted to <file>.xodr
 * @return int errorcode
 */
int main(int argc,  char** argv)
{   
	/*
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Error during initialization! :\n"
				<< message << "\n";
		XMLString::release(&message);
		return 1;
	}


	XMLCh tempStr[100];
	XMLString::transcode("LS", tempStr, 99);
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
	DOMLSParser* parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

	// optionally you can set some features on this builder
	if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMValidate, true))
		parser->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
	if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMNamespaces, true))
		parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
	if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMDatatypeNormalization, true))
		parser->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, true);


	char* xmlFile = "all.xml";
	DOMDocument *doc = 0;

	try {
		doc = parser->parseURI(xmlFile);
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
				<< message << "\n";
		XMLString::release(&message);
		return -1;
	}
	catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		cout << "Exception message is: \n"
				<< message << "\n";
		XMLString::release(&message);
		return -1;
	}
	catch (...) {
		cout << "Unexpected Exception \n" ;
		return -1;
	}

	parser->release();

	DOMElement* root = doc->getDocumentElement();

	XMLCh *tag = XMLString::transcode("segments");
	DOMNodeList * segments = root->getChildNodes();  

	XMLSize_t index = 1;
	DOMNode * te = segments->item(index);

	char* test =  XMLString::transcode(te->getNodeName());

	cout << test << endl;
	return 0;
	*/


    if (argc < 2) 
	{
        cerr << "ERR: no input file provided." << endl; 
		//return -1;
    }

	pugi::xml_document in;
	pugi::xml_document out;
	roadNetwork data;

	if (parseXML(in, data, argv[1])) 
	{
		cerr << "ERR: error in parseXML" << endl;
		return -1;
	}
	if (buildSegments(in, data)) 
	{
		cerr << "ERR: error in buildSegments" << endl;
		return -1;
	}
	if (linkSegments(in, data)) 
	{
		cerr << "ERR: error in linkSegments" << endl;
		return -1;
	}
	if (closeRoadNetwork(in, data)) 
	{
		cerr << "ERR: error in closeRoadNetwork" << endl;
		return -1;
	}
	if (createXML(out, data)) 
	{
		cerr << "ERR: error durining createXML" << endl;
		return -1;
	}

	return 0;
} 

