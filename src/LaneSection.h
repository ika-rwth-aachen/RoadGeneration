#pragma once
#include <string>
using namespace std;

class LaneSection {


public:

	void setStart(double sIn);
	void print(ofstream& myOutputfile, int kind);
	void printLL(ofstream& myOutputfile, int kind);
	void printCL(ofstream& myOutputfile, int kind);
	void printRL(ofstream& myOutputfile, int kind);

	double getStart();

private:
	double s =0;

};