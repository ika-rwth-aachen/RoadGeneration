#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Reader.cpp"
#include "Printer.cpp"
#include "RoadNetwork.h"
using namespace std;



int main() {
	RoadNetwork roadNetwork;
	RoadSegment roadSegment;
	ifstream myInputfile("Beispiel_Input_TKreuzung.xml");
	//ifstream myInputfile("example.txt");
	ofstream myOutputfile("Output.xodr");
	
	parseRoadNetwork(myInputfile, roadNetwork, myOutputfile, roadSegment);
	//printRoadNetwork(myOutputfile, roadNetwork);
	printRN(myOutputfile, roadSegment);

	return 0;
} 

