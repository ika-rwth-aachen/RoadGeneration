#pragma once
#include <fstream>
#include "LaneSection.h"
#include <iostream>
using namespace std;

void LaneSection::setStart(double sIn) {
	s = sIn;
}

double LaneSection::getStart() {
	return s;
}

void LaneSection::print(ofstream& myOutputfile, int kind) {
	switch (kind) {

	case 2:
		myOutputfile << "			<laneSection" << " s=\"" << s << "\">\n";
		break;
	case 1:
		myOutputfile << "			<laneSection>\n";
		break;
	case 0:
		myOutputfile << "			</laneSection>\n";
		break;

	}
}

void LaneSection::printLL(ofstream& myOutputfile, int kind) {
	switch (kind) {

	case 1:
		myOutputfile << "				<left>\n";
		break;
	case 2:
		myOutputfile << "				<left>\n";
		break;
	case 0:
		myOutputfile << "				</left>\n";
		break;

	}
}
void LaneSection::printCL(ofstream& myOutputfile, int kind) {
	switch (kind) {

	case 1:
		myOutputfile << "				<center>\n";
		break;
	case 2:
		myOutputfile << "				<center>\n";
		break;
	case 0:
		myOutputfile << "				</center>\n";
		break;

	}
}

void LaneSection::printRL(ofstream& myOutputfile, int kind) {
	switch (kind) {

	case 1:
		myOutputfile << "				<right>\n";
		break;
	case 2:
		myOutputfile << "				<right>\n";
		break;
	case 0:
		myOutputfile << "				</right>\n";
		break;

	}
}