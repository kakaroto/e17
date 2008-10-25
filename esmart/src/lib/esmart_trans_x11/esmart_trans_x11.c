/**************************************************************************
 * An evas smart object template
 * 
 *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Data.h>
#include <assert.h>

#include "Esmart_Trans_X11.h"

typedef struct _Esmart_Trans_X11 Esmart_Trans_X11;

struct _Esmart_Trans_X11
{
    Evas_Object *obj, *clip;
    int x, y, w, h;
};

typedef struct {
   int x, y, w, h;
   Evas_Object *obj;
   Esmart_Trans_X11_Type type;
   Ecore_X_Window win;
   Ecore_X_Pixmap pmap_id;
   Ecore_Timer *timer;
} Esmart_Trans_Object;

static Evas_Smart * _esmart_trans_x11_smart_get(void);
/* smart object handlers */
static void _esmart_trans_x11_add(Evas_Object *o);
static void _esmart_trans_x11_del(Evas_Object *o);
static void _esmart_trans_x11_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _esmart_trans_x11_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _esmart_trans_x11_show(Evas_Object *o);
static void _esmart_trans_x11_hide(Evas_Object *o);
static void _esmart_trans_x11_color_set(Evas_Object *o, int r, int g, int b, int a);
static void _esmart_trans_x11_clip_set(Evas_Object *o, Evas_Object *clip);
static void _esmart_trans_x11_clip_unset(Evas_Object *o);
static int _esmart_trans_x11_property_cb(void *data, int type, void *event);

static Ecore_List       *_objects = NULL;
static Ecore_X_Window   rroot, vroot;
static Ecore_X_Atom     rootpmap, rootcolor;
static Ecore_X_Atom     x_virtual_roots, x_current_desktop, x_num_desktops;
static Ecore_X_Atom     x_pixmap, x_window, x_cardinal;
static Ecore_X_Atom     enlightenment_desktop;

static Ecore_Event_Handler *_root_prop_hnd = NULL;

static int
_esmart_trans_x11_timer_cb(void *data)
{
   Esmart_Trans_Object *o;
   
   o = (Esmart_Trans_Object *) data;
   esmart_trans_x11_freshen(o->obj, o->x, o->y, o->w, o->h);
   o->timer = NULL;
   return 0;
}   

static Esmart_Trans_Object *
_esmart_trans_x11_object_find(Evas_Object *o)
{
   Ecore_List_Node *n;
   Esmart_Trans_Object *eto = NULL;
  
   for (n = _objects->first; n; n = n->next)
   {
      eto = (Esmart_Trans_Object *) n->data;
      if (eto->obj == o)
         break;
   }

   return eto;
}

static Evas_Object *
_esmart_trans_x11_screengrab_get(Evas *evas, Evas_Object *old, int x, int y, int w, int h)
{
   Evas_Object          *new = NULL;
   Imlib_Image          im;

   if (old)
      evas_object_del(old);

   imlib_context_set_display(ecore_x_display_get());
   imlib_context_set_visual(DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get())));
   imlib_context_set_colormap(DefaultColormap(ecore_x_display_get(), DefaultScreen(ecore_x_display_get())));
   imlib_context_set_drawable(DefaultRootWindow(ecore_x_display_get()));
   im = imlib_create_image_from_drawable(0, x, y, w, h, 1);
   new = evas_object_image_add(evas);
   evas_object_image_alpha_set(new, 0);
   evas_object_image_size_set(new, w, h);
   if (im)
   {
      imlib_context_set_image(im);
      imlib_image_set_format("argb");
      evas_object_image_data_copy_set(new, imlib_image_get_data_for_reading_only());
      imlib_free_image_and_decache();
   }

   evas_object_image_fill_set(new, 0, 0, w, h);
   evas_object_resize(new, w, h);
   evas_object_move(new, 0, 0);
   evas_object_layer_set(new, -9999);
   evas_object_image_data_update_add(new, 0, 0, w, h);
   evas_object_show(new);

   return new;
}

static Evas_Object *
_esmart_trans_x11_pixmap_get(Evas *evas, Evas_Object *old, int x, int y, int w, int h)
{
   int                  num_desks = 0, ret, current_desk;
   unsigned char        *data;
   Evas_Object          *new = NULL;
   Ecore_X_Pixmap       p;
   Ecore_X_Window       *vroot_list = NULL;
   int                  offscreen = 0;
   int                  ox = 0, oy = 0;

   if (old)
      evas_object_del(old);
   
   imlib_context_set_display(ecore_x_display_get());
   imlib_context_set_visual(DefaultVisual(ecore_x_display_get(),DefaultScreen(ecore_x_display_get())));
   imlib_context_set_colormap(DefaultColormap(ecore_x_display_get(),DefaultScreen(ecore_x_display_get())));

   /* Attempt to find the current virtual desktop using NetWM properties */
   vroot = rroot;	/* Fall back to real root */
   if (ecore_x_window_prop_property_get(rroot, x_current_desktop, 
                                        x_cardinal,
                                        32, &data, &ret))
   {
      current_desk = *((int *) data);
      free (data);

      if (ecore_x_window_prop_property_get(rroot, x_num_desktops,
                                           x_cardinal,
                                           32, &data, &ret))
      {
         num_desks = *((int *) data);
         free(data);
      }
      
      if (ecore_x_window_prop_property_get(rroot, x_virtual_roots, x_window,
                                           32, &data, &ret))
      {
         vroot_list = (Ecore_X_Window *) data;

         if (current_desk < num_desks)
            vroot = vroot_list[current_desk];
         free(data);
      }
   }

   if (rootpmap)
   {
      /* Fetch the root pixmap */
      ret = ecore_x_window_prop_property_get(vroot, rootpmap, 
                                             x_pixmap, 32, &data, &ret);
      if (ret && (p = *((Ecore_X_Pixmap *) data)))
      {
         Imlib_Image    im;
         int            px, py, pw, ph;
            
         ecore_x_pixmap_geometry_get(p, &px, &py, &pw, &ph);
         if (pw && ph) {
            imlib_context_set_drawable(*((Ecore_X_Pixmap *) data));

            /* Check if the object will fit within the pixmap's boundaries */
            if ((x >= px) && (y >= py) && ((x + w) <= (px + pw))
                  && ((y + h) <= (py + ph)))
            {
               im = imlib_create_image_from_drawable(0, x, y, w, h, 1);
               imlib_context_set_image(im);
            }
            else
            {
               Imlib_Image dest;
               int         sx, sy, dx, dy;
              
               /* Resolve offscreen coordinates by drawing only visible component */
               if (x < 0)
               {
                  w += x;
                  ox = -x;
                  x = 0;
               }

               if (y < 0)
               {
                  h += y;
                  oy = -y;
                  y = 0;
               }

	       if (w <= 0)
		 w = 1;

	       if (h <= 0)
		 h = 1;
               
               offscreen = 1;
               dest = imlib_create_image_from_drawable(0, px, py, pw, ph, 1);
               im = imlib_create_image(w, h);
               imlib_context_set_image(im);
               imlib_image_clear();
               imlib_context_set_cliprect(0, 0, w, h);

               dx = x % pw;
               dy = y % ph;

               for (sy = 0; sy < (h + dy); sy += ph)
                  for (sx = 0; sx < (w + dx); sx += pw)
                     imlib_blend_image_onto_image(dest, 1, 0, 0, pw, ph, 
                                                  sx - dx, sy - dy, pw, ph);
                  
               imlib_context_set_image(dest);
               imlib_free_image_and_decache();
               imlib_context_set_image(im);
            }

            new = evas_object_image_add(evas);
            evas_object_image_alpha_set(new, 0);
            evas_object_image_size_set(new, w, h);
	    if (im)
	    {
               imlib_image_set_format("argb");
               evas_object_image_data_copy_set(new, imlib_image_get_data_for_reading_only());
               imlib_free_image_and_decache();
	    }

            evas_object_image_fill_set(new, 0, 0, w, h);
            evas_object_resize(new, w, h);
            evas_object_move(new, ox, oy);
            evas_object_layer_set(new, -9999);
            evas_object_image_data_update_add(new, 0, 0, w, h);
            evas_object_show(new);
         } /* if (pw && ph) */
         else /* This could happen with E16.5 or lesser:
                 Try to get pixmap from multiple desktop? */
            fprintf(stderr, "Esmart_Trans Error: Got invalid pixmap from root window! Ignored.\n");
      } /* if ((p = *((Ecore_X_Pixmap *) data))) */
      else
         fprintf(stderr, "Esmart_Trans Error: Could not read root window pixmap property!\n");

      if (data)
         free(data);
   } /* ecore_x_window_prop_property_get() */
   else
      fprintf(stderr, "Esmart_Trans Error: Could not obtain root pixmap atom.\n");

   if (!new) /* Fallback if no root pixmap is found */
   {
      int r = 0, g = 0, b = 0;

      new = evas_object_rectangle_add(evas);
      evas_object_resize(new, w, h);
      evas_object_move(new, 0, 0);
      evas_object_layer_set(new, -9999);
      
      if (ecore_x_window_prop_property_get(vroot, rootcolor, x_cardinal,
                                           32, &data, &ret))
      {
         unsigned long pixel = *((unsigned long *) data);
         r = (pixel >> 16) & 0xff;
         g = (pixel >> 8) & 0xff;
         b = pixel & 0xff;
         free (data);
      }
      else
      {
         fprintf(stderr, "Esmart_Trans Error: Cannot create transparency pixmap: no valid wallpaper and no background color set.\n");
      }

      evas_object_color_set(new, r, g, b, 255);
      evas_object_show(new);
   }

   return new;
}

/* keep a global copy of this, so it only has to be created once */
EAPI void
esmart_trans_x11_freshen(Evas_Object *o, int x, int y, int w, int h)
{
  Esmart_Trans_X11 *data;
  Esmart_Trans_Object *eto;

  Ecore_List_Node *n;

  static Ecore_X_Window old_vroot = 0;

  /* Search for requested object in list */
  for (n = _objects->first; n; n = n->next)
  {
     eto = (Esmart_Trans_Object *) n->data;
     if (eto->obj == o)
     {
        /* Update geometry for callback function(s) */
        eto->x = x;
        eto->y = y;
        eto->w = w;
        eto->h = h;
        
        /* Update the trans object */
        if((data = evas_object_smart_data_get(o)))
        {
           if (eto->type == Esmart_Trans_X11_Type_Background)
              data->obj =
                 _esmart_trans_x11_pixmap_get(evas_object_evas_get(data->clip),
                                              data->obj, x, y, w, h);
           else
              data->obj = _esmart_trans_x11_screengrab_get(evas_object_evas_get(data->clip),
                                                           data->obj, x, y, w, h);
           evas_object_pass_events_set(data->obj, 1);
           evas_object_clip_set(data->obj, data->clip);
           evas_object_move(data->clip, data->x, data->y);
           evas_object_resize(data->clip, data->w, data->h);
        }
        else
        {
           fprintf(stderr, "esmart_trans_x11_freshen: Eek, what happened to my object?\n");
        }

        /* Check for change in desktop, update event masks accordingly */
        if (vroot != old_vroot)
        {
           if (old_vroot != rroot)
              ecore_x_event_mask_unset(old_vroot, ECORE_X_EVENT_MASK_WINDOW_PROPERTY);
           ecore_x_event_mask_set(vroot, ECORE_X_EVENT_MASK_WINDOW_PROPERTY);
           old_vroot = vroot;
        }
        return;
     }
  }

  fprintf(stderr, "esmart_trans_x11_freshen: I know not this object you speak of.\n");
}

/*** external API ***/

EAPI Evas_Object *
esmart_trans_x11_new(Evas *e)
{
  Evas_Object *x11_trans_object;
  Esmart_Trans_Object *eto = NULL;

  /* Initialize objects, atoms and events if called for the first time */
  if (!_objects)
  {
     _objects = ecore_list_new();
     x_pixmap = ecore_x_atom_get("PIXMAP");
     rootpmap = ecore_x_atom_get("_XROOTPMAP_ID");
     rootcolor = ecore_x_atom_get("_XROOTCOLOR_PIXEL");
     x_window = ecore_x_atom_get("WINDOW");
     x_cardinal = ecore_x_atom_get("CARDINAL");
     x_virtual_roots = ecore_x_atom_get("_NET_VIRTUAL_ROOTS");
     x_current_desktop = ecore_x_atom_get("_NET_CURRENT_DESKTOP");
     x_num_desktops = ecore_x_atom_get("_NET_NUMBER_OF_DESKTOPS");
     enlightenment_desktop = ecore_x_atom_get("ENLIGHTENMENT_DESKTOP");
  }
  
  /* Get the trans object */
  x11_trans_object = evas_object_smart_add(e, _esmart_trans_x11_smart_get());
  
  /* Add to object list */
  eto = calloc(1, sizeof(Esmart_Trans_Object));
  eto->obj = x11_trans_object;
  eto->type = Esmart_Trans_X11_Type_Background;
  ecore_list_append(_objects, eto);

  return x11_trans_object;
}

EAPI void
esmart_trans_x11_type_set(Evas_Object *o, Esmart_Trans_X11_Type type)
{
   Esmart_Trans_Object *eto;

   if ((eto = _esmart_trans_x11_object_find(o)))
      eto->type = type;
}

EAPI Esmart_Trans_X11_Type
esmart_trans_x11_type_get(Evas_Object *o)
{
   Esmart_Trans_Object *eto;

   if ((eto = _esmart_trans_x11_object_find(o)))
      return eto->type;
   else
      return Esmart_Trans_X11_Type_Background;
}

EAPI void
esmart_trans_x11_window_set(Evas_Object *o, Ecore_X_Window win)
{
   Esmart_Trans_Object *eto;

   if((eto = _esmart_trans_x11_object_find(o)))
      eto->win = win;
}

/*** smart object handler functions ***/

/* Callback to handle property events on the root window */
static int
_esmart_trans_x11_property_cb(void *data, int type, void *event)
{
   Ecore_X_Event_Window_Property *e;
   Ecore_List_Node *n;

   if (!_objects)
      return 1;
   
   e = (Ecore_X_Event_Window_Property *) event;

   if ((e->win == rroot && (e->atom == x_current_desktop ||
                            e->atom == enlightenment_desktop)) ||
       (e->win == vroot && (e->atom == rootpmap || e->atom == rootcolor)))
   {
      /* Background may have changed: freshen all trans objects */
      for (n = _objects->first; n; n = n->next)
      {
         Esmart_Trans_Object *o;
         o = (Esmart_Trans_Object *) n->data;
         /* For desktop changes, do not freshen non-sticky windows */
#if 0
         if ((e->atom == x_current_desktop || e->atom == enlightenment_desktop)
             && !(ecore_x_window_prop_state_isset(e->win, 
                                                  ECORE_X_WINDOW_STATE_STICKY)))
            continue;
#endif
         /* Use timer to avoid consecutive events causing refreshes */
         if (o->timer)
            continue;
         else
            o->timer = ecore_timer_add(0.2, _esmart_trans_x11_timer_cb, o);
      }
   }

   return 1;
}

static const Evas_Smart_Class _esmart_trans_x11_smart_class = {
  "x11_trans_object",
  EVAS_SMART_CLASS_VERSION,
  _esmart_trans_x11_add,
  _esmart_trans_x11_del,
  _esmart_trans_x11_move,
  _esmart_trans_x11_resize,
  _esmart_trans_x11_show,
  _esmart_trans_x11_hide,
  _esmart_trans_x11_color_set,
  _esmart_trans_x11_clip_set,
  _esmart_trans_x11_clip_unset,
  NULL,
  NULL,
  NULL
};

static Evas_Smart *
_esmart_trans_x11_smart_get(void)
{
  static Evas_Smart *smart = NULL;

  if (smart)
     return smart;
  
  smart = evas_smart_class_new(&_esmart_trans_x11_smart_class);

  /* Get the real root window */
  rroot = DefaultRootWindow(ecore_x_display_get());

  ecore_x_event_mask_set(rroot, ECORE_X_EVENT_MASK_WINDOW_PROPERTY);
  _root_prop_hnd = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, 
                                           _esmart_trans_x11_property_cb, 
                                           NULL);

  return smart; 
}

static void
_esmart_trans_x11_add(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
 
  data = (Esmart_Trans_X11*)malloc(sizeof(Esmart_Trans_X11));
  memset(data, 0, sizeof(Esmart_Trans_X11));
      
  data->clip = evas_object_rectangle_add(evas_object_evas_get(o));
  evas_object_color_set(data->clip, 255, 255, 255, 255);
  evas_object_pass_events_set(data->clip, 1);
  
  evas_object_smart_data_set(o, data);
}


static void
_esmart_trans_x11_del(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
	if(data->obj)
	    evas_object_del(data->obj);
	if(data->clip)
	    evas_object_del(data->clip);
	data->obj = NULL;
	data->clip = NULL;
	free(data);
  }
}

static void
_esmart_trans_x11_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    if(data->x == x && data->y == y) return;
    evas_object_move(data->clip, x, y);
    data->x = x;
    data->y = y;
  }
}

static void
_esmart_trans_x11_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
  {
    if(data->w == w && data->h == h) return;
    evas_object_move(data->clip, w, h);
    data->w = w;
    data->h = h;
    evas_object_resize(data->clip, w, h);
  }
}

static void
_esmart_trans_x11_show(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_show(data->clip);
}

static void
_esmart_trans_x11_hide(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_hide(data->clip);
}

static void
_esmart_trans_x11_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
  evas_object_color_set(data->clip, r, g, b, a);
}

static void
_esmart_trans_x11_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_clip_set(data->clip, clip);
}

static void
_esmart_trans_x11_clip_unset(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_clip_unset(data->clip);
}

