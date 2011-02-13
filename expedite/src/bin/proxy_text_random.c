#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME proxy_text_random
#define NAME "Proxy Text Random"
#define ICON "text.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

/* private data */
static Evas_Object *o_texts[OBNUM];

/* setup */
static void _setup(void)
{
   int i = 0;
   Evas_Object *o;
   Evas_Coord x, y, w, h;
   char buf[1024];
   const char *strs[] = {
      "Big", "Smelly", "Fish", "Pants", "Octopus", "Garden", "There", "I",
	"Am", "You", "Are", "Erogenous", "We", "Stick", "Wet", "Fishy",
	"Fiddly", "Family", "Lair", "Monkeys", "Magazine"
   };
   srnd();
   o = evas_object_text_add(evas);
   o_texts[0] = o;
   evas_object_text_font_set(o, "Vera-Bold", 20);
   snprintf(buf, sizeof(buf), "%s %s %s %s.",
		   strs[rnd() % (sizeof(strs) / sizeof(char *))],
		   strs[rnd() % (sizeof(strs) / sizeof(char *))],
		   strs[rnd() % (sizeof(strs) / sizeof(char *))],
		   strs[rnd() % (sizeof(strs) / sizeof(char *))]);
   evas_object_text_text_set(o, buf);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_geometry_get(o, NULL, NULL, &w, &h);
   x = (win_w / 2) - (w / 2);
   x += sin((double)((i * 13)) / (36.7 * SLOW)) * (w / 2);
   y = (win_h / 2) - (h / 2);
   y += cos((double)((i * 28)) / (43.8 * SLOW)) * (w / 2);
   evas_object_move(o, x, y);
   evas_object_show(o);
   for (i = 1  ; i < OBNUM ; i ++)
     {
	o = evas_object_image_add(evas);
	o_texts[i] = o;
	evas_object_image_source_set(o, o_texts[0]);
	evas_object_geometry_get(o_texts[0], NULL, NULL, &w, &h);
	evas_object_resize(o, w, h);
	evas_object_image_fill_set(o, 0, 0, w, h);
	x = (win_w / 2) - (w / 2);
	x += sin((double)((i * 13)) / (36.7 * SLOW)) * (w / 2);
	y = (win_h / 2) - (h / 2);
	y += cos((double)((i * 28)) / (43.8 * SLOW)) * (w / 2);
	evas_object_move(o, x, y);

	evas_object_show(o);
     }


   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < OBNUM; i++) evas_object_del(o_texts[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i,w,h;
   char buf[1024];
   const char *strs[] = {
      "Big", "Smelly", "Fish", "Pants", "Octopus", "Garden", "There", "I",
	"Am", "You", "Are", "Erogenous", "We", "Stick", "Wet", "Fishy",
	"Fiddly", "Family", "Lair", "Monkeys", "Magazine"
   };
   snprintf(buf, sizeof(buf), "%s %s %s %s.",
		   strs[rnd() % (sizeof(strs) / sizeof(char *))],
		   strs[rnd() % (sizeof(strs) / sizeof(char *))],
		   strs[rnd() % (sizeof(strs) / sizeof(char *))],
		   strs[rnd() % (sizeof(strs) / sizeof(char *))]);
   evas_object_text_text_set(o_texts[0], buf);
   evas_object_geometry_get(o_texts[0], NULL, NULL, &w, &h);
   for (i = 1; i < OBNUM; i++)
     {
        evas_object_resize(o_texts[i],w,h);
        evas_object_image_fill_set(o_texts[i],0,0,w,h);
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
