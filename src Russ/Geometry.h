#pragma once
#include <string>
using namespace std;



struct Data {
	string type;
	double s;
	double x;
	double y;
	double hdg;
	double length;
	double curvature;
	double curvStart;
	double curvEnd;
};

struct Point {
	int idE;
	double sE;
	double xE;
	double yE;
	double hdgE;
};


class Geometry {



public:


	
	 void setType(string inType);
	 void setId(int inId);
	 void setCTEO(int inCteo);
	 void setCTSO(string inCtso);
	 void setLength(double inLength);
	 void setArc(double R);
	 void setSpiral(double Rs, double Re);
	 //void calculateCoordinates(string type, int cteo, string ctso, double length, EPoint ePoint);
	 //void calculateCoordinates(string type, int cteo, string ctso, double length, EPoint ePoint, double radius);
	 //void calculateCoordinates(string type, int cteo, string ctso, double length, EPoint ePoint, double sRadius, double eRadius);
	 void calculateCoordinates(Point point2, double length);
	 void calculateCoordinates(Point point2, double length, double radius);
	 void calculateCoordinates(Point point2, double length, double sRadius, double eRadius);
	 void print(ofstream& myOutputfile, int kind);
	 void setPoint(double s, double x, double y, double hdg);
	 void rotateData(Point point, double angle);
	 void setData(Point point, double length);
	 void setData(Point point, double length, double radius);
	 void setData(Point point, double length, double sRadius, double eRadius);
	 void setS(double sIn);
	 //void calculatePoint(string type, );

	 //getPoint()

	 
	 Point  getPoint();
	 Data getData();
	 string getType();
	 int getId();
	 int getCTEO();
	 string getCTSO();
	 double getLength();
	 double getArc();
	 double getSRadius();
	 double getERadius();
	
private:

	Point point;
	Data data;
	int id;
	int cteo;
	string ctso;
	double radius;
	double sRadius;
	double eRadius;

};