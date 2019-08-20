// file geometries.h

/**
 * @brief function adds a geometry which is here a line
 * 
 * @param geo   vector of all geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  errorcode
 */
int addLine(vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2)
{
    geometry g;

    g.s = 0;
    if (geo.size() > 0) g.s = geo.back().s + geo.back().length;
    g.type = 1;
    g.c = 0;
    g.c1 = 0;
    g.c2 = 0;
    g.x = x1;
    g.y = y1;
    g.hdg = phi1;
    g.length = sqrt(pow(x2-x1,2) + pow(y2-y1,2));

    geo.push_back(g);
}

/**
 * @brief function adds a geometry which is here an arc -> TODO: Verbundskurve
 * 
 * @param geo   vector of all geometries
 * @param x1    x component of start point
 * @param y1    y component of start point
 * @param phi1  angle of start point
 * @param x2    x component of end point
 * @param y2    y component of end point
 * @param phi2  angle of end point
 * @return int  errorcode
 */
int addArc(vector<geometry> &geo, double x1, double y1, double phi1, double x2, double y2, double phi2)
{
    double a = phi2-phi1;
    fixAngle(a);

    double dist = sqrt(pow(x2-x1,2)  + pow(y2-y1,2));
    double R = (dist / 2) / sin(a / 2);

    geometry g;

    g.s = 0;
    if (geo.size() > 0) g.s = geo.back().s + geo.back().length;
    g.type = 2;
    g.c = 1/R;
    g.c1 = 0;
    g.c2 = 0;
    g.x = x1;
    g.y = y1;
    g.hdg = phi1;
    g.length = abs(R * a);

    geo.push_back(g);
}