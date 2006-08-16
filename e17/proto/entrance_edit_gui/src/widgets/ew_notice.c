#include <Etk.h>
#include "Entrance_Widgets.h"

static void _ew_cb_destroy(void *);
static void _ew_cb_emit_response(Etk_Object *, void *data);

Entrance_Dialog 
ew_notice_new(const char *title)
{
	Entrance_Dialog ew = ew_dialog_new(title, EW_FALSE);
	if(!ew) 
	{
		return NULL;
	}

	etk_signal_connect("destroyed", ETK_OBJECT(ew->owner), ETK_CALLBACK(_ew_cb_emit_response), NULL);
	return ew;
}

void 
ew_notice_show(Entrance_Dialog ew)
{
	ew_dialog_show(ew);
}

Entrance_Widget
ew_notice_group_add(Entrance_Dialog d, const char *title, int direction)
{
	Entrance_Widget ew = ew_group_new(title, direction);
	if(!ew) 
	{
		return;
	}

	etk_box_append(ETK_BOX(d->box), ew->owner, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	return ew;
}

void 
ew_notice_button_add(Entrance_Dialog ew, const char *name, int response_id, void (*response_event)(void *, int, void *), void *data)
{
	Etk_Widget *btn = etk_dialog_button_add(ETK_DIALOG(ew->owner), name, response_id);
	if(response_event && !ew->extra) {
		etk_signal_connect("response", ETK_OBJECT(ew->owner), ETK_CALLBACK(response_event), data);
		ew->extra = response_event;
	}
}

void 
ew_notice_close_button_add(Entrance_Dialog ew, void (*response_event)(void *, int, void *), void *data)
{
	ew_notice_button_add(ew, _("Close"), EW_NOTICE_CLOSE_BUTTON, response_event, data);
}

void 
ew_notice_apply_button_add(Entrance_Dialog ew, void (*response_event)(void *, int, void *), void *data)
{
	ew_notice_button_add(ew, _("Apply"), EW_NOTICE_APPLY_BUTTON, response_event, data);
}

void 
ew_notice_ok_button_add(Entrance_Dialog ew, void (*response_event)(void *, int, void *), void *data)
{
	ew_notice_button_add(ew, _("Ok"), EW_NOTICE_OK_BUTTON, response_event, data);
}

void
ew_notice_yes_button_add(Entrance_Dialog ew, void (*response_event)(void *, int, void *), void *data) {
	ew_notice_button_add(ew, _("Yes"), EW_NOTICE_YES_BUTTON, response_event, data);
}

void
ew_notice_no_button_add(Entrance_Dialog ew, void (*response_event)(void *, int, void *), void *data) {
	ew_notice_button_add(ew, _("No"), EW_NOTICE_NO_BUTTON, response_event, data);
}

void
ew_notice_cancel_button_add(Entrance_Dialog ew, void (*response_event)(void *, int, void *), void *data) {
	ew_notice_button_add(ew, _("Cancel"), EW_NOTICE_CANCEL_BUTTON, response_event, data);
}

void
ew_notice_destroy(Entrance_Dialog ew)
{
	ew_dialog_destroy(ew);
}

/*privates*/
static void
_ew_cb_emit_response(Etk_Object *win, void *data) {
	etk_signal_emit_by_name("response", ETK_OBJECT(win), NULL, ETK_DIALOG(win), EW_NOTICE_CLOSE_BUTTON, NULL);
}
