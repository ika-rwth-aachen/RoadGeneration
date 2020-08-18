#include "closeRoadConnection.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include "helper.h"

/**
 * @file closeRoadConnection.cpp
 *
 * @brief file contains methods for building the geometry of a new road
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */


const int P = 1;
const int N = -1;
const int O = 0;
const int I = 2;



/**
 * @brief function generates geometries recursivly so that the two input points are connected with a feasible geometry
 * 
 * @param geo   vector which contains all output geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  error code
 */
int closeRoadConnection(std::vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2)
{
    // goal: compute new road from point 1 to point 2, preserving angles

    // compute m and b of points for line equations
    double m1 = tan(phi1);
    double b1 = y1 - m1 * x1;

    double m2 = tan(phi2);
    double b2 = y2 - m2 * x2;

    // type is general type (p = parallel, i=identical, 0=no intersection)
    // type1 / type2 defines how point1 / point2 lie to the intersetion point
    int type1, type2, type = 0;

    double d1, d2, d;
    double iPx, iPy;

    // determine type and compute intersection point if it exists
    if (m1 == m2)
    {
        if (b1 != b2)
            type = p;
        if (b1 == b2)
            type = i;
    }
    else
    {
        // compute intersection point and determine type
        computeIP(x1, y1, phi1, x2, y2, phi2, type, type1, type2, iPx, iPy);

        d1 = sqrt(pow(x1 - iPx, 2) + pow(y1 - iPy, 2));
        d2 = sqrt(pow(x2 - iPx, 2) + pow(y2 - iPy, 2));
        d = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }

    // --- case i -------------------------------------------------------------
    if (type == i)
    {
        double beta = phi1 - phi2;
        fixAngle(beta);

        // case: -> <-    or  <-  ->
        if (beta == M_PI)
        {
            // create helping point at Tmp position
            double xTmp = x1 + cos(phi1 + M_PI / 4) * d / 4;
            double yTmp = y1 + sin(phi1 + M_PI / 4) * d / 4;
            double phiTmp = phi1 + M_PI / 2;
            if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
            {
                std::cerr << "ERR: error in addArc function." << std::endl;
                return 1;
            }
            if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
            {
                std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
                return 1;
            }
        }

        // case: ->  ->
        if (beta == 0)
        {
            // rotate start point around 90 degree and test IP again
            int t, t1, t2;
            computeIP(x1, y1, phi1 + M_PI / 2, x2, y2, phi2, t, t1, t2, iPx, iPy);

            // if end points to start
            if (t2 == P)
            {
                // create helping point at Tmp position
                double xTmp = x1 + cos(phi1 + M_PI / 4) * d / 4;
                double yTmp = y1 + sin(phi1 + M_PI / 4) * d / 4;
                double phiTmp = phi1 + M_PI / 2;
                if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
                {
                    std::cerr << "ERR: error in addArc function." << std::endl;
                    return 1;
                }
                if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
                {
                    std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
                    return 1;
                }
            }
            // if start points to end
            if (t2 == N)
            {
                addLine(geo, x1, y1, phi1, x2, y2, phi2);
            }
        }
    }

    // --- case p -------------------------------------------------------------
    if (type == P)
    {
        double beta = phi1 - phi2;
        fixAngle(beta);

        // case: -> <-    or  <-  ->
        if (beta == M_PI)
        {
            // rotate start point around 90 degree and test IP again
            int t, t1, t2;
            computeIP(x1, y1, phi1 + M_PI / 2, x2, y2, phi2, t, t1, t2, iPx, iPy);

            if ((t1 == P && t2 == O) ||
                (t1 == P && t2 == N) ||
                (t1 == N && t2 == P) ||
                (t1 == N && t2 == O))
            {
                double h = sqrt(pow(x1 - iPx, 2) + pow(y1 - iPy, 2));
                double xTmp = x1 + cos(phi1 + M_PI / 4) * h / 4;
                double yTmp = y1 + sin(phi1 + M_PI / 4) * h / 4;
                double phiTmp = phi1 + M_PI / 2;
                if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
                {
                    std::cerr << "ERR: error in addArc function." << std::endl;
                    return 1;
                }
                if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
                {
                    std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
                    return 1;
                }
            }
            if ((t1 == P && t2 == P) || (t1 == N && t2 == N))
            {
                int t, t1, t2;
                computeIP(x1, y1, phi1, x2, y2, phi2 + M_PI / 2, t, t1, t2, iPx, iPy);

                addLine(geo, x1, y1, phi1, iPx, iPy, phi1);
                if (closeRoadConnection(geo, iPx, iPy, phi1, x2, y2, phi2))
                {
                    std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
                    return 1;
                }
            }
        }

        // case: ->  ->
        if (beta == 0)
        {
            // rotate start point around 90 degree and test IP again
            int t, t1, t2;
            computeIP(x1, y1, phi1 + M_PI / 2, x2, y2, phi2, t, t1, t2, iPx, iPy);

            if (t1 == P)
            {
                double h = sqrt(pow(x1 - iPx, 2) + pow(y1 - iPy, 2));
                double xTmp = x1 + cos(phi1 + M_PI / 4) * h / 4;
                double yTmp = y1 + sin(phi1 + M_PI / 4) * h / 4;
                double phiTmp = phi1 + M_PI / 2;
                if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
                {
                    std::cerr << "ERR: error in addArc function." << std::endl;
                    return 1;
                }
                if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
                {
                    std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
                    return 1;
                }
            }
            if (t1 == N)
            {
                double h = sqrt(pow(x1 - iPx, 2) + pow(y1 - iPy, 2));
                double xTmp = x1 + cos(phi1 - M_PI / 4) * h / 4;
                double yTmp = y1 + sin(phi1 - M_PI / 4) * h / 4;
                double phiTmp = phi1 - M_PI / 2;
                if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
                {
                    std::cerr << "ERR: error in addArc function." << std::endl;
                    return 1;
                }
                if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
                {
                    std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
                    return 1;
                }
            }
        }
    }
    // --- case pn -------------------------------------------------------------
    if (type1 == P && type2 == N)
    {
        // in this case the two point can be connected instantly
        if (d1 > d2)
        {
            double xTmp = x1 + cos(phi1) * (d1 - d2);
            double yTmp = y1 + sin(phi1) * (d1 - d2);
            addLine(geo, x1, y1, phi1, xTmp, yTmp, phi1);
            if (addArc(geo, xTmp, yTmp, phi1, x2, y2, phi2))
            {
                std::cerr << "ERR: error in addArc function." << std::endl;
                return 1;
            }
        }

        if (d1 < d2)
        {
            double xTmp = x2 - cos(phi2) * (d2 - d1);
            double yTmp = y2 - sin(phi2) * (d2 - d1);
            if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phi2))
            {
                std::cerr << "ERR: error in addArc function." << std::endl;
                return 1;
            }
            if (addLine(geo, xTmp, yTmp, phi2, x2, y2, phi2))
            {
                std::cerr << "ERR: error in addLine function." << std::endl;
                return 1;
            }
        }

        if (d1 == d2)
        {
            if (addArc(geo, x1, y1, phi1, x2, y2, phi2))
            {
                std::cerr << "ERR: error in addArc function." << std::endl;
                return 1;
            }
        }
    }
    // --- case oo -------------------------------------------------------------
    if (type1 == O && type2 == O)
    {
        // almost nothing to be done
        if (addLine(geo, x1, y1, phi1, x2, y2, phi2))
        {
            std::cerr << "ERR: error in addLine function." << std::endl;
            return 1;
        }
    }

    // --- case np pp nn 0p 0n -------------------------------------------------
    if ((type1 == N && type2 == P) ||
        (type1 == P && type2 == P) ||
        (type1 == N && type2 == N) ||
        (type1 == 0 && type2 == N) ||
        (type1 == 0 && type2 == P))
    {
        double alpha = atan2(y2 - y1, x2 - x1) - phi1;
        fixAngle(alpha);
        if (alpha > 0)
            alpha = M_PI / 4;
        if (alpha < 0)
            alpha = -M_PI / 4;

        // create helping point at Tmp position
        double xTmp = x1 + cos(phi1 + alpha) * d / 4;
        double yTmp = y1 + sin(phi1 + alpha) * d / 4;
        double phiTmp = phi1 + 2 * alpha;
        if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
        {
            std::cerr << "ERR: error in addArc function." << std::endl;
            return 1;
        }
        if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
        {
            std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
            return 1;
        }
    }

    // --- p0 && n0 ------------------------------------------------------------
    if ((type1 == P && type2 == O) ||
        (type1 == N && type2 == O))
    {
        double beta = phi1 - phi2;
        fixAngle(beta);
        if (beta > 0)
            beta = -M_PI / 4;
        if (beta < 0)
            beta = M_PI / 4;

        // create helping point at Tmp position
        double xTmp = x1 + cos(phi1 + beta) * d / 4;
        double yTmp = y1 + sin(phi1 + beta) * d / 4;
        double phiTmp = phi1 + 2 * beta;
        if (addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp))
        {
            std::cerr << "ERR: error in addArc function." << std::endl;
            return 1;
        }
        if (closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2))
        {
            std::cerr << "ERR: error in closeRoadConnection function." << std::endl;
            return 1;
        }
    }

    return 0;
}

