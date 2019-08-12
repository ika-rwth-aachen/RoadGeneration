// file buildSegments.h

#include "generateRoad.h"
#include "createLaneConnection.h"
#include "createRoadConnections.h"

#define all 0 
#define n "none" 
#define d "broken" 
#define s "solid" 

#include "tjunction.h"
#include "xjunction.h"
#include "roundAbout.h"
#include "connectingRoad.h"

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
					tjunction(*itt, data);
				}

				if ((string)itt->name() == "xjunction")
				{
					cout << "Processing xjunction" << endl;
					xjunction(*itt, data);

				}

				if ((string)itt->name() == "njunction")
				{
					cout << "Processing njunction" << endl;
					// TODO
				}
			}
		}

		if ((string)it->name() == "roundabout")
		{
			cout << "Processing roundabout" << endl;
			roundAbout(*it, data);
		}

		if ((string)it->name() == "connectingRoad")
		{
			cout << "Processing connectingRoad" << endl;
			connectingRoad(*it, data);
		}
	}

    return 0;
}