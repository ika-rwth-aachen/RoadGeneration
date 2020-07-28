#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include <string>
#include <vector>
#include "utils/settings.h"
#include "utils/interface.h"
#include "pugixml.hpp"
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/validators/common/Grammar.hpp>



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
        std::vector<control> &getController();
        std::vector<junction> &getJunctions();

        int buildSegments(pugi::xml_document &doc());

        void pushRoad(road r);
        void pushJunction(junction j);
        void pushControl(control c);
        void setFile(std::string file);

        



};

#endif