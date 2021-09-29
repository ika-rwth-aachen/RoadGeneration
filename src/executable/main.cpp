/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 */

#include <stdio.h>
#include <iostream>
#include "libImports.h"
#include "helperExec.h"
#include "settingsExec.h"
#include <string>

using namespace std;

int main(int argc, char** argv){

    string schemaLocation = PROJ_DIR;
    string xml = "/xml";
    schemaLocation += xml;    

    settingsExec settings;
    if (parseArgs(argc, argv, settings)){
        return -1;
    }

    setFileName(settings.fileName);
    setXMLSchemaLocation(&schemaLocation[0]);
    setOutputName(settings.outputName);
    setSilentMode(settings.silentMode);

    execPipeline();

    return 0;
}


