/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file helperExec.h
 *
 * @brief This file contains helper functions for the executable
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de
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
 * @brief functions prints the help message for the executable.
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