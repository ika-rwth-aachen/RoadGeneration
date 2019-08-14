// file closeRoadNetwork.h

#include "closeRoadConnection.h"

/**
 * @brief function which closes all roads and add correct lane Sections
 * 
 * @param doc 	tree structure which contains the input data
 * @param data 	roadNetwork data where the openDrive structure should be generated
 * @return int 	errorcode
 */
int closeRoadNetwork(pugi::xml_document &doc, roadNetwork &data)
{
    cout << "Processing closeRoadNetwork" << endl;

	pugi::xml_node cRN = doc.child("roadNetwork").child("closeRoadNetwork");

	if(!cRN) 
	{
		cerr << "ERR: 'closeRoadNetwork' is not specified in input file."  << endl;
		cerr << "\t -> skip closing"  << endl;
		return 0;
	}

    // we assume that all segments are already linked

	// add other specified segments
    int cc = 0;
	for (pugi::xml_node segmentLink : cRN.children("segmentLink"))
	{
        cc++;

        road rConnection;
        rConnection.id = 1000 + cc;
        
		int fromSegment = segmentLink.attribute("fromSegment").as_int();
		int toSegment = segmentLink.attribute("toSegment").as_int();
		int fromRoad = segmentLink.attribute("fromRoad").as_int();
		int toRoad = segmentLink.attribute("toRoad").as_int();	
		string fromPos = (string)segmentLink.attribute("fromPos").value();
		string toPos = (string)segmentLink.attribute("toPos").value();

		double fromX,fromY,fromHdg;
		double toX,toY,toHdg;
        laneSection lS1, lS2;

		// save from position
		for(auto&& r : data.roads)
		{
			if (r.junction != fromSegment || r.id != fromRoad) continue;
            
            rConnection.predecessor.elementId = r.id;

			if (fromPos == "start")
			{
				fromX = r.geometries.front().x;
				fromY = r.geometries.front().y;
				fromHdg = r.geometries.front().hdg;
                r.predecessor.elementId = rConnection.id;
                lS1 = r.laneSections.front();
			}
			else if (fromPos == "end")
			{
				geometry g = r.geometries.back();
				curve(g.length,g, g.x, g.y, g.hdg, 1);
				fromX = g.x;
				fromY = g.y;
				fromHdg = g.hdg;
                r.successor.elementId = rConnection.id;
                lS1 = r.laneSections.back();
			}
			else
			{
				cerr << "ERR: wrong position for fromPos is specified." << endl;
				cerr << "\t -> use 'start' or 'end'" << endl;
				return 1;
			}
		}

		// save to position
		for(auto&& r : data.roads)
		{
			if (r.junction != toSegment || r.id != toRoad) continue;

            rConnection.successor.elementId = r.id;

			if (toPos == "start")
			{
				toX = r.geometries.front().x;
				toY = r.geometries.front().y;
				toHdg = r.geometries.front().hdg;
                r.predecessor.elementId = rConnection.id;
                lS2 = r.laneSections.front();
			}
			else if (toPos == "end")
			{
				geometry g = r.geometries.back();
				toX = g.x;
				toY = g.y;
				toHdg = g.hdg;
				curve(g.length,g, toX, toY, toHdg, 1);
                r.successor.elementId = rConnection.id;				
                lS2 = r.laneSections.back();
			}
			else
			{
				cerr << "ERR: wrong position for toPos is specified." << endl;
				cerr << "\t -> use 'start' or 'end'" << endl;
				return 1;
			}
        }

        // adjust toHdg
        toHdg += M_PI;
        fixAngle(toHdg);

        // compute geometry between fromX, fromY, fromPhi, toX, toY, toPhi
        if (closeRoadConnection(rConnection.geometries, fromX, fromY, fromHdg, toX, toY, toHdg))
        {
            cerr << "ERR: error in closeRoadConnection function." << endl;
            return 1;
        }

        // --- compute length --------------------------------------------------
    	for (int j = 0; j < rConnection.geometries.size(); j++)
        {
        	rConnection.length += rConnection.geometries[j].length;
        }

		// --- add correct lane ------------------------------------------------
		vector<laneSection> secs;

        // flip lanes if in false direction 
        if (fromPos == "start") 
        for (int j = 0; j < lS1.lanes.size(); j++)
            lS1.lanes[j].id *= -1;

        if (toPos == "end") 
        for (int j = 0; j < lS2.lanes.size(); j++)
            lS2.lanes[j].id *= -1;

        lS1.s = 0;
        secs.push_back(lS1);

        double dPos = findMaxLaneId(lS1) - findMaxLaneId(lS2);
        double dNeg = findMinLaneId(lS1) - findMinLaneId(lS2);
    
        double sNeeded = 0;
        while (dPos < 0) 
        {   
            addLaneWidening(secs, findMaxLaneId(lS1), sNeeded, 0);
            sNeeded += 50;
            dPos++;
        }
        while (dNeg < 0) 
        {   
            addLaneWidening(secs, findMinLaneId(lS1), sNeeded, 0);
            sNeeded += 50;
            dNeg++;
        }
        while (dPos > 0) 
        {   
            addLaneDrop(secs, findMaxLaneId(lS1), sNeeded, 0);
            sNeeded += 50;
            dPos--;
        }
        while (dNeg > 0) 
        {   
            addLaneDrop(secs, findMinLaneId(lS1), sNeeded, 0);
            sNeeded += 50;
            dNeg--;
        }

    	rConnection.laneSections = secs;
		
        data.roads.push_back(rConnection);
	}
    return 0;
}