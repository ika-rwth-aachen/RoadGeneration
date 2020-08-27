#include "roadNetwork.h"

#include <iostream>
#include <stdio.h>


#include "buildRoad.h"
#include "addObjects.h"
#include "generation/createLaneConnection.h"
#include "generation/createRoadConnection.h"

#define non "none"
#define bro "broken"
#define sol "solid"

#include "generation/junctionWrapper.h"
#include "generation/roundAbout.h"
#include "connectingRoad.h"

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