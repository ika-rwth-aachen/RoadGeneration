// file closeRoadConnection.h

#define p 1
#define n -1
#define o 0
#define i 2

#include "geometries.h"

/**
 * @brief function generates a geometry (maybe recursivly) so that the two points are closed
 * 
 * @param geo   vector which contains all geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  errorcode
 */
int closeRoadConnection(vector<geometry> geo, double x1, double y1, double phi1, double x2, double y2, double phi2)
{
    // goal: compute new road from point 1 into point 2, preserving angles

    // compute intersection point

    double m1 = tan(phi1);
    double b1 = y1 - m1 * x1;

    double m2 = tan(phi2);
    double b2 = y2 - m2 * x2;

    int type1, type2, type = 0;

    double d1,d2,d;
    double iPx,iPy;

    // intersection point of lines
    if (m1 ==  m2)
    {
        if (b1 != b2) type = p;
        if (b1 == b2) type = i;
    }
    else
    {    
        // compute intersection point and determin type
        computeIP(x1,y1,phi1,x2,y2,phi2,type,type1,type2,iPx,iPy);
                
        d1 = sqrt(pow(x1-iPx,2) + pow(y1-iPy,2));
        d2 = sqrt(pow(x2-iPx,2) + pow(y2-iPy,2));
        d  = sqrt(pow(x2-x1,2) + pow(y2-y1,2));

        cout << "Type is now defined" << endl;
    }
    // --- case i -------------------------------------------------------------
    if (type == i)
    {
        double beta = phi1-phi2;
        fixAngle(beta);

        // case: -> <-    or  <-  -> 
        if (beta == M_PI)
        {
            double xTmp = x1 + cos(phi1 + M_PI/4) * d/4;
            double yTmp = y1 + sin(phi1 + M_PI/4) * d/4;
            double phiTmp = phi1 + M_PI/2;
            addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
            closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
        }

        // case: ->  -> 
        if (beta == 0)
        {
            int t, t1, t2;
            computeIP(x1,y1,phi1+M_PI/2,x2,y2,phi2,t,t1,t2,iPx,iPy);

            // if end points to start
            if(t2 == p)
            {
                double xTmp = x1 + cos(phi1 + M_PI/4) * d/4;
                double yTmp = y1 + sin(phi1 + M_PI/4) * d/4;
                double phiTmp = phi1 + M_PI/2;
                addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
                closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
            }
            // if start points to end
            if (t2 == n)
            {
                addLine(geo,x1,y1,phi1,x2,y2,phi2);
            }
        }
    }
     // --- case p -------------------------------------------------------------
    if (type == p)
    {
        double beta = phi1-phi2;
        fixAngle(beta);

        // case: -> <-    or  <-  -> 
        if (beta == M_PI)
        {
            int t, t1, t2;
            computeIP(x1,y1,phi1+M_PI/2,x2,y2,phi2,t,t1,t2,iPx,iPy);

            if ((t1 == p && t2 == o) ||
                (t1 == p && t2 == n) ||
                (t1 == n && t2 == p) ||
                (t1 == n && t2 == o))
            {
                double h = sqrt(pow(x1-iPx,2) + pow(y1-iPy,2));
                double xTmp = x1 + cos(phi1 + M_PI/4) * h/4;
                double yTmp = y1 + sin(phi1 + M_PI/4) * h/4;
                double phiTmp = phi1 + M_PI/2;
                addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
                closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
            }
            if ((t1 == p && t2 == p) ||
                (t1 == n && t2 == n))
            {
                int t, t1, t2;
                computeIP(x1,y1,phi1,x2,y2,phi2+M_PI/2,t,t1,t2,iPx,iPy);
                
                addLine(geo, x1, y1, phi1, iPx, iPy, phi1);
                closeRoadConnection(geo, iPx, iPy, phi1, x2, y2, phi2);
            }
        }

        // case: ->  -> 
        if (beta == 0)
        {
            int t, t1, t2;
            computeIP(x1,y1,phi1+M_PI/2,x2,y2,phi2,t,t1,t2,iPx,iPy);

            if(t1 == p)
            {
                double h = sqrt(pow(x1-iPx,2) + pow(y1-iPy,2));
                double xTmp = x1 + cos(phi1 + M_PI/4) * h/4;
                double yTmp = y1 + sin(phi1 + M_PI/4) * h/4;
                double phiTmp = phi1 + M_PI/2;
                addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
                closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
            }
            if(t1 == n)
            {
                double h = sqrt(pow(x1-iPx,2) + pow(y1-iPy,2));
                double xTmp = x1 + cos(phi1 - M_PI/4) * h/4;
                double yTmp = y1 + sin(phi1 - M_PI/4) * h/4;
                double phiTmp = phi1 - M_PI/2;
                addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
                closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
            }
        }
    }
    // --- case pn -------------------------------------------------------------
    if (type1 == p && type2 == n)
    {
        if (d1 > d2)
        {
            double xTmp = x1 + cos(phi1) * (d1-d2);
            double yTmp = y1 + sin(phi1) * (d1-d2);
            addLine(geo, x1, y1, phi1, xTmp, yTmp, phi1);
            addArc(geo, xTmp, yTmp, phi1, x2, y2, phi2);
        }

        if (d1 < d2)
        {
            double xTmp = x2 - cos(phi2) * (d2-d1);
            double yTmp = y2 - sin(phi2) * (d2-d1);
            addArc(geo, x1, x2, phi1, xTmp, yTmp, phi1);
            addLine(geo, xTmp, yTmp, phi2, x2, y2, phi2);
        }

        if (d1 == d2)
        {
            addArc(geo, x1, y1, phi1, x2, y2, phi2);
        }
    }
    // --- case oo -------------------------------------------------------------
    if (type1 == o && type2 == o)
    {
        addLine(geo, x1, y1, phi1, x2, y2, phi2);
    }
    // --- case np pp nn 0p 0n -------------------------------------------------
    if ((type1 == n && type2 == p) ||
        (type1 == p && type2 == p) ||
        (type1 == n && type2 == n) ||
        (type1 == 0 && type2 == n) ||
        (type1 == 0 && type2 == p))
    {
        double alpha = atan2(y2-y1,x2-x1);
        fixAngle(alpha);
        if (alpha > 0) alpha =  M_PI/4;
        if (alpha < 0) alpha = -M_PI/4;
        double xTmp = x1 + cos(phi1 + alpha) * d/4;
        double yTmp = y1 + sin(phi1 + alpha) * d/4;
        double phiTmp = phi1 + 2 * alpha;
        addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
        closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
    }
    // --- p0 && n0 ------------------------------------------------------------
    if ((type1 == p && type2 == o) ||
        (type1 == n && type2 == o))
    {
        double beta = phi1-phi2;
        fixAngle(beta);

        if (beta > 0) beta = -M_PI/4;
        if (beta < 0) beta =  M_PI/4;
        double xTmp = x1 + cos(phi1 + beta) * d/4;
        double yTmp = y1 + sin(phi1 + beta) * d/4;
        double phiTmp = phi1 + 2 * beta;
        addArc(geo, x1, y1, phi1, xTmp, yTmp, phiTmp);
        closeRoadConnection(geo, xTmp, yTmp, phiTmp, x2, y2, phi2);
    }
    
    return 0;
}