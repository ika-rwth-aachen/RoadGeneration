#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include <string>
#include <vector>
#include "../../utils/settings.h"
#include "../../utils/interface.h"



class RoadNetwork
{

    private:

        std::string file;
        std::vector<road> roads;
        std::vector<junction> junctions;
        std::vector<control> controller; 

    public:

        int nSignal = 0;
        int nSegment = 0;

        RoadNetwork();
        RoadNetwork(std::string file);
        ~RoadNetwork();

        std::string getFile();

         std::vector<road> &getRoads();

        void pushRoad(road r);

         std::vector<junction> &getJunctions();

         void pushJunction(junction j);

         std::vector<control> &getController();

         void pushControl(control c);

        void setFile(std::string file);


};

#endif