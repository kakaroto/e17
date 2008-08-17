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

	ew_title_set(ew, title);
	if(title)
	{
		ew->owner = etk_frame_new(title);
	}
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
	etk_box_append(ETK_BOX(g->box), ew->owner, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
}

