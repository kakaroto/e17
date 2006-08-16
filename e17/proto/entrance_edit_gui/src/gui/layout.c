#include <Entrance_Widgets.h>
#include <stdio.h>

static Entrance_Widget group_display;
static Entrance_Entry display_greeting_before;
static Entrance_Entry display_greeting_after;
static Entrance_Entry display_date_format;
static Entrance_Entry display_time_format;

void
egui_layout_dialog_show() {
	ew_messagebox_ok("notice", "This is a message", EW_MESSAGEBOX_ICON_MESSAGE);
}

