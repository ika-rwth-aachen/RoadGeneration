#ifndef CREATE_LANE_CONNECTION
#define CREATE_LANE_CONNECTION

/**
 * @file createLaneConnection.h
 *
 * @brief file contains method for generating lane connection in junction area
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "interface.h"
#include "helper.h"

/**
 * @brief function creates a new lane for a connecting road
 * 
 * @param r         connecting road
 * @param lS1       lanesection of adjacent road at start 
 * @param lS2       lanesection of adjacent road at end
 * @param from      start lane Id
 * @param to        end lane Id
 * @param left      left roadmarking 
 * @param right     right roadmarking
 * @return int      error code
 */
int createLaneConnection(road &r, laneSection lS1, laneSection lS2, int from, int to, std::string left, std::string right);

#endif