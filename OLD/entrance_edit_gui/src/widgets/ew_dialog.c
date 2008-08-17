#include <Etk.h>
#include "Entrance_Widgets.h"

static void _ew_cb_destroy(void *);
static void _ew_dialog_add_bottom(Entrance_Dialog d, Entrance_Widget ew);

Entrance_Dialog 
_ew_dialog_new()
{
	Entrance_Dialog ed = calloc(1, sizeof(*ed));
	if(ed)
	{
		ed->owner = NULL;
		ed->box = NULL;
		ed->hbox = NULL;
		ed->extra = NULL;
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

	if(toplevel)
		etk_signal_connect("destroyed", ETK_OBJECT(ew->owner), ETK_CALLBACK(_ew_cb_destroy), NULL);
	return ew;
}

void
ew_dialog_onclose_set(Entrance_Dialog ew, void (*func)(void*, void*), void *data)
{
	etk_signal_connect("destroyed", ETK_OBJECT(ew->owner), ETK_CALLBACK(func), data);
}

void 
ew_dialog_show(Entrance_Dialog ew)
{
	etk_dialog_pack_in_main_area(ETK_DIALOG(ew->owner), ew->box, ETK_TRUE, ETK_TRUE, 
			0);
	etk_widget_show_all(ew->owner);
}



Entrance_Widget
ew_dialog_group_add(Entrance_Dialog d, const char *title, int direction)
{
	Entrance_Widget ew = ew_group_new(title, direction);
	if(!ew) 
	{
		return;
	}

	etk_box_append(ETK_BOX(d->box), ew->owner, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	return ew;
}

/*void 
ew_dialog_add(Entrance_Dialog d, Entrance_Widget ew)
{
	if(d && ew)
		etk_box_append(ETK_BOX(d->box), ew->box, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
}*/


void
ew_dialog_button_add(Entrance_Dialog ew, const char *title, void (*func)(void*, void*), void *data)
{
	/*TODO: poke all buttons into an ecore_list*/
	Entrance_Widget button = ew_button_new(title, func, data);
	_ew_dialog_add_bottom(ew, button);
}


void 
ew_dialog_close_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *data)
{
	ew_dialog_button_add(ew, _("Close"), func, data);
}

void 
ew_dialog_apply_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *data)
{
	ew_dialog_button_add(ew, _("Apply"), func, data);
}

void 
ew_dialog_ok_button_add(Entrance_Dialog ew, void (*func)(void *, void*), void *data)
{
	ew_dialog_button_add(ew, _("Ok"), func, data);
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

static void
_ew_dialog_add_bottom(Entrance_Dialog d, Entrance_Widget ew)
{
	if(d && ew)
		etk_dialog_pack_widget_in_action_area(ETK_DIALOG(d->owner), ETK_WIDGET(ew->owner), ETK_TRUE, ETK_TRUE, 10);
}

