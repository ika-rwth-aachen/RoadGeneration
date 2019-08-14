#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

std::string::size_type sz;

using namespace std;

#include "pugixml.hpp"
#include "interface.h"
#include "helper.h"
#include "io.h"
#include "buildSegments.h"
#include "linkSegments.h"

/**
 * @brief main function for the tool roadGeneration
 * an input xml file with given structure (input.xsd) is converted in a valid openDrive format
 * 
 * @param argc amount of parameter calls
 * @param argv argv[1] is the specified input file <file>.xml which will be converted to <file>.xodr
 * @return int errorcode
 */
int main(int argc,  char** argv)
{   
    if (argc == 2) {
        //cerr << "ERR: no input file provided." << endl; return -1;
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
	if (createXML(out, data)) 
	{
		cerr << "ERR: error durining createXML" << endl;
		return -1;
	}

	return 0;
} 

