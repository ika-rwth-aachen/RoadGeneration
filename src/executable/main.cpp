#include <stdio.h>
#include <iostream>
#include "libfiles/export.h"
#include "helper_exec.h"
#include "settings_exec.h"


using namespace std;

int main(int argc, char** argv){

    settings settings;
    if (parseArgs(argc, argv, settings)){
        return -1;
    }

    setFileName(settings.fileName);
    setOutputName(settings.outputName);
    setSilentMode(settings.silentMode);

    execPipeline();
    return 0;
}


