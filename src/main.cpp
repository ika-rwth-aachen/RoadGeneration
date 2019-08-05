#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

std::string::size_type sz;

using namespace std;

#include "pugixml.hpp"
#include "interface.h"
#include "helper.h"
#include "io.h"
#include "buildSegments.h"
#include "linkSegments.h"

int main(int argc,  char** argv)
{   
    if (argc < 2) {
        cerr << "ERR: no input file provided." << endl; return -1;
    }

	pugi::xml_document in;
	pugi::xml_document out;
	roadNetwork data;

	parseXML(in, data, argv[1]);	
	buildSegments(in, data);
	linkSegments(in, data);
	createXML(out, data);

	return 0;
} 

