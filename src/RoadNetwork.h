#pragma once
#include <fstream>
#include "Geometry.h"
using namespace std;


class RoadNetwork {


public:

	RoadNetwork();
	~RoadNetwork();

	void print(ofstream& myOutputfile, int kind);
	

	Geometry getGeometry();


private:
	Geometry geometry;

};