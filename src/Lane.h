#pragma once
#include <string>
using namespace std;

struct LaneWidth {
	double sOffset = 0;
	double a = 3.75;
	double b = 0;
	double c = 0;
	double d = 0;
};

struct RoadMark {
	double sOffset = 0;
	string type = "solid";
	string weight = "standard";
	string color = "standard";
	double width = 0.13;
	string laneChange = "none";
};



class Lane {


public:

	void setId(int idIn);
	void setType(string typeIn);
	void print(ofstream& myOutputfile, int kind);
	void setLaneWidth(double width);
	void printLW(ofstream& myOutputfile);
	void setRoadMark(double sOffset, string type, string color, double width);
	void printRM(ofstream& myOutputfile);
	void setRoadMarkType(string type);

	int getId();
	string getType();
	LaneWidth getLaneWidth();
	RoadMark getRoadMark();
private:

	RoadMark roadMark;
	LaneWidth laneWidth;
	int id;
	string type = "driving";
};
