#include <etk/Etk.h>
#include "Entrance_Widgets.h"

Entrance_Widget
ew_group_new(const char *title)
{
	Entrance_Widget ew = ew_new();
	if(!ew)
	{
		return NULL;
	}

	ew->owner = etk_frame_new(title);

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

