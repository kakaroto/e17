#include "Elicit.h"

static Evas_Smart *_smart;

typedef struct _Elicit_Zoom Elicit_Zoom;

struct _Elicit_Zoom
{
  Evas_Object *shot;
  Evas_Object *grid;

  Evas_Coord ow, oh; // current object size
  int iw, ih; // how large of a shot to take
  int zoom;

  int has_data;
};

static void _smart_init(void);
static void _smart_add(Evas_Object *o);
static void _smart_del(Evas_Object *o);
static void _smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object *o);
static void _smart_hide(Evas_Object *o);


Evas_Object *
elicit_zoom_add(Evas *evas) 
{
  _smart_init();
  return evas_object_smart_add(evas, _smart);
  
}

void
elicit_zoom_zoom_set(Evas_Object *o, double zoom)
{
  Elicit_Zoom *z;
  int iw, ih;

  z = evas_object_smart_data_get(o);
  if (z->zoom == zoom) return;

  z->zoom = zoom;
  evas_object_image_size_get(z->shot, &iw, &ih);
  evas_object_image_fill_set(z->shot, 0, 0, iw * zoom, ih * zoom);

  evas_object_image_fill_set(z->grid, 0, 0, zoom, zoom);
}

void
elicit_zoom_grid_visible_set(Evas_Object *o, int visible)
{
  Elicit_Zoom *z;
  z = evas_object_smart_data_get(o);

  elicit_config_grid_visible_set(visible);
  if (!z->has_data) return;
  if (visible)
    evas_object_show(z->grid);
  else
    evas_object_hide(z->grid);
}

void
elicit_zoom(Evas_Object *o)
{
  Elicit_Zoom *z;
  Imlib_Image *im;
  int x, y;
  int px, py;
  int dw, dh;
  int tr;
  Evas_Coord sw, sh;
  Window dummy;

  z = evas_object_smart_data_get(o);
  ecore_x_pointer_last_xy_get(&px, &py);

  x = px - .5 * z->iw;
  y = py - .5 * z->ih;

  /* keep shot within desktop bounds */
  ecore_x_window_size_get(RootWindow(ecore_x_display_get(),0), &dw, &dh);
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x + z->iw > dw) x = dw - z->iw;
  if (y + z->ih > dh) y = dh - z->ih;

  /* setup the imlib context */
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
  imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));

  /* copy the correct part of the screen */
  im = imlib_create_image_from_drawable(0, x, y, z->iw, z->ih, 1);
  imlib_context_set_image(im);
  imlib_image_set_format("argb");

  elicit_zoom_data_set(o, imlib_image_get_data_for_reading_only(), z->iw, z->ih);

  imlib_free_image();
}

void
elicit_zoom_data_get(Evas_Object *o, void **data, int *w, int *h) 
{
  Elicit_Zoom *z;
  int iw, ih;

  z = evas_object_smart_data_get(o);
  if (!z) return;

  evas_object_image_size_get(z->shot, &iw, &ih);
  if (w) *w = iw;
  if (h) *h = ih;
  if (data) *data = evas_object_image_data_get(z->shot, 1);
}

void
elicit_zoom_data_set(Evas_Object *o, void *data, int w, int h)
{
  Elicit_Zoom *z;

  z = evas_object_smart_data_get(o);
  if (!z) return;

  /* get the object ready, copy the data in */
  evas_object_image_alpha_set(z->shot, 0);
  evas_object_image_size_set(z->shot, z->iw, z->ih);
  evas_object_image_smooth_scale_set(z->shot, 0);
  evas_object_image_data_copy_set(z->shot, data);
  
  /* tell evas that we changed part of the image data */
  evas_object_image_data_update_add(z->shot, 0, 0, z->iw, z->ih);

  /* set it to fill at the current zoom level */
  evas_object_image_fill_set(z->shot, 0, 0, z->iw * z->zoom, z->ih * z->zoom);

  z->has_data = 1;
}


static void
_smart_init(void)
{
   if (_smart) return;
   _smart = evas_smart_new("elicit_zoom",
			     _smart_add,
			     _smart_del, 
			     NULL, NULL, NULL, NULL, NULL,
			     _smart_move,
			     _smart_resize,
			     _smart_show,
			     _smart_hide,
           NULL, // color_set
           NULL, // clip_set
           NULL, // clip_unset
			     NULL);
}

static void
_smart_add(Evas_Object *o)
{
  Elicit_Zoom *z;
  Evas *evas;
  char buf[PATH_MAX];

  z = calloc(1, sizeof(Elicit_Zoom));
  if (!z) return;

  evas_object_smart_data_set(o, z);

  evas = evas_object_evas_get(o);

  z->shot = evas_object_image_add(evas);
  evas_object_smart_member_add(z->shot, o);

  z->grid = evas_object_image_add(evas);

  snprintf(buf, sizeof(buf), DATADIR"/images/grid_cell.png");
  evas_object_image_file_set(z->grid, buf, "");
  evas_object_smart_member_add(z->grid, o);

  elicit_zoom_zoom_set(o, 4);
}

static void
_smart_del(Evas_Object *o)
{
  Elicit_Zoom *z;

  z = evas_object_smart_data_get(o);

  if (z->shot) evas_object_del(z->shot);
  if (z->grid) evas_object_del(z->grid);

  free(z);
  evas_object_smart_data_set(o, NULL);
}

static void
_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Elicit_Zoom *z;

  z = evas_object_smart_data_get(o);

  evas_object_move(z->grid, x, y);
  evas_object_move(z->shot, x, y);
}

static void
_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Elicit_Zoom *z;

  z = evas_object_smart_data_get(o);
  evas_object_resize(z->shot, w, h);
  evas_object_resize(z->grid, w, h);

  z->ow = w;
  z->oh = h;

  z->iw = (w / z->zoom) + (w % z->zoom ? 1 : 0);
  z->ih = (h / z->zoom) + (h % z->zoom ? 1 : 0);
}

static void
_smart_show(Evas_Object *o)
{
  Elicit_Zoom *z;

  z = evas_object_smart_data_get(o);

  evas_object_show(z->shot);
  if (z->has_data && elicit_config_grid_visible_get()) evas_object_show(z->grid);
}

static void
_smart_hide(Evas_Object *o)
{
  Elicit_Zoom *z;

  z = evas_object_smart_data_get(o);

  evas_object_hide(z->shot);
  evas_object_hide(z->grid);
}




