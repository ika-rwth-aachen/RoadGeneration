#ifndef SETTINGS_H
#define SETTINGS_H

/**
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

    int versionMajor = 1; // OpenDrive major version
    int versionMinor = 5; // OpenDrive minor version

    // basis in world coordinate system
    double north = 0;
    double south = 0;
    double west = 0;
    double east = 0;
};

#endif