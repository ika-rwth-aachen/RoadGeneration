#pragma once
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "Road.h"
#include "Geometry.h"
#include "RoadNetwork.h"
#include "Lanes.h"
#include "LaneSection.h"
#include "Lane.h"
#include <vector>
#include <cmath>
using namespace std;


struct RoadSegment {
	vector<Geometry> geometryStore;
	vector<Road> roadStore;
	vector<Lanes> lanesStore;
	vector<LaneSection> laneSectionStore;
	vector<Lane> laneStore;
	vector<Point> pointStore;
	vector<Point> iPointStore;
};






// get tag from line
static string getTag(string line) {

	string tag;
	string::size_type length;
	string::size_type start;

	start = 0;
	start = line.find("<");
	line = line.substr(start);


	//</tag>
	if (line.substr(0,2)=="</") {

		length = line.find(">");
		tag = line.substr(2, length - 2);
	}
	//<tag/>
	else if (line.find("/>") < line.find(" ")) {
		
		length = line.find("/");
		tag = line.substr(1, length-1);
	}
	//<tag ..... />
	else if (line.find("/>") > line.find(" ")){

		length = line.find(" ");
		tag = line.substr(1,length-1);

	}
	// <tag>
	else { 

		length = line.find(">");
		tag = line.substr(1, length-1);

	}

	return tag;
}

// get kind of tag from line
static int getKindOfTag(string line) {

	string tag;
	string::size_type start;
	int kind;

	start = line.find("<");
	line = line.substr(start);


	//</tag> close tag
	if(line.substr(0, 2) == "</") {

		kind = 0;
	}
	//<tag/> open/close tag
	else if (line.find("/>") < line.find(" ")) {
		  
		kind = 1;
	}
	//<tag ..... /> open/close tag with attributes 
	else if ((line.find("/>") > line.find(" "))&&(line.find("/") < line.find(">"))) {
		kind = 1;

	}
	// <tag> open tag
	else {

		kind = 2;
	}

	return kind;
}


// Check if tag has any attributes
 static bool hasAttribute(string line) {

	if (line.find("/>") > line.find(" ")) {

		return true;

	}
	else {

		return false;

	}
}


/*map<string, string> attributes;
// Store all attributes of a tag in a map
void storeAttributes(string line) {

	if (hasAttribute(line) == true) {

		string name;
		string value;
		string::size_type sPosition;
		string::size_type ePosition;
		string::size_type length;
		//getline(myInputfile, line);
		sPosition = line.find("<");
		ePosition = sPosition;
		//map<string, string> attributes;

		while (line.substr(ePosition + 1, 1) != ">") {

			sPosition = line.find(" ", ePosition) + 1;
			ePosition = line.find("=", sPosition);
			length = ePosition - sPosition;
			name = line.substr(sPosition, length);
			sPosition = line.find("\"", ePosition) + 1;
			ePosition = line.find("\"", sPosition);
			length = ePosition - sPosition;
			value = line.substr(sPosition, length);
			attributes[name] = value;

		}
		for (map<string, string>::iterator it = attributes.begin(); it != attributes.end(); it++) {

			cout << it->first << " = " << it->second << '\n';

		}
	}
}*/

 static string getInput(string line, string attribute) {

	if (hasAttribute(line) == true) {

		string temp = "no attributes";
		string::size_type length;
		string::size_type sPos;
		string::size_type ePos;


		temp = line.substr(line.find(attribute));
		sPos = temp.find("=\"")+2;
		if (temp.find(" ")>temp.find(">")) {
			ePos = temp.find(">") - 1;
		}
		else {
			ePos = temp.find("\" ");
		}
		
		length = ePos - sPos;
		temp = temp.substr(sPos, length);
		return temp;
	}
	else {
		return 0;
	}

}


 static void doTagCommand(string tag, string line, RoadNetwork roadNetwork, ofstream& myOutputfile, int kind, RoadSegment& roadSegment) {


	 /*static vector<Geometry> geometryStore;
	 static vector<Road> roadStore;
	 static vector<Lanes> lanesStore;
	 static vector<LaneSection> laneSectionStore;
	 static vector<Lane> laneStore;
	 static vector<Point> pointStore; 
	 static vector<Point> iPointStore;*/

	 //ersten Vektoreintrag bef�lle
	 
	 //store.push_back(roadSegment);

	 if (tag == "RoadNetwork") {
		 //roadNetwork.print(myOutputfile, kind);

	 }
	 else if (tag == "mainRoad") {
		 if (kind == 0) {
		 }
		 else {
			 Road road;
			 road.setId(getInput(line, "id"));
			 //road.setStartId(getInput(line, "idStart"));
			 //road.setEndId(getInput(line, "idEnd"));
			 roadSegment.roadStore.push_back(road);
		 }
	 }
	 else if (tag == "accessRoad") {
		 if (kind == 0) {
		 }
		 else {
			 Road road;
			 road.setId(getInput(line, "id"));
			 //road.setStartId(getInput(line, "idStart"));
			 //road.setEndId(getInput(line, "idEnd"));
			 roadSegment.roadStore.push_back(road);
		 }
	 }
	 else if (tag == "road") {
		 if (kind == 0) {
			 Road* road = &roadSegment.roadStore.back();
			 //road->print(myOutputfile, kind);
		 }
		 else {
			 Road* road = &roadSegment.roadStore.back();
			 road->setLength(stod(getInput(line, "length")));
			// road->print(myOutputfile, kind);
		 }
	 }
	 else if (tag == "planView") {
		 Road* road = &roadSegment.roadStore.back();
		// road->printPV(myOutputfile, kind);
	 }
	 else if (tag == "geometry") {
		 if (kind==0) {
			 Geometry* geometry = &roadSegment.geometryStore.back();
		//	 geometry->print(myOutputfile, kind);
		 }else {
			 Geometry geometry;
			 geometry.setId(stoi(getInput(line, "id")));
			 geometry.setLength(stod(getInput(line, "length")));
			 geometry.setCTEO(stoi(getInput(line, "connectToEndOf")));
			 geometry.setCTSO(getInput(line, "connectToStartOf"));	
			 roadSegment.geometryStore.push_back(geometry);
		 }
	 }
	 else if (tag == "line") {
		 Geometry* geometry = &roadSegment.geometryStore.back();
		 if (geometry->getCTEO() == 0) {
			 geometry->setPoint(0, 0, 0, 0);
			 roadSegment.pointStore.push_back(geometry->getPoint());
		 }
		 Point* point = &roadSegment.pointStore.back();
		 string type = "line";
		 geometry->setType(type);
		 geometry->setData(*point, geometry->getLength());
		 geometry->calculateCoordinates(*point, geometry->getLength());
		 roadSegment.pointStore.push_back(geometry->getPoint());
		// geometry->print(myOutputfile, kind);
	 }
	 /*else if (tag == "line") {
		 Geometry* geometry = &geometryStore.back();
		 if (geometry->getCTEO() == 0) {
			 geometry->setPoint(0, 0, 0, 0);
			 pointStore.push_back(geometry->getPoint());
		 }
		 Point* point = &pointStore.back();
		 string type = "line";
		 geometry->setType(type);
		 geometry->calculateCoordinates(geometry->getType(), geometry->getCTEO(), geometry->getCTSO(), geometry->getLength(), *point);
		 pointStore.push_back(geometry->getPoint());
		 geometry->print(myOutputfile, kind);
	 }*/
	else if (tag == "arc") {
		 Geometry* geometry = &roadSegment.geometryStore.back();
		 if (geometry->getCTEO() == 0) {
			 geometry->setPoint(0, 0, 0, 0);
			 roadSegment.pointStore.push_back(geometry->getPoint());
		 }
		 Point* point = &roadSegment.pointStore.back();
		 string type = "arc";
		 geometry->setType(type);
		 geometry->setArc(stod(getInput(line, "R")));
		 geometry->setData(*point, geometry->getLength(), geometry->getArc());
		 geometry->calculateCoordinates(*point, geometry->getLength(), geometry->getArc());
		 roadSegment.pointStore.push_back(geometry->getPoint());
		// geometry->print(myOutputfile, kind);
		 
	 }
	 else if (tag == "spiral") {
		 Geometry* geometry = &roadSegment.geometryStore.back();
		 if (geometry->getCTEO() == 0) {
			 geometry->setPoint(0, 0, 0, 0);
			 roadSegment.pointStore.push_back(geometry->getPoint());
		 }
		 Point* point = &roadSegment.pointStore.back();
		 string type = "spiral";
		 geometry->setType(type);
		 geometry->setSpiral(stod(getInput(line, "Rs")), stod(getInput(line, "Re")));
		 geometry->setData(*point, geometry->getLength(), geometry->getSRadius(), geometry->getERadius());
		 geometry->calculateCoordinates(*point, geometry->getLength(), geometry->getSRadius(), geometry->getERadius());
		 roadSegment.pointStore.push_back(geometry->getPoint());
		// geometry->print(myOutputfile, kind);
	 }
	 else if (tag == "lanes") {
		 if (kind == 0) {
			 Lanes* lanes = &roadSegment.lanesStore.back();
			// lanes->print(myOutputfile, kind);
		 }
		 else {
			 Lanes lanes;
			 roadSegment.lanesStore.push_back(lanes);
		//	 lanes.print(myOutputfile, kind);
		 }

	 }
	 else if (tag == "laneSection") {
		 if (kind == 0) {
			 LaneSection* laneSection = &roadSegment.laneSectionStore.back();
			// laneSection->print(myOutputfile, kind);
		 }
		 else {
			 LaneSection laneSection;
			 laneSection.setStart(stod(getInput(line, "s")));
			 roadSegment.laneSectionStore.push_back(laneSection);
			// laneSection.print(myOutputfile, kind);
		 }

	 }
	 else if (tag == "leftLanes") {
		 LaneSection* laneSection = &roadSegment.laneSectionStore.back();
		 //laneSection->printLL(myOutputfile, kind);
	 }
	 else if (tag == "centerLine") {
		 LaneSection* laneSection = &roadSegment.laneSectionStore.back();
		// laneSection->printCL(myOutputfile, kind);

	 }
	 else if (tag == "rightLanes") {
		 LaneSection* laneSection = &roadSegment.laneSectionStore.back();
		// laneSection->printRL(myOutputfile, kind);

	 }
	 else if (tag == "lane") {
		 if (kind == 0) {
			 Lane* lane = &roadSegment.laneStore.back();
			// lane->print(myOutputfile, kind);
		 }
		 else {	 
			 Lane lane;
			 lane.setId(stoi(getInput(line, "LaneId")));
			 lane.setType(getInput(line, "type"));
			 roadSegment.laneStore.push_back(lane);
			// lane.print(myOutputfile, kind);
		 }

	 }
	 else if (tag == "laneWidth") {
		 Lane* lane = &roadSegment.laneStore.back();
		 lane->setLaneWidth(stod(getInput(line, "w")));
		// lane->printLW(myOutputfile);
		 int i = roadSegment.laneStore.size();
		 roadSegment.laneStore.push_back(*lane);
		 roadSegment.laneStore.erase(roadSegment.laneStore.begin() + i);

	 }
	 else if (tag == "roadMark") {
		 Lane* lane = &roadSegment.laneStore.back();
		 lane->setRoadMark(stod(getInput(line, "sOffset")), getInput(line, "type"), getInput(line, "color"), stod(getInput(line, "width")));
		// lane->printRM(myOutputfile);
		 int i = roadSegment.laneStore.size();
		 roadSegment.laneStore.push_back(*lane);
		 roadSegment.laneStore.erase(roadSegment.laneStore.begin() +i);
	 }
	 else if (tag == "intersectionPoint") {
		 Geometry* geometry = &roadSegment.geometryStore.front();
		 Geometry* geometry2 = &roadSegment.geometryStore.back();
		 Point* point = &roadSegment.pointStore.front();
		 Point* point2 = &roadSegment.pointStore.back();
		 Point iPoint;
		 geometry->calculateCoordinates(*point, stod(getInput(line, "iPOnMainRoad")));
		 iPoint = geometry->getPoint();
		 roadSegment.iPointStore.push_back(iPoint);
		 geometry2->rotateData(iPoint, stod(getInput(line, "angleToReferenceRoad")));
		 geometry2->setPoint(0, iPoint.xE, iPoint.yE, geometry2->getData().hdg);
		 //geometry2->print(myOutputfile, kind);
		 roadSegment.pointStore[1] = geometry2->getPoint();
	 }
	 else if (tag == "couplerArea") {
		 // EInlesen der Daten
		 // BErechnen der schnittpunktkoordinaten der Referenzlinien
		 // Ermitteln der ANzahl neuer STra�en		 
		 // Definieren der Oreintierung der Stra�en und L�n		 
		 if (kind == 0) {

		 }
		 else {

			 double tempLength = stod(getInput(line, "sOffset"));
			 cout << tempLength;
			 Geometry* geometry = &roadSegment.geometryStore.front();
			 Geometry* geometry2 = &roadSegment.geometryStore.back();
			 Road* road = &roadSegment.roadStore[0];
			 Road* road2 = &roadSegment.roadStore[1];
			 Point point11; //Point on MainRoad
			 Point point12; //Point on MainRoad
			 Point point21; //Point on AccessRoad
			 Point* point = &roadSegment.pointStore[0]; //Startpoint of Geometry 1
			 Point* point2 = &roadSegment.pointStore[1]; //Startpoint of Geometry 2
			 Point* iPoint = &roadSegment.iPointStore.back();
			 double l0 = geometry->getLength();

			 geometry2->calculateCoordinates(*point2, tempLength);
			 point21 = geometry2->getPoint();
			 geometry2->setPoint(0, point21.xE, point21.yE, point21.hdgE);
			 double dl = (geometry2->getLength() - tempLength);
			 geometry2->setLength(dl);
			 geometry2->setData(point21, dl);
			 geometry2->setS(0);
			 roadSegment.pointStore[1] = geometry2->getPoint();
			// geometry2->print(myOutputfile, kind);
			 road2->setId("2");
			 road2->setLength(geometry2->getLength());

			 double deltaX = iPoint->xE- point->xE;
			 double deltaY = iPoint->yE - point->yE;
			 double deltaL = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
			 double l1 = deltaL-tempLength;
			 geometry->setLength(l1);
			// geometry->print(myOutputfile, kind);
			 roadSegment.geometryStore[0]=(*geometry);
			 road->setId("1");
			 road->setLength(geometry->getLength());

			 geometry->calculateCoordinates(*point, l1);
			 point11 = geometry->getPoint();
			 Geometry geometry3;
			 geometry3.setData(point11, (2 * tempLength));
			 geometry3.setType("line");
			 geometry3.setS(0);
			// geometry3.print(myOutputfile, kind);
			 roadSegment.geometryStore.push_back(geometry3);
			 Road road3;
			 road3.setId("3");
			 road3.setLength(geometry3.getLength());
			 roadSegment.roadStore.push_back(road3);
			 Lanes lanes3;
			 roadSegment.lanesStore.push_back(lanes3);
			 LaneSection laneSection3;
			 roadSegment.laneSectionStore.push_back(laneSection3);
			 Lane lane31;
			 lane31.setId(1);
			 lane31.setRoadMarkType("broken");
			 roadSegment.laneStore.push_back(lane31);
			 Lane lane32;
			 lane32.setId(0);
			 lane32.setRoadMarkType("broken");
			 roadSegment.laneStore.push_back(lane32);
			 Lane lane33;
			 lane33.setId(-1);
			 roadSegment.laneStore.push_back(lane33);

			 geometry3.calculateCoordinates(point11, (2 * tempLength));
			 point12 = geometry3.getPoint();
			 Geometry geometry4;
			 double l4 = l0 - deltaL - tempLength;
			 geometry4.setData(point12, l4);
			 geometry4.setType("line");
			 geometry4.setS(0);
			// geometry4.print(myOutputfile, kind);
			 roadSegment.geometryStore.push_back(geometry4);
			 Road road4;
			 road4.setId("4");
			 road4.setLength(geometry4.getLength());
			 roadSegment.roadStore.push_back(road4);
			 Lanes lanes4;
			 roadSegment.lanesStore.push_back(lanes4);
			 LaneSection laneSection4;
			 roadSegment.laneSectionStore.push_back(laneSection4);
			 Lane lane41;
			 lane41.setId(1);
			 roadSegment.laneStore.push_back(lane41);
			 Lane lane42;
			 lane42.setId(0);
			 lane42.setRoadMarkType("broken");
			 roadSegment.laneStore.push_back(lane42);
			 Lane lane43;
			 lane43.setId(-1);
			 roadSegment.laneStore.push_back(lane43);

			 Geometry geometry5;
			 //double lA1 = (3.1416*tempLength) / 2;
			 double deltaX1 = point21.xE - point11.xE;
			 double deltaY1 = point21.yE - point11.yE;
			 double sehne1 = sqrt(pow(deltaX1, 2) + pow(deltaY1, 2));
			 double alpha1 = point21.hdgE - point11.hdgE;
			 double R1 = (sehne1 / (2 * sin(alpha1 / 2)));
			 double lA1 = R1*alpha1;
			 geometry5.setType("arc");
			 geometry5.setData(point11, lA1, R1);
			 geometry5.setS(0);
			// geometry5.print(myOutputfile, kind);
			 roadSegment.geometryStore.push_back(geometry5);
			 Road road5;
			 road5.setId("5");
			 road5.setLength(geometry5.getLength());
			 roadSegment.roadStore.push_back(road5);
			 Lanes lanes5;
			 roadSegment.lanesStore.push_back(lanes5);
			 LaneSection laneSection5;
			 roadSegment.laneSectionStore.push_back(laneSection5);
			 Lane lane51;
			 lane51.setId(1);
			 lane51.setRoadMarkType("solid");
			 roadSegment.laneStore.push_back(lane51);
			 Lane lane52;
			 lane52.setId(0);
			 lane52.setRoadMarkType("none");
			 roadSegment.laneStore.push_back(lane52);
			 Lane lane53;
			 lane53.setId(-1);
			 lane53.setRoadMarkType("none");
			 roadSegment.laneStore.push_back(lane53);

			 Geometry geometry6;
			 //double lA2 = (3.1416*tempLength) / 2;
			 double deltaX2 = point21.xE - point12.xE;
			 double deltaY2 = point21.yE - point12.yE;
			 double sehne2 = sqrt(pow(deltaX2, 2) + pow(deltaY2, 2));
			 double alpha2 = 3.1416 - alpha1;
			 double R2 = (sehne2 / (2 * sin(alpha2 / 2)));
			 double lA2 = R2 * alpha2;
			 geometry6.setType("arc");
			 geometry6.setData(point21, lA2, R2);
			 geometry6.setS(0);
			 geometry6.rotateData(point21, -3.1416);
			// geometry6.print(myOutputfile, kind);
			 roadSegment.geometryStore.push_back(geometry6);
			 Road road6;
			 road6.setId("6");
			 road6.setLength(geometry6.getLength());
			 roadSegment.roadStore.push_back(road6);
			 Lanes lanes6;
			 roadSegment.lanesStore.push_back(lanes6);
			 LaneSection laneSection6;
			 roadSegment.laneSectionStore.push_back(laneSection6);
			 Lane lane61;
			 lane61.setId(1);
			 lane61.setRoadMarkType("solid");
			 roadSegment.laneStore.push_back(lane61);
			 Lane lane62;
			 lane62.setId(0);
			 lane62.setRoadMarkType("none");
			 roadSegment.laneStore.push_back(lane62);
			 Lane lane63;
			 lane63.setId(-1);
			 lane63.setRoadMarkType("none");
			 roadSegment.laneStore.push_back(lane63);
		 }
	 }
}

/*static void printRN(ofstream& myOutputfile, int kind, vector<Geometry>& geometryStore, RoadNetwork& roadNetwork) {
	roadNetwork.print(myOutputfile, kind);
	geometryStore[1].print(myOutputfile, kind);
}*/

 static void parseRoadNetwork(ifstream& myInputfile, RoadNetwork roadNetwork, ofstream& myOutputfile, RoadSegment& roadSegment) {

	string line;
	static vector<RoadSegment> store;

	if (myInputfile.is_open())
	{
		while (getline(myInputfile, line))
		{
			if (!line.empty())
			{
				//cout << "line: " << line << endl;
				string tag = getTag(line);
				//cout << tag << "\n";
				int kind = getKindOfTag(line);
				//cout << kind << "\n";
				doTagCommand(tag, line, roadNetwork, myOutputfile, kind, roadSegment);
			}
		}
		
		myInputfile.close();
	}
	else {
		cout << "Unable to open file";
	}
}

 /*static void printRN(ofstream& myOutputfile, RoadSegment& roadSegment) {
	 myOutputfile << "<OpenDRIVE> \n";
	 for (auto elem : roadSegment.roadStore) {
		 elem.print(myOutputfile, 1);
		 elem.printPV(myOutputfile, 1);
		 for (auto elem : roadSegment.geometryStore) {
			 elem.print(myOutputfile, 1);
			 elem.print(myOutputfile, 0);
		 }
		 elem.printPV(myOutputfile, 0);

		 for (auto elem : roadSegment.lanesStore) {
			 elem.print(myOutputfile,2);

			 for (auto elem : roadSegment.laneSectionStore) {
				 elem.print(myOutputfile, 1);

				 elem.printLL(myOutputfile, 2);
				 for (auto elem : roadSegment.laneStore) {
					 elem.print(myOutputfile, 2);
					 elem.printLW(myOutputfile);
					 elem.printRM(myOutputfile);
					 elem.print(myOutputfile, 0);
				 }
				 elem.printLL(myOutputfile, 0);

				 elem.printCL(myOutputfile, 2);
				 for (auto elem : roadSegment.laneStore) {
					 elem.print(myOutputfile, 2);
					 elem.printRM(myOutputfile);
					 elem.print(myOutputfile, 0);
				 }
				 elem.printCL(myOutputfile, 0);

				 elem.printRL(myOutputfile, 2);
				 for (auto elem : roadSegment.laneStore) {
					 elem.print(myOutputfile, 2);
					 elem.printLW(myOutputfile);
					 elem.printRM(myOutputfile);
					 elem.print(myOutputfile, 0);
				 }
				 elem.printRL(myOutputfile, 0);

				 elem.print(myOutputfile, 0);
			 }
			 elem.print(myOutputfile, 0);
		 }

		 elem.print(myOutputfile, 0);
	 }
	 myOutputfile << "</OpenDRIVE> \n";

 }

 static void printRN(ofstream& myOutputfile, RoadSegment& roadSegment) {
	 myOutputfile << "<OpenDRIVE> \n";
	 int a = 0;
	 int i = 0;

	 cout << roadSegment.roadStore.size() << "\n";
	 cout << roadSegment.lanesStore.size() << "\n";
	 cout << roadSegment.laneStore.size() << "\n";
	 cout << roadSegment.geometryStore.size() << "\n";
	 cout << roadSegment.laneSectionStore.size() << "\n";

	 for (auto elem : roadSegment.roadStore) { 
		 elem.print(myOutputfile, 1);
		 elem.printPV(myOutputfile, 1);
		 roadSegment.geometryStore[a].print(myOutputfile, 1);
		 roadSegment.geometryStore[a].print(myOutputfile, 0);
		 elem.printPV(myOutputfile, 0);
		 roadSegment.lanesStore[a].print(myOutputfile, 2);
		 roadSegment.laneSectionStore[a].print(myOutputfile, 2);
		 roadSegment.laneSectionStore[a].printLL(myOutputfile, 2);	
		 roadSegment.laneStore[a+i].print(myOutputfile, 2);
		 roadSegment.laneStore[a+i].printLW(myOutputfile);
		 roadSegment.laneStore[a+i].printRM(myOutputfile);
		 roadSegment.laneStore[a+i].print(myOutputfile, 0);
		 i++;
		 roadSegment.laneSectionStore[a].printLL(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].printCL(myOutputfile, 2);
		 roadSegment.laneStore[a+i].print(myOutputfile, 2);
		 roadSegment.laneStore[a+i].printRM(myOutputfile);
		 roadSegment.laneStore[a+i].print(myOutputfile, 0);
		 i++;
		 roadSegment.laneSectionStore[a].printCL(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].printRL(myOutputfile, 2);
		 roadSegment.laneStore[a+i].print(myOutputfile, 2);
		 roadSegment.laneStore[a+i].printLW(myOutputfile);
		 roadSegment.laneStore[a+i].printRM(myOutputfile);
		 roadSegment.laneStore[a+i].print(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].printRL(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].print(myOutputfile, 0);
		 roadSegment.lanesStore[a].print(myOutputfile, 0);
		 elem.print(myOutputfile, 0);
		 a++;
			
	 }
	 myOutputfile << "</OpenDRIVE>";

 }*/

 static void printRN(ofstream& myOutputfile, RoadSegment& roadSegment) {
	 myOutputfile << "<OpenDRIVE> \n";
	 int a = 0;
	 int i = 0;

	 cout << roadSegment.roadStore.size() << "\n";
	 cout << roadSegment.lanesStore.size() << "\n";
	 cout << roadSegment.laneStore.size() << "\n";
	 cout << roadSegment.geometryStore.size() << "\n";
	 cout << roadSegment.laneSectionStore.size() << "\n";

	int c = 0;
	 for (auto elem : roadSegment.roadStore) {

		 elem.print(myOutputfile, 1);

		 elem.printPV(myOutputfile, 1);
		 std::string::size_type sz;
		 Geometry e = roadSegment.geometryStore[stoi(elem.getId(),&sz)-1];
		 e.print(myOutputfile, 1);
	 	 e.print(myOutputfile, 0);
		 elem.printPV(myOutputfile, 0);

		 roadSegment.lanesStore[a].print(myOutputfile, 2);
		 roadSegment.laneSectionStore[a].print(myOutputfile, 2);
		 roadSegment.laneSectionStore[a].printLL(myOutputfile, 2);
		 roadSegment.laneStore[i].print(myOutputfile, 2);
		 roadSegment.laneStore[i].printLW(myOutputfile);
		 roadSegment.laneStore[i].printRM(myOutputfile);
		 roadSegment.laneStore[i].print(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].printLL(myOutputfile, 0);
		 i++;
		 roadSegment.laneSectionStore[a].printCL(myOutputfile, 2);
		 roadSegment.laneStore[i].print(myOutputfile, 2);
		 roadSegment.laneStore[i].printRM(myOutputfile);
		 roadSegment.laneStore[i].print(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].printCL(myOutputfile, 0);
		 i++;
		 roadSegment.laneSectionStore[a].printRL(myOutputfile, 2);
		 roadSegment.laneStore[i].print(myOutputfile, 2);
		 roadSegment.laneStore[i].printLW(myOutputfile);
		 roadSegment.laneStore[i].printRM(myOutputfile);
		 roadSegment.laneStore[i].print(myOutputfile, 0);
		 roadSegment.laneSectionStore[a].printRL(myOutputfile, 0);
		 i++;
		 roadSegment.laneSectionStore[a].print(myOutputfile, 0);
		 roadSegment.lanesStore[a].print(myOutputfile, 0);
		 elem.print(myOutputfile, 0);
		 a++;

	 }
	 myOutputfile << "</OpenDRIVE>";
	 myOutputfile.close();
 }


 