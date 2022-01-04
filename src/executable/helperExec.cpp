#include "helperExec.h"

using namespace std;

const char *HELP_MESSAGE =
  "\nRoad Generation \n\n"
  "Usage: \n"
  "    road-generation <fileName>       Generates a .xodr file from input file.\n"
  "\nOptions:\n"
  "    -h                               Display help message.\n"
  "    -s                               Disable console output.\n"
  "    -d <fileDir>                     Specify output file directory.\n"
  "    -o <fileName>                    Specify output file name.\n\n";


/**
 * @brief prints the help message for the road generaction executable to cout.
 * 
 */
void printHelpMessage()
{
    std::cout << HELP_MESSAGE << std:: endl;       
}

/**
 * @brief parses the command line arguments from the main function and stores them in a settings struct.
 * 
 * @param argc argc from main
 * @param argv argv from main
 * @param settings setting struct to store the parsed parameter
 * @return int error code
 */
int parseArgs(int argc, char **argv, settingsExec &settings) {
    bool foundFile = false, setOutputName = false;
    char defaultPath[]="";
    settings.outputName = defaultPath;    

    if(argc < 2){
        std::cout << "ERR: too few arguments!" << std::endl;
        return -1;
    }
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            if(strlen(argv[i]) != 2) {
                std::cout << "ERR: wrong args!" << std::endl;
                return -1;
            }
            
            switch(argv[i][1]){  // look for params
                case 'o':
                    if(argc <= i +1){
                        std::cout <<"ERR: wrong args!" << std::endl;
                        return -1;
                    }
                    settings.outputName = argv[++i];
                    setOutputName = true;
                break;
                
                case 'h':
                    printHelpMessage();
                    return 1;
                
                case 's':
                    settings.silentMode = true;
                break;

                default:
                    std::cout << "ERR: wrong args!" << std::endl;
                    return -1;
            }
        }
        else{
            foundFile = true;
            settings.fileName = argv[i];
        }
    }

    if(!foundFile){
        std::cout << "ERR: too few arguments!" << std::endl;
        return -1;
    }

    if(!setOutputName) settings.outputName = settings.fileName;
    

    return 0;
}