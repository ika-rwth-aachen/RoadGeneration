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


#endif
