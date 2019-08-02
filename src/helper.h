// file helper.h

int fixAngle(double &a)
{
    a = fmod(a + 2*M_PI, 2 * M_PI);
    if (a > M_PI) a = a - 2 *M_PI; 

    return 0;
}