#ifndef ROAD_NETWORK_H
#define ROAD_NETWORK_H

#include <string>
#include <vector>
#include "../../utils/interface.h"

class RoadNetwork
{
    std::string _file;

    std::vector<road> roads;
    std::vector<junction> junctions;
    std::vector<control> controller;

 

    public:

        RoadNetwork(std::string file);

        std::string getFile(){
            return _file;
        }

        std::vector<road> getRoads(){
            return roads;
        }

         std::vector<junction> getJunctions(){
            return junctions;
        }

         std::vector<control> getController(){
            return controller;
        }

        void setFile(std::string file){
            _file  = file;
        }

        int nSignal = 0;
        int nSegment = 0;

     



};

#endif