#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_mask_10
#define NAME "Image Mask 10"
#define ICON "blend.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;
/* private data */
static Evas_Object *o_images[1];
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
   
   for (i = 0; i < 1; i++)
     {
	o = evas_object_image_add(evas);
	o_images[i] = o;
	evas_object_image_file_set(o, build_path("texture.png"), NULL);
	evas_object_image_fill_set(o, 0, 0, win_w * 4, win_h * 4);
	evas_object_resize(o, win_w * 4, win_h * 4);
	evas_object_clip_set(o, o_mask);
	evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < 1; i++) evas_object_del(o_images[i]);
   evas_object_del(o_mask);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Map *m;
   Evas_Coord x, y, w, h;
   for (i = 0; i < 1; i++)
     {
	w = win_w * 4;
	h = win_h * 4;
	x = (win_w / 2) - (w / 2);
	y = (win_h / 2) - (h / 2);
        m = evas_map_new(4);
        evas_map_util_points_populate_from_geometry(m, 
                                                    -win_w, -win_h,
                                                    win_w * 4, win_h * 4, 0);
        evas_map_util_rotate(m, f, win_w / 2, win_h / 2);
        evas_object_map_enable_set(o_images[i], 1);
        evas_object_map_set(o_images[i], m);
        evas_map_free(m);
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
