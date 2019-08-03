#include <iostream>
#include <fstream>
#include <string>


// TODO remove
#include "Reader.cpp"
#include "Printer.cpp"
#include "RoadNetwork.h"
// TODO remove


#include "pugixml.hpp"
#include "interface.h"
#include "helper.h"
#include "io.h"
#include "buildOpenDriveData.h"

using namespace std;


int main() 
{
	pugi::xml_document in;
	pugi::xml_document out;
	roadNetwork data;

	parseXML(in);	
	buildOpenDriveData(in, data);
	createXML(out, data);

	// -------------------------------------------------------------------------
	// TODO remove
	/*
	RoadNetwork roadNetwork;
	RoadSegment roadSegment;
	ifstream myInputfile("../russ/inputRuss.xml");
	ofstream myOutputfile("Output.xodr");
	
	parseRoadNetwork(myInputfile, roadNetwork, myOutputfile, roadSegment);
	printRN(myOutputfile, roadSegment);
	*/
	// TODO remove
	// -------------------------------------------------------------------------

	return 0;
} 

