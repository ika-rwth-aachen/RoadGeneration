// file closeRoadNetwork.h

#include "closeRoadConnection.h"

/**
 * @brief function which closes all roads and add correct lane sections
 * 
 * @param doc 	tree structure which contains the input data
 * @param data 	roadNetwork structure generated by this tool
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
	for (pugi::xml_node segmentLink : cRN.children("segmentLink"))
	{
		data.nSegment++;
        road rConnection;
        rConnection.id = data.nSegment * 100 + 1;
        
		int fromSegment = segmentLink.attribute("fromSegment").as_int();
		int toSegment = segmentLink.attribute("toSegment").as_int();
		int fromRoad = segmentLink.attribute("fromRoad").as_int();
		int toRoad = segmentLink.attribute("toRoad").as_int();	
		string fromPos = (string)segmentLink.attribute("fromPos").value();
		string toPos = (string)segmentLink.attribute("toPos").value();

		double fromX,fromY,fromHdg;
		double toX,toY,toHdg;
        laneSection lS1, lS2;
		bool found = false;

		// save from position
		for(auto&& r : data.roads)
		{
			if (r.id != fromRoad) continue;
				
            found = true;
            rConnection.predecessor.id = r.id;
            rConnection.predecessor.contactPoint = endType;

			if (fromPos == "start")
			{
				fromX = r.geometries.front().x;
				fromY = r.geometries.front().y;
				fromHdg = r.geometries.front().hdg;
                r.predecessor.id = rConnection.id;
				r.predecessor.contactPoint = startType;
                lS1 = r.laneSections.front();
			}
			else if (fromPos == "end")
			{
				geometry g = r.geometries.back();
				curve(g.length,g, g.x, g.y, g.hdg, 1);
				fromX = g.x;
				fromY = g.y;
				fromHdg = g.hdg;
                r.successor.id = rConnection.id;
				r.successor.contactPoint = startType;
                lS1 = r.laneSections.back();
			}
			else
			{
				cerr << "ERR: wrong position for fromPos is specified." << endl;
				cerr << "\t -> use 'start' or 'end'" << endl;
				return 1;
			}
		}
		if (!found)
		{
			cerr << "ERR: from road not found." << endl;
			return 1;
		}

		// save to position
		found = false;
		for(auto&& r : data.roads)
		{
			if (r.junction != toSegment || r.id != toRoad) continue;
            
			found = true;
            rConnection.successor.id = r.id;
            rConnection.successor.contactPoint = endType;

			if (toPos == "start")
			{
				toX = r.geometries.front().x;
				toY = r.geometries.front().y;
				toHdg = r.geometries.front().hdg;
                r.predecessor.id = rConnection.id;
				r.predecessor.contactPoint = endType;
                lS2 = r.laneSections.front();
			}
			else if (toPos == "end")
			{
				geometry g = r.geometries.back();
				toX = g.x;
				toY = g.y;
				toHdg = g.hdg;
				curve(g.length,g, toX, toY, toHdg, 1);
                r.successor.id = rConnection.id;				
				r.successor.contactPoint = endType;
                lS2 = r.laneSections.back();
			}
			else
			{
				cerr << "ERR: wrong position for toPos is specified." << endl;
				cerr << "\t -> use 'start' or 'end'" << endl;
				return 1;
			}
        }
		if (!found)
		{
			cerr << "ERR: to road not found." << endl;
			return 1;
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

		// --- add correct lanes -----------------------------------------------
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

        double dPos = abs(findMaxLaneId(lS2)) - abs(findMaxLaneId(lS1));
        double dNeg = abs(findMinLaneId(lS2)) - abs(findMinLaneId(lS1));
    
		double ds = 25;
        double sNeeded = 0;
        while (dPos > 0) 
        {   
            addLaneWidening(secs, findMaxLaneId(lS1), sNeeded, ds, false);
            sNeeded += ds;
            dPos--;
        }
        while (dNeg > 0) 
        {   
            addLaneWidening(secs, findMinLaneId(lS1), sNeeded, ds, false);
            sNeeded += ds;
            dNeg--;
        }
        while (dPos < 0) 
        {   
            addLaneDrop(secs, findMaxLaneId(lS1), sNeeded, ds);
            sNeeded += ds;
            dPos++;
        }
        while (dNeg < 0) 
        {   
            addLaneDrop(secs, findMinLaneId(lS1), sNeeded, ds);
            sNeeded += ds;
            dNeg++;
        }

		// --- adjust lanewidth of last section to be equal to lS2 -------------
		for (int j = 0; j < secs.back().lanes.size(); j++)
		{
			lane curLane = secs.back().lanes[j];
			int id = curLane.id;
			
			lane refLane;
			findLane(lS2,refLane,id);
			
			double w1 = curLane.w.a;
			double w2 = refLane.w.a;
			if (w1 == w2) continue;

			double length = rConnection.length - sNeeded; 

			curLane.w.d = -2 * (w2 - w1) / pow(length,3);
    		curLane.w.c =  3 * (w2 - w1) / pow(length,2);
    		curLane.w.b = 0;
    		curLane.w.a = w1;

			secs.back().lanes[j] = curLane;
		}
		
		road tmpRoad;
		int fromRoadId = findRoad(data.roads,tmpRoad, fromRoad);
		int toRoadId   = findRoad(data.roads,tmpRoad, toRoad);

		// add lane links
		for (int j = 0; j < secs.front().lanes.size(); j++) 
		{
			secs.front().lanes[j].preId = secs.front().lanes[j].id;
			if (fromPos == "start") 
			{
				secs.front().lanes[j].preId *= -1;
				data.roads[fromRoadId].laneSections.front().lanes[j].preId *= -1;
			}
		}

		for (int j = 0; j < secs.back().lanes.size(); j++)
		{
			secs.back().lanes[j].sucId = secs.back().lanes[j].id;
			if (toPos == "end") 
			{
				secs.back().lanes[j].sucId *= -1;
				data.roads[toRoadId].laneSections.back().lanes[j].sucId *= -1;
			}
		}

    	rConnection.laneSections = secs;

        data.roads.push_back(rConnection);
	}
    return 0;
}