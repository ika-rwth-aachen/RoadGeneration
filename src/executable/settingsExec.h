#ifndef SETTINGS
#define SETTINGS

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>


/**
 * @brief A helper struct that stores the command line arguments for the executable.
 * 
 */
struct settingsExec{
    char* fileName;
    char* outputName;
    bool silentMode = false;

};

#endif