// file interface.h

enum contactPointType {startType, endType};
enum geometryType {line, spiral, arc};
enum linkType {roadType, junctionType};

struct connection
{
    int id = -1;
    int from = -1;
    int to = -1;
    contactPointType contactPoint;
    int fromLane = -1;
    int toLane = -1;
};

struct junction
{
    int id = -1;
    vector<connection> connections;
};

struct roadmark
{
    double s = 0;
    string type = "solid";
    string weight = "standard";
    string color = "white";
    double width = 0.15;
};

struct width
{
    double s = 0;
    double a = 3.5;
    double b = 0;
    double c = 0;
    double d = 0;
};

struct offset
{
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0;
};

struct material
{
    double s = 0;
    string surface = "asphalt";
    double friction = 0.8;
    double roughness = 0.015;
};

struct lane
{
    int id = -1;
    string type = "driving";
    bool turnLeft = false;
    bool turnStraight = true;
    bool turnRight = false;
    bool level = true;          
    double speed = 50;
    width w;
    roadmark rm;
    material m;
    int preId = -1;
    int sucId = -1;
};

struct laneSection
{
    int id = -1;
    double s = 0;
    vector<lane> lanes;
    offset o;
};

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

struct link
{
    int id = -1;
    linkType elementType = roadType;
    contactPointType contactPoint;
};

struct object
{
    int id = -1;
    string type = "";
    double s = 0;
    double t = 0;
    double z = 0;
    double hdg = 0;
    string orientation = "none";
    double length = 0;
    double width = 0;
    double height = 0;

    bool repeat = false;
    double len = 0;
    double distance = 0;
};

struct sign
{
    int id = -1;
    string type = "";
    string subtype = "-1";
    int rule = -1;
    double value = -1;
    double s = 0;
    double t = 0;
    double z = 0;
    string orientation = "none";
    double width = 0;
    double height = 0;
    bool dynamic = false;
    string country = "OpenDRIVE";
};

struct control
{
    int id = -1;
    vector<sign> signs;
};

struct road
{
    int id = -1;
    int inputId = -1;
    int junction = -1;
    string type = "town";
    string classification = "";
    double length = 0;

    link predecessor;
    link successor;

    vector<geometry> geometries;
    vector<laneSection> laneSections;
    vector<object> objects;
    vector<sign> signs;
};

struct roadNetwork
{
    string file;

    vector<road> roads;
    vector<junction> junctions;
    vector<control> controller;

    int nSignal = 0;
    int nSegment = 0;
    
    int versionMajor = 1;
    int versionMinor = 5;
};