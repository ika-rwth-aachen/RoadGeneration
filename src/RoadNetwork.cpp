#pragma once
#include "RoadNetwork.h"
#include <fstream>

RoadNetwork::RoadNetwork() {

}
RoadNetwork::~RoadNetwork() {

}




Geometry RoadNetwork::getGeometry() {
	return geometry;
}

void RoadNetwork::print(ofstream& myOutputfile, int kind) {

	switch (kind) {

	case 2:
		myOutputfile << "<OpenDRIVE> \n";
		break;
	case 1:
		myOutputfile << "</OpenDRIVE> \n";
		break;
	case 0:
		myOutputfile << "<OpenDRIVE/> \n";
		break;
	}
}