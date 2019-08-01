#pragma once
#include <fstream>
#include "Lanes.h"
#include <iostream>
using namespace std;


void Lanes::print(ofstream& myOutputfile, int kind) {
	switch (kind) {

	case 1:
		myOutputfile << "		<lanes>\n";
		break;
	case 2:
		myOutputfile << "		<lanes>\n";
		break;
	case 0:
		myOutputfile << "		</lanes>\n";
		break;

	}
}