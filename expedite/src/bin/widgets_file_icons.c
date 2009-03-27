#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME widgets_file_icons_start
#define NAME "Widgets File Icons"
#define ICON "flower.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;
/* private data */
#define NUM 512
#define ICON_SIZE 64
static Evas_Object *o_images[NUM];
static Evas_Object *o_texts[NUM];

static const char *icons[] = 
{
   "bug.png",
   "bulb.png",
   "camera.png",
   "colorbox.png",
     
   "e.png",
   "error.png",
   "flower.png",
   "house.png",
     
   "mushroom.png",
   "pulse.png",
   "typewriter.png",
   "warning.png",
     
   "watch.png"
};

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   for (i = 0; i < NUM; i++)
     {
	o = evas_object_image_add(evas);
	o_images[i] = o;
        evas_object_image_file_set(o, build_path(icons[i % 13]), NULL);
	evas_object_image_fill_set(o, 0, 0, ICON_SIZE, ICON_SIZE);
	evas_object_resize(o, ICON_SIZE, ICON_SIZE);
	evas_object_show(o);
        
        o = evas_object_text_add(evas);
        o_texts[i] = o;
        evas_object_text_font_set(o, "Vera-Bold", 10);
        evas_object_text_text_set(o, icons[i % 13]);
        evas_object_text_style_set(o, EVAS_TEXT_STYLE_FAR_SOFT_SHADOW);
        evas_object_color_set(o, 255, 255, 255, 255);
        evas_object_text_shadow_color_set(o, 0, 0, 0, 24);
        evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < NUM; i++) evas_object_del(o_images[i]);
   for (i = 0; i < NUM; i++) evas_object_del(o_texts[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Coord x, y, w, h, tw, th, cent;
   x = 0;
   y = 0 - f;
   w = ICON_SIZE;
   h = ICON_SIZE;
   for (i = 0; i < NUM; i++)
     {
	evas_object_move(o_images[i], x + 8, y);
        evas_object_geometry_get(o_texts[i], NULL, NULL, &tw, &th);
        cent = (ICON_SIZE + 16 - tw) / 2;
	evas_object_move(o_texts[i], x + cent, y + ICON_SIZE + 4);
        x += ICON_SIZE + 16;
        if (x > win_w)
          {
             x = 0;
             y += ICON_SIZE + 16;
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
