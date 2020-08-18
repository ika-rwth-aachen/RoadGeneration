#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include "settings.h"
#include "interface.h"

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
        //Implement as member function ?
        int closeRoadNetwork(pugi::xml_document &doc);

        void pushRoad(road r);
        void pushJunction(junction j);
        void pushControl(control c);
        void setFile(std::string file);

        /**
         * @brief function creates all segments which can be either a junction, roundabout or connectingroad
         * 
         * @param doc 	tree structure which contains the input data
         * @param data 	roadNetwork data where the openDrive structure should be generated
         * @return int 	error code
         */
        int buildSegments(pugi::xml_document &doc);

        



};

#endif