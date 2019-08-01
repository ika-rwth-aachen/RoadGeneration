#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Reader.cpp"
#include "Printer.cpp"
#include "RoadNetwork.h"

using namespace std;

#include "pugixml.hpp"

int main() 
{
	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_string("<node attr='3'></node>");
	pugi::xml_parse_result result = doc.load_file("../xml/test.xml");
	
	std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("RoadNetwork").child("segments").child("tJunction").child("mainRoad").attribute("id").value() << std::endl;
	
	pugi::xml_node tool = doc.child("RoadNetwork");

	if(tool) cout << "test"  << endl;
	else cout << "test2" << endl; 

	tool.set_name("rea");
	tool.attribute("test").set_value("6");

	tool.append_child("node");

	doc.save(std::cout);
	doc.save_file("s.opdr");
	cout << "kjK" <<endl;

	// -------------------------------------------------------------------------
	
	RoadNetwork roadNetwork;
	RoadSegment roadSegment;
	ifstream myInputfile("../russ/inputRuss.xml");
	ofstream myOutputfile("Output.xodr");
	
	parseRoadNetwork(myInputfile, roadNetwork, myOutputfile, roadSegment);
	printRN(myOutputfile, roadSegment);

	return 0;
} 

