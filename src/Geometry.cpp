#pragma once
#include <fstream>
#include "Geometry.h"
#include <cmath>
#include <iostream>
using namespace std;




void Geometry::setType(string inType) {
	 data.type = inType;
}

void Geometry::setId(int inId) {
	id = inId;

}

void Geometry::setS(double sIn) {
	data.s = sIn;
}

void Geometry::setCTEO(int inCteo) {
	cteo = inCteo;

}

void Geometry::setCTSO(string inCtso) {
	ctso = inCtso;

}

void Geometry::setLength(double inLength) {
	data.length = inLength;
}

void Geometry::setArc(double R) {
	radius = R;

}

void Geometry::setSpiral(double Rs, double Re) {
	sRadius = Rs;
	eRadius = Re;

}

void Geometry::setPoint(double s, double x, double y, double hdg) {
	
	point.sE = s;
	point.xE = x;
	point.yE = y;
	point.hdgE = hdg;
}

void Geometry::calculateCoordinates(Point point2, double length) {

	
	point.idE = getId();
	point.sE = point2.sE + length;
	point.xE = point2.xE + length*cos(point2.hdgE);
	point.yE = point2.yE + length * sin(point2.hdgE);;
	point.hdgE = point2.hdgE;
}

void Geometry::calculateCoordinates(Point point2, double length, double radius) {
	double xE;
	double yE;
	
	point.idE = getId();
	point.sE = point2.sE + length;
	xE = radius * sin((length / radius));
	yE = radius * (1 - cos((length / radius)));
	point.hdgE = point2.hdgE + (length / radius);

	// Rotieren des Koordinatensystems zur Bestimmung des Kreisbogens in die letzte tangentiale Orientierung
	point.xE = point2.xE + (xE * cos(point2.hdgE)) - (yE*sin(point2.hdgE));
	point.yE = point2.yE + (xE * sin(point2.hdgE)) + (yE*cos(point2.hdgE));

}

void Geometry::calculateCoordinates(Point point2, double length, double sRadius, double eRadius) {
	double xE; // relative Koordinaten
	double yE;
	double tau;
	double beta;
	tau = (length / 2)*((1 / eRadius) + (1 / sRadius));
	
	
	/*point.idE = getId();
	point.sE = point2.sE + length;
	xE = length*(1 - ((pow(length, 2)) / (40 * pow(eRadius, 2))) + ((pow(length, 4)) / (3456 * pow(eRadius, 4))) - ((pow(length, 6)) / (599040 * (pow(eRadius, 6)))));
	yE = length*((length / (6 * eRadius)) - (pow(length, 3) / (336 * pow(eRadius, 3))) + (pow(length, 5) / (42240 * pow(eRadius, 5))));
	point.hdgE = point2.hdgE + (length / (2 * eRadius));

	// Rotieren des Koordinatensystems zur Bestimmung der Spirale in die letzte tangentiale Orientierung
	point.xE = point2.xE + (xE * cos(point2.hdgE)) - (yE*sin(point2.hdgE));
	point.yE = point2.yE + (xE * sin(point2.hdgE)) + (yE*cos(point2.hdgE));
	*/
	point.sE = point2.sE + length;
	xE = length * (1 - ((pow(length, 2)) / (40 * pow(eRadius, 2))) + ((pow(length, 4)) / (3456 * pow(eRadius, 4))) - ((pow(length, 6)) / (599040 * (pow(eRadius, 6)))));
	yE = length * ((length / (6 * eRadius)) - (pow(length, 3) / (336 * pow(eRadius, 3))) + (pow(length, 5) / (42240 * pow(eRadius, 5))));
	point.hdgE = point2.hdgE + (tau);

	// Rotieren des Koordinatensystems zur Bestimmung der Spirale in die letzte tangentiale Orientierung
	point.xE = point2.xE + (xE * cos(point2.hdgE)) - (yE*sin(point2.hdgE));
	point.yE = point2.yE + (xE * sin(point2.hdgE)) + (yE*cos(point2.hdgE));


}

void Geometry::setData(Point point, double length) {
	
	//Startkoordinaten zuweisen
		data.length = length;
		data.s = point.sE;
		data.x = point.xE;
		data.y = point.yE;
		data.hdg = point.hdgE;
}

void Geometry::setData(Point point, double length, double radius) {

	//Startkoordinaten zuweisen
	data.length = length;
	data.curvature = (1 / radius);
	data.s = point.sE;
	data.x = point.xE;
	data.y = point.yE;
	data.hdg = point.hdgE;
}

void Geometry::setData(Point point, double length, double sRadius, double eRadius) {

	//Startkoordinaten zuweisen
	data.length = length;
	data.curvStart = (1 / sRadius);
	data.curvEnd = (1 / eRadius);
	data.s = point.sE;
	data.x = point.xE;
	data.y = point.yE;
	data.hdg = point.hdgE;
}

void Geometry::rotateData(Point point, double angle) {

	data.x = point.xE;
	data.y = point.yE;
	data.hdg = point.hdgE + angle;

}

/*void Geometry::calculateCoordinates(string type, int cteo, string ctso, double length, EPoint ePoint) {

	data.length = length;

	if (cteo == 0) {

		data.s = 0;
		data.x = 0;
		data.y = 0;
		data.hdg = 0;
		
	}
	else {  //Startkoordinaten zuweisen
		
		data.s = ePoint.sE;
		data.x = ePoint.xE;
		data.y = ePoint.yE;
		data.hdg = ePoint.hdgE;

	}

	// Berechnung des Endpunktes
	ePoint.idE = getId();
	ePoint.sE = data.s + data.length;
	ePoint.xE = data.x + data.length*cos(data.hdg);
	ePoint.yE = data.y + data.length*sin(data.hdg);
	ePoint.hdgE = data.hdg;
	setEPoint(ePoint.sE, ePoint.xE, ePoint.yE, ePoint.hdgE);
}

void Geometry::calculateCoordinates(string type, int cteo, string ctso, double length, EPoint ePoint, double radius) {

	data.curvature = (1 / radius);
	data.length = length;

	if (cteo == 0) {

		data.s = 0;
		data.x = 0;
		data.y = 0;
		data.hdg = 0;

	}
	else { //Startkoordinaten zuweisen

		data.s = ePoint.sE;
		data.x = ePoint.xE;
		data.y = ePoint.yE;
		data.hdg = ePoint.hdgE;

	}

	// Berechnung des Endpunktes
	ePoint.idE = getId();
	ePoint.sE = data.s + data.length;
	ePoint.xE = data.x + radius*sin((data.length / radius));
	ePoint.yE = data.y + radius*(1-cos((data.length / radius)));
	ePoint.hdgE = data.hdg + (data.length/radius);
	setEPoint(ePoint.sE, ePoint.xE, ePoint.yE, ePoint.hdgE);
}

void Geometry::calculateCoordinates(string type, int cteo, string ctso, double length, EPoint ePoint, double sRadius, double eRadius) {

	data.curvStart = (1 / sRadius);
	data.curvEnd = (1 / eRadius);
	data.length = length;

	if (cteo == 0) {

		data.s = 0;
		data.x = 0;
		data.y = 0;
		data.hdg = 0;

	}
	else { //Startkoordinaten zuweisen

		data.s = ePoint.sE;
		data.x = ePoint.xE;
		data.y = ePoint.yE;
		data.hdg = ePoint.hdgE;

	}

	// Berechnung des Endpunktes
	ePoint.idE = getId();
	ePoint.sE = data.s + data.length;
	ePoint.xE = data.x + data.length*(1-(pow(data.length, 2) / (40 * pow(eRadius, 2))) + (pow(data.length, 4) / (3456 * pow(eRadius, 4)))+ (pow(data.length, 6) / (38338560 * pow(eRadius, 6))));
	ePoint.yE = data.y + data.length*((data.length/(6*eRadius))-(pow(data.length, 3)/(336*pow(eRadius, 3)))+ (pow(data.length, 5) / (42240 * pow(eRadius, 5))));
	ePoint.hdgE = data.hdg + (data.length/ (2*eRadius));
	setEPoint(ePoint.sE, ePoint.xE, ePoint.yE, ePoint.hdgE);

}*/

Data Geometry::getData() {
	return data;
}

Point Geometry::getPoint() {
	return point;
}

string Geometry::getType() {
	return data.type;
}

int Geometry::getId() {
	return id;
}

int Geometry::getCTEO() {
	return cteo;
}

string Geometry::getCTSO() {
	return ctso;
}

double Geometry::getLength() {
	return data.length;
}

double Geometry::getArc() {
	return radius;
}

double Geometry::getSRadius() {
	return sRadius;
}

double Geometry::getERadius() {
	return eRadius;
}




void Geometry::print(ofstream& myOutputfile, int kind) {
	
	switch (kind) {

	case 1:
		//myOutputfile <<"	<road length=\"" << data.length << "\" id=\"" << getRId() << "\">\n";
		//myOutputfile << "		<planView>\n";
		myOutputfile <<"			<geometry s=\"" << data.s << "\" x=\"" << data.x << "\" y=\"" << data.y << "\" hdg=\"" << data.hdg << "\" length=\"" << data.length << "\">\n";
		if (data.type == "line") {
			myOutputfile << "				<" << data.type << "/>\n";
		}
		else if (data.type == "arc") {
			myOutputfile << "				<arc curvature=\""<< data.curvature << "\"/>\n";
		}
		else if (data.type == "spiral") {
			myOutputfile << "				<spiral curvStart=\"" << data.curvStart << "\" curvEnd=\"" << data.curvEnd << "\"/>\n";
		}
		break;
	case 2:
		//myOutputfile <<"	<road length=\"" << data.length << "\" id=\"" << getRId() << "\">\n";
		//myOutputfile << "		<planView>\n";
		myOutputfile << "			<geometry s=\"" << data.s << "\" x=\"" << data.x << "\" y=\"" << data.y << "\" hdg=\"" << data.hdg << "\" length=\"" << data.length << "\">\n";
		if (data.type == "line") {
			myOutputfile << "				<" << data.type << "/>\n";
		}
		else if (data.type == "arc") {
			myOutputfile << "				<arc curvature=\"" << data.curvature << "\"/>\n";
		}
		else if (data.type == "spiral") {
			myOutputfile << "				<spiral curvStart=\"" << data.curvStart << "\" curvEnd=\"" << data.curvEnd << "\"/>\n";
		}
		break;
	case 0:
		myOutputfile << "			</geometry>\n";
		break;
	//case 2:
	//	myOutputfile << "	<geometry s=\"" << data.s << "\" x=\"" << data.x << "\" y=\"" << data.y << "\" hdg=\"" << data.hdg << "\" length=\"" << data.length << "\"/>\n";
	//	break;
	}

}
