#ifndef E_THUMB_H
#define E_THUMB_H

#include<Evas.h>

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
struct _E_Thumb
{
    struct {
	int mtime;
	int w, h;
	char *name;
    } thumb, file;	/* info about the thumb and source files */
    char *uri;		/* uri for the file we're caching */
    char *format;	/* image format of the real image */
    Evas_Object *image;	/* thumb image that's displayed */
    double x, y, w, h;	/* smart object location/geometry */
};
typedef struct _E_Thumb E_Thumb;

/**
 * e_thumb_init - create the directories ethumb is going to cache to
 */
void e_thumb_init(void);

/**
 * e_thumb_free - free an E_Thumb smart object 
 */
void e_thumb_free(Evas_Object *o);
/**
 * e_thumb_new - create a new E_Thumb object.  If the freedesktop cached
 * thumbnail exists it loads it, if not it creates it.  There's some
 * metadata in there if you're curious.
 * @evas - the evas we want to add the object to
 * @file - the name of the file we want a thumb of
 */
Evas_Object * e_thumb_new(Evas *evas, const char *file);
/**
 * e_thumb_file_get - get the full path to the thumbnail's real image
 * @o - The smart object we want the filename for
 * Returns a pointer to the filename, you should NOT free this memory
 */
const char* e_thumb_file_get(Evas_Object *o);
/**
 * e_thumb_geometry_get - Get the geometry of the source image
 * @o - The smart object we want the geometry of
 * @w - A pointer to somewhere we can store the width
 * @h - A pointer to somewhere we can store the height
 */
void e_thumb_geometry_get(Evas_Object *o, int *w, int *h);

/**
 * e_thumb_evas_object_get - Load the image file to an evas object
 * @o - The smart object we want the geometry of
 * Returns a valid Evas_Object if imlib2 or evas can load the file
 */
Evas_Object *e_thumb_evas_object_get(Evas_Object *o);

/**
 * e_thumb_format_get - get the format of the image this thumb is for
 * @o - The smart object we want the format for
 * NOTE: Don't free this string, dupe it if you wanna keep it around
 */
const char * e_thumb_format_get(Evas_Object *o);

#endif
