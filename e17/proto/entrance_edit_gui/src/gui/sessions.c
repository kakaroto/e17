#include <Entrance_Widgets.h>
#include <stdio.h>

void
egui_sessions_dialog_show() {
	Entrance_Widget fileselector = ew_fileselector_new("test fileselector", NULL, EW_TRUE, EW_TRUE, NULL, NULL);
	ew_dialog_show(fileselector);
}

