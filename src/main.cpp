#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include "pugixml.hpp"
#include "interface.h"
#include "helper.h"
#include "io.h"
#include "buildOpenDriveData.h"

int main(int argc,  char** argv)
{   
    if (argc < 2) {
        cerr << "ERR: no input file provided." << endl; return -1;
    }

	pugi::xml_document in;
	pugi::xml_document out;
	roadNetwork data;

	parseXML(in, data, argv[1]);	
	buildOpenDriveData(in, data);
	createXML(out, data);

	return 0;
} 

