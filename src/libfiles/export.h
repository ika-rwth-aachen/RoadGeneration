/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file export.h
 *
 * @brief This file contains the most important functions for using the road generation library
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */
#ifndef EXPORT_H
#define EXPORT_H

#ifdef _WIN32
#   define EXPORTED  __declspec( dllexport )
#else
# define EXPORTED
#endif

#include<string>

struct r_config{
    bool silentMode = false;
    char* filename;
    char* outputName;
    char *xmlSchemeLocation;
};





/**
 * @brief Sets the filename of the input file
 * @param file file name * 
 */
extern "C" EXPORTED void setFileName(char* file);

/**
 * @brief set path to xml schema files
 * @param file set path
 */
extern "C" EXPORTED void setXMLSchemaLocation(char* file);

/**
 * @brief set log file location
 * @param file set file location
 */
extern "C" EXPORTED void setLogFile(char* file);

/**
 * @brief execute the pippeline on filename that is stored in settings
 * 
 */
extern "C" EXPORTED int execPipeline();

/**
 * @brief execute the pipeline on given filename
 * @param file filename to run the pipeline on
 */
extern "C" EXPORTED int executePipeline(char* file);
/**
 * @brief execute the pipeline on given filename provided a config file
 * @param file filename to run the pipeline on
 */
extern "C" EXPORTED int executePipelineCfg(r_config cfg);

/**
 * @brief set the output file name
 * @param file output file
 */
extern "C" EXPORTED void setOutputName(char* file);

/**
 * @brief sets the silent mode. Silent mode disables most console output
 * @param sMode sets silent mode to True or False
 */
extern "C" EXPORTED void setSilentMode(bool sMode);


#endif

