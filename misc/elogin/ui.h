#ifndef __ELOGIN_UI_H__
#define __ELOGIN_UI_H__

#include <Ecore.h>
#include <Evas.h>
#include <Ebits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>

#include "object.h"
#include "box.h"
#include "label.h"
#include "entry.h"
#include "button.h"

#define PACKAGE_DATA_DIR "/usr/local/e17/share"
#define MAX_EVAS_COLORS (216)
#define MAX_FONT_CACHE  (1024 * 1024)
#define MAX_IMAGE_CACHE (16 * (1024 * 1024))
#define FONT_DIRECTORY  PACKAGE_DATA_DIR"/enlightenment/data/fonts/"

Evas evas;

Evas_Render_Method render_method;

#endif /* __ELOGIN_UI_H__ */
