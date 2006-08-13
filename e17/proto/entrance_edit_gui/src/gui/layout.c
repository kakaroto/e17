#include <Entrance_Widgets.h>
#include <stdio.h>

void
egui_layout_dialog_show() {
	Entrance_Notice en = ew_notice_new(ENTRANCE_NOTICE_MESSAGE_DIALOG, "notice", "This is a message", NULL);
	if(en) {
	ew_notice_ok_button_add(en, NULL);
	ew_notice_show(en);
	}
}

