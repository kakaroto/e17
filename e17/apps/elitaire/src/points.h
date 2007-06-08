#ifndef POINTS_H
#define POINTS_H

#ifdef __cplusplus
extern "C" {
#endif
	
typedef enum _pointsType{
	POINTS_TYPE_UNKNOWN,
	POINTS_TYPE_INTEGER_GOOD,	// high points are good
	POINTS_TYPE_INTEGER_BAD,	// high points are bad
	POINTS_TYPE_FLOAT_GOOD,
	POINTS_TYPE_FLOAT_BAD,
	POINTS_TYPE_MONEY		// money is always good ;-)
} pointsType;

const char * pointsType_point_string_get(float points, pointsType type);

#ifdef __cplusplus
}
#endif

#endif
