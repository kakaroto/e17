#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_mask_4
#define NAME "Image Mask 4"
#define ICON "blend.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;
/* private data */
static Evas_Object *o_images[OBNUM];
static Evas_Object *o_mask;

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   
   o = evas_object_image_add(evas);
   o_mask = o;
   evas_object_image_file_set(o, build_path("e-logo-mask.png"), NULL);
   evas_object_image_fill_set(o, 0, 0, 720, 420);
   evas_object_resize(o, 720, 420);
   evas_object_move(o, (win_w - 720) / 2, (win_h - 420) / 2);
   evas_object_show(o);
   
   for (i = 0; i < OBNUM; i++)
     {
	o = evas_object_image_add(evas);
	o_images[i] = o;
	evas_object_image_file_set(o, build_path("logo.png"), NULL);
	evas_object_image_fill_set(o, 0, 0, 120, 160);
	evas_object_resize(o, 120, 160);
	evas_object_clip_set(o, o_mask);
	evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < OBNUM; i++) evas_object_del(o_images[i]);
   evas_object_del(o_mask);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Coord x, y, w, h;
   for (i = 0; i < OBNUM; i++)
     {
	w = 120;
	h = 160;
	x = (win_w / 2) - (w / 2);
	x += sin((double)(f + (i * 13)) / (36.7 * SLOW)) * (w / 2);
	y = (win_h / 2) - (h / 2);
	y += cos((double)(f + (i * 28)) / (43.8 * SLOW)) * (h / 2);
	evas_object_move(o_images[i], x, y);
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
