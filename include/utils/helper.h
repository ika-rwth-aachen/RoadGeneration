#ifndef HELPER
#define HELPER

/**
 * @file helper.h
 *
 * @brief file contains helper functionalities
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include "interface.h"
#include "road.h"

#include <vector>
#include <string>

template<typename ... Args>

/**
 * @brief function used for xsd checking output
 * 
 * @param format    xsd validation file
 * @param args      here the project directory PROJ_DIR
 * @return          output buffer 
 */
std::string string_format(const std::string &format, Args... args)
{
    auto size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    char *buf(new char[size]);
    snprintf(buf, size, format.c_str(), args...);

    return std::string(buf, buf + size - 1);
}


/**
 * @brief signum function
 * 
 * @param d         input 
 * @return int      sng(d)
 */
int sgn(double d);

/**
 * @brief function fixes the input angle in the range of [-M_PI, M_PI]
 * 
 * @param a     angle
 * @return int  error code
 */
int fixAngle(double &a);
/**
 * @brief function find the minimum laneId of current lanesection
 * 
 * @param sec   lanesection for which the laneId should be computed
 * @return int  minimum laneId -> if not found value 100 is stored
 */
int findMinLaneId(laneSection sec);

/**
 * @brief function find the maximum laneId of current lanesection
 * 
 * @param sec   lanesection for which the laneId should be computed
 * @return int  maximum laneId -> if not found value -100 is stored
 */
int findMaxLaneId(laneSection sec);

/**
 * @brief function determines of given laneId is at the boundary of the current lanesection
 * 
 * @param sec       lanesection for which the boundary should be proofed
 * @param id        laneid for which the boundary should be proofed
 * @return true     if the lane is a boundary-lane
 * @return false    if the lane is not a boundary-lane
 */
bool isBoundary(laneSection sec, int id);

/**
 * @brief function returns the lane with given id
 * 
 * @param sec   lanesection in which the lane is stored
 * @param l     lane which has the laneId id
 * @param id    laneId of the lane to find
 * @return int  position in laneSection std::vector 
 */
int findLane(laneSection sec, lane &l, int id);

/**
 * @brief function returns the road with the given id
 * 
 * @param roads road std::vector in which the road is stored
 * @param l     road which has the roadId id
 * @param id    roadId of the lane to find
 * @return int  position in road std::vector
 */
int findRoad(std::vector<Road> roads, Road &r, int id);

/**
 * @brief function determines lanewidth of the given lane at positon s
 * 
 * @param l         lane for which the lanewidth should be computed
 * @param s         position where the lanewidth should be computed
 * @return double   computed lanewidth
 */
double laneWidth(lane l, double s);

/**
 * @brief function computes the tOfset of a given lane inside of a lanesection at position s
 * 
 * @param sec       lanesection for which the tOffset should be computed
 * @param id        laneId of the lane with the tOffset
 * @param s         s position of the tOffset
 * @return double   computed tOffset
 */
double findTOffset(laneSection sec, int id, double s);

/**
 * @brief function shifts all lanes in given lanesection from lane with laneId
 * 
 * @param sec   lanesection which should be shifted
 * @param id    lane which is the start of the shift; all outer lanes are shifted
 * @param dir   dir = 1 shift to outer side, die = -1 shift to inner side
 * @return int  error code 
 */
int shiftLanes(laneSection &sec, int id, int dir);

/**
 * @brief function finds the id of the left turn lane
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of left lane
 */
int findLeftLane(laneSection sec, int side);

/**
 * @brief function finds the id of the right turn lane
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of right lane
 */
int findRightLane(laneSection sec, int side);

/**
 * @brief function finds the id of the inner middle lane (not marked) 
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of left lane
 */
int findInnerMiddleLane(laneSection sec, int side);

/**
 * @brief function finds the id of the outer middle lane (not marked) 
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of left lane
 */
int findOuterMiddleLane(laneSection sec, int side);

/**
 * @brief function calculates the possible lane connections
 * 
 * @param r1    input from road
 * @param r2    input to road
 * @param from  output defining from lane
 * @param to    output defining from lane
 * @param nF    amount of possible lanes from
 * @param nT    amount of possile lanes to
 * @param mode  either right-right / middle-middle / left-left
 * @return int  error code
 */
int calcFromTo(Road r1, Road r2, int &from, int &to, int &nF, int &nT, int mode);

/**
 * @brief function sorts the roads r2,r3,r4 to their corresponding angle
 * 
 * @param r1    reference road with reference angle
 * @param r2    first additional road
 * @param r3    second additional road
 * @param r4    third additional road
 * @return int  error code
 */
int sortRoads(Road r1, Road &r2, Road &r3, Road &r4);

/**
 * @brief function sorts the roads r2,r3 to their corresponding angle
 * 
 * @param r1    reference road with reference angle
 * @param r2    first additional road
 * @param r3    second additional road
 * @return int  error code
 */
int sortRoads(Road r1, Road &r2, Road &r3);

/**
 * @brief function computes intersection point
 *      
 * @param x1        x position of first point 
 * @param y1        y position of first point 
 * @param phi1      angle of first point 
 * @param x2        x position of second point       
 * @param y2        y position of second point 
 * @param phi2      angle of second point        
 * @param type      type (parallel, identical, other)
 * @param type1     type of first point (p,n,0)
 * @param type2     type of second point (p,n,0)
 * @param iPx       x position of intersection point
 * @param iPy       y position of intersection point
 * @return int      error code
 */
int computeIP(double x1, double y1, double phi1, double x2, double y2, double phi2, int &type, int &type1, int &type2, double &iPx, double &iPy);


/**
 * @brief function get contactpoint name 
 * 
 * @param t         contactpoint
 * @return string   name of contactpoint
 */
std::string getContactPointType(contactPointType t);

/**
 * @brief function get geometrytype name
 * 
 * @param t         geometrytype
 * @return string   name of geometrytype
 */
std::string getGeometryType(geometryType t);

/**
 * @brief funtion get linktype name
 * 
 * @param t         linkname
 * @return string   name of linkname
 */
std::string getLinkType(linkType t);

/**
 * @brief function compares objects by id
 * 
 * @param a         object 1
 * @param b         object 2
 * @return true     id of object 1 is smaller than id of object 2
 * @return false    id of object 2 is smaller than id of object 1
 */
bool compareObjects(const object &a, const object &b);

/**
 * @brief function compares objects by id
 * 
 * @param a         signal 1
 * @param b         signal 2
 * @return true     id of signal 1 is smaller than id of signal 2    
 * @return false    id of signal 2 is smaller than id of signal 1
 */
bool compareSignals(const sign &a, const sign &b);

/**
 * @brief function compares lanes by id
 * 
 * @param a         lane 1    
 * @param b         lane 2
 * @return true     id of lane 1 is smaller than id of lane 2  
 * @return false    id of lane 2 is smaller than id of lane 1
 */
bool compareLanes(const lane &a, const lane &b);

#endif