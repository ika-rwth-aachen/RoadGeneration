/**
 * @file main.cpp
 *
 * @brief main file which starts the road generation
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif

std::string::size_type st;
using namespace std;

#include "utils/settings.h"
#include "utils/interface.h"

#include "roadNetwork.h"

#include "helper.h"
#include "utils/xml.h"
//#include "generation/buildSegments.h"
#include "includes.h"
#include "connection/linkSegments.h"
#include "connection/closeRoadNetwork.h"


settings setting;

/**
 * @brief main function for the tool 'road-generation'
 * an input xml file with given structure (defined in xml/input.xsd) is converted into a valid OpenDrive format (defined in xml/output.xsd)
 * 
 * @param argc amount of parameters
 * @param argv argv[1] is the specified input file <file>.xml which will be converted to <file>.xodr
 * @return int error code
 */
int main(int argc, char **argv)
{
	char *file;

	if (argc == 2)
	{
		file = argv[1];
	}
	else
	{
		cerr << "ERR: no input file provided." << endl;
		return -1;
	}

	freopen("log.txt", "a", stderr);
	cerr << "\nError log for run with attribute: " << file << endl;

	printLogo();

	// --- initialization ------------------------------------------------------
	pugi::xml_document in;
	pugi::xml_document out;
	RoadNetwork data;

	// --- pipeline ------------------------------------------------------------
	
	

	if (validateInput(file))
	{
		cerr << "ERR: error in validateInput" << endl;
		return -1;
	}
	if (parseXML(in, data, file))
	{
		cerr << "ERR: error in parseXML" << endl;
		return -1;
	}	

	

	if (data.buildSegments(in))
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
		cerr << "ERR: error during createXML" << endl;
		return -1;
	}
	if (validateOutput(data))
	{
		cerr << "ERR: error in validateOutput" << endl;
		return -1;
	}

	cout << "done1" << endl;

	return 0;
}
