#ifndef __THUMBNAIL_GENERIC_H_
#define __THUMBNAIL_GENERIC_H_

#include <limits.h>
#include "entropy_generic.h"



typedef struct entropy_thumbnail entropy_thumbnail;
struct entropy_thumbnail {
        char filename[PATH_MAX];
	char thumbnail_filename[PATH_MAX];
        int keys;
        Ecore_Hash* text;

	void* parent; /*The file object we reference - or other object */
};


entropy_thumbnail* entropy_thumbnail_new();



#endif

