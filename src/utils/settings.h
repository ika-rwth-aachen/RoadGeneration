/**
 * Road-Generation
 * --------------------------------------------------------
 * Copyright (c) 2021 Institut für Kraftfahrzeuge, RWTH Aachen, ika
 * Report bugs and download new versions https://github.com/ika-rwth-aachen/RoadGeneration
 *
 * This library is distributed under the MIT License.
 * 
 * @file settings.h
 *
 * @brief file contains the settings
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */


/**
 * @brief lane width settings
 * 
 */
struct widths
{
    double standard = 3.5;
    double main = 4;
    double access = 3;
};

/**
 * @brief speed limit settings
 * 
 */
struct speeds
{
    double standard = 50;
    double main = 50;
    double access = 30;
};

/**
 * @brief lane changes settings 
 * 
 * @param s     length of additional lane
 * @param ds    length of widening part
 * 
 */
struct laneChanges
{
    double s = 25;
    double ds = 25;
};

/**
 * @brief bus stop settings
 * 
 */
struct busStops
{
    double length = 20;
    double widening = 7.5;
};

/**
 * @brief general settings
 * 
 */
struct settings
{
    speeds speed;
    widths width;
    laneChanges laneChange;
    busStops busStop;

    std::string xmlSchemeLocation; //this has to be set to the xml schema input path

    bool silentMode = false; //silent mode disables console outputs
    bool overwriteLog = true;
    int warnings = 0; // counts number of warnings

    int versionMajor = 1; // OpenDrive major version
    int versionMinor = 4; // OpenDrive minor version

    float minConnectingRoadLength = 0.01f;

    // basis in world coordinate system
    double north = 0;
    double south = 0;
    double west = 0;
    double east = 0;
};

