#include <Etk.h>
#include <Entrance_Widgets.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

static void _ew_messagebox_cb_ok(void *, int, void *);

Entrance_Dialog
_ew_messagebox_new(const char *title, const char *message, const char *icon) 
{
	Entrance_Dialog ew = ew_notice_new(title);
	Entrance_Widget group;
	Entrance_Widget _image;
	Entrance_Widget _text;


	if(!ew) 
	{
		return NULL;
	}

	group = ew_notice_group_add(ew, NULL, EW_GROUP_HORIZONTAL);

	if(icon) 
	{
		_image = ew_image_new(ICON_SIZE,ICON_SIZE);
		char image[100] = "icons/dialog/";
		strcat(image, icon);
		ew_image_edje_load(_image, PACKAGE_DATA_DIR "/icons/icons.edj", image);
		ew_group_add(group, _image);
	}

	if(message)
	{
		_text = ew_label_new(message);
		if(_text)
			ew_group_add(group, _text);
	}

	return ew;
}

Entrance_Dialog
ew_messagebox_ok(const char *title, const char *message, const char *icon) 
{
	Entrance_Dialog ew = _ew_messagebox_new(title, message, icon);
	if(!ew)
		return NULL;

	ew_notice_ok_button_add(ew, _ew_messagebox_cb_ok, ew);
	ew_notice_show(ew);

	return ew;
}

Entrance_Dialog
ew_messagebox_ok_cancel(const char *title, const char *message, const char *icon, void (*funct)(void *, int, void *)) {
	Entrance_Dialog ew = _ew_messagebox_new(title, message, icon);
	if(!ew)
		return NULL;
	ew_notice_ok_button_add(ew, funct, NULL);
	ew_notice_cancel_button_add(ew, NULL, NULL);

	ew_notice_show(ew);
	return ew;
}

Entrance_Dialog
ew_messagebox_yes_no(const char *title, const char *message, const char *icon, void (*funct)(void *, int, void *)) {
	Entrance_Dialog ew = _ew_messagebox_new(title, message, icon);
	if(!ew)
		return NULL;
	ew_notice_yes_button_add(ew, funct, NULL);
	ew_notice_no_button_add(ew, NULL, NULL);

	ew_notice_show(ew);
	return ew;
}

Entrance_Dialog
ew_messagebox_yes_no_cancel(const char *title, const char *message, const char *icon, void (*funct)(void *, int, void *)) {
	Entrance_Dialog ew = _ew_messagebox_new(title, message, icon);
	if(!ew)
		return NULL;
	ew_notice_yes_button_add(ew, funct, NULL);
	ew_notice_no_button_add(ew, NULL, NULL);
	ew_notice_cancel_button_add(ew, NULL, NULL);

	ew_notice_show(ew);
	return ew;
}

static void
_ew_messagebox_cb_ok(void *win, int response_type, void *data) {
	Entrance_Dialog ew = data;
	ew_dialog_destroy(ew);
}
