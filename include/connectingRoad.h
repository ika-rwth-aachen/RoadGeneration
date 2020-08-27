#ifndef CONNECTING_ROAD
#define CONNECTING_ROAD
/**
 * @file connectingRoad.h
 *
 * @brief file contains method for generating connecting road segment
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "roadNetwork.h"
#include "pugixml.hpp"

/**
 * @brief function generates the road for a connecting road which is specified in the input file
 *  
 * @param node  input data from the input file for the connecting road
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int connectingRoad(pugi::xml_node &node, RoadNetwork &data);


#endif