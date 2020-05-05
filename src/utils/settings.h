// file settings.h

struct widths
{
    double access = 3;
    double main = 4;
    double standard = 3.5;
};

struct speeds
{
    double access = 30;
    double main = 50;
    double standard = 50;
};

struct laneChanges
{
    double s = 25;
    double ds = 25;
};

struct busStops
{
    double length = 20;
    double widing = 7.5;
};

struct settings
{
    speeds speed;
    widths width;
    laneChanges laneChange;
    busStops busStop;

    int versionMajor = 1;
    int versionMinor = 5;
    double north = 0;
    double south = 0; 
    double west = 0;
    double east = 0;
};