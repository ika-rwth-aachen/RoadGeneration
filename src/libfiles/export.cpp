/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file export.cpp
 *
 * @brief This file contains the most important functions for using the road generation library
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#include "export.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif

std::string::size_type st;
using namespace std;

#include "utils/settings.h"
#include "utils/interface.h"
#include "utils/helper.h"
#include "utils/xml.h"
#include "generation/buildSegments.h"
#include "connection/linkSegments.h"
#include "connection/closeRoadNetwork.h"

settings setting;

EXPORTED void setFileName(char* file){
	_fileName = file;
}

EXPORTED void setOverwriteLog(bool b){
	setting.overwriteLog = b;
}

EXPORTED void setLogFile(char* file){
	_logfile = file;
}

EXPORTED void setOutputName(char* outName){
	_outName = outName;
	_setOutput = true;
}

EXPORTED int execPipeline(){
	return executePipeline(_fileName);
}

EXPORTED void setSilentMode(bool sMode){
	setting.silentMode = sMode;
}


EXPORTED void setXMLSchemaLocation(char* file){
	setting.xmlSchemaLocation = file;
}


EXPORTED int executePipeline(char* file)
{

	if (file == NULL){
		cout << "ERR: no file has been provided!" << endl;
		return -1;
	}

	if(!_setOutput){
		_outName = file;
	}

	(void)! freopen(_logfile.c_str(), (setting.overwriteLog)? "w":"a", stderr); //(void)! suppresses the unused return warning..

	char dt[100];
	getTimeStamp(dt);
	cerr << "\n" << dt << " Error log for run with attribute: " << file << endl;

	if (setting.xmlSchemaLocation == ""){
		cerr << "ERR: xml scheme  NOT SET" << endl;
		return -1;
	}

	if(!setting.silentMode){
		cout << file << endl;
		printLogo();
	}
	
	// --- initialization ------------------------------------------------------

	xmlTree inputxml;

	roadNetwork data;
	string outputFile = _outName;
	data.outputFile = outputFile.substr(0, outputFile.find(".xml"));
    data.outputFile = data.outputFile.substr(0, outputFile.find(".xodr"));

	setting.warnings = 0;
	
	// --- pipeline ------------------------------------------------------------
	if (validateInput(file, inputxml))
	{
		cerr << "ERR: error in validateInput" << endl;
		return -1;
	}

	if (buildSegments(inputxml, data))
	{
		cerr << "ERR: error in buildSegments" << endl;
		return -1;
	}
	if (linkSegments(inputxml, data))
	{
		cerr << "ERR: error in linkSegments" << endl;
		return -1;
	}
	if (closeRoadNetwork(inputxml, data))
	{
		cerr << "ERR: error in closeRoadNetwork" << endl;
		return -1;
	}
	if (createXMLXercesC(data))
	{
		cerr << "ERR: error during createXML" << endl;
		return -1;
	}

	if (validateOutput(data))
	{
		cerr << "ERR: error in validateOutput" << endl;
		return -1;
	}

	//warning handling
	if(setting.warnings > 0)
	{
		cout << "\nFinished with " << setting.warnings << " warning(s), check out the error log for more information." << endl;
	}

	return 0;
}
