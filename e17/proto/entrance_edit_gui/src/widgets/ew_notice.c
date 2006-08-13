#include <Etk.h>
#include "Entrance_Widgets.h"
#include <string.h>

Entrance_Notice
_ew_notice_new() {
	Entrance_Notice ew = calloc(1, sizeof(*ew));
	if(ew) {
		ew->owner = NULL;
		ew->box = NULL;
	}

	return ew;
}

Entrance_Notice
ew_notice_new(const char *type, const char *title, const char *message,	void (*delete_event)(Etk_Window *window)) {
	Entrance_Notice ew = _ew_notice_new();
	char image[100] = "status/dialog-";
	Etk_Widget *_hbox;

	if(!ew) {
		return NULL;
	}

	strcat(image, type);
	
	ew->owner = etk_dialog_new();
	etk_window_title_set(ETK_WINDOW(ew->owner), title);
	etk_window_wmclass_set(ETK_WINDOW(ew->owner), title, title); /* TODO: should use the wmclass and wmname of the main window/dialog instead */

	etk_container_border_width_set(ETK_CONTAINER(ew->owner), 5);
	etk_dialog_has_separator_set(ETK_DIALOG(ew->owner), ETK_TRUE);
	if(delete_event)
		etk_signal_connect("delete_event", ETK_OBJECT(ew->owner), ETK_CALLBACK(delete_event), NULL);
	
	ew->box = etk_hbox_new(ETK_FALSE, 10);
	
	_hbox = etk_hbox_new(ETK_FALSE, 10);
	etk_box_pack_start(ETK_BOX(_hbox), etk_image_new_from_edje(etk_theme_icon_theme_get(), image), ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_end(ETK_BOX(_hbox), etk_label_new(message), ETK_TRUE, ETK_TRUE, 0);
	etk_dialog_pack_in_main_area(ETK_DIALOG(ew->owner), _hbox, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);

	return ew;
}

int
ew_notice_show(Entrance_Notice ew) {
	etk_widget_show_all(ew->owner);
}

void
ew_notice_destroy(Entrance_Notice ew) {
	etk_object_destroy(ETK_OBJECT(ew->owner));	
}

void
ew_notice_yes_button_add(Entrance_Notice ew, void (*response_event)(Etk_Dialog *, int, void *)) {
	Etk_Widget *btn = etk_dialog_button_add(ETK_DIALOG(ew->owner), _("OK"), ENTRANCE_NOTICE_YES_BUTTON);
	if(response_event)
		etk_signal_connect("response", ETK_OBJECT(ew->owner), ETK_CALLBACK(response_event), NULL);
}

void
ew_notice_no_button_add(Entrance_Notice ew, void (*response_event)(Etk_Dialog *, int, void *)) {
	Etk_Widget *btn = etk_dialog_button_add(ETK_DIALOG(ew->owner), _("OK"), ENTRANCE_NOTICE_NO_BUTTON);
	if(response_event)
		etk_signal_connect("response", ETK_OBJECT(ew->owner), ETK_CALLBACK(response_event), NULL);
}

void
ew_notice_cancel_button_add(Entrance_Notice ew, void (*response_event)(Etk_Dialog *, int, void *)) {
	Etk_Widget *btn = etk_dialog_button_add(ETK_DIALOG(ew->owner), _("OK"), ENTRANCE_NOTICE_CANCEL_BUTTON);
	if(response_event)
		etk_signal_connect("response", ETK_OBJECT(ew->owner), ETK_CALLBACK(response_event), NULL);
}

void
ew_notice_ok_button_add(Entrance_Notice ew, void (*response_event)(Etk_Dialog *, int, void *)) {
	Etk_Widget *btn = etk_dialog_button_add(ETK_DIALOG(ew->owner), _("OK"), ENTRANCE_NOTICE_OK_BUTTON);
	if(response_event)
		etk_signal_connect("response", ETK_OBJECT(ew->owner), ETK_CALLBACK(response_event), NULL);
}

