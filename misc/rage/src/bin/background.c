#include "main.h"

static Evas_Object *o_background = NULL;

void
background_init(void)
{
   o_background = edje_object_add(evas);
   edje_object_file_set(o_background, theme, "background");
   layout_swallow("background", o_background);
   evas_object_show(o_background);
}

void
background_show(void)
{
   evas_object_show(o_background);
}

void
background_hide(void)
{
   evas_object_hide(o_background);
}
