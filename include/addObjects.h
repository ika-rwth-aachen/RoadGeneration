#ifndef ADD_OBJECT
#define ADD_OBJECT

#include <string>

#include "utils/interface.h"
#include "roadNetwork.h"
#include "pugixml.hpp"


/**
 * @file addObjects.h
 *
 * @brief file contains methodology for object creation
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

/**
 * @brief function adds a traffic island to the lane structure
 * 
 * @param o         object data
 * @param r         road data
 * @return int      error code
 */
int addTrafficIsland(object o, road &r);

/**
 * @brief function generates a roadwork
 * 
 * @param o         object containing length and position of roadwork   
 * @param r         road which contains the roadwork
 * @param laneId    lane which contains the roadwork
 * @return int      error code
 */
int addRoadWork(object o, road &r, int laneId);

/**
 * @brief function adds parking splot objects
 * 
 * @param o         object data
 * @param r         road data
 * @return int      error code
 */
int addParking(object o, road &r);

/**
 * @brief function adds a bus top
 * 
 * @param o     objct data
 * @param r     road data
 * @return int  error code
 */
int addBusStop(object o, road &r);

/**
 * @brief function gets the object position
 * 
 * @param node      object input data
 * @param o         object data
 * @return int      error code
 */
int getPosition(pugi::xml_node node, object &o);

/**
 * @brief function creates objects
 * 
 * @param inRoad    road input data from input file
 * @param r         road data 
 * @param data      roadNetwork structure where the generated roads and junctions are stored
 * @return int      error code
 */
int addObjects(pugi::xml_node inRoad, road &r, RoadNetwork &data);

/**
 * @brief function creates a signal which is automatically generated
 * 
 * @param r             road which contains the signal
 * @param data          roadNetwork data, which holds the controls
 * @param s             s position of signal
 * @param t             t position of signal
 * @param type          type of signal
 * @param subtype       subtype of signal
 * @param controller    controllerof signal
 * @return int          error code
 */
int addSignal(road &r, RoadNetwork &data, double s, double t, std::string type, std::string subtype, int controller);

#endif