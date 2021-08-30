/**
 * @file main.cpp
 * @brief This file is the main file for the Road-Generation executable. It makes use of the Road-Generation library.
 * @author Jannik Busse (jannik.busse@ika.rwth-aachen.de)
 * @date 2021-08-30
 * 
 */

#include <stdio.h>
#include <iostream>
#include "libImports.h"
#include "helperExec.h"
#include "settingsExec.h"
#include <string>

using namespace std;

int main(int argc, char** argv){

    string schemeLocation = PROJ_DIR;
    schemeLocation += "/xml";    

    settingsExec settings;
    if (parseArgs(argc, argv, settings)){
        return -1;
    }

    setFileName(settings.fileName);
    setXMLSchemeLocation(&schemeLocation[0]);
    setOutputName(settings.outputName);
    setSilentMode(settings.silentMode);

    execPipeline();

    
    return 0;
}


