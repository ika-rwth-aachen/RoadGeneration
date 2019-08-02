// file curve.h

int curve(double s, double c, double c1, double c2, double &x, double &y, double &phi)
{
    int type;
    
    if (c == 0 && c1 == 0 && c2 == 0)   type = 1;
    if (c != 0 && c1 == 0 && c2 == 0)   type = 2;
    if (c == 0 && (c1 != 0 || c2 != 0)) type = 3;
    
    if (type == 1)
    {
        x += s * cos(phi);
        y += s * sin(phi);
    }
    if (type == 2)
    {
        double tmpPhi = fmod(phi + s * c, 2 * M_PI);

        x += ( sin(tmpPhi) - sin(phi)) / c;
        y += (-cos(tmpPhi) + cos(phi)) / c;
        phi += c*s;
    }
    if (type == 3)
    {
        //TODO
    }
    return 0;
}