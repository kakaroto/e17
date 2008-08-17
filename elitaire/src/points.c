/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "points.h"
#include <stdio.h>
#include <monetary.h>
#include <string.h>

const char * pointsType_point_string_get(float points, pointsType type)
{
    static char buffer[24];

    switch (type) {
    case POINTS_TYPE_INTEGER_GOOD:
    case POINTS_TYPE_INTEGER_BAD:
        snprintf(buffer, sizeof(buffer), "%i", (int) points);
        break;
    
    case POINTS_TYPE_MONEY:
        strfmon(buffer, sizeof(buffer), "%i", (double) points);
        break;
    
    default:
        snprintf(buffer, sizeof(buffer), "%.2f", points);
        break;
    }

    return buffer;
}
