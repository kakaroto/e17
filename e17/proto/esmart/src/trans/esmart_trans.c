/**************************************************************************
 * An evas smart object template
 * 
 *
 ***************************************************************************/

#include <stdlib.h>
#include "config.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <Ecore_X.h>

#include "Esmart_Trans.h"

static Evas_Smart * _esmart_trans_x11_smart_get(void);
/* smart object handlers */
static void _esmart_trans_x11_add(Evas_Object *o);
static void _esmart_trans_x11_del(Evas_Object *o);
static void _esmart_trans_x11_layer_set(Evas_Object *o, int l);
static void _esmart_trans_x11_raise(Evas_Object *o);
static void _esmart_trans_x11_lower(Evas_Object *o);
static void _esmart_trans_x11_stack_above(Evas_Object *o, Evas_Object *above);
static void _esmart_trans_x11_stack_below(Evas_Object *o, Evas_Object *below);
static void _esmart_trans_x11_move(Evas_Object *o, double x, double y);
static void _esmart_trans_x11_resize(Evas_Object *o, double w, double h);
static void _esmart_trans_x11_show(Evas_Object *o);
static void _esmart_trans_x11_hide(Evas_Object *o);
static void _esmart_trans_x11_color_set(Evas_Object *o, int r, int g, int b, int a);
static void _esmart_trans_x11_clip_set(Evas_Object *o, Evas_Object *clip);
static void _esmart_trans_x11_clip_unset(Evas_Object *o);

static Evas_Object *
_esmart_trans_x11_pixmap_get(Evas *evas, Evas_Object *old, int x, int y, int w, int h)
{
   int            ret;
   unsigned char  *data;
   Evas_Object    *new = NULL;
   Ecore_X_Pixmap p;
   Ecore_X_Atom   x_pixmap;
   Ecore_X_Atom   rootpmap, rootcolor;
   Ecore_X_Window root, *root_list;
   int            offscreen = 0;

   int            ox = 0, oy = 0;

   if (old)
      evas_object_del(old);
   
   x_pixmap = ecore_x_atom_get("PIXMAP");
   rootpmap = ecore_x_atom_get("_XROOTPMAP_ID");
   rootcolor = ecore_x_atom_get("_XROOTCOLOR_PIXEL");
   root_list = ecore_x_window_root_list(&ret);
   if(ret)
      root = *root_list;
   else
      root = 0; /* Paranoid */

   if (rootpmap)
   {
      ret = ecore_x_window_prop_property_get(root, rootpmap, 
                                             x_pixmap, 32, &data, &ret);
      if (ret && (p = *((Ecore_X_Pixmap *) data)))
      {
         Imlib_Image    im;
         unsigned int   pw, ph;
         int            px, py;
            
         ecore_x_pixmap_geometry_get(p, &px, &py, &pw, &ph);
         if (pw && ph) {
            imlib_context_set_drawable(*((Ecore_X_Pixmap *) data));

            /* Check if the trans object will fit within the pixmap's boundaries */
            if ((x >= px) && (y >= py) && ((x + w) <= (py + ((signed int) pw))) 
                  && ((y + h) <= (py + ((signed int) ph))))
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
               
               offscreen = 1;
               im = imlib_create_image_from_drawable(0, px, py, pw, ph, 1);
               dest = imlib_create_image(w, h);
               imlib_context_set_image(dest);
               imlib_image_clear();
               imlib_context_set_cliprect(0, 0, w, h);

               dx = x % pw;
               dy = y % ph;

               for (sy = 0; sy < (h + dy); sy += ph)
                  for (sx = 0; sx < (w + dx); sx += pw)
                     imlib_blend_image_onto_image(im, 1, 0, 0, pw, ph, 
                                                  sx - dx, sy - dy, pw, ph);
                  
               imlib_context_set_image(im);
               imlib_free_image();
               imlib_context_set_image(dest);
            }

            imlib_image_set_format("argb");
            new = evas_object_image_add(evas);
            evas_object_image_alpha_set(new, 0);
            evas_object_image_size_set(new, w, h);
            evas_object_image_data_copy_set(new, imlib_image_get_data_for_reading_only());
            imlib_free_image();

            evas_object_image_fill_set(new, 0, 0, w, h);
            evas_object_resize(new, w, h);
            evas_object_move(new, ox, oy);
            evas_object_layer_set(new, -9999);
            evas_object_image_data_update_add(new, 0, 0, w, h);
            evas_object_show(new);
         } /* if (pw && ph) */
         else /* This could happen with E16: Try to get pixmap from multiple desktop? */
            fprintf(stderr, "Esmart_Trans Error: Got invalid pixmap from root window! Ignored.\n");
      } /* if ((p = *((Ecore_X_Pixmap *) data))) */
      else
      {
         free(data);
         fprintf(stderr, "Esmart_Trans Error: Could not read root window pixmap property!\n");
      }
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
      
      if (ecore_x_window_prop_property_get(root, rootcolor, ecore_x_atom_get("CARDINAL"),
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
void
esmart_trans_x11_freshen(Evas_Object *o, int x, int y, int w, int h)
{
  Esmart_Trans_X11 *data;
  if((data = evas_object_smart_data_get(o)))
  {
      data->obj =
      _esmart_trans_x11_pixmap_get(evas_object_evas_get(data->clip),
	data->obj, x, y, w, h);
      evas_object_pass_events_set(data->obj, 1);
      evas_object_clip_set(data->obj, data->clip);
      evas_object_move(data->clip, data->x, data->y);
      evas_object_resize(data->clip, data->w, data->h);
  }

}
/*** external API ***/

Evas_Object *
esmart_trans_x11_new(Evas *e)
{
  Evas_Object *x11_trans_object;
  
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_visual(DefaultVisual(ecore_x_display_get(),DefaultScreen(ecore_x_display_get())));
  imlib_context_set_colormap(DefaultColormap(ecore_x_display_get(),DefaultScreen(ecore_x_display_get())));
  
  x11_trans_object = evas_object_smart_add(e,
				_esmart_trans_x11_smart_get());
  return x11_trans_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_esmart_trans_x11_smart_get(void)
{
  Evas_Smart *smart = NULL;

  smart = evas_smart_new ("x11_trans_object",
                          _esmart_trans_x11_add,
                          _esmart_trans_x11_del,
                          _esmart_trans_x11_layer_set,
                          _esmart_trans_x11_raise,
                          _esmart_trans_x11_lower,
                          _esmart_trans_x11_stack_above,
                          _esmart_trans_x11_stack_below,
                          _esmart_trans_x11_move,
                          _esmart_trans_x11_resize,
                          _esmart_trans_x11_show,
                          _esmart_trans_x11_hide,
                          _esmart_trans_x11_color_set,
                          _esmart_trans_x11_clip_set,
                          _esmart_trans_x11_clip_unset,
                          NULL
                          );

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
_esmart_trans_x11_layer_set(Evas_Object *o, int l)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_layer_set(data->clip, l);
}

static void
_esmart_trans_x11_raise(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_raise(data->clip);
}

static void
_esmart_trans_x11_lower(Evas_Object *o)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
	evas_object_lower(data->clip);
}

static void
_esmart_trans_x11_stack_above(Evas_Object *o, Evas_Object *above)
{
  Esmart_Trans_X11 *data;
  
  if((data = evas_object_smart_data_get(o)))
    evas_object_stack_above(data->clip, above);
}

static void
_esmart_trans_x11_stack_below(Evas_Object *o, Evas_Object *below)
{
  Esmart_Trans_X11 *data;
  
  data = evas_object_smart_data_get(o);

  if((data = evas_object_smart_data_get(o)))
    evas_object_stack_below(data->clip, below);
}

static void
_esmart_trans_x11_move(Evas_Object *o, double x, double y)
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
_esmart_trans_x11_resize(Evas_Object *o, double w, double h)
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

