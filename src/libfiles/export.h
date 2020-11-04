#ifndef EXPORT_H
#define EXPORT_H

char* _fileName;
char* _logfile = "log.txt";

extern "C" void setFileName(char* file);
extern "C" int execPipeline();
extern "C" int executePipeline(char* file);

#endif
