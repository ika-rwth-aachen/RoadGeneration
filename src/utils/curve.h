/**
 * @file curve.h
 *
 * @brief file contains curve function for calculating positions along geometries
 *
 * @author Christian Geller
 * Contact: christian.geller@rwth-aachen.de
 *
 */

#include <math.h>

/**
 * @brief function computes the fresnel integral
 * 
 * @param s     position s in a spiral
 * @param x     result for x component
 * @param y     result for y component
 * @return int  error code
 */
int fresnel(double s, double &x, double &y)
{
    int sgn = (s > 0) - (s < 0);

    double s2 = pow(s, 2);
    double s4 = pow(s, 4);

    double xn[6];
    double yn[6];
    double xd[7];
    double yd[7];

    xn[0] = -2.99181919401019853726E3,
    xn[1] = 7.08840045257738576863E5,
    xn[2] = -6.29741486205862506537E7,
    xn[3] = 2.54890880573376359104E9,
    xn[4] = -4.42979518059697779103E10,
    xn[5] = 3.18016297876567817986E11;

    xd[0] = 1.00000000000000000000E0,
    xd[1] = 2.81376268889994315696E2,
    xd[2] = 4.55847810806532581675E4,
    xd[3] = 5.17343888770096400730E6,
    xd[4] = 4.19320245898111231129E8,
    xd[5] = 2.24411795645340920940E10,
    xd[6] = 6.07366389490084639049E11;

    yn[0] = -4.98843114573573548651E-8,
    yn[1] = 9.50428062829859605134E-6,
    yn[2] = -6.45191435683965050962E-4,
    yn[3] = 1.88843319396703850064E-2,
    yn[4] = -2.05525900955013891793E-1,
    yn[5] = 9.99999999999999998822E-1;

    yd[0] = 3.99982968972495980367E-12,
    yd[1] = 9.15439215774657478799E-10,
    yd[2] = 1.25001862479598821474E-7,
    yd[3] = 1.22262789024179030997E-5,
    yd[4] = 8.68029542941784300606E-4,
    yd[5] = 4.12142090722199792936E-2,
    yd[6] = 1.00000000000000000118E0;

    double res1 = 0;
    res1 += pow(s4, 0) * xn[5];
    res1 += pow(s4, 1) * xn[4];
    res1 += pow(s4, 2) * xn[3];
    res1 += pow(s4, 3) * xn[2];
    res1 += pow(s4, 4) * xn[1];
    res1 += pow(s4, 5) * xn[0];

    double res2 = 0;
    res2 += pow(s4, 0) * xd[6];
    res2 += pow(s4, 1) * xd[5];
    res2 += pow(s4, 2) * xd[4];
    res2 += pow(s4, 3) * xd[3];
    res2 += pow(s4, 4) * xd[2];
    res2 += pow(s4, 5) * xd[1];
    res2 += pow(s4, 6) * xd[0];

    double res3 = 0;
    res3 += pow(s4, 0) * yn[5];
    res3 += pow(s4, 1) * yn[4];
    res3 += pow(s4, 2) * yn[3];
    res3 += pow(s4, 3) * yn[2];
    res3 += pow(s4, 4) * yn[1];
    res3 += pow(s4, 5) * yn[0];

    double res4 = 0;
    res4 += pow(s4, 0) * yd[6];
    res4 += pow(s4, 1) * yd[5];
    res4 += pow(s4, 2) * yd[4];
    res4 += pow(s4, 3) * yd[3];
    res4 += pow(s4, 4) * yd[2];
    res4 += pow(s4, 5) * yd[1];
    res4 += pow(s4, 6) * yd[0];

    x = sgn * abs(s) * res3 / res4;
    y = sgn * abs(s) * s2 * res1 / res2;

    return 0;
}

/**
 * @brief function computes the x, y, phi value for a given geometry at position s
 * 
 * @param s     position where x, y, phi should be computed
 * @param geo   geometry
 * @param x     start value of geometry and holds resulting value for x
 * @param y     start value of geometry and holds resulting value for y
 * @param phi   start value of geometry and holds resulting value for phi
 * @param fd    determines if finite differences should be used for derivative
 * @return int  error code
 */
int curve(double s, geometry geo, double &x, double &y, double &phi, int fd)
{
    geometryType type = geo.type;
    double c = geo.c;
    double c1 = geo.c1;
    double c2 = geo.c2;
    double ds = geo.length;

    if (type == line)
    {
        x += s * cos(phi);
        y += s * sin(phi);
    }
    if (type == arc)
    {
        double tmpPhi = fmod(phi + s * c, 2 * M_PI);

        x += (sin(tmpPhi) - sin(phi)) / c;
        y += (-cos(tmpPhi) + cos(phi)) / c;
        phi += c * s;
    }
    if (type == spiral)
    {
        double sigma = (c2 - c1) / ds;

        double a = sqrt(M_PI) / sqrt(abs(sigma));
        double k1 = c1;
        double k2 = k1 + s * sigma;

        double s1 = k1 / sigma;
        double s2 = k2 / sigma;

        double x1, y1, x2, y2;

        fresnel(s1 / a, x1, y1);
        fresnel(s2 / a, x2, y2);

        double tau = 0.5 * s1 * sigma * s1;

        y1 *= sgn(sigma);
        y2 *= sgn(sigma);

        x2 -= x1;
        y2 -= y1;

        x += a * (cos(phi - tau) * x2 - sin(phi - tau) * y2);
        y += a * (sin(phi - tau) * x2 + cos(phi - tau) * y2);

        // calculate angle with finite differences
        if (fd)
        {
            double xTmp1 = 0;
            double xTmp2 = 0;
            double yTmp1 = 0;
            double yTmp2 = 0;
            double phiTmp = 0;
            curve(s - 0.001, geo, xTmp1, yTmp1, phiTmp, 0);
            curve(s + 0.001, geo, xTmp2, yTmp2, phiTmp, 0);

            phi += atan2((yTmp2 - yTmp1), (xTmp2 - xTmp1));
        }
    }
    return 0;
}