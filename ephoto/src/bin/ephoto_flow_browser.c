#include "ephoto.h"

void add_flow_view(void)
{
	Evas_Object *o;

	o = ephoto_flow_add(em->e);
	em->flow = o;
}

void show_flow_view(Eina_List *node, Eina_List *list)
{
	ephoto_flow_current_node_set(em->flow, node);
	ephoto_flow_item_list_set(em->flow, list);
	evas_object_show(em->flow);
	edje_object_part_swallow(em->bg, "ephoto.swallow.content", em->flow);
}

void hide_flow_view(void)
{
	evas_object_hide(em->flow);
	edje_object_part_unswallow(em->bg, em->flow);
}
