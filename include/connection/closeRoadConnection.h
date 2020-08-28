#ifndef CLOSE_ROAD_CONNECTION
#define CLOSE_ROAD_CONNECTION

/**
 * @file closeRoadConnection.h
 *
 * @brief file contains methods for building the geometry of a new road
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "geometries.h"
#include "interface.h"

/**
 * @brief function generates geometries recursivly so that the two input points are connected with a feasible geometry
 * 
 * @param geo   vector which contains all output geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  error code
 */
int closeRoadConnection(std::vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2);

#endif