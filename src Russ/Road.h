#pragma once
#include <string>
using namespace std;

class Road {


public:

	void setLength(double lengthIn);
	void setId(string idIn);
	void print(ofstream& myOutputfile, int kind);
	void printPV(ofstream& myOutputfile, int kind);

	double getLength();
	string getId();

private:
	double length;
	string id;
};
