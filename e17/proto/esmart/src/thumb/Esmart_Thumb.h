#ifndef E_THUMB_H
#define E_THUMB_H

#include<Evas.h>
#include<Epsilon.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * E_Thumb - In an effort to easily generate thumbnails this smart object
 * came to be.  Check out http://triq.net/~jens/thumbnail-spec/index.html
 * In addition to that you can get a little bit of metadata from the smart
 * object like the source image's geometry and filetype.  The code is there
 * to support more than just file:// uris but for now that's all it's doing.
 * 
 * Version 0.0.1 - August 23rd 2003 <atmos@atmos.org>
 *  - This was done hastily, there will be fixes
 */
struct _Esmart_Thumb
{
  Epsilon *e;
  Epsilon_Info *info;
  Evas_Object *image;		/* thumb image that's displayed */
  double x, y, w, h;		/* smart object location/geometry */
  int tw, th;
};
typedef struct _Esmart_Thumb Esmart_Thumb;

/**
 * esmart_thumb_free - free an E_Thumb smart object 
 */
void esmart_thumb_free (Evas_Object * o);
/**
 * esmart_thumb_new - create a new E_Thumb object.  If the freedesktop cached
 * thumbnail exists it loads it, if not it creates it.  There's some
 * metadata in there if you're curious.
 * @evas - the evas we want to add the object to
 * @file - the name of the file we want a thumb of
 */
Evas_Object *esmart_thumb_new (Evas * evas, const char *file);
/**
 * esmart_thumb_file_get - get the full path to the thumbnail's real image
 * @o - The smart object we want the filename for
 * Returns a pointer to the filename, you should NOT free this memory
 */
const char *esmart_thumb_file_get (Evas_Object * o);
/**
 * esmart_thumb_geometry_get - Get the geometry of the source image
 * @o - The smart object we want the geometry of
 * @w - A pointer to somewhere we can store the width
 * @h - A pointer to somewhere we can store the height
 */
void esmart_thumb_geometry_get (Evas_Object * o, int *w, int *h);

/**
 * esmart_thumb_evas_object_get - Load the image file to an evas object
 * @o - The smart object we want the geometry of
 * Returns a valid Evas_Object if imlib2 or evas can load the file
 */
Evas_Object *esmart_thumb_evas_object_get (Evas_Object * o);

/**
 * esmart_thumb_format_get - get the format of the image this thumb is for
 * @o - The smart object we want the format for
 * NOTE: Don't free this string, dupe it if you wanna keep it around
 */
const char *esmart_thumb_format_get (Evas_Object * o);

/** 
 * esmart_thumb_freshen - forced reloading of the thumbnail image from disk
 * @o - The smart object we want the format for
 * Returns EPSILON_OK if the cache was bad and we regenerated, EPSILON_FAIL
 * if the cached image is still valid
 */
int esmart_thumb_freshen (Evas_Object * o);

#ifdef __cplusplus
}
#endif

#endif
