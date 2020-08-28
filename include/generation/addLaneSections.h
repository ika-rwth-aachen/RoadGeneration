#ifndef ADD_LANE_SECTIONS
#define ADD_LANE_SECTIONS

/**
 * @file addLaneSections.h
 *
 * @brief file contains methodology for lanesection changes such as lanedrop, lane widening or restricted lanes
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "interface.h"
#include "helper.h"

#include <stdio.h>
#include <vector>

/**
 * @brief function adds a laneSection with laneWideing to a given lanesection set
 * 
 * @param secs          std::vector of all lanesections of a road
 * @param side          determines the road's side of the widening
 * @param s             position of lane widening
 * @param ds            length of lane widening
 * @param addouterLane  specifies if additional lane is on the outer side or not
 * @return int          error code
 */
int addLaneWidening(std::vector<laneSection> &secs, int side, double s, double ds, bool addOuterLane);

/**
 * @brief function adds a laneSection with laneDrop to a given lanesection set
 * 
 * @param secs      std::vector of all lanesections of a road
 * @param side      determines the road's side of the drop
 * @param s         position of laneDrop
 * @param ds        length of laneDrop
 * @return int      error code
 */
int addLaneDrop(std::vector<laneSection> &secs, int side, double s, double ds);

/**
 * @brief function adds a laneSection with restricted area after a lane widening
 * 
 * @param secs      std::vector of all lanesections of a road
 * @param side      determines the road's side of the widening
 * @param s         position of lane widening
 * @param ds1       length of lane widening
 * @param ds2       length of restricted areay (always lager than ds1)
 * @return int      error code
 */
int addRestrictedAreaWidening(std::vector<laneSection> &secs, int side, double s, double ds1, double ds2);

/**
 * @brief function adds a laneSection with restricted area after a lane drop
 * 
 * @param secs      std::vector of all lanesections of a road
 * @param side      determines the road's side of the drop
 * @param s         position of lane drop
 * @param ds1       length of lane drop
 * @param ds2       length of restricted areay (always lager than ds1)
 * @return int      error code
 */
int addRestrictedAreaDrop(std::vector<laneSection> &secs, int side, double s, double ds1, double ds2);

/**
 * @brief function creates laneWidenings in junction areas (in opposite s direction), for additional turning lanes 
 * 
 * @param r                 road data
 * @param s                 start of widening
 * @param s                 length of widening
 * @param turn              1 = left lane turn, -1 = right lane turn
 * @param verschwenkung     determines if a 'verschwenkung' exists
 * @param restricted        determines if widening is a restricted area
 * @return int              error code
 */
int laneWideningJunction(road &r, double s, double ds, int turn, bool verschwenkung, bool restricted);

#endif