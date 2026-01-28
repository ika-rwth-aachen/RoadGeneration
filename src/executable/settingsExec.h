#pragma once
/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 */



#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>


/**
 * @brief A helper struct that stores the command line arguments for the executable.
 * 
 */
struct config{
    char* filename;
    char* outputName;
    bool silentMode = false;
    char *xmlSchemeLocation;
};


extern std::string _logfile;
