#ifndef EXPORT_H
#define EXPORT_H

char* _fileName;

extern "C" void setFileName(char* file);

extern "C" int execPipeline();

extern "C" int executePipeline(char* file);

#endif
