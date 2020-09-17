#ifndef CURVE
#define CURVE

/**
 * @file curve.h
 *
 * @brief file contains curve function for calculating positions along geometries
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */


#include "helper.h"

#include <math.h>

/**
 * @brief function computes the fresnel integral
 * 
 * @param s     position s in a spiral
 * @param x     result for x component
 * @param y     result for y component
 * @return int  error code
 */
int fresnel(double s, double &x, double &y);

/**
 * @brief function computes the x, y, phi value for a given geometry at position s
 * 
 * @param s     position where x, y, phi should be computed
 * @param geo   geometry
 * @param x     start value of geometry and holds resulting value for x
 * @param y     start value of geometry and holds resulting value for y
 * @param phi   start value of geometry and holds resulting value for phi
 * @param fd    determines if finite differences should be used for derivative
 * @return int  error code
 */
int curve(double s, geometry geo, double &x, double &y, double &phi, int fd);

#endif