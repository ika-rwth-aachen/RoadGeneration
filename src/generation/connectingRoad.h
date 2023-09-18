/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file connectingRoad.h
 *
 * @brief file contains method for generating connecting road segment
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */
#include "generation/elevation.h"
#include "algorithm"

extern settings setting;

/**
 * @brief function generates the road for a connecting road which is specified in the input file
 *  
 * @param node  input data from the input file for the connecting road
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int connectingRoad(DOMElement *node, roadNetwork &data)
{
    // define segment
    data.nSegment++;
    DOMElement* mainRoad = getChildWithName(node, "road");

    DOMElement* dummy = NULL;

    if (!mainRoad)
    {
        cerr << "ERR: specified road is not found.";
        return 1;
    }

    //--- generate roads ------------------------------------------------------
    if(!setting.suppressOutput)
        cout << "\tGenerating roads" << endl;

    road r;
    int id = readIntAttrFromNode(mainRoad, "id");
    r.id = 100 * readIntAttrFromNode(node, "id") + id;
    r.inputSegmentId = readIntAttrFromNode(node, "id");
    r.junction = readIntAttrFromNode(node, "id"); 

    // read in elevation data---------------------------------
    DOMElement* elevationProfileNode = getChildWithName(node, "elevationProfile");
    for (DOMElement *itt = elevationProfileNode->getFirstElementChild();itt != NULL; itt = itt->getNextElementSibling())
    {
            if (readNameFromNode(itt) != "elevationPoint")
                continue;
            elevationProfile ep;
            ep.sOffset = readDoubleAttrFromNode(itt, "s");
            ep.tOffset = readDoubleAttrFromNode(itt, "height");
            ep.radius  = readDoubleAttrFromNode(itt, "r");
            ep.inputId = readIntAttrFromNode(itt, "id");
            cout << "found elevation point " << ep.inputId << " " << ep.sOffset << " " << ep.tOffset << " " << ep.radius << endl;

            r.elevationProfiles.push_back(ep);
    }

    //----------------------------------
    r.isConnectingRoad = true; // <- is needed to fix a bug that is caused by using junction attribute to store segment id in linking segments

    //sort elevation by offset. "<" relation is defined in elevationProfile
    std::sort(r.elevationProfiles.begin(), r.elevationProfiles.end());



    DOMElement* tmp = getChildWithName(node, "road");

    if (buildRoad(mainRoad, r, 0, INFINITY, dummy, 0, 0, 0, 0))
    {
        cerr << "ERR: error in buildRoad" << endl;
        return 1;
    }
    if (addObjects(mainRoad, r, data))
    {
        cerr << "ERR: error in addObjects" << endl;
        return 1;
    }
   

    data.roads.push_back(r);

    return 0;
}