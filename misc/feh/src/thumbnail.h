#ifndef THIMBNAIL_H
#define THIMBNAIL_H


#include "feh.h"
#include "filelist.h"
#include "winwidget.h"

typedef struct thumbnail {
	int x;
	int y;
	int w;
	int h;
	feh_file *file;
	struct feh_thumbnail  *next;
} feh_thumbnail;


void feh_thumbnail_add(feh_file *fil, int x, int y, int w, int h);
feh_file* feh_thumbnail_get_file_from_coords(int x, int y);
feh_thumbnail* feh_thumbnail_get_thumbnail_from_coords(int x, int y);
	
	
	
#endif

