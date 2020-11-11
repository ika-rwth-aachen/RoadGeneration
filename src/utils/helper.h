/**
 * @file helper.h
 *
 * @brief file contains helper functionalities
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include <stdio.h>
#include <string.h>
const char *HELP_MESSAGE =
  "\nRoad Generation \n\n"
  "Usage: \n"
  "    road-generation <fileName>       Generates a .xodr file from input file.\n"
  "\nOptions:\n"
  "    -h                               Display help message.\n"
  "    -s                               Disable console output.\n"
  "    -d <fileDir>                     Specify output file directory.\n"
  "    -o <fileName>                    Specify output file name.\n\n";


/**
 * @brief signum function
 * 
 * @param d         input 
 * @return int      sng(d)
 */
int sgn(double d)
{
    if (d > 0)
        return 1;
    if (d < 0)
        return -1;
    return 0;
}

/**
 * @brief function fixes the input angle in the range of [-M_PI, M_PI]
 * 
 * @param a     angle
 * @return int  error code
 */
int fixAngle(double &a)
{
    a = fmod(a + 2 * M_PI, 2 * M_PI);
    if (a > M_PI)
        a = a - 2 * M_PI;

    return 0;
}

/**
 * @brief function find the minimum laneId of current lanesection
 * 
 * @param sec   lanesection for which the laneId should be computed
 * @return int  minimum laneId -> if not found value 100 is stored
 */
int findMinLaneId(laneSection sec)
{
    int min = 100;

    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id < min)
            min = sec.lanes[i].id;
    }
    return min;
}

/**
 * @brief function find the maximum laneId of current lanesection
 * 
 * @param sec   lanesection for which the laneId should be computed
 * @return int  maximum laneId -> if not found value -100 is stored
 */
int findMaxLaneId(laneSection sec)
{
    int max = -100;

    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id > max)
            max = sec.lanes[i].id;
    }
    return max;
}

/**
 * @brief function determines of given laneId is at the boundary of the current lanesection
 * 
 * @param sec       lanesection for which the boundary should be proofed
 * @param id        laneid for which the boundary should be proofed
 * @return true     if the lane is a boundary-lane
 * @return false    if the lane is not a boundary-lane
 */
bool isBoundary(laneSection sec, int id)
{
    if (id == findMaxLaneId(sec))
        return true;
    if (id == findMinLaneId(sec))
        return true;

    return false;
}

/**
 * @brief function returns the lane with given id
 * 
 * @param sec   lanesection in which the lane is stored
 * @param l     lane which has the laneId id
 * @param id    laneId of the lane to find
 * @return int  position in laneSection vector 
 */
int findLane(laneSection sec, lane &l, int id)
{
    for (int i = 0; i < sec.lanes.size(); i++)
    {
        if (sec.lanes[i].id == id)
        {
            l = sec.lanes[i];
            return i;
        }
    }
    return -1;
}

/**
 * @brief function returns the road with the given id
 * 
 * @param roads road vector in which the road is stored
 * @param l     road which has the roadId id
 * @param id    roadId of the lane to find
 * @return int  position in road vector
 */
int findRoad(vector<road> roads, road &r, int id)
{
    for (int i = 0; i < roads.size(); i++)
    {
        if (roads[i].id == id)
        {
            r = roads[i];
            return i;
        }
    }
    return -1;
}

/**
 * @brief function determines lanewidth of the given lane at positon s
 * 
 * @param l         lane for which the lanewidth should be computed
 * @param s         position where the lanewidth should be computed
 * @return double   computed lanewidth
 */
double laneWidth(lane l, double s)
{
    return l.w.a + l.w.b * s + l.w.c * s * s + l.w.d * s * s * s;
}

/**
 * @brief function computes the tOfset of a given lane inside of a lanesection at position s
 * 
 * @param sec       lanesection for which the tOffset should be computed
 * @param id        laneId of the lane with the tOffset
 * @param s         s position of the tOffset
 * @return double   computed tOffset
 */
double findTOffset(laneSection sec, int id, double s)
{
    double tOffset = 0;
    int cur = 0;
    int search = sgn(id) + id;

    while (cur != search)
    {
        lane l;
        int err = findLane(sec, l, cur);
        s += l.w.s;
        tOffset += sgn(id) * laneWidth(l, s);
        cur += sgn(id);
    }

    return tOffset;
}

/**
 * @brief function shifts all lanes in given lanesection from lane with laneId
 * 
 * @param sec   lanesection which should be shifted
 * @param id    lane which is the start of the shift; all outer lanes are shifted
 * @param dir   dir = 1 shift to outer side, die = -1 shift to inner side
 * @return int  error code 
 */
int shiftLanes(laneSection &sec, int id, int dir)
{
    int side = sgn(id);
    int start, end;

    // start point definition
    if (dir == 1 && side == -1)
    {
        start = findMinLaneId(sec);
        end = id - side;
    }
    if (dir == 1 && side == 1)
    {
        start = findMaxLaneId(sec);
        end = id - side;
    }
    if (dir == -1 && side == -1)
    {
        start = id;
        end = findMinLaneId(sec) + side;
    }
    if (dir == -1 && side == 1)
    {
        start = id;
        end = findMaxLaneId(sec) + side;
    }

    while (start != end)
    {
        for (int i = 0; i < sec.lanes.size(); i++)
        {
            if (sec.lanes[i].id == start)
            {
                sec.lanes[i].id += dir * side;
            }
        }
        start -= dir * side;
    }

    return 0;
}

/**
 * @brief function finds the id of the left turn lane
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of left lane
 */
int findLeftLane(laneSection sec, int side)
{

    int search = sgn(side);
    int end;

    if (side > 0)
        end = findMaxLaneId(sec) + sgn(side);
    if (side < 0)
        end = findMinLaneId(sec) + sgn(side);

    // search for leftest marked lane
    while (search != end)
    {
        lane l;
        findLane(sec, l, search);
        if (l.type == "driving" && l.turnLeft)
            return search;
        search += sgn(side);
    }

    return 0;
}

/**
 * @brief function finds the id of the right turn lane
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of right lane
 */
int findRightLane(laneSection sec, int side)
{

    int search;
    int end = 0;

    if (side > 0)
        search = findMaxLaneId(sec);
    if (side < 0)
        search = findMinLaneId(sec);

    // search for rightest marked lane
    while (search != end)
    {
        lane l;
        findLane(sec, l, search);
        if (l.type == "driving" && l.turnRight)
            return search;
        search -= sgn(side);
    }

    return 0;
}

/**
 * @brief function finds the id of the inner middle lane (not marked) 
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of left lane
 */
int findInnerMiddleLane(laneSection sec, int side)
{

    int search = sgn(side);
    int end;

    if (side > 0)
        end = findMaxLaneId(sec) + sgn(side);
    if (side < 0)
        end = findMinLaneId(sec) + sgn(side);

    // search for inner middle marked lane
    while (search != end)
    {
        lane l;
        findLane(sec, l, search);
        if (l.type == "driving" && l.turnStraight)
            return search;
        search += sgn(side);
    }

    return 0;
}

/**
 * @brief function finds the id of the outer middle lane (not marked) 
 * 
 * @param sec   laneSection 
 * @param side  determines current side 
 * @return int  laneId of left lane
 */
int findOuterMiddleLane(laneSection sec, int side)
{
    int search;
    int end = 0;

    if (side > 0)
        search = findMaxLaneId(sec);
    if (side < 0)
        search = findMinLaneId(sec);

    // search for outer middle marked lane
    while (search != end)
    {
        lane l;
        findLane(sec, l, search);
        if (l.type == "driving" && l.turnStraight)
            return search;
        search -= sgn(side);
    }

    return 0;
}

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
int calcFromTo(road r1, road r2, int &from, int &to, int &nF, int &nT, int mode)
{
    int r1_F_L = findLeftLane(r1.laneSections.front(), 1);
    int r1_F_MI = findInnerMiddleLane(r1.laneSections.front(), 1);
    int r1_F_MO = findOuterMiddleLane(r1.laneSections.front(), 1);
    int r1_F_R = findRightLane(r1.laneSections.front(), 1);

    int r1_T_L = findLeftLane(r1.laneSections.front(), -1);
    int r1_T_MI = findInnerMiddleLane(r1.laneSections.front(), -1);
    int r1_T_MO = findOuterMiddleLane(r1.laneSections.front(), -1);
    int r1_T_R = findRightLane(r1.laneSections.front(), -1);

    int r2_F_L = findLeftLane(r2.laneSections.front(), 1);
    int r2_F_MI = findInnerMiddleLane(r2.laneSections.front(), 1);
    int r2_F_MO = findOuterMiddleLane(r2.laneSections.front(), 1);
    int r2_F_R = findRightLane(r2.laneSections.front(), 1);

    int r2_T_L = findLeftLane(r2.laneSections.front(), -1);
    int r2_T_MI = findInnerMiddleLane(r2.laneSections.front(), -1);
    int r2_T_MO = findOuterMiddleLane(r2.laneSections.front(), -1);
    int r2_T_R = findRightLane(r2.laneSections.front(), -1);

    // left to left
    if (mode == 1)
    {
        if (r1_F_L != 0)
        {
            nF = r1_F_MI - r1_F_L;
            from = r1_F_L;
        }
        else
        {
            nF = r1_F_MO - r1_F_MI + 1;
            from = r1_F_MI;
        }

        if (r2_T_L != 0)
        {
            nT = r2_T_L - r2_T_MI;
            to = r2_T_L;
        }
        else
        {
            nT = r2_T_MI - r2_T_MO + 1;
            to = r2_T_MI;
        }
    }

    // middle to middle
    if (mode == 0)
    {
        nF = r1_F_MO - r1_F_MI + 1;
        from = r1_F_MI;

        nT = r2_T_MI - r2_T_MO + 1;
        to = r2_T_MI;
    }

    // right to right
    if (mode == -1)
    {
        if (r1_F_R != 0)
        {
            nF = r1_F_R - r1_F_MO;
            from = r1_F_R;
        }
        else
        {
            nF = r1_F_MO - r1_F_MI + 1;
            from = r1_F_MO;
        }

        if (r2_T_R != 0)
        {
            nT = r2_T_MO - r2_T_R;
            to = r2_T_R;
        }
        else
        {
            nT = r2_T_MI - r2_T_MO + 1;
            to = r2_T_MO;
        }
    }
    return 0;
}

/**
 * @brief function sorts the roads r2,r3,r4 to their corresponding angle
 * 
 * @param r1    reference road with reference angle
 * @param r2    first additional road
 * @param r3    second additional road
 * @param r4    third additional road
 * @return int  error code
 */
int sortRoads(road r1, road &r2, road &r3, road &r4)
{
    double phi1 = r2.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    double phi2 = r3.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    double phi3 = r4.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    fixAngle(phi1);
    fixAngle(phi2);
    fixAngle(phi3);

    road tmpR2 = r2;
    road tmpR3 = r3;
    road tmpR4 = r4;

    if (phi1 < phi2 && phi1 < phi3)
    {
        r2 = tmpR2;
        if (phi2 < phi3)
        {
            r3 = tmpR3;
            r4 = tmpR4;
        }
        else
        {
            r3 = tmpR4;
            r4 = tmpR3;
        }
    }
    else if (phi2 < phi1 && phi2 < phi3)
    {
        r2 = tmpR3;
        if (phi1 < phi3)
        {
            r3 = tmpR2;
            r4 = tmpR4;
        }
        else
        {
            r3 = tmpR4;
            r4 = tmpR2;
        }
    }
    else if (phi3 < phi1 && phi3 < phi2)
    {
        r2 = tmpR4;
        if (phi1 < phi2)
        {
            r3 = tmpR2;
            r4 = tmpR3;
        }
        else
        {
            r3 = tmpR3;
            r4 = tmpR2;
        }
    }
    else
    {
        cerr << "ERR: angles at intersection point are not defined correct.";
        return 1;
    }
    return 0;
}

/**
 * @brief function sorts the roads r2,r3 to their corresponding angle
 * 
 * @param r1    reference road with reference angle
 * @param r2    first additional road
 * @param r3    second additional road
 * @return int  error code
 */
int sortRoads(road r1, road &r2, road &r3)
{
    double phi1 = r2.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    double phi2 = r3.geometries.front().hdg - r1.geometries.front().hdg + M_PI;
    fixAngle(phi1);
    fixAngle(phi2);

    road tmpR2 = r2;
    road tmpR3 = r3;

    if (phi1 < phi2)
    {
        r2 = tmpR2;
        r3 = tmpR3;
    }
    else if (phi2 < phi1)
    {
        r2 = tmpR3;
        r3 = tmpR2;
    }
    else
    {
        cerr << "ERR: angles at intersection point are not defined correct.";
        return 1;
    }
    return 0;
}

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
int computeIP(double x1, double y1, double phi1, double x2, double y2, double phi2, int &type, int &type1, int &type2, double &iPx, double &iPy)
{
    double t2 = (y1 - y2 + tan(phi1) * (x2 - x1)) / (sin(phi2) - tan(phi1) * cos(phi2));
    double t1 = (x2 - x1 + cos(phi2) * t2) / cos(phi1);

    if (t1 > 0)
        type1 = 1;
    if (t1 == 0)
        type1 = 0;
    if (t1 < 0)
        type1 = -1;

    if (t2 > 0)
        type2 = 1;
    if (t2 == 0)
        type2 = 0;
    if (t2 < 0)
        type2 = -1;

    iPx = x1 + t1 * cos(phi1);
    iPy = y1 + t1 * sin(phi1);

    return 0;
}

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
 * @brief function get contactpoint name 
 * 
 * @param t         contactpoint
 * @return string   name of contactpoint
 */
std::string getContactPointType(contactPointType t)
{
    if (t == startType)
        return "start";
    if (t == endType)
        return "end";
    if (t == noneType)
        return "none";

    return "none";
}

/**
 * @brief function get geometrytype name
 * 
 * @param t         geometrytype
 * @return string   name of geometrytype
 */
std::string getGeometryType(geometryType t)
{
    if (t == line)
        return "line";
    if (t == spiral)
        return "spiral";
    if (t == arc)
        return "arc";
    return "none";
}

/**
 * @brief funtion get linktype name
 * 
 * @param t         linkname
 * @return string   name of linkname
 */
std::string getLinkType(linkType t)
{
    if (t == roadType)
        return "road";
    if (t == junctionType)
        return "junction";
    return "none";
}

/**
 * @brief function compares objects by id
 * 
 * @param a         object 1
 * @param b         object 2
 * @return true     id of object 1 is smaller than id of object 2
 * @return false    id of object 2 is smaller than id of object 1
 */
bool compareObjects(const object &a, const object &b)
{
    return a.id < b.id;
}

/**
 * @brief function compares objects by id
 * 
 * @param a         signal 1
 * @param b         signal 2
 * @return true     id of signal 1 is smaller than id of signal 2    
 * @return false    id of signal 2 is smaller than id of signal 1
 */
bool compareSignals(const sign &a, const sign &b)
{
    return a.id < b.id;
}

/**
 * @brief function compares lanes by id
 * 
 * @param a         lane 1    
 * @param b         lane 2
 * @return true     id of lane 1 is smaller than id of lane 2  
 * @return false    id of lane 2 is smaller than id of lane 1
 */
bool compareLanes(const lane &a, const lane &b)
{
    return a.id < b.id;
}


void printHelpMessage()
{
    std::cout << HELP_MESSAGE << std:: endl;       
}

int generateOutputName(char* fileLocation, string &outputName)
{
    string file;
    outputName = outputName.substr(0, outputName.find("."));
    if(strlen(fileLocation) <= 0) return 0;
    
    string name = outputName;
    name = name.substr(name.find_last_of("\\/")+1);
    file = strcat(fileLocation,"/");
    file.append(name);
       

    
    outputName = file;
    return 0;
}

int parseArgs(int argc, char **argv, char* &file, std::string &outputName)
{
    extern settings setting;
    bool foundFile = false, setOutputName = false;
    char defaultPath[]="";
    char* targetFolder = defaultPath;
    outputName = defaultPath;
    

    if(argc < 2){
        std::cout << "ERR: too few arguments!" << std::endl;
        return -1;
    }
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            if(strlen(argv[i]) != 2) {
                std::cout << "ERR: wrong args!" << std::endl;
                return -1;
            }
            
            switch(argv[i][1]){  // look for params
                case 'd': 
                    std::cout << i << "  " <<argc << std::endl;

                    if(argc <= i +1){
                        std::cout <<"ERR: wrong args!" << std::endl;
                        return -1;
                    }
                    targetFolder = argv[++i];
                break;

                case 'o':
                    if(argc <= i +1){
                        std::cout <<"ERR: wrong args!" << std::endl;
                        return -1;
                    }
                    outputName = argv[++i];
                    setOutputName = true;
                break;
                
                case 'h':
                    return 1;
                
                case 's':
                    setting.silentMode = true;
                break;

                default:
                    std::cout << "ERR: wrong args!" << std::endl;
                    return -1;
            }
        }
        else{
            foundFile = true;
            file = argv[i];
        }
    }

    if(!foundFile){
        std::cout << "ERR: too few arguments!" << std::endl;
        return -1;
    }

    if(!setOutputName) outputName = file;

    generateOutputName(targetFolder, outputName);

    return 0;
}