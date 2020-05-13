/**
 * @file buildSegments.h
 *
 * @brief file contains method for generating segments
 * 
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
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

/**
 * @brief function creates all segments which can be either a junction, roundabout or connectingroad
 * 
 * @param doc 	tree structure which contains the input data
 * @param data 	roadNetwork data where the openDrive structure should be generated
 * @return int 	error code
 */
int buildSegments(pugi::xml_document &doc, roadNetwork &data)
{
	pugi::xml_node segments = doc.child("roadNetwork").child("segments");
	if (!segments)
		cerr << "ERR: 'segments' not found in input file." << endl;

	for (pugi::xml_node_iterator it = segments.begin(); it != segments.end(); ++it)
	{
		if ((string)it->name() == "junction")
		{
			cout << "Processing junction" << endl;
			if (junctionWrapper(*it, data))
			{
				cerr << "ERR: error in junction." << endl;
				return 1;
			}
		}

		if ((string)it->name() == "roundabout")
		{
			cout << "Processing roundabout" << endl;
			if (roundAbout(*it, data))
			{
				cerr << "ERR: error in roundabout." << endl;
				return 1;
			}
		}

		if ((string)it->name() == "connectingRoad")
		{
			cout << "Processing connectingRoad" << endl;
			if (connectingRoad(*it, data))
			{
				cerr << "ERR: error in connectingRoad." << endl;
				return 1;
			}
		}
	}

	return 0;
}