#include "main.h"

static Evas_Object *o_layout = NULL;

/* create the screen layout - edje defines it with swallow regions and anything
 else the theme wants to do */
int
layout_init(void)
{
   o_layout = edje_object_add(evas);
   evas_object_move(o_layout, 0, 0);
   if (!edje_object_file_set(o_layout, theme, "layout"))
     {
	int err = edje_object_load_error_get(o_layout);
	const char *errmsg = edje_load_error_str(err);
	fprintf(stderr,
		"ERROR: could not load edje \"%s\", group \"layout\": %s\n",
		theme, errmsg);
	return 0;
     }

   layout_resize();
   evas_object_show(o_layout);
   return 1;
}

/* handle a window/screen resize */
void
layout_resize(void)
{
   Evas_Coord w, h;
   
   evas_output_viewport_get(evas, NULL, NULL, &w, &h);
   evas_object_resize(o_layout, w, h);
}

/* swallow an object into a named layout location */
void
layout_swallow(char *layout, Evas_Object *obj)
{
   edje_object_part_swallow(o_layout, layout, obj);
}
