// file buildSegments.h

int linkSegments(pugi::xml_document &doc, roadNetwork &data)
{
	pugi::xml_node interfaces = doc.child("roadNetwork").child("interfaces");

	if(!interfaces) cout << "ERR: 'interfaces' not found in input file."  << endl;

	// define reference system
	int referenceId = interfaces.attribute("referenceId").as_int();
	int angleOffset = interfaces.attribute("angleOffset").as_int();
	int xOffset = interfaces.attribute("xOffset").as_int();
	int yOffset = interfaces.attribute("yOffset").as_int();

	for(auto&& r : data.roads)
	{
		if (r.junction != referenceId) continue;

		for (auto&& g : r.geometries)
		{
			double x = g.x * cos(angleOffset) - g.y * sin(angleOffset);
			double y = g.x * sin(angleOffset) + g.y * cos(angleOffset);

			g.x = x + xOffset;
			g.y = y + yOffset;
			g.hdg = g.hdg + angleOffset;
		}
	}

	// add further segments
	for (pugi::xml_node segmentLink : interfaces.children("segmentLink"))
	{
		int fromSegment = segmentLink.attribute("fromSegment").as_int();
		int toSegment = segmentLink.attribute("toSegment").as_int();
		int fromRoad = segmentLink.attribute("fromRoad").as_int();
		int toRoad = segmentLink.attribute("toRoad").as_int();	
		double fromPos = segmentLink.attribute("fromPos").as_double();
		double toPos = segmentLink.attribute("toPos").as_double();

		//fromPos == 1 -> start of Road (s=0)
		//fromPos == 2 -> end of Road (s=length)

		// we assume that fromSegement was already linked to referenceframe

		double fromX,fromY,fromHdg;
		double toX,toY,toHdg;

		// save from position
		for(auto&& r : data.roads)
		{
			if (r.junction != fromSegment || r.id != fromRoad) continue;

			if (fromPos == 1)
			{
				fromX = r.geometries.front().x;
				fromY = r.geometries.front().y;
				fromHdg = r.geometries.front().hdg;
			}
			else if (fromPos == 2)
			{
				geometry g = r.geometries.back();
				curve(g.length,g, g.x, g.y, g.hdg, 1);
				fromX = g.x;
				fromY = g.y;
				fromHdg = g.hdg;
			}
			else
			{
				cout << "ERR: wrong position." << endl;
				exit(0);
			}
		}

		// save to position
		for(auto&& r : data.roads)
		{
			if (r.junction != toSegment || r.id != toRoad) continue;

			if (toPos == 1)
			{
				toX = r.geometries.front().x;
				toY = r.geometries.front().y;
				toHdg = r.geometries.front().hdg;
			}
			else if (toPos == 2)
			{
				geometry g = r.geometries.back();
				toX = g.x;
				toY = g.y;
				toHdg = g.hdg;
				curve(g.length,g, toX, toY, toHdg, 1);				
			}
			else
			{
				cout << "ERR: wrong position." << endl;
				exit(0);
			}

			// rotate and shift current road according to from position
			double dx,dy;
			
			double dPhi = fromHdg - toHdg + M_PI;
			if (fromPos == 1) dPhi += M_PI;
			if (toPos == 1) dPhi += M_PI;
			fixAngle(dPhi);

			if (toPos == 2)
			{
				geometry g = r.geometries.back();
				toX = g.x * cos(dPhi) - g.y * sin(dPhi);
				toY = g.x * sin(dPhi) + g.y * cos(dPhi);
				toHdg = g.hdg + dPhi;
				curve(g.length,g, toX, toY, toHdg, 1);	
			}
			
			dx = fromX - toX;
			dy = fromY - toY;

			for(auto&& r2 : data.roads)
			{
				if (r2.junction != toSegment) continue;
			
				for (auto&& g : r2.geometries)
				{
					double x = g.x * cos(dPhi) - g.y * sin(dPhi);
					double y = g.x * sin(dPhi) + g.y * cos(dPhi);

					g.x = x + dx;
					g.y = y + dy;
					g.hdg = g.hdg + dPhi;
					fixAngle(g.hdg);
				}	
			}
		}

	}

    return 0;
}