#ifndef ROUNDABOUT
#define ROUNDABOUT

/**
 * @file roundAbout.h
 *
 * @brief file contains method for generating roundabout
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "roadNetwork.h"

/**
 * @brief function generates the roads and junctions for a roundabout which is specified in the input file
 *  
 * @param node  input data from the input file for the roundAbout
 * @param data  roadNetwork structure where the generated roads and junctions are stored
 * @return int  error code
 */
int roundAbout(pugi::xml_node &node, RoadNetwork &data);

#endif