#pragma once
#include <fstream>
#include "Road.h"
#include <iostream>
using namespace std;

string Road::getId() {
	return id;
}

double Road::getLength() {
	return length;
}

void Road::setId(string idIn) {
	id = idIn;
}

void Road::setLength(double lengthIn) {
	length = lengthIn;
}

void Road::print(ofstream& myOutputfile, int kind) {
	if (kind == 0) {
	myOutputfile << "	</road>\n";
	}
	else {
		myOutputfile << "	<road length=\"" << getLength() << "\" id=\"" << getId() << "\">\n";
	}
}
	

void Road::printPV(ofstream& myOutputfile, int kind) {
	if (kind == 0) {

		myOutputfile << "		</planView>\n";
	} 
	else {
		myOutputfile << "		<planView>\n";
	}
}