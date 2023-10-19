/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file interface.h
 *
 * @brief This file contains important data structures
 *
 * @author Jannik Busse, Christian Geller
 * Contact: jannik.busse@rwth-aachen.de, christian.geller@rwth-aachen.de
 *
 */
#pragma once


#define UNASSIGNED 0

extern settings setting;

// definition of basic types
enum junctionGroupType
{
    roundaboutType,
    unknownType
};
enum contactPointType
{
    startType,
    endType,
    noneType
};
enum geometryType
{
    line,
    spiral,
    arc
};
enum linkType
{
    roadType,
    junctionType
};

/**
 * @brief connection between two lanes
 * 
 */
struct connection
{
    int id = -1;
    int from = -1;
    int to = -1;
    contactPointType contactPoint = noneType;
    int fromLane = -1;
    int toLane = -1;

    string to_string()
    {
        string res = 
        "connection:      " + std::to_string(id) + "\n"
        "from:            " + std::to_string(from) + "\n"
        "to:              " + std::to_string(to) + "\n"
        "connection type: " + std::to_string(contactPoint) + "\n"
        "fromLane:        " + std::to_string(fromLane) + "\n"
        "toLane:          " + std::to_string(toLane);
        return res;
    }
};

/**
 * @brief junction holding several connections
 * 
 */
struct junction
{
    int id = -1;
    vector<connection> connections;


    string to_string()
    {
        string res = 
        "junction " + std::to_string(id) + "\n";
        for(connection c: connections)
        {
            res = res + c.to_string() + "\n";
        }
        return res;
    }

};

/**
 * @brief roadmark holding lane appearance properties
 * 
 */
struct roadmark
{
    double s = 0;
    string type = "solid";
    string weight = "standard";
    string color = "white";
    double width = 0.15;
};

/**
 * @brief width holding the lanewidth with a 3rd order polynomial
 * 
 */
struct width
{
    double s = 0;
    double a = setting.width.standard;
    double b = 0;
    double c = 0;
    double d = 0;
};

/**
 * @brief offset holding the lanesection offset with a 3rd order polynomial
 * 
 */
struct offset
{
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
};

/**
 * @brief material holding the material properties of a lane
 * 
 */
struct material
{
    double s = 0;
    string surface = "asphalt";
    double friction = 0.8;
    double roughness = 0.015;
};

/**
 * @brief lane holding all properties of a lane
 * 
 */
struct lane
{
    int id = -1;
    string type = "driving";
    bool turnLeft = false;    // determine if lane is special left turn lane
    bool turnStraight = true; // determine if lane is normal lane
    bool turnRight = false;   // determine if lane is special right turn lane
    bool level = true;        // determine if lane is parallel to xy plane

    double speed = setting.speed.standard;
    width w;
    roadmark rm;
    material m;
    int preId = -1; // predecessor
    int sucId = -1; // successor
};

/**
 * @brief laneSection holds several lanes in a defined s section
 * 
 */
struct laneSection
{
    int id = -1;
    double s = 0;
    vector<lane> lanes;
    offset o;
};

/**
 * @brief geometry holds all properties of a geometry on the reference line
 * 
 */
struct geometry
{
    geometryType type;
    double s = -1;
    double x = -1;
    double y = -1;
    double hdg = -1;
    double length = -1;
    double c = -1;
    double c1 = -1;
    double c2 = -1;
};

/**
 * @brief link is either predecessor or successor and defines id and properties of previous/next element
 * 
 */
struct link
{
    int id = -1;
    linkType elementType = roadType;
    contactPointType contactPoint = noneType;
};

/**
 * @brief object holds all object properties
 * 
 */
struct object
{
    int id = -1;
    string type = ""; // special OpenDRIVE code to define type
    double s = 0;
    double t = 0;
    double z = 0;
    double hdg = 0;
    string orientation = "none";
    double length = 0;
    double width = 0;
    double height = 0;

    // special parameters for repeating functionality
    bool repeat = false;
    double len = 0;
    double distance = 0;
};

/**
 * @brief sign holds all signal properties
 * 
 */
struct sign
{
    int id = -1;
    string type = ""; // special OpenDRIVE code to define type
    string subtype = "-1";
    int rule = -1;
    double value = -1;
    double s = 0;
    double t = 0;
    double z = 0;
    string orientation = "none";
    double width = 0.4;
    double height = 2;
    bool dynamic = false;
    string country = "OpenDRIVE";
};

/**
 * @brief control to define a controller with several signals
 * 
 */
struct control
{
    int id = -1;
    vector<sign> signs;
};

/**
 * @brief Holds the output elevation polynom
 * 
 */
struct elevationPolynom
{
    double a = 0; //unit m, elevation at @s (ds=0)
    double b = 0; //unit 1
    double c = 0; //unit 1/m
    double d = 0; //unit 1/m^2
    double s = 0; //unit m
};

/**
 * @brief stores the elevation data for each road segment along the s coordinate as a 3rd deg. polynomial
 * elev(ds) = a + b * ds + c * ds^2 + d*ds^3
 * elev = elevation at given pos
 * ds   = distance along s, restarts at each new element
 *  
 */
struct elevationProfile
{
    
    vector<elevationPolynom> outputElevation; //these are the polynoms that are generated from this elevationprofile
    //these are the raw input format values s equals x coordinate and t equals y.
    int inputId    = -1;
    double radius  = 0;
    double sOffset = 0;
    double tOffset = 0;

    bool operator<(const struct elevationProfile& other) const
    {
        return sOffset < other.sOffset;
    };
};


/**
 * @brief road holding all properties of a road
 * 
 */
struct road
{
    bool isConnectingRoad = false; // this variable fixes the problem that junction is used in linkSegments as placeholder for "inputSegmentId". 
    bool isLinkedToNetwork = false; //stores if the road is linked to the reference network
    bool elevationIntegrated = false; //flag to check if the elevation is calculated properly in the elevation profile calculation

    double length = 0;
    double elevationOffset = 0;//absolute elevation offset

    int id = -1;
    int inputSegmentId = -1;
    int inputId = -1;     // original road id from input file
    int roundAboutInputSegment = -1; //use this only for generated roads that belong to a roundabout to store the segment id of the roundabout!
    int junction = -1;
    
    //inputid has to be set to another value for roundabout roads so that ids dont clash in the linking segment
    string inputPos = ""; // specifying part of original road from input file
    string type = "town";
    string classification = ""; // either 'main' or 'access'

    link predecessor;
    link successor;

    vector<geometry> geometries;
    vector<laneSection> laneSections;
    vector<object> objects;
    vector<sign> signs;
    vector<elevationProfile> elevationProfiles;

    /**
     * @brief Get the Relative Elevation At road at start or end position TODO: replace this method with getRelativeElevationAtEnd()
     * 
     * @param pos -1: elevation at s = 0. 1: elevation at s = max
     * @return double relative elevation
     */
    double getRelativeElevationAt(int pos)
    {
        if(pos == -1)
        {
            return 0; //this should always be 0
        }
        else if(pos == 1)
        {
            if(elevationProfiles.size() > 0)
                return elevationProfiles.back().tOffset;
            return 0;
        }
        return 0;
    }
};


struct junctionGroup
{

    int id;
    string name;
    junctionGroupType type = roundaboutType;
    vector<int> juncIds; 

    junctionGroup()
    {
        
    }

    ~junctionGroup()
    {

    }



};

/**
 * @brief roadNetwork is the overall struct holding all data
 * 
 */
struct roadNetwork
{
    roadNetwork()
    {
    }

    string file;
    string outputFile;
    // main data properties
    vector<road> roads;
    vector<junction> junctions;
    vector<control> controller;
    vector<junctionGroup> juncGroups;

    int refRoad = -1; //store reference road id as input it
    int refElev = 0;  //store road elevation

    // global counters
    int nSignal = 0;
    int nSegment = 0;
};