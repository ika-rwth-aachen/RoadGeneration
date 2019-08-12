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

	// add further segments
	for (pugi::xml_node segmentLink : interfaces.children("segmentLink"))
	{
		int fromSegment = segmentLink.attribute("fromSegment").as_int();
		int toSegment = segmentLink.attribute("toSegment").as_int();
		int fromRoad = segmentLink.attribute("fromRoad").as_int();
		int toRoad = segmentLink.attribute("toRoad").as_int();	
		double fromPos = segmentLink.attribute("fromPos").as_double();
		double toPos = segmentLink.attribute("toPos").as_double();

		// TODO
	}

    return 0;
}