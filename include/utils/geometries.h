#ifndef GEOMETRY
#define GEOMETRY

/**
 * @file geometries.h
 *
 * @brief file contains geometry functionalities
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "interface.h"

#include <vector>


/**
 * @brief function adds a line
 * 
 * @param geo   vector of all geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point (not used here)
 * @return int  error code
 */
int addLine(std::vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2);

/**
 * @brief function adds an arc
 * 
 * @param geo   vector of all geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  error code
 */
int addArc(std::vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2);

/**
 * @brief function adds a composite curve
 * 
 * @param geo   vector of all geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  error code
 */
int addCompositeCurve(std::vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2);

#endif