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

bool _setOutput = false;
char* _fileName;
std::string _outName = "";
std::string _logfile = "log.txt";

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
 * @brief execute the pippeline on fileName that is stored in settings
 * 
 */
extern "C" EXPORTED int execPipeline();

/**
 * @brief execute the pipeline on given fileName
 * @param file filename to run the pipeline on
 */
extern "C" EXPORTED int executePipeline(char* file);

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

/**
 * @brief set overwriting the error log to ture or false
 *  @param b true if errorlog should be overwritten
 */
extern "C" EXPORTED void setOverwriteLog(bool b);



#endif

