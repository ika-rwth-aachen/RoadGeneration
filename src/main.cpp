#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif

std::string::size_type sz;

using namespace std;

#include "interface.h"
#include "helper.h"
#include "io.h"
#include "buildSegments.h"
#include "linkSegments.h"
#include "closeRoadNetwork.h"

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
    if (argc < 2) 
	{
        cout << "ERR: no input file provided." << endl; 
		return -1;
    }

	pugi::xml_document in;
	pugi::xml_document out;
	roadNetwork data;

	if (validateInput(argv[1])) 
	{
		cerr << "ERR: error in Inputfile" << endl;
		return -1;
	}
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
	if (validateOutput(data.file))
	{
		cerr << "ERR: error in Outputfile" << endl;
		return -1;
	}

	return 0;
} 

