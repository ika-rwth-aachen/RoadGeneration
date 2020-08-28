#ifndef CREATE_ROAD_CONNECTION
#define CREATE_ROAD_CONNECTION

/**
 * @file createRoadConnection.h
 *
 * @brief file contains method for generating road connection in junction area
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "interface.h"

#include <string>

/**
 * @brief function creates a new road connection 
 * 
 * @param r1                road at start point
 * @param r2                road at end point
 * @param r                 resulting road which is connection from r1 to r2
 * @param junc              current junction 
 * @param fromId            start lane Id
 * @param toId              end lane Id
 * @param laneMarkLeft      left roadmarking
 * @param laneMarkRight     right roadmarking
 * @return int              error code
 */
int createRoadConnection(road r1, road r2, road &r, junction &junc, int fromId, int toId, std::string laneMarkLeft, std::string laneMarkRight);

#endif