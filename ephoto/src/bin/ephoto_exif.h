#ifdef BUILD_EXIF_SUPPORT
#ifndef _EPHOTO_EXIF_H_
#define _EPHOTO_EXIF_H_

#include "ephoto.h"
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>

int image_has_exif_data(const char *file);
Ecore_Hash *get_exif_data(const char *file);

#endif
#endif
