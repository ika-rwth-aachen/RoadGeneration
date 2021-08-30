/**
 * @file helperExec.cpp
 * @brief This file provides helper functions for the executable.
 * @author Jannik Busse (jannik.busse@ika.rwth-aachen.de)
 * @date 2021-08-30
 * 
 */

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