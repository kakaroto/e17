#include "ephoto.h"

void add_slideshow(void)
{
	Evas_Object *o;

	o = ephoto_slide_add(em->e);
	em->slideshow = o;
}

void show_slideshow(Eina_List *node, Eina_List *list)
{
	ephoto_slide_current_node_set(em->slideshow, node);
	ephoto_slide_item_list_set(em->slideshow, list);
	evas_object_show(em->slideshow);
	edje_object_part_swallow(em->bg, "ephoto.swallow.content", em->slideshow);
	ephoto_slide_start(em->slideshow);
}

void hide_slideshow(void)
{
	ephoto_slide_stop(em->slideshow);
	evas_object_hide(em->slideshow);
	edje_object_part_unswallow(em->bg, em->slideshow);
}

