#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_crossfade_start
#define NAME "Image Crossfade"
#define ICON "mushroom.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

/* private data */
static Evas_Object *o_images[2];

/* setup */
static void _setup(void)
{
   Evas_Object *o;

   o = evas_object_image_add(evas);
   o_images[0] = o;
   evas_object_image_file_set(o, build_path("im1.png"), NULL);
   evas_object_image_fill_set(o, 0, 0, 720, 420);
   evas_object_resize(o, 720, 420);
   evas_object_show(o);

   o = evas_object_image_add(evas);
   o_images[1] = o;
   evas_object_image_file_set(o, build_path("im2.png"), NULL);
   evas_object_image_fill_set(o, 0, 0, 720, 420);
   evas_object_resize(o, 720, 420);
   evas_object_show(o);

   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < 2; i++) evas_object_del(o_images[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int a;

   a = f & 0x1f;
   a = ((a << 3) | (a >> 2)) & 0xff;
   evas_object_color_set(o_images[1], a, a, a, a);
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
