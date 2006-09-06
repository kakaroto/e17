#ifndef _ESMART_THUMB_H
#define _ESMART_THUMB_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#include <Evas.h>

#ifdef __cplusplus
extern "C"
{
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

/**
 * esmart_thumb_free - free an E_Thumb smart object 
 */
  EAPI void esmart_thumb_free (Evas_Object * o);
/**
 * esmart_thumb_new - create a new E_Thumb object.  If the freedesktop cached
 * thumbnail exists it loads it, if not it creates it.  There's some
 * metadata in there if you're curious.
 * @evas - the evas we want to add the object to
 * @file - the name of the file we want a thumb of
 */
  EAPI Evas_Object *esmart_thumb_new (Evas * evas, const char *file);
/**
 * esmart_thumb_file_get - get the full path to the thumbnail's real image
 * @o - The smart object we want the filename for
 * Returns a pointer to the filename, you should NOT free this memory
 */
  EAPI const char *esmart_thumb_file_get (Evas_Object * o);
/**
 * esmart_thumb_geometry_get - Get the geometry of the source image
 * @o - The smart object we want the geometry of
 * @w - A pointer to somewhere we can store the width
 * @h - A pointer to somewhere we can store the height
 */
  EAPI void esmart_thumb_geometry_get (Evas_Object * o, int *w, int *h);

/**
 * esmart_thumb_evas_object_get - Load the image file to an evas object
 * @o - The smart object we want the geometry of
 * @orient - 1 to auto orient, 0 to just load as is
 * Returns a valid Evas_Object if imlib2 or evas can load the file
 */
  EAPI Evas_Object *esmart_thumb_evas_object_get (Evas_Object * o, int orient);

/**
 * esmart_thumb_evas_object_image_get - the actual image object in the thumb
 * @o - the object returned from esmart_thumb_new
 * Returns a valid Evas_Object* on success, NULL on no image
 */
  EAPI Evas_Object *esmart_thumb_evas_object_image_get (Evas_Object * o);

/**
 * esmart_thumb_format_get - get the format of the image this thumb is for
 * @o - The smart object we want the format for
 * NOTE: Don't free this string, dupe it if you wanna keep it around
 */
  EAPI const char *esmart_thumb_format_get (Evas_Object * o);

/** 
 * esmart_thumb_freshen - forced reloading of the thumbnail image from disk
 * @o - The smart object we want the format for
 * Returns EPSILON_OK if the cache was bad and we regenerated, EPSILON_FAIL
 * if the cached image is still valid
 */
  EAPI int esmart_thumb_freshen (Evas_Object * o);

/** 
 * esmart_thumb_exif_get - Get whether or not the exif data is present
 * @o - The smart object we're curious about
 * Returns 1 if exif data is present in the esmart_thumb, 0 if not
 */
  EAPI int esmart_thumb_exif_get (Evas_Object * o);
/** 
 * esmart_thumb_exif_data_as_string_get - Get an exif tag as a string
 * @o - The smart object we're curious about
 * @prop - The exif prop you want
 * Returns NULL if not found, string value for the tag if present
 */
  EAPI const char *esmart_thumb_exif_data_as_string_get (Evas_Object * o, int lvl,
							 int prop);
/** 
 * esmart_thumb_exif_data_as_int_get - Get an exif tag as an int
 * @o - The smart object we're curious about
 * @prop - The exif prop you want
 * Returns -1 if not found, string value for the tag if present
 */
  EAPI int esmart_thumb_exif_data_as_int_get (Evas_Object * o, int lvl, int prop);

#ifdef __cplusplus
}
#endif

#endif
