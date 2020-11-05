#ifndef EXPORT_H
#define EXPORT_H

#include<string>

char* _fileName;
std::string _logfile = "log.txt";

extern "C" void setFileName(char* file);
extern "C" void setLogFile(std::string file);
extern "C" int execPipeline();
extern "C" int executePipeline(char* file);

#endif
