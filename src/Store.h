#pragma once
#include <vector>
#include "Road.h"
#include "Geometry.h"
#include "RoadNetwork.h"
#include "Lanes.h"
#include "LaneSection.h"
#include "Lane.h"

struct Roadelements {
	static vector<Geometry> geometryStore;
	static vector<Road> roadStore;
	static vector<Lanes> lanesStore;
	static vector<LaneSection> laneSectionStore;
	static vector<Lane> laneStore;
	static vector<Point> pointStore;
	static vector<Point> iPointStore;
};


 


