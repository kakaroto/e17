#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_data_ycbcr601pl_wide_stride_start
#define NAME "Image Data YCbCr 601 Pointer List Wide Stride"
#define ICON "mushroom.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

/* private data */
static Evas_Object *o_images[1];
static unsigned char *yp = NULL, *up = NULL, *vp = NULL;

/* setup */
static void _setup(void)
{
   int i, y;
   FILE *f;
   unsigned char **data, **lp;
   Evas_Object *o;
   for (i = 0; i < 1; i++)
     {
	o = evas_object_image_add(evas);
	o_images[i] = o;
	evas_object_image_colorspace_set(o, EVAS_COLORSPACE_YCBCR422P601_PL);
	evas_object_image_size_set(o, 320, 480);
	evas_object_image_alpha_set(o, 0);
	evas_object_image_fill_set(o, 0, 0, 640, 480);
	evas_object_resize(o, 640, 480);
	evas_object_show(o);
	yp = malloc(640 * 480);
	up = malloc(320 * 240);
	vp = malloc(320 * 240);
	f = fopen(build_path("tp.yuv"), "rb");
	if (f)
	  {
	     fread(yp, 640 * 480, 1, f);
	     fread(up, 320 * 240, 1, f);
	     fread(vp, 320 * 240, 1, f);
	     fclose(f);
	  }
	data = evas_object_image_data_get(o_images[i], 1);
	if (data)
	  {
	     lp = data;
	     for (y = 0; y < 480; y++)
	       {
		  *lp = yp + (y * 640);
		  lp++;
	       }
	     for (y = 0; y < 240; y++)
	       {
		  *lp = up + (y * 320);
		  lp++;
	       }
	     for (y = 0; y < 240; y++)
	       {
		  *lp = vp + (y * 320);
		  lp++;
	       }
	     evas_object_image_data_set(o_images[i], data);
	     evas_object_image_data_update_add(o_images[i], 0, 0, 320, 480);
	  }
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < 1; i++) evas_object_del(o_images[i]);
   free(yp);
   free(up);
   free(vp);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Coord x, y, w, h;
   for (i = 0; i < 1; i++)
     {
	unsigned char **data, **lp;

        w = 640;
	h = 480;
	x = (win_w / 2) - (w / 2);
	y = (win_h / 2) - (h / 2);
	evas_object_move(o_images[i], x, y);
	evas_object_resize(o_images[i], w, h);
	evas_object_image_fill_set(o_images[i], 0, 0, w, h);

	w = 320 - 16 + f;
	if (w < 1) w = 1;
	else if (w > 640) w = 640;

	w &= ~0x1;

	evas_object_image_size_set(o_images[i], w, 480);
	data = evas_object_image_data_get(o_images[i], 1);
	if (data)
	  {
	     lp = data;
	     for (y = 0; y < 480; y++)
	       {
		  *lp = yp + (y * 640);
		  lp++;
	       }
	     for (y = 0; y < 240; y++)
	       {
		  *lp = up + (y * 320);
		  lp++;
	       }
	     for (y = 0; y < 240; y++)
	       {
		  *lp = vp + (y * 320);
		  lp++;
	       }
	     evas_object_image_data_set(o_images[i], data);
	     evas_object_image_data_update_add(o_images[i], 0, 0, w, 480);
	  }
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
