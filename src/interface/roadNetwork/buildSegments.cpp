/**
 * @file buildSegments.cpp
 *
 * @brief file contains method for generating segments
 * 
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "roadNetwork.h"
#include "road.h"
#include "generation/addObjects.h"
#include "createLaneConnection.h"
#include "createRoadConnection.h"
#include "junctionWrapper.h"
#include "roundAbout.h"
#include "connectingRoad.h"

#include <iostream>
#include <stdio.h>


 /**
         * @brief function creates all segments which can be either a junction, roundabout or connectingroad
         * 
         * @param doc 	tree structure which contains the input data         
         * @return int 	error code
*/
int RoadNetwork::buildSegments(pugi::xml_document &doc)
{
    RoadNetwork& data = *this;

	pugi::xml_node segments = doc.child("roadNetwork").child("segments");
	if (!segments)
		std::cerr << "ERR: 'segments' not found in input file." << std::endl;

	for (pugi::xml_node_iterator it = segments.begin(); it != segments.end(); ++it)
	{
		if ((std::string)it->name() == "junction")
		{
			std::cout << "Processing junction" << std::endl;
			if (junctionWrapper(*it, data))
			{
				std::cerr << "ERR: error in junction." << std::endl;
				return 1;
			}
		}

		if ((std::string)it->name() == "roundabout")
		{
			std::cout << "Processing roundabout" << std::endl;
			if (roundAbout(*it, data))
			{
				std::cerr << "ERR: error in roundabout." << std::endl;
				return 1;
			}
		}

		if ((std::string)it->name() == "connectingRoad")
		{
			std::cout << "Processing connectingRoad" << std::endl;
			if (connectingRoad(*it, data))
			{
				std::cerr << "ERR: error in connectingRoad." << std::endl;
				return 1;
			}
		}
	}

	return 0;
}