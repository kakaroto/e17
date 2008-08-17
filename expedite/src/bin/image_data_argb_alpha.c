#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_data_argb_alpha_start
#define NAME "Image Data ARGB Alpha"
#define ICON "mushroom.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

/* private data */
static Evas_Object *o_images[1];

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   for (i = 0; i < 1; i++)
     {
	o = evas_object_image_add(evas);
	o_images[i] = o;
        evas_object_image_colorspace_set(o, EVAS_COLORSPACE_ARGB8888);
	evas_object_image_size_set(o, 640, 480);
	evas_object_image_alpha_set(o, 1);
	evas_object_image_fill_set(o, 0, 0, 640, 480);
	evas_object_resize(o, 640, 480);
	evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < 1; i++) evas_object_del(o_images[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Coord x, y, w, h;
   for (i = 0; i < 1; i++)
     {
	unsigned int *data, *p;
	int ff, a, r, g, b;
        w = 640;
	h = 480;
	x = (win_w / 2) - (w / 2);
	y = (win_h / 2) - (h / 2);
	evas_object_move(o_images[i], x, y);
	evas_object_resize(o_images[i], w, h);
	evas_object_image_fill_set(o_images[i], 0, 0, w, h);
	data = evas_object_image_data_get(o_images[i], 1);
	p = data;
	ff = (f ^ (f << 8) ^ (f << 16) ^ (f << 24));
	for (y = 0; y < 480; y++)
	  {
	     for (x = 0; x < 640; x++)
	       {
		  r = (x * y / 7) + f;
		  g = (x / 2);
		  b = (y / 2);
		  a = (x + y);
		  r &= 0xff;
		  g &= 0xff;
		  b &= 0xff;
		  a &= 0xff;
		  r = (a * r) / 255;
		  g = (a * g) / 255;
		  b = (a * b) / 255;
		  *p = (a << 24) | (r << 16) | (g << 8) | b;
		  p++;
	       }
	  }
	evas_object_image_data_set(o_images[i], data);
	evas_object_image_data_update_add(o_images[i], 0, 0, 640, 480);
     }
   FPS_STD(NAME);
}

/* prepend special key handlers if interactive (before STD) */
static void _key(char *key)
{
   KEY_STD;
}












/* template stuff - ignore */
# endif
#endif

#ifdef UI
_ui_menu_item_add(ICON, NAME, FNAME);
#endif

#ifdef PROTO
void FNAME(void);
#endif

#ifndef PROTO
# ifndef UI
void FNAME(void)
{
   ui_func_set(_key, _loop);
   _setup();
}
# endif
#endif
#undef FNAME
#undef NAME
#undef ICON
