/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file closeRoadNetwork.h
 *
 * @brief file contains methods for closing two defined points by a new road
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#include "closeRoadConnection.h"

extern settings setting;

/**
 * @brief function closes roads by adding new road structures
 * 
 * @param doc 	tree structure which contains the input data
 * @param data 	roadNetwork structure generated by this tool
 * @return int 	error code
 */
int closeRoadNetwork(const DOMElement* rootNode, roadNetwork &data)
{
	if(!setting.silentMode)
		std::cout << "Processing closeRoadNetwork" << std::endl;

	DOMElement* closeRoad = getChildWithName(rootNode, "closeRoads"); 
	

	if (closeRoad == NULL)
	{
		throwWarning("'closeRoadNetwork' is not specified in input file.\n\t -> skip closing", true);
		return 0;
	}

	// assumption is that all segments are already linked

	for (DOMElement *segmentLink = closeRoad->getFirstElementChild();segmentLink != NULL; segmentLink = segmentLink->getNextElementSibling())
	{
		if(readNameFromNode(segmentLink) != "segmentLink") continue;

		road rConnection;
		data.nSegment++;
		rConnection.id = data.nSegment * 100 + 1;


		int fromSegment = readIntAttrFromNode(segmentLink, "fromSegment");
		int toSegment = readIntAttrFromNode(segmentLink, "toSegment");
		int fromRoadId = readIntAttrFromNode(segmentLink, "fromRoad");
		int toRoadId = readIntAttrFromNode(segmentLink, "toRoad");
		std::string fromPos = readStrAttrFromNode(segmentLink, "fromPos");
		std::string toPos = readStrAttrFromNode(segmentLink, "toPos");
		road fromRoad;
		road toRoad;



		double fromX, fromY, fromHdg;
		double toX, toY, toHdg;
		laneSection lS1, lS2;
		bool found;

		// check if fromSegment is junction
		bool fromIsJunction = false;
		for (auto &&j : data.junctions)
			if (j.id == fromSegment)
				fromIsJunction = true;

		// check if toSegment is junction
		bool toIsJunction = false;
		for (auto &&j : data.junctions)
			if (j.id == toSegment)
				toIsJunction = true;

		// save from position
		found = false;
		for (auto &&r : data.roads)
		{
			if (r.junction != fromSegment || r.inputId != fromRoadId)
				continue;
			if (fromIsJunction && r.inputPos != fromPos)
				continue;

			fromRoad = r;
			fromRoadId = r.id;
			found = true;

			// if junction, the contact point is always at "end" of a road
			if (fromIsJunction)
				fromPos = "end";

			if (fromPos == "start")
			{
				fromX = r.geometries.front().x;
				fromY = r.geometries.front().y;
				fromHdg = r.geometries.front().hdg;
				r.predecessor.id = rConnection.id;
				r.predecessor.contactPoint = startType;
				rConnection.predecessor.id = fromRoadId;
				rConnection.predecessor.contactPoint = startType;
				lS1 = r.laneSections.front();
			}
			else if (fromPos == "end")
			{
				geometry g = r.geometries.back();
				curve(g.length, g, g.x, g.y, g.hdg, 1);
				fromX = g.x;
				fromY = g.y;
				fromHdg = g.hdg;
				r.successor.id = rConnection.id;
				r.successor.contactPoint = startType;
				rConnection.predecessor.id = fromRoadId;
				rConnection.predecessor.contactPoint = endType;
				lS1 = r.laneSections.back();
			}
			else
			{
				std::cerr << "ERR: wrong position for fromPos is specified." << std::endl;
				std::cerr << "\t -> use 'start' or 'end'" << std::endl;
				return 1;
			}
		}
		if (!found)
		{
			std::cerr << "ERR: from road not found." << std::endl;
			return 1;
		}

		// save to position
		found = false;
		for (auto &&r : data.roads)
		{
			if (r.junction != toSegment || r.inputId != toRoadId)
				continue;
			if (toIsJunction && r.inputPos != toPos)
				continue;

			toRoad = r;
			toRoadId = r.id;
			found = true;

			// if junction, the contact point is always at "end" of a road
			if (fromIsJunction)
				toPos = "end";

			if (toPos == "start")
			{
				toX = r.geometries.front().x;
				toY = r.geometries.front().y;
				toHdg = r.geometries.front().hdg;
				r.predecessor.id = rConnection.id;
				r.predecessor.contactPoint = endType;
				rConnection.successor.id = r.id;
				rConnection.successor.contactPoint = startType;
				lS2 = r.laneSections.front();
			}
			else if (toPos == "end")
			{
				geometry g = r.geometries.back();
				toX = g.x;
				toY = g.y;
				toHdg = g.hdg;
				curve(g.length, g, toX, toY, toHdg, 1);
				r.successor.id = rConnection.id;
				r.successor.contactPoint = endType;
				rConnection.successor.id = r.id;
				rConnection.successor.contactPoint = endType;
				lS2 = r.laneSections.back();
			}
			else
			{
				std::cerr << "ERR: wrong position for toPos is specified." << std::endl;
				std::cerr << "\t -> use 'start' or 'end'" << std::endl;
				return 1;
			}
		}
		if (!found)
		{
			std::cerr << "ERR: to road not found." << std::endl;
			return 1;
		}

		// adjust toHdg
		if(toHdg != 0) //testing
			toHdg += M_PI;
		fixAngle(toHdg);

		// --- build new geometry ----------------------------------------------
		if (closeRoadConnection(rConnection.geometries, fromX, fromY, fromHdg, toX, toY, toHdg)) //the tohdg angle is wrong some of the time
		{
			std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
			return 1;
		}


		
		// --- compute length --------------------------------------------------
		for (int j = 0; j < rConnection.geometries.size(); j++)
		{
			rConnection.length += rConnection.geometries[j].length;
		}

		// --- add lanes -------------------------------------------------------
		std::vector<laneSection> secs;

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

		double ds = setting.laneChange.ds;
		double s = 0;
		// add widenings if necessary
		while (dPos > 0)
		{
			addLaneWidening(secs, findMaxLaneId(lS1), s, ds, false);
			s += ds;
			dPos--;
		}
		while (dNeg > 0)
		{
			addLaneWidening(secs, findMinLaneId(lS1), s, ds, false);
			s += ds;
			dNeg--;
		}
		while (dPos < 0)
		{
			addLaneDrop(secs, findMaxLaneId(lS1), s, ds);
			s += ds;
			dPos++;
		}
		while (dNeg < 0)
		{
			addLaneDrop(secs, findMinLaneId(lS1), s, ds);
			s += ds;
			dNeg++;
		}

		


		// --- adjust lanewidth of last section to be equal to lS2 -------------
		for (int j = 0; j < secs.back().lanes.size(); j++)
		{
			lane curLane = secs.back().lanes[j];
			int id = curLane.id;

			lane refLane;
			findLane(lS2, refLane, id);

			double w1 = curLane.w.a;
			double w2 = refLane.w.a;
			if (w1 == w2)
				continue;

			double length = rConnection.length - s;

			curLane.w.d = -2 * (w2 - w1) / pow(length, 3);
			curLane.w.c = 3 * (w2 - w1) / pow(length, 2);
			curLane.w.b = 0;
			curLane.w.a = w1;

			secs.back().lanes[j] = curLane;
		}

		road tmpRoad;
		int fr = findRoad(data.roads, tmpRoad, fromRoadId);
		int tr = findRoad(data.roads, tmpRoad, toRoadId);

		// --- add lane links --------------------------------------------------
		for (int j = 0; j < secs.front().lanes.size(); j++)
		{
			secs.front().lanes[j].preId = secs.front().lanes[j].id;
			data.roads[fr].laneSections.front().lanes[j].sucId = secs.front().lanes[j].id;

			if (fromPos == "start")
			{
				secs.front().lanes[j].preId *= -1;
				data.roads[fr].laneSections.front().lanes[j].preId *= -1;
			}
		}

		for (int j = 0; j < secs.back().lanes.size(); j++)
		{
			secs.back().lanes[j].sucId = secs.back().lanes[j].id;
			data.roads[tr].laneSections.back().lanes[j].sucId = secs.back().lanes[j].id;

			if (toPos == "end")
			{
				secs.back().lanes[j].sucId *= -1;
				data.roads[tr].laneSections.back().lanes[j].sucId *= -1;
			}
		}
		

		rConnection.laneSections = secs;

		data.roads.push_back(rConnection);
	}

	
	return 0;
}