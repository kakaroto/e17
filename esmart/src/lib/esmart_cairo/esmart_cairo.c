#include "Esmart_Cairo.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static const char DATA_KEY[] = "esmart_cairo_surface";

static void
_err(const char *function, const char *msg, ...)
{
   va_list ap;

   fprintf(stderr, "ERROR: %s() ", function);

   va_start(ap, msg);
   vfprintf(stderr, msg, ap);
   va_end(ap);

   fputc('\n', stderr);
}
#define err(msg, ...) _err(__FUNCTION__, msg, ##__VA_ARGS__)

static void
_esmart_image_cairo_destroy_surface(Evas_Object *o)
{
   cairo_surface_t *surface;

   surface = evas_object_data_del(o, DATA_KEY);
   if (!surface)
     return;

   cairo_surface_destroy(surface);
}

static void
_esmart_image_cairo_died(void *data, Evas *e, Evas_Object *o, void *einfo)
{
   _esmart_image_cairo_destroy_surface(o);
}

/**
 * Creates a new Evas Object image that is associated with cairo image surface.
 *
 * This function will automatically create the cairo image surface
 * based on the given parameters. The surface can be retrieved with
 * esmart_image_cairo_surface_get().
 *
 * The returned object is a regular Evas_Object of "image" type, like
 * those created with evas_object_image_add(), but the pixels are set
 * to be read from the created cairo surface.
 *
 * This cairo surface may be replaced later with a call to
 * esmart_image_cairo_surface_set(), this will destroy the current
 * surface and use the new one.
 *
 * Users are encouraged to use evas_object_image_* functions, like
 * evas_object_image_fill_set() in order to fine tune behavior, but be
 * aware that some functions that change buffer access are dangerous,
 * for instance, using evas_object_image_size_set() will lead to
 * problems.
 *
 * After cairo surface is updated, users must call functions to notify
 * it changed, like evas_object_image_pixels_dirty_set() or
 * evas_object_image_data_update_add().
 *
 * @param evas canvas to add the image object.
 * @param w width of the created surface.
 * @param h height of the created surface.
 * @param alpha if surface will have alpha channel or not.
 * @return the image object.
 *
 * @see esmart_image_cairo_new_from_surface()
 * @see esmart_image_cairo_surface_set()
 */
Evas_Object *
esmart_image_cairo_new(Evas *evas, Evas_Coord w, Evas_Coord h, Evas_Bool alpha)
{
   cairo_surface_t *surface;
   cairo_format_t format;

   if (alpha)
     format = CAIRO_FORMAT_ARGB32;
   else
     format = CAIRO_FORMAT_RGB24;

   surface = cairo_image_surface_create(format, w, h);
   esmart_image_cairo_new_from_surface(evas, surface);
}

/**
 * Creates a new Evas Object image that is associated with cairo image surface.
 *
 * This function will use the provided surface, that should be an
 * image surface using either ARGB32 or RGB24 formats.
 *
 * The returned object is a regular Evas_Object of "image" type, like
 * those created with evas_object_image_add(), but the pixels are set
 * to be read from the created cairo surface.
 *
 * This cairo surface may be replaced later with a call to
 * esmart_image_cairo_surface_set(), this will destroy the current
 * surface and use the new one.
 *
 * Users are encouraged to use evas_object_image_* functions, like
 * evas_object_image_fill_set() in order to fine tune behavior, but be
 * aware that some functions that change buffer access are dangerous,
 * for instance, using evas_object_image_size_set() will lead to
 * problems.
 *
 * After cairo surface is updated, users must call functions to notify
 * it changed, like evas_object_image_pixels_dirty_set() or
 * evas_object_image_data_update_add().
 *
 * @param evas canvas to add the image object.
 * @param cairo_surface the surface to use, width, height and alpha will
 *        be retrieved from this parameter.
 * @return the image object.
 *
 * @see esmart_image_cairo_new()
 * @see esmart_image_cairo_surface_set()
 */
Evas_Object *
esmart_image_cairo_new_from_surface(Evas *evas, cairo_surface_t *cairo_surface)
{
   Evas_Object *o;

   o = evas_object_image_add(evas);
   if (!o)
     return NULL;

   if (!esmart_image_cairo_surface_set(o, cairo_surface))
     {
	evas_object_del(o);
	return NULL;
     }

   return o;
}

/**
 * Get the associated cairo surface.
 *
 * @param o the image object to get associated cairo surface.
 * @return cairo surface or NULL if none is associated.
 */
cairo_surface_t *
esmart_image_cairo_surface_get(Evas_Object *o)
{
   return evas_object_data_get(o, DATA_KEY);
}

/**
 * Set the associated cairo surface.
 *
 * This will make a regular Evas object of image type use contents of cairo
 * image surface, every path stroke on the give surface will show on as the
 * contents of this object.
 *
 * If there was already a surface, then it will be destroyed and the
 * new one will be used. If @a cairo_surface is NULL, then just the
 * old is destroyed. However if @a cairo_surface is not NULL but is
 * somehow invalid, the old one is preserved.
 *
 * Evas image object will have some properties set automatically, they are:
 *  - image_alpha: based on image format, if ARGB32 it's set to 1, 0 otherwise;
 *  - image_size: based on surface size;
 *  - image_fill: also based on surface size;
 *  - size: also based on surface size.
 *
 * Remember that any changes to @a cairo_surface will not have effect
 * until evas_object_image_data_update_add() or
 * evas_object_image_pixels_dirty_set() is called.
 *
 * Image fill is set to surface size, so if object is resized, the
 * surface contents will be tiled. If instead you like to have it
 * scaled by Evas, change image fill property with
 * evas_object_image_fill_set() or use
 * esmart_image_cairo_fill_auto_set(). If you wish to resize the
 * surface itself, then you need to create another cairo image surface
 * and call this function again.
 *
 * @param o the image object to associate cairo surface.
 * @param cairo_surface the image surface to associate.
 * @return 1 on success, 0 on failure.
 */
Evas_Bool
esmart_image_cairo_surface_set(Evas_Object *o, cairo_surface_t *cairo_surface)
{
   cairo_status_t status;
   cairo_format_t format;
   cairo_surface_type_t type;
   Evas_Coord w, h;
   int has_alpha;

   if (!o)
     {
	err("o == NULL");
	return 0;
     }

   if (!cairo_surface)
     {
	evas_object_event_callback_del(
           o, EVAS_CALLBACK_DEL, _esmart_image_cairo_died);
	_esmart_image_cairo_destroy_surface(o);
	return 1;
     }

   status = cairo_surface_status(cairo_surface);
   if (status != CAIRO_STATUS_SUCCESS)
     {
	err("invalid status for cairo surface: %s",
	    cairo_status_to_string(status));
	return 0;
     }

   type = cairo_surface_get_type(cairo_surface);
   if (type != CAIRO_SURFACE_TYPE_IMAGE)
     {
	err("invalid surface type: %d, expected %d",
	    type, CAIRO_SURFACE_TYPE_IMAGE);
	return 0;
     }

   format = cairo_image_surface_get_format(cairo_surface);
   if (format == CAIRO_FORMAT_ARGB32)
     has_alpha = 1;
   else if (format == CAIRO_FORMAT_RGB24)
     has_alpha = 0;
   else
     {
	err("unsupported format for given surface: %d", format);
	return 0;
     }

   _esmart_image_cairo_destroy_surface(o);

   w = cairo_image_surface_get_width(cairo_surface);
   h = cairo_image_surface_get_height(cairo_surface);

   evas_object_image_alpha_set(o, has_alpha);
   evas_object_image_size_set(o, w, h);
   evas_object_image_fill_set(o, 0, 0, w, h);
   evas_object_image_data_set(o, cairo_image_surface_get_data(cairo_surface));
   evas_object_resize(o, w, h);

   evas_object_data_set(o, DATA_KEY, cairo_surface);

   evas_object_event_callback_del(
      o, EVAS_CALLBACK_DEL, _esmart_image_cairo_died);
   evas_object_event_callback_add(
      o, EVAS_CALLBACK_DEL, _esmart_image_cairo_died, NULL);

   return 1;
}

static void
_esmart_image_cairo_fill_auto(Evas_Object *o)
{
   Evas_Coord w, h;

   evas_object_geometry_get(o, NULL, NULL, &w, &h);
   evas_object_image_fill_set(o, 0, 0, w, h);
}

static void
_esmart_image_cairo_resized(void *data, Evas *e, Evas_Object *o, void *einfo)
{
   _esmart_image_cairo_fill_auto(o);
}

/**
 * Have the image_fill property to follow object size.
 *
 * Use this helper function to call evas_object_image_fill_set() with
 * the current object size and keep them the same after object
 * resizes.
 *
 * @param o the evas image object to use.
 * @param enable if true, enable this behavior, false disables it.
 */
void
esmart_image_cairo_fill_auto_set(Evas_Object *o, Evas_Bool enable)
{
   evas_object_event_callback_del(
      o, EVAS_CALLBACK_RESIZE, _esmart_image_cairo_resized);

   if (enable)
     {
	evas_object_event_callback_add(
           o, EVAS_CALLBACK_RESIZE, _esmart_image_cairo_resized, NULL);
	_esmart_image_cairo_fill_auto(o);
     }
   else
     {
	cairo_surface_t *cairo_surface;
	int w, h;

	cairo_surface = evas_object_data_get(o, DATA_KEY);
	if (!cairo_surface)
	  {
	     err("no cairo surface found for object");
	     return;
	  }

	w = cairo_image_surface_get_width(cairo_surface);
	h = cairo_image_surface_get_height(cairo_surface);
	evas_object_image_fill_set(o, 0, 0, w, h);
     }
}
