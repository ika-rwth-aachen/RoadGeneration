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


#include "utils/settings.h"
#include "utils/interface.h"
#include "utils/helper.h"
#include "utils/xml.h"
#include "generation/buildSegments.h"
#include "connection/linkSegments.h"
#include "connection/closeRoadNetwork.h"

settings setting;

EXPORTED void setFileName(char* file){
	setting.filename = file;
}


EXPORTED void setLogFile(char* file){
	setting.logfile = file;
}

EXPORTED void setOutputName(char* outName){
	setting.outname = outName;
	setting.outputNameSet = true;
}

EXPORTED int execPipeline(){
	return executePipeline(setting.filename);
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
		std::cout << "ERR: no file has been provided!" << std::endl;
		return -1;
	}

	
	// --- initialization ------------------------------------------------------

	xmlTree inputxml;
	roadNetwork data;
	std::string outputFile = setting.outname;
	char dt[100];//stores timestamp as string

	data.outputFile = outputFile.substr(0, outputFile.find(".xml"));
    data.outputFile = data.outputFile.substr(0, outputFile.find(".xodr"));
	
	setting.warnings = 0;

	if(!setting.outputNameSet){
		setting.outname = file;
	}

	(void)! freopen(setting.logfile.c_str(), "a", stderr); //(void)! suppresses the unused return warning..


	getTimeStamp(dt);
	std::cerr << "\n" << dt << " Error log for run with attribute: " << file << std::endl;

	if (setting.xmlSchemaLocation == ""){
		std::cerr << "ERR: xml scheme  NOT SET" << std::endl;
		return -1;
	}

	if(!setting.silentMode){
		std::cout << file << std::endl;
		printLogo();
	}

	
	// --- pipeline ------------------------------------------------------------


	if (validateInput(file, inputxml))
	{
		std::cerr << "ERR: error in validateInput" << std::endl;
		return -1;
	}

	if (buildSegments(inputxml.getRootElement(), data))
	{
		std::cerr << "ERR: error in buildSegments" << std::endl;
		return -1;
	}
	if (linkSegments(inputxml.getRootElement(), data))
	{
		std::cerr << "ERR: error in linkSegments" << std::endl;
		return -1;
	}
	if (closeRoadNetwork(inputxml.getRootElement(), data))
	{
		std::cerr << "ERR: error in closeRoadNetwork" << std::endl;
		return -1;
	}

	if (createXMLXercesC(data))
	{
		std::cerr << "ERR: error during createXML" << std::endl;
		return -1;
	}

	if (validateOutput(data))
	{
		std::cerr << "ERR: error in validateOutput" << std::endl;
		return -1;
	}

	//handle warnings
	if(!setting.silentMode)
	{
		if(setting.warnings > 0)
		{	
				std::cout << "\nFinished with " << setting.warnings << " warning(s), check out the error log for more information." << std::endl;
		}else{
			std::cout <<"\nFinished successfully" << std::endl;
		}
	}

	//Cleanup ----------------

	terminateXMLUtils();

	return 0;
}

EXPORTED int executePipelineCfg(r_config cfg)
{
	setOutputName(cfg.outputName);
	setting.silentMode = cfg.silentMode;
	setting.xmlSchemaLocation = cfg.xmlSchemeLocation;
	setFileName(cfg.filename);
	execPipeline();
	return 0;
}
