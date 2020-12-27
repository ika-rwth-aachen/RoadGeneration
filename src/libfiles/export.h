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


extern "C" EXPORTED void setFileName(char* file);
extern "C" EXPORTED void setXMLSchemaLocation(std::string file);
extern "C" EXPORTED void setLogFile(char* file);
extern "C" EXPORTED int execPipeline();
extern "C" EXPORTED int executePipeline(char* file);
extern "C" EXPORTED void setOutputName(char* file);
extern "C" EXPORTED void setSilentMode(bool sMode);


#endif
