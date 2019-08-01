#pragma once
#include <fstream>
#include "Lane.h"
#include <iostream>
using namespace std;




void Lane::setId(int idIn) {
	id = idIn;
}

void Lane::setType(string typeIn) {
	type = typeIn;
}

void Lane::setLaneWidth(double width) {
	laneWidth.sOffset = 0.0;
	laneWidth.a = width;
	laneWidth.b = 0.0;
	laneWidth.c = 0.0;
	laneWidth.d = 0.0;
}

void Lane::setRoadMarkType(string type) {
	roadMark.type = type;
}


void Lane::setRoadMark(double sOffset, string type, string color, double width) {
	roadMark.sOffset = sOffset;
	roadMark.type = type;
	roadMark.weight = "standard";
	roadMark.color = color;
	roadMark.width = width;
	roadMark.laneChange = "none";
}

int Lane::getId() {
	return id;
}

string Lane::getType() {
	return type;
}

LaneWidth Lane::getLaneWidth() {
	return laneWidth;
}
RoadMark Lane::getRoadMark() {
	return roadMark;
}


void Lane::print(ofstream& myOutputfile, int kind) {
	switch (kind) {

	case 1:
		myOutputfile << "					<lane>\n";
		break;
	case 2:
		myOutputfile << "					<lane id=\"" << getId() << "\" type=\"" << getType() << "\" level=\"false\">\n";
		break;
	case 0:
		myOutputfile << "					</lane>\n";
		break;

	}
}

void Lane::printLW(ofstream& myOutputfile) {
	myOutputfile << "						<width sOffset=\"" << laneWidth.sOffset << "\" a=\"" << laneWidth.a << "\" b=\"" << laneWidth.b << "\" c=\"" << laneWidth.c << "\" d=\"" << laneWidth.d << "\"/>\n";
}

void Lane::printRM(ofstream& myOutputfile) {
	myOutputfile << "						<roadMark sOffset=\"" << roadMark.sOffset << "\" type=\"" << roadMark.type << "\" weight=\"" << roadMark.weight << "\" color=\"" << roadMark.color << "\" width=\"" << roadMark.width << "\" laneChange=\"" << roadMark.laneChange << "\"/>\n";
}