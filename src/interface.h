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
    vector<laneLink> laneLinks;
};

struct junction
{
    int id;
    vector<connection> connections;
};

struct roadmark
{
    double s;
    int type;
    int weight;
    int color;
    double width;
};

struct width
{
    double s;
    double a;
    double b;
    double c;
    double d;
};

struct lane
{
    int id;
    int type;
    int level;
    int predecessor;
    int successor;
    width w;
    roadmark rm;
};

struct laneSection
{
    double s;
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
    int type;
    int id;
    int contactPoint;
};

struct road
{
    double s;
    int type;

    link predecessor;
    link successor;

    vector<geometry> geometries;
    vector<laneSection> laneSections;
};

struct roadNetwork
{
    vector<road> roads;
    vector<junction> junctions;
};