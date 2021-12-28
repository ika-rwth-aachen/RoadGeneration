/**
 * @file libimports.h
 * @brief This file calls the library functions from the Road-Generation library.
 * @author Jannik Busse (jannik.busse@ika.rwth-aachen.de)
 * @date 2021-08-30
 * 
 */

#ifndef EXPORT_H
#define EXPORT_H

#include<string>


char* _fileName;
char* _outName;
std::string _logfile = "log.txt";


extern "C" void setFileName(char* file);
extern "C" void setLogFile(char* file);
extern "C" int execPipeline();
extern "C" int executePipeline(char* file);
extern "C" void setOutputName(char* file);
extern "C" void setSilentMode(bool sMode);
extern "C" void setXMLSchemeLocation(char* file);


#endif
