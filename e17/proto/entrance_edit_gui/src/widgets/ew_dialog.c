#include <etk/Etk.h>
#include "Entrance_Widgets.h"

static void _ew_cb_destroy(void *);

Entrance_Dialog 
_ew_dialog_new()
{
	Entrance_Dialog ed = calloc(1, sizeof(_Entrance_Dialog));
	if(ed)
	{
		ed->owner = NULL;
		ed->box = NULL;
		ed->hbox = NULL;
	}

	return ed;
}

Entrance_Dialog 
ew_dialog_new(const char *title, int toplevel)
{
	Entrance_Dialog ew = _ew_dialog_new();
	if(!ew) 
	{
		return NULL;
	}

	ew->owner = etk_dialog_new();
	etk_window_title_set(ETK_WINDOW(ew->owner), title);
	/*FIXME: currently using title verbatim in window class. right or wrong?*/
	etk_window_wmclass_set(ETK_WINDOW(ew->owner), title, title);

	ew->box = etk_vbox_new(0, 10);   
	etk_container_border_width_set(ETK_CONTAINER(ew->owner), 5); /*its hardcoded and bad. i know, but just to keep things uniform*/

	ew->hbox = etk_hbox_new(0,10);

	if(toplevel)
		etk_signal_connect("destroyed", ETK_OBJECT(ew->owner), ETK_CALLBACK(_ew_cb_destroy), NULL);
	return ew;
}

void 
ew_dialog_show(Entrance_Dialog ew)
{
	etk_box_pack_start(ETK_BOX(ew->box), ew->hbox, ETK_TRUE, ETK_TRUE, 0);
	etk_dialog_pack_in_main_area(ETK_DIALOG(ew->owner), ew->box, ETK_TRUE, ETK_TRUE, 
			0, ETK_FALSE);
	etk_widget_show_all(ew->owner);
}

/*void 
ew_dialog_add(Entrance_Dialog d, Entrance_Widget ew)
{
	if(d && ew)
		etk_box_pack_start(ETK_BOX(d->box), ew->box, ETK_TRUE, ETK_TRUE, 0);
}*/

Entrance_Widget
ew_dialog_group_add(Entrance_Dialog d, const char *title)
{
	Entrance_Widget ew = ew_group_new(title);
	if(!ew) 
	{
		return;
	}

	etk_box_pack_start(ETK_BOX(d->box), ew->owner, ETK_TRUE, ETK_TRUE, 0);

	return ew;
}

void 
ew_dialog_close_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *data)
{
	Etk_Widget *button = etk_button_new_with_label(_("Close"));
	etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(func), data);
	etk_box_pack_start(ETK_BOX(ew->hbox), button, ETK_TRUE, ETK_TRUE, 0);
}

void 
ew_dialog_apply_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *data)
{
	Etk_Widget *button = etk_button_new_with_label(_("Apply"));
	etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(func), data);
	etk_box_pack_start(ETK_BOX(ew->hbox), button, ETK_TRUE, ETK_TRUE, 0);
}

void 
ew_dialog_ok_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *data)
{
	Etk_Widget *button = etk_button_new_with_label(_("Ok"));
	etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(func), data);
	etk_box_pack_start(ETK_BOX(ew->hbox), button, ETK_TRUE, ETK_TRUE, 0);
}

void
ew_dialog_destroy(Entrance_Dialog ew)
{
	etk_object_destroy(ETK_OBJECT(ew->owner));
}

/*privates*/
static void
_ew_cb_destroy(void *data)
{
	ew_main_quit();
}
