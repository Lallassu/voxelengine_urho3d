#include "utils.h"

float utils::Distance(Vector3 from, Vector3 to) 
{
    return sqrt(pow(to.x_-from.x_, 2) + pow(to.y_-from.y_, 2) + pow(to.z_-from.z_, 2));
}

Color utils::GetBloodColor()
{
    int rd = (int)Random(10);
    int r = 128;
    int g = 20;
    int b = 20;
    if(rd > 8) {
        r = 128;
        g = 7;
        b = 7;
    } else if(rd > 6) {
        r = 158;
        g = 7;
        b = 7; 
    } else if(rd > 4) {
        r = 158;
        g = 20;
        b = 20;
    }
    return Color(r, g, b, 1);
}
