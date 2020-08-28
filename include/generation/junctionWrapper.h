#ifndef JUNCTION_WRAPPER
#define JUNCTION_WRAPPER
/**
 * @file junctionWrapper.h
 *
 * @brief file contains method for generating junctions
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "addObjects.h"
#include "buildRoad.h"
#include "createRoadConnection.h"

#include <stdio.h>
#include <string>

/**
 * @brief function takes the junction as input argument and calls the junction type function
 * 
 * @param node  input data from the input file for the tjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */

int junctionWrapper(pugi::xml_node &node, RoadNetwork &data);

/**
 * @brief function generates the roads and junctions for a x junction which is specified in the input file
 *  
 * @param node  input data from the input file for the xjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int xjunction(pugi::xml_node &node, RoadNetwork& data);


/**
 * @brief function generates the roads and junctions for a t junction which is specified in the input file
 *  
 * @param node  input data from the input file for the tjunction
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int tjunction(pugi::xml_node &node, RoadNetwork &data);


#endif