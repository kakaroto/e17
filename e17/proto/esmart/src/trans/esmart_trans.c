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

/**
 * Stolen from ev and iconbar, hopefully this should be shareable now
 */
static Evas_Object *
_esmart_trans_x11_pixmap_get(Evas *evas, Evas_Object *old,
                        int x, int y, int w, int h)
{
  Atom            prop,type; 
  int             format;
  unsigned long   length,after;
  unsigned char  *data;
  Evas_Object    *new=NULL;
  Pixmap          p;

  if(old)
    evas_object_del(old);

  if((prop=XInternAtom(ecore_x_display_get(),"_XROOTPMAP_ID",True))!=None)
  {
    /* FIXME: CACHE ME! */
    int ret=XGetWindowProperty(ecore_x_display_get(), 
		               RootWindow(ecore_x_display_get(), 0),
                               prop, 0L, 1L, False, AnyPropertyType, &type,
                               &format,&length, &after,&data);
    if((ret==Success)&&(type==XA_PIXMAP)&&((p=*((Pixmap *)data))!=None)) {
      Imlib_Image  im;
      unsigned int pw,ph, pb,pd;
      int          px,py;
      Window       win_dummy;
      Status       st;

      st=XGetGeometry(ecore_x_display_get(),p,&win_dummy, &px,&py,&pw,&ph, &pb, &pd);
      if(st&&(pw>0)&&(ph>0)) {
#  ifdef NOIR_DEBUG
        fprintf(stderr,"bg_ebg_trans: transparency update %3d,%3d %3dx%3d\n",x,y,w,h);
#  endif

        imlib_context_set_drawable(*((Pixmap *)data));

	if((x>=px)&&(y>=py)&&((x+w)<=(py+((signed int)pw)))&&((y+h)<=(py+((signed int)ph)))) 
	{
	    im = imlib_create_image_from_drawable (0, x, y, w, h, 1);
	    imlib_context_set_image (im);
	}
	else /* tiled root pixmap */
	{
	    Imlib_Image  dst;
	    int          sx, sy, dx, dy;

	    im=imlib_create_image_from_drawable(0, px, py, pw, ph, 1);
	    dst=imlib_create_image(w, h);
	    imlib_context_set_image(dst);
	    imlib_image_clear();
	    imlib_context_set_cliprect(0, 0, w, h);

	    dx = (x%pw);
	    dy = (y%ph);

       /* There really ought to be a better way to do this, like
        * negative coordinates, but I'm not sure those are valid */
       imlib_blend_image_onto_image(im, 1, dx, dy, pw - dx, ph - dy, 
                                    0, 0, pw - dx, ph - dy);
       imlib_blend_image_onto_image(im, 1, 0, dy, dx, pw - dy,
                                    pw - dx, 0, dx, ph -dy);
       imlib_blend_image_onto_image(im, 1, dx, 0, pw - dx, dy,
                                    0, ph - dy, pw - dx, dy);
       imlib_blend_image_onto_image(im, 1, 0, 0, dx, dy,
                                    pw - dx, ph - dy, dx, dy);
                                    

       for (sx = 0; sx < w; sx += pw)
          for (sy = 0; sy < h; sy += ph)
             if(sx || sy)
              imlib_image_copy_rect(0, 0, pw, ph, sx, sy);
	    
       imlib_context_set_image(im);
	    imlib_free_image();
	    imlib_context_set_image(dst); 
	}
        imlib_image_set_format("argb");
        new=evas_object_image_add(evas);
        evas_object_image_alpha_set(new,0);
        evas_object_image_size_set(new,w,h);   /* thanks rephorm */
        evas_object_image_data_copy_set(new,imlib_image_get_data_for_reading_only());
        imlib_free_image();

        evas_object_image_fill_set(new,0,0,w,h);
        evas_object_resize(new,w,h);
        evas_object_move(new,0,0);
        evas_object_layer_set(new,-9999);
        evas_object_image_data_update_add(new,0,0,w,h);
        evas_object_show(new); }
#if 1
      else  /* this can happen with e16 */
         /* It would be nice to somehow make this actually attempt to
          * get the background pixmap from e16 on alternate desktops. */
        fprintf(stderr,"bg_ebg_trans: got invalid pixmap from root-window, ignoring...\n");
#endif
    }
    else
      fprintf(stderr,"bg_ebg_trans: could not read root-window property _XROOTPMAP_ID...\n"); }
  else
    fprintf(stderr,"bg_ebg_trans: could not get XAtom _XROOTPMAP_ID...\n");

  if(!new) {  /* fallback if no root pixmap is found */
#if 1
    fprintf(stderr,"bg_ebg_trans: cannot create transparency pixmap, no valid wallpaper set.\n");
#endif
    new=evas_object_rectangle_add(evas);
    evas_object_resize(new,w,h);
    evas_object_move(new,0,0);
    evas_object_layer_set(new,-9999);
    evas_object_color_set(new, 127,127,127, 255);
    evas_object_show(new); }

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
  memset(data, 0, sizeof(Esmart_Trans_X11*));
      
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

