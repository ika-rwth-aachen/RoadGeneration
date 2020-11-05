/**
 * @file main.cpp
 *
 * @brief main file which starts the road generation
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */
#include "export.h"
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
#include "utils/helper.h"
#include "utils/xml.h"
#include "generation/buildSegments.h"
#include "connection/linkSegments.h"
#include "connection/closeRoadNetwork.h"

settings setting;

void setFileName(char* file){
	_fileName = file;
}

void setLogFile(char* file){
	_logfile = file;
}


int execPipeline(){
	return executePipeline(_fileName);
}

int executePipeline(char* file)
{
	cout << "Executing on " << file << "." << endl;

	freopen(_logfile.c_str(), "a", stderr);
	cerr << "\nError log for run with attribute: " << file << endl;

	// --- initialization ------------------------------------------------------
	pugi::xml_document in;
	pugi::xml_document out;
	roadNetwork data;

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
	if (buildSegments(in, data))
	{
		cerr << "EcallMainRR: error in buildSegments" << endl;
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

	return 0;
}
