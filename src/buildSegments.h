// file buildSegments.h

#include "generateRoad.h"
#include "createLaneConnection.h"
#include "createRoadConnections.h"

#define all 0 
#define non "none" 
#define bro "broken" 
#define sol "solid" 

#include "tjunction.h"
#include "xjunction.h"
#include "roundAbout.h"
#include "connectingRoad.h"

/**
 * @brief function creates all specified segments which can be either a junction, roundabout or connectingroad
 * 
 * @param doc 	tree structure which contains the input data
 * @param data 	roadNetwork data where the openDrive structure should be generated
 * @return int 	errorcode
 */
int buildSegments(pugi::xml_document &doc, roadNetwork &data)
{
	pugi::xml_node segments = doc.child("roadNetwork").child("segments");

	if(!segments) cout << "ERR: 'segments' not found in input file."  << endl;

	for (pugi::xml_node_iterator it = segments.begin(); it != segments.end(); ++it)
	{
		if ((string)it->name() == "junctions")
			{
			for (pugi::xml_node_iterator itt = it->begin(); itt != it->end(); ++itt)
			{
				if ((string)itt->name() == "tjunction")
				{
					cout << "Processing tjunction" << endl;
					if (tjunction(*itt, data))
					{
						cerr << "ERR: error in tjunction." << endl;
						return 1;
					}
				}

				if ((string)itt->name() == "xjunction")
				{
					cout << "Processing xjunction" << endl;
					if (xjunction(*itt, data))
					{
						cerr << "ERR: error in xjunction." << endl;
						return 1;
					}

				}

				if ((string)itt->name() == "njunction")
				{
					cout << "Processing njunction" << endl;
					if (false)
					{
						cerr << "ERR: error in njunction." << endl;
						return 1;
					}
				}
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