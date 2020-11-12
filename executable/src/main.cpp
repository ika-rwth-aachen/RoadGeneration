#include <stdio.h>
#include <iostream>
#include "exports.h"
#include "helper.h"
#include "settings.h"


using namespace std;

int main(int argc, char** argv){

    settings settings;
    if (parseArgs(argc, argv, settings)){
        return -1;
    }

    cout << settings.fileName << "\n" << settings.outputName << "\n" << settings.silentMode << endl; 

    setFileName(settings.fileName);
    setOutputName(settings.outputName);
    setSilentMode(settings.silentMode);

    execPipeline();
    return 0;
}


