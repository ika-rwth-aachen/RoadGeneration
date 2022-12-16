/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file main.cpp
 *
 * @author Jannik Busse
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#include <stdio.h>
#include <iostream>
#include "libImports.h"
#include "helperExec.h"
#include "settingsExec.h"
#include <string>

using namespace std;

/**
 * @brief main function that calls the roadgeneration library functions
 * 
 * @param argc 
 * @param argv 
 * @return int: error code. 0 means that no errors occured. 
 */
int main(int argc, char** argv){

    string schemeLocation = PROJ_DIR;
    schemeLocation += "/xml";    

    settingsExec settings;
    if (parseArgs(argc, argv, settings)){
        return -1;
    }

    setFileName(settings.fileName);
    setXMLSchemeLocation(&schemeLocation[0]);
    setOverwriteLog(settings.overwriteLog);
    setOutputName(settings.outputName);
    setSilentMode(settings.silentMode);
    execPipeline();

    return 0;
}