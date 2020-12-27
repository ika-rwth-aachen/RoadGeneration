#ifndef HELPER
#define HELPER

#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h>
#include "settings_exec.h"


void printHelpMessage();
int generateOutputName(char* fileLocation, std::string &outputName);
int parseArgs(int argc, char** argv, settings& settings);



#endif