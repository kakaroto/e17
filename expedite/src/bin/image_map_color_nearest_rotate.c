#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_map_color_nearest_rotate_start
#define NAME "Image Map Color Nearest Rotate"
#define ICON "mushroom.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

/* private data */
static Evas_Object *o_images[OBNUM];

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   for (i = 0; i < (OBNUM / 2); i++)
     {
	o = evas_object_image_add(evas);
	o_images[i] = o;
        evas_object_image_file_set(o, build_path("logo.png"), NULL);
	evas_object_image_fill_set(o, 0, 0, 120, 160);
	evas_object_resize(o, 120, 160);
        evas_object_image_smooth_scale_set(o, 0);
	evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < (OBNUM / 2); i++) evas_object_del(o_images[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Map *m;
   Evas_Coord x, y, w, h;
   m = evas_map_new(4);
   evas_map_smooth_set(m, 0);
   for (i = 0; i < (OBNUM / 2); i++)
     {
        w = 120;
	h = 160;
	x = (win_w / 2) - (w / 2);
        x += sin((double)(f + (i * 13)) / (36.7 * SLOW)) * (w / 2);
	y = (win_h / 2) - (h / 2);
        y += cos((double)(f + (i * 28)) / (43.8 * SLOW)) * (h / 2);

	evas_map_util_points_populate_from_geometry(m, x, y, w, h, 0);

        evas_map_point_color_set(m, 0, 255, 255, 255, 255);
        evas_map_point_color_set(m, 1, 255, 0, 0, 255);
        evas_map_point_color_set(m, 2, 0, 0, 255, 255);
        evas_map_point_color_set(m, 3, 0, 255, 0, 255);

        evas_map_util_rotate(m, f, x + (w / 2), y + (h / 2));

        evas_object_map_enable_set(o_images[i], 1);
        evas_object_map_set(o_images[i], m);
     }
   evas_map_free(m);
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
