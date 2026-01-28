/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 */

#ifndef EXPORT_H
#define EXPORT_H

#include <string>
#include "settingsExec.h"

//use struct from lib instead.
struct r_config{
    bool silentMode = false;
    char* filename;
    char* outputName;
    char *xmlSchemeLocation;
};


extern "C" void setFileName(char* file);
extern "C" void setLogFile(char* file);
extern "C" int execPipeline();
extern "C" int executePipeline(char* file);
extern "C" void setOutputName(char* file);
extern "C" void setSilentMode(bool sMode);
extern "C" void setXMLSchemaLocation(char* file);
extern "C"  int executePipelineCfg(r_config cfg);



#endif
