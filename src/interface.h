// file interface.h

struct laneLink
{
    int from;
    int to;
};

struct connection
{
    int id;
    int from;
    int to;
    string contactPoint = "start";
    int fromLane;
    int toLane;
};

struct junction
{
    int id;
    vector<connection> connections;
};

struct roadmark
{
    double s = 0;
    string type = "solid";
    string weight = "standard";
    string color = "white";
    double width = 0.2;
};

struct width
{
    double s = 0;
    double a = 3.25;
    double b = 0;
    double c = 0;
    double d = 0;
};

struct lane
{
    int id;
    string type = "driving";
    int level = 0;
    width w;
    roadmark rm;
};

struct laneSection
{
    int id;
    double s = 0;
    vector<lane> lanes;
};

struct geometry
{
    int type;
    double s;
    double x;
    double y;
    double hdg;
    double length;
    double c;
    double c1;
    double c2;
};

struct link
{
    string elementType = "road";
    int elementId = -1;
    string contactPoint = "start";
};

struct road
{
    double length = 0;
    int id;
    int junction;
    string type = "town";

    link predecessor;
    link successor;

    vector<geometry> geometries;
    vector<laneSection> laneSections;
};

struct roadNetwork
{
    string file;
    vector<road> roads;
    vector<junction> junctions;
};