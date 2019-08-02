// file buildOpenDriveData.h

std::string::size_type sz;

#include "tjunction.h"

int buildOpenDriveData(pugi::xml_document &doc, roadNetwork &data)
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
				}

				if ((string)itt->name() == "njunction")
				{
					cout << "Processing njunction" << endl;
				}
			}
		}

		if ((string)it->name() == "roundabout")
		{
			cout << "Processing roundabout" << endl;
		}

		if ((string)it->name() == "connectingRoad")
		{
			cout << "Processing rounconnectingRoaddabout" << endl;
		}
	}

    return 0;
}