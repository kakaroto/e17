#ifndef __ENTICE_H__
#define __ENTICE_H__

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <Evas.h>
#include <Ecore.h>
#include <Imlib2.h>

#include <config.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define MAX_EVAS_COLORS (216)
#define MAX_FONT_CACHE  (512 * 1024)
#define MAX_IMAGE_CACHE (16 * (1024 * 1024))
#define FONT_DIRECTORY  PACKAGE_DATA_DIR"/data/fonts/"
#define IMAGE_DIRECTORY PACKAGE_DATA_DIR"/data/images/"
#define FN              FONT_DIRECTORY
#define IM              IMAGE_DIRECTORY
#define W               400
#define H               300
#define RENDER_ENGINE   RENDER_METHOD_ALPHA_SOFTWARE
/* #define RENDER_ENGINE   RENDER_METHOD_BASIC_HARDWARE */
/* #define RENDER_ENGINE   RENDER_METHOD_3D_HARDWARE */

typedef struct _image Image;

struct _image
{
   char               *file;
   pid_t               generator;
   char               *thumb;
   Evas_Object        *o_thumb;
   int                 modified;
   int                 subst;
};

#include "buttons.h"
#include "event.h"
#include "fade.h"
#include "file.h"
#include "globals.h"
#include "handler.h"
#include "image.h"
#include "list.h"
#include "misc.h"
#include "panel.h"
#include "thumb.h"

#endif /* __ENTICE_H__ */
