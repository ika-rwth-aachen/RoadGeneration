#ifndef HELPER
#define HELPER

#include <iostream>
#include <string.h>
#include <vector>
#include <stdio.h>
#include "settingsExec.h"

/**
 * @brief functions prints the help message for the executable 
 */
void printHelpMessage();

/**
 * @brief parses the args for the executable
 * 
 * @param argc      argc from command line
 * @param argv      argv from command line
 * @param settings  settings struct to store settings 
 * @return int  error code
 */
int parseArgs(int argc, char** argv, settingsExec& settings);



#endif