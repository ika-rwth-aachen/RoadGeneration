#ifndef BUILD_ROAD
#define BUILD_ROAD

/**
 * @file buildRoad.h
 *
 * @brief file contains method for building up a road
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "curve.h"
#include "addLaneSections.h"
#include "pugixml.hpp"

extern settings setting;

/**
 * @brief function computes first and last considered geometry in s interval 
 * 
 * @param roadIn        road input data
 * @param foundfirst    id of the first considerd geometry
 * @param foundlast     id of the last considerd geometry
 * @param sStart        start of the s interval
 * @param sEnd          end of the s interval
 * @return int          error code
 */
int computeFirstLast(pugi::xml_node roadIn, int &foundfirst, int &foundlast, double &sStart, double &sEnd);

/**
 * @brief function genetares the geometries of the reference line based on an s intervall
 * 
 * @param roadIn    road input data
 * @param r         road data containing the reference line information
 * @param sStart    start of the s interval
 * @param sEnd      end of the s interval
 * @return int      error code
 */
int generateGeometries(pugi::xml_node roadIn, road &r, double &sStart, double &sEnd);

/**
 * @brief function shift the geometries of the reference line
 * 
 * @param r         road input data 
 * @param sStart    start of s interval 
 * @param sEnd      end of s interval 
 * @param s0        s0 is origin of reference line
 * @param x0        global x position of origin
 * @param y0        global y position of origin
 * @param phi0      global angle of origin
 * @return int      error code
 */
int shiftGeometries(road &r, double sStart, double sEnd, double s0, double x0, double y0, double phi0);

/**
 * @brief function flips geometries of reference line
 * 
 * @param r     road data
 * @int         error code
 */
int flipGeometries(road &r);

/**
 * @brief function adds lanes to the road structure
 * 
 * @param roadIn    road input data
 * @param r         road data
 * @param mode      defines the mode (flipped or not)
 * @return int      error code
 */
int addLanes(pugi::xml_node roadIn, road &r, int mode);

/**
 * @brief function adds additional lane sections for changing lane structure
 *  
 * @param roadIn                road input data
 * @param r                     road data
 * @param automaticWidening     automatic widing input data
 * @return int                  error code
 */
int addLaneSectionChanges(pugi::xml_node roadIn, road &r, pugi::xml_node automaticWidening);

/**
 * @brief function builds a road based on the input data
 * 
 * @param road              road input data
 * @param r                 road data
 * @param sStart            starting s position
 * @param sEnd              ending s position
 * @param automaticWidening automaticWiding input data
 * @param s0                position of s where x0, y0, phi0 should be reached
 * @param x0                reference x position
 * @param y0                reference y position
 * @param phi0              reference angle
 * @return int              error code
 */
int buildRoad(pugi::xml_node roadIn, road &r, double sStart, double sEnd, pugi::xml_node automaticWidening, double s0, double x0, double y0, double phi0);

#endif