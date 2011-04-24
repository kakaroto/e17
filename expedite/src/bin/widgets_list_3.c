#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME widgets_list_3_start
#define NAME "Widgets List 3"
#define ICON "widgets.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;
/* private data */
#define NUM 512
#define ICON_SIZE 64
static Evas_Object *o_images[NUM];
static Evas_Object *o_icons[NUM];
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

static const char *labels[] = 
{
   "Andrew",
   "Alex",
   "Amanda",
   "Arthur",
   "Astrid",
   "Avery",
   
   "Beethoven",
   "Billy",
   "Bob",
   "Bundy",
   
   "Candy",   
   "Carsten",
   
   "Danny",
   "Dennis",
   "Dirk",
   "Doug",
   
   "Edmond",
   "Erik",
   
   "Fernando",
   "Frank",
   "Frederick",
   
   "Gabby",
   "George",
   "Gilroy",
   "Goodrich",
   "Gumby",
};

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   for (i = 0; i < NUM; i++)
     {
	o = evas_object_image_filled_add(evas);
	o_images[i] = o;
        evas_object_image_border_set(o, 2, 2, 2, 2);
        evas_object_image_file_set(o, build_path("pan.png"), NULL);
	evas_object_resize(o, win_w, ICON_SIZE);
	evas_object_show(o);
        
	o = evas_object_image_filled_add(evas);
	o_icons[i] = o;
        evas_object_image_border_set(o, 2, 2, 2, 2);
        evas_object_image_file_set(o, build_path(icons[i % 13]), NULL);
	evas_object_resize(o, ICON_SIZE - 8, ICON_SIZE - 8);
	evas_object_show(o);
        
        o = evas_object_text_add(evas);
        o_texts[i] = o;
        evas_object_text_font_set(o, "Vera-Bold", 10);
        evas_object_text_text_set(o, labels[i % 26]);
        evas_object_color_set(o, 0, 0, 0, 255);
        evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < NUM; i++) evas_object_del(o_images[i]);
   for (i = 0; i < NUM; i++) evas_object_del(o_icons[i]);
   for (i = 0; i < NUM; i++) evas_object_del(o_texts[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Coord x, y, tw, th, cent;
   x = 0;
   y = 0 - f;
   for (i = 0; i < NUM; i++)
     {
	evas_object_move(o_images[i], x, y);
	evas_object_move(o_icons[i], x + 4, y + 4);
        evas_object_geometry_get(o_texts[i], NULL, NULL, &tw, &th);
        cent = (ICON_SIZE - th) / 2;
	evas_object_move(o_texts[i], x + 8 + ICON_SIZE + 8, y + cent);
        y += ICON_SIZE;
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
