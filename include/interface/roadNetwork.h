#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include "settings.h"
#include "interface.h"
#include "road.h"
#include "pugixml.hpp"

#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#define non "none"
#define bro "broken"
#define sol "solid"

class RoadNetwork
{
    private:

        std::string file;
        std::string outputFile;
        std::vector<Road> roads;
        std::vector<junction> junctions;
        std::vector<control> controller; 

    public:

        int nSignal = 0;
        int nSegment = 0;

        RoadNetwork();
        RoadNetwork(std::string file);
        ~RoadNetwork();

        std::string getFile();
        std::string getOutputFile();
        std::vector<Road> &getRoads();
        std::vector<control> &getController();
        std::vector<junction> &getJunctions();

        int buildSegments(pugi::xml_document &doc());
        int linkSegments(pugi::xml_document &doc);
        int closeRoadNetwork(pugi::xml_document &doc);

        void pushRoad(Road r);
        void pushJunction(junction j);
        void pushControl(control c);
        void setFile(std::string file);
        void setOutputFile(std::string file);


        /**
         * @brief function creates all segments which can be either a junction, roundabout or connectingroad
         * 
         * @param doc 	tree structure which contains the input data
         * @return int 	error code
         */
        int buildSegments(pugi::xml_document &doc);
};

#endif