/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file buildSegments.h
 *
 * @brief file contains a method for generating segments
 * 
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#include "buildRoad.h"
#include "addObjects.h"
#include "createLaneConnection.h"
#include "createRoadConnection.h"

#define non "none"
#define bro "broken"
#define sol "solid"

#include "junctionWrapper.h"
#include "roundAbout.h"
#include "connectingRoad.h"

extern settings setting;
/**
 * @brief function creates all segments which can be either a junction, roundabout or connectingroad
 * 
 * @param inputxml 	tree structure which contains the input data
 * @param data 	roadNetwork data where the openDrive structure should be generated
 * @return int 	error code
 */
int buildSegments(const DOMElement* rootNode, roadNetwork &data)
{

	DOMElement *roadNode = getChildWithName(rootNode, "segments");
	if (roadNode == NULL)
	{
		std::cerr << "ERR: 'segments' not found in input file." << std::endl;
		std::cout << "ERR: 'segments' not found in input file." << std::endl;
		return 1;
	}

	DOMNodeList* segmentNodes = roadNode->getChildNodes();


	for(DOMElement* em = roadNode->getFirstElementChild(); em != NULL ;em = em->getNextElementSibling())
	{
		if (readNameFromNode(em) == "junction")
		{
			if(!setting.silentMode)
				std::cout << "Processing junction" << std::endl;
			if (junctionWrapper(em, data))
			{
		    		std::cerr << "ERR: error in junction." << std::endl;
					return 1;
			}
		}

		if (readNameFromNode(em) == "roundabout")
		{	
			if(!setting.silentMode)
				std::cout << "Processing roundabout" << std::endl;
			if (roundAbout(em, data))
			{
				std::cerr << "ERR: error in roundabout." << std::endl;
				return 1;
			}
		}

		if (readNameFromNode(em) == "connectingRoad" )
		{
			if(!setting.silentMode)
				std::cout << "Processing connectingRoad" << std::endl;
			if (connectingRoad(em, data))
			{
				std::cerr << "ERR: error in connectingRoad." << std::endl;
				return 1;
			}
		}
	}

	return 0;
}