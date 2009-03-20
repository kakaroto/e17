#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>

#include "Eina.h"
#include "Enesim.h"

#include "config.h"

#define MAX_PLANES 4

typedef enum
{
	ROP_BLEND,
	ROP_FILL,
	ROPS
} Rop;

typedef enum
{
	PIXEL_OPAQUE,
	PIXEL_TRANSPARENT,
	PIXELS
} Pixel_Type;

typedef enum
{
	TYPE_UINT8,
	TYPE_UINT16,
	TYPE_UINT32,
	TYPES
} Type;

typedef enum
{
	COLOR_ALPHA,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,
	COLORS
} Color_Name;

typedef struct _Color
{
	Color_Name name;
	unsigned int offset;
	unsigned int length;
	unsigned int type;
} Color;

typedef struct _Plane
{
	Color colors[COLORS]; /* an array of colors this plane has */
	unsigned int num_colors; /* number of colors */
	unsigned int length; /* the length in bits of the plane */
	unsigned int type; /* the data type to use */
	unsigned int contiguous; /* is contiguous or skips bits to fit on the data type length */ 
} Plane;

typedef struct _Format
{
	Enesim_Format sformat;
	Plane planes[MAX_PLANES]; /* an array of planes a format has */
	unsigned int num_planes; /* number of planes */
	const char *name; /* name of the format */ /* FIXME remove this we can use the sformat instead */
	unsigned char premul; /* is it premul data? */
} Format;

extern Format *formats[];
extern const char * surface_format_name[ENESIM_SURFACE_FORMATS];

#endif /*COMMON_H_*/
