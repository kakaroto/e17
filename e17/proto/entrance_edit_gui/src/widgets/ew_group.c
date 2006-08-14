#include <Etk.h>
#include "Entrance_Widgets.h"

Entrance_Widget
ew_group_new(const char *title, int direction)
{
	Entrance_Widget ew = ew_new();
	if(!ew)
	{
		return NULL;
	}

	if(title)
	  ew->owner = etk_frame_new(title);
	else
	  ew->owner = etk_vbox_new(ETK_FALSE, 0);

	if(direction) /*HORIZONTAL*/
	  ew->box = etk_hbox_new(ETK_FALSE, 0);
	else
	  ew->box = etk_vbox_new(ETK_FALSE, 0);

	etk_container_add(ETK_CONTAINER(ew->owner), ew->box);

	return ew;
}

void
ew_group_add(Entrance_Widget g, void *w)
{
	if(!g || !w) {
		return;
	}

	Entrance_Widget ew = w;
	etk_box_pack_start(ETK_BOX(g->box), ew->owner, ETK_TRUE, ETK_TRUE, 0);
}

