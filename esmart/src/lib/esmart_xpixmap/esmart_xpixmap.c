/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "config.h"
#include <X11/Xlib.h>
#include <Ecore.h>
#include <Ecore_X.h>

#include "Esmart_XPixmap.h"

#define E_OBJ_NAME "esmart_xpixmap"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
   Evas_Object         *obj;
   Eina_List           *handlers;
   Ecore_X_Display     *dpy;

   Ecore_X_Window       win;
   Ecore_X_Pixmap       pixmap;

   XImage              *xim;
};

static void _pixels_get(void *data, Evas_Object *obj);

static int  _damage_cb(void *data, int type, void *event);
static int  _destroy_cb(void *data, int type, void *event);
static int  _configure_cb(void *data, int type, void *event);

static void _smart_init(void);
static void _smart_add(Evas_Object *obj, Ecore_X_Pixmap pixmap, Ecore_X_Window win);
static void _smart_del(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object *obj);

/***********/
/* Globals */
/***********/
static Evas_Smart *smart = NULL;

EAPI Evas_Object *
esmart_xpixmap_new(Evas *e, Ecore_X_Pixmap pixmap, Ecore_X_Window win)
{
   Evas_Object *o;

   _smart_init();
   o = evas_object_smart_add(e, smart);

   _smart_add(o, pixmap, win);
   return o;
}

/**********************/
/* Internal functions */
/**********************/
static void
_pixels_get(void *data, Evas_Object *obj)
{
   int iw, ih;
   int px, py, pw, ph;
   Smart_Data *sd;
   unsigned char *bgra_data;

   sd = data;
   if (sd->xim)
     {
        XDestroyImage(sd->xim);
        sd->xim = NULL;
     }

   evas_object_image_size_get(obj, &iw, &ih);
   ecore_x_drawable_geometry_get(sd->pixmap, &px, &py, &pw, &ph);
   sd->xim = XGetImage(sd->dpy, sd->pixmap, px, py, pw, ph, AllPlanes, ZPixmap);


   if ((pw != iw) || (ph != ih))
     {
        evas_object_image_alpha_set(obj, 1);
	evas_object_image_size_set(obj, pw, ph);
     }
   if ((iw < 1) || (ih < 1))
     evas_object_image_pixels_dirty_set(obj, 0);
   else
     {
        evas_object_image_data_set(obj, sd->xim->data);
        evas_object_image_pixels_dirty_set(obj, 0);
     }
}

static int
_damage_cb(void *data, int type, void *event)
{
   Smart_Data *sd;
   Ecore_X_Event_Damage *ev;

   if (!(sd = data)) return 1;
   if (!(ev = event)) return 1;
   if (sd->win != ev->drawable) return 1;
   evas_object_image_pixels_dirty_set(sd->obj, 1);
   return 1;
}

static int
_destroy_cb(void *data, int type, void *event)
{
   Smart_Data *sd;
   Ecore_X_Event_Window_Destroy *ev;

   if (!(sd = data)) return 1;
   if (!(ev = event)) return 1;
   if (sd->win != ev->win) return 1;
   ecore_x_pixmap_del(sd->pixmap);
   sd->pixmap = None;
   return 1;
}

static int
_configure_cb(void *data, int type, void *event)
{
   Smart_Data *sd;
   Ecore_X_Event_Window_Configure *ev;

   if (!(sd = data)) return 1;
   if (!(ev = event)) return 1;
   if (sd->win != ev->win) return 1;
   ecore_x_pixmap_del(sd->pixmap);
   sd->pixmap = ecore_x_composite_name_window_pixmap_get(sd->win);
   evas_object_image_pixels_dirty_set(sd->obj, 1);
   return 1;
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/
static void
_smart_init(void)
{
   if (smart) return;
     {
	static const Evas_Smart_Class sc =
	  {
	     E_OBJ_NAME,
	       EVAS_SMART_CLASS_VERSION,
	       NULL,
	       _smart_del,
	       _smart_move,
	       _smart_resize,
	       _smart_show,
	       _smart_hide,
	       _smart_color_set,
	       _smart_clip_set,
	       _smart_clip_unset,
	       NULL,            /* Calculate */
	       NULL,            /* Member add */
	       NULL,            /* Member del */
	       NULL             /* Data */
	  };
        smart = evas_smart_class_new(&sc);
     }
}

static void
_smart_add(Evas_Object *obj, Ecore_X_Pixmap pixmap, Ecore_X_Window win)
{
   Smart_Data *sd;
   unsigned int *pixel;

   sd = calloc(1, sizeof(Smart_Data));
   if (!sd) return;
   sd->obj = evas_object_image_add(evas_object_evas_get(obj));
   if (pixmap)
     sd->pixmap = pixmap;
   else if (win)
     {
        sd->win = win;
        sd->pixmap = ecore_x_composite_name_window_pixmap_get(win);
        sd->handlers = eina_list_append(sd->handlers,
              ecore_event_handler_add(ECORE_X_EVENT_DAMAGE_NOTIFY, _damage_cb, sd));
        sd->handlers = eina_list_append(sd->handlers,
              ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY, _destroy_cb, sd));
        sd->handlers = eina_list_append(sd->handlers,
              ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CONFIGURE, _configure_cb, sd));
     }

   sd->dpy = ecore_x_display_get();
   evas_object_image_pixels_get_callback_set(sd->obj, _pixels_get, sd);
   evas_object_smart_member_add(sd->obj, obj);

   /* XXX: handle shaped windows */
   evas_object_image_alpha_set(sd->obj, 1);
   evas_object_smart_data_set(obj, sd);
   _pixels_get(sd, sd->obj);
}

static void
_smart_del(Evas_Object *obj)
{
   Smart_Data *sd;
   Ecore_Event_Handler *h;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   if (sd->xim)
     XDestroyImage(sd->xim);
   EINA_LIST_FREE(sd->handlers, h)
      ecore_event_handler_del(h);
   evas_object_del(sd->obj);
   free(sd);
}

static void
_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_move(sd->obj, x, y);
}

static void
_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_image_fill_set(sd->obj, 0, 0, w, h);
   evas_object_resize(sd->obj, w, h);
}

static void
_smart_show(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_show(sd->obj);

}

static void
_smart_hide(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_hide(sd->obj);
}

static void
_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_color_set(sd->obj, r, g, b, a);
}

static void
_smart_clip_set(Evas_Object *obj, Evas_Object * clip)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_set(sd->obj, clip);
}

static void
_smart_clip_unset(Evas_Object *obj)
{
   Smart_Data *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd) return;
   evas_object_clip_unset(sd->obj);
}
