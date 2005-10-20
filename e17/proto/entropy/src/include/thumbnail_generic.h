#ifndef __THUMBNAIL_GENERIC_H_
#define __THUMBNAIL_GENERIC_H_

#include "entropy_generic.h"


typedef struct entropy_thumbnail entropy_thumbnail;
struct entropy_thumbnail {
        char filename[255];
	char thumbnail_filename[255];
        int keys;
        Ecore_Hash* text;

	void* parent; /*The file object we reference - or other object */
};


entropy_thumbnail* entropy_thumbnail_new();



#endif

