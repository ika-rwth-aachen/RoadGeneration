/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file linkSegments.h
 *
 * @brief file contains methodology for linking two segments 
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */

#include<map>
#include<queue>


/**
 * @brief Resolves conflicts that occur when segments are linked end to end or start to start
 * 
 * @param data road network data
 * @return int error code
 */
int resolveAlignmentConflicts(roadNetwork &data)
{
	//assumption that all user provided segments are linked by now.
	for(road &r: data.roads)
	{

		if(r.successor.id != -1)
		{
			road *suc;
			for(road &rr: data.roads)
			{
				if(rr.id == r.successor.id)
				{
					suc = &rr;
				}
			}

			if(isJunction(data, r.id) || isJunction(data, suc->id))
			{
				continue;
			}

			if(r.successor.contactPoint == suc->predecessor.contactPoint)
			{
				int borderingLaneSection = 0; 
				if(r.successor.contactPoint == endType)
					borderingLaneSection = r.laneSections.size() - 1;
				vector<int> laneIds;
				for(lane &l: r.laneSections.at(borderingLaneSection).lanes)
				{
					laneIds.insert(laneIds.begin(), l.id);
				}

				//switch all lane links
				for(int i = 0; i < r.laneSections.at(borderingLaneSection).lanes.size(); i ++)
				{
					r.laneSections.at(borderingLaneSection).lanes[i].sucId = laneIds[i];
				}
		
				// switch everything for the successor aswell
				if(suc->predecessor.contactPoint == endType)
					borderingLaneSection = suc->laneSections.size() - 1;

				laneIds.clear();
				for(lane &l: suc->laneSections.at(borderingLaneSection).lanes)
				{
					laneIds.insert(laneIds.begin(), l.id);
				}

				//switch all lane links
				for(int i = 0; i < suc->laneSections.at(borderingLaneSection).lanes.size(); i ++)
				{
					suc->laneSections.at(borderingLaneSection).lanes[i].preId = laneIds[i];
				}
				cout << "switched road " << r.id << " and succ " <<suc->id << endl;

			}
		}
	}
	return 0;
}

/**
 * @brief Removes lane links to junctions or empty roads
 * 
 * @param data 
 * @return int error code
 */
int resolveIllegalLinkConflcits(roadNetwork &data)
{
	for(road &r: data.roads)
	{
		if(isJunction(data, r.id))
			continue;
		
		if(isJunction(data, r.successor.id) || r.successor.id == -1)
		{
			int lsId = 0;
			if(r.successor.contactPoint == endType)
				lsId = r.laneSections.size() -1;
			for(lane & l: r.laneSections.at(lsId).lanes)
			{
				l.sucId = UNASSIGNED;
			}
		}

		if(isJunction(data, r.predecessor.id) || r.predecessor.id == -1)
		{
			int lsId = 0;
			if(r.predecessor.contactPoint == endType)
				lsId = r.laneSections.size() -1;
			for(lane & l: r.laneSections.at(lsId).lanes)
			{
				l.preId = UNASSIGNED;
			}
		}

		

	}
	return 0;
}
/**
 * @brief Resolves lane linking conflicts that result from segments being linked start to start or end to end
 * 
 * @return int 
 */
int resolveLaneLinkConflicts(roadNetwork &data)
{	
	if(resolveAlignmentConflicts(data))
	{
		throwError("Could not resolve Alignment conflicts");
		return 1;
	}
	if (resolveIllegalLinkConflcits(data))
	{
		throwError("Could not remove lane links to junctions");
		return 1;
	}
	
	return 0;
}

/**
 * @brief Transforms toSegments position according to the from segment and the semgment link data to the coordinate system of the reference segment
 * 
 * @param segmentLink segment to link
 * @param data road network data
 * @param swap if true the from and toSegment and road are swapped
 * @return int error code
 */
int transformRoad(DOMElement *segmentLink, roadNetwork &data, bool swap = false)
{

	// get properties
	
	int fromSegment = readIntAttrFromNode(segmentLink, "fromSegment");
	int toSegment = readIntAttrFromNode(segmentLink, "toSegment");
	int fromRoadId = readIntAttrFromNode(segmentLink, "fromRoad");
	int toRoadId = readIntAttrFromNode(segmentLink, "toRoad");
	string fromPos = readStrAttrFromNode(segmentLink, "fromPos");
	string toPos = readStrAttrFromNode(segmentLink, "toPos");
	if(swap) 
	{
		fromSegment = readIntAttrFromNode(segmentLink, "toSegment");
		toSegment = readIntAttrFromNode(segmentLink, "fromSegment");
		fromRoadId = readIntAttrFromNode(segmentLink, "toRoad");
		toRoadId = readIntAttrFromNode(segmentLink, "fromRoad");
		fromPos = readStrAttrFromNode(segmentLink, "toPos");
		toPos = readStrAttrFromNode(segmentLink, "fromPos");
	}
	road fromRoad;
	road toRoad;



	// assumption is that "fromSegement" is already linked to reference frame

	double fromX, fromY, fromHdg;
	double toX, toY, toHdg;
	bool fromIsRoundabout 	= false;
	bool toIsRoundabout  	= false;
	bool fromIsJunction 	= false;
	bool toIsJunction 		= false;

	// check if fromSegment is junction
	for (auto &&j : data.junctions)
		if (j.id == fromSegment)
			fromIsJunction = true;

	// check if toSegment is junction
	for (auto &&j : data.junctions)
		if (j.id == toSegment)
			toIsJunction = true;


	/*check if either one of the segments is a roundabout*/

	for(junctionGroup &jg: data.juncGroups)
	{
		if(jg.type != roundaboutType) continue;

		if(toSegment == jg.id)
		{
			toIsRoundabout = true;

		}
		if(fromSegment == jg.id)
		{
			fromIsRoundabout= true;

		}
	}
	//-------------------END roundabout namespace fix---------------------------------

	// save from position
	for (auto &&r : data.roads)
	{
			
		if (r.inputSegmentId != fromSegment || r.inputId != fromRoadId)
			continue;
		if (fromIsJunction && r.inputPos != fromPos)
			continue;


		fromRoad = r;
		fromRoadId = r.id;
		// if junction, the contact point is always at "end" of a road
		if (fromIsJunction || fromIsRoundabout)
		{
			if(fromPos == "start")
			{
				throwWarning("junctions and roundabouts must be linked at end positions!");
			}
			fromPos = "end";

		}

		if (fromPos == "start")
		{
			fromX = r.geometries.front().x;
			fromY = r.geometries.front().y;
			fromHdg = r.geometries.front().hdg;
		}
		else if (fromPos == "end")
		{
			geometry g = r.geometries.back();
			curve(g.length, g, g.x, g.y, g.hdg, 1);
			fromX = g.x;
			fromY = g.y;
			fromHdg = g.hdg;
		}
	}

	// save to position
	for (auto &&r : data.roads)
	{
		if (r.junction != toSegment || r.inputId != toRoadId) 
			continue;
		if (toIsJunction && r.inputPos != toPos)
		{
			continue;
		}
		toRoad = r;
		toRoadId = r.id;


		// if junction, the contact point is always at "end" of a road
		if (toIsJunction || toIsRoundabout)
		{
			if(toPos == "start")
			{
				throwWarning("junctions and roundabouts must be linked at end positions!");
			}
			toPos = "end";
	
		}
		if (toPos == "start")
		{
			toX = r.geometries.front().x;
			toY = r.geometries.front().y;
			toHdg = r.geometries.front().hdg;
		}
		else if (toPos == "end")
		{
			geometry g = r.geometries.back();
			toX = g.x;
			toY = g.y;
			toHdg = g.hdg;
			curve(g.length, g, toX, toY, toHdg, 1);
		}
	}

	// --- rotate and shift current road according to from position --------
	double dx, dy;

	// compute hdgOffset between the two segments
	double dPhi = fromHdg - toHdg + M_PI;
	if (fromPos == "start")
		dPhi += M_PI;
	if (toPos == "start")
		dPhi += M_PI;
	fixAngle(dPhi);

	// if toPos is end, the actual toPos has to be computed
	if (toPos == "end")
	{
		if(toRoad.id == -1){
			std::cerr << "ERR: 'Road linking is wrong!'" << std::endl;
			std::cerr << "    couldn't find toSegment " << toSegment << " or toRoadID " << toRoadId << std::endl;
			return -1;
		}
		geometry g = toRoad.geometries.back();
		toX = g.x * cos(dPhi) - g.y * sin(dPhi);
		toY = g.x * sin(dPhi) + g.y * cos(dPhi);
		toHdg = g.hdg + dPhi;
		curve(g.length, g, toX, toY, toHdg, 1);
	}

	// compute x / y offset between the two segments
	dx = fromX - toX;
	dy = fromY - toY;

	// shift all geometries which belong to the toSegment according two the offsets determined above
	for (auto &&r : data.roads)
	{
		if (r.junction != toSegment && r.roundAboutInputSegment != toSegment)
			continue;
		

		for (auto &&g : r.geometries)
		{
			double x = g.x * cos(dPhi) - g.y * sin(dPhi);
			double y = g.x * sin(dPhi) + g.y * cos(dPhi);

			g.x = x + dx;
			g.y = y + dy;
			g.hdg = g.hdg + dPhi;
			fixAngle(g.hdg);
		}
	}

	for (auto &&r : data.roads)
	{
		if(!swap)
		{
			if (r.id == toRoadId){
				r.predecessor.id = fromRoadId;
				r.predecessor.contactPoint = (fromPos == "start") ? startType : endType;
			}
			
			if (r.id == fromRoadId){
				r.successor.id = toRoadId;
				r.successor.contactPoint = (toPos == "start") ? startType : endType;
			}
		}
		else
		{
			if (r.id == toRoadId){
				r.successor.id = fromRoadId;
				r.successor.contactPoint = (fromPos == "start") ? startType : endType;
			}
			
			if (r.id == fromRoadId){
				r.predecessor.id = toRoadId;
				r.predecessor.contactPoint = (toPos == "start") ? startType : endType;
			}
		}
	}
	//mark every road that belongs to the from- or toSegment as linked
	for(auto &r: data.roads)
	{
		if(r.junction == toSegment || r.roundAboutInputSegment == toSegment || r.junction == fromSegment || r.roundAboutInputSegment == fromSegment)
		{
			r.isLinkedToNetwork = true;
		}
		if(r.inputId == toSegment || r.inputId == toSegment)
		{
			r.isLinkedToNetwork = true;
		}
	}
	return 0;
}

extern settings setting;

/**
 * @brief function links all specified segments 
 *  	the reference frame has to be specified
 * 		two segments can be linked by determine two contactpoints of the roads 
 * 
 * @param doc 	tree structure which contains the input data
 * @param data 	roadNetwork structure generated by the tool
 * @return int 	error code
 */
int linkSegments(const DOMElement* rootNode, roadNetwork &data)
{
	if(!setting.silentMode)
		cout << "Processing linkSegments" << endl;


	DOMElement *links = getChildWithName(rootNode, "links");
	if (links == NULL)
	{
		throwWarning("'links' are not specified in input file.\n\t -> skip segment linking", true);

		resolveLaneLinkConflicts(data);
		return 0;
	}

	// define reference system
	int refId = readIntAttrFromNode(links, "refId");
	double hdgOffset = readDoubleAttrFromNode(links, "hdgOffset");
	double xOffset = readDoubleAttrFromNode(links, "xOffset");
	double yOffset = readDoubleAttrFromNode(links, "yOffset");


	for (auto &&r : data.roads)
	{
		if (r.junction != refId)
			continue;

		for (auto &&g : r.geometries)
		{

			// transform geometries of reference segement into reference system
			double x = g.x * cos(hdgOffset) - g.y * sin(hdgOffset);
			double y = g.x * sin(hdgOffset) + g.y * cos(hdgOffset);

			g.x = x + xOffset;
			g.y = y + yOffset;
			g.hdg = g.hdg + hdgOffset;
		}
	}

	//generate a map to store all outgoing links of each segment
	std::map<int, vector<int>> outgoing_connections;
	std::map<int, vector<int>> incoming_connections;

	for (DOMElement *segmentLink = links->getFirstElementChild();segmentLink != NULL; segmentLink = segmentLink->getNextElementSibling())
	{
		int fromSegment = readIntAttrFromNode(segmentLink, "fromSegment");
		int toSegment = readIntAttrFromNode(segmentLink, "toSegment");

		if(!isIn(outgoing_connections[fromSegment], toSegment))
			outgoing_connections[fromSegment].push_back(toSegment);

		if(!isIn(incoming_connections[toSegment], fromSegment))
			incoming_connections[toSegment].push_back(fromSegment);

	}

	queue<int> toDo = queue<int>(); //remaining segments
	vector<int> transformedIds;
	toDo.push(refId);

	while(!toDo.empty())
	{
		int curId = toDo.front();
		toDo.pop();
		if(isIn(transformedIds, curId)) continue;

		//first find the element in the list. skip if it is the ref element 

		for(int e:  outgoing_connections[curId])
		{
			if(isIn(transformedIds, e)) continue;
		
			for (DOMElement *segmentLink = links->getFirstElementChild();segmentLink != NULL; segmentLink = segmentLink->getNextElementSibling())
			{
				if(curId != readIntAttrFromNode(segmentLink, "fromSegment") || e != readIntAttrFromNode(segmentLink, "toSegment")) continue;
				//process the element------------------
				transformRoad(segmentLink, data);
				//end processessing the element--------
				break; //prevents multiple processing steps if they are defined in the xml (they shoudlnt be)

			}
			toDo.push(e);

		}
		transformedIds.push_back(curId);

		for(int incoming_id : incoming_connections[curId])
		{
			if(isIn(transformedIds, incoming_id)) continue;

			for (DOMElement *segmentLink = links->getFirstElementChild(); segmentLink != NULL; segmentLink = segmentLink->getNextElementSibling())
			{
				if(curId != readIntAttrFromNode(segmentLink, "toSegment") || incoming_id != readIntAttrFromNode(segmentLink, "fromSegment")) continue;
				//process the element------------------
				transformRoad(segmentLink, data, true);
				break; //prevents multiple processing steps if they are defined in the xml (they shoudlnt be)

			}
			toDo.push(incoming_id);
		}
	}

	//check if all roads are connected to the network
	vector<road*> v;
	for(road r: data.roads)
	{
		if(!r.isLinkedToNetwork)
		{
			v.push_back(&r);
		}
	}
	if(v.size() > 0)
	{
		throwWarning("'Not all roads are connected to the road network!'");
		for(road* p: v)
		{
			if(!setting.silentMode)
				cout << "\tRoad " << p->inputSegmentId << " is not linked"<< endl;
			std::cerr << "\tRoad " << p->inputSegmentId << " is not linked"<< endl;
		}


	}

	resolveLaneLinkConflicts(data);

	return 0;
}


