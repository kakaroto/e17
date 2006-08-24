#include <etk/Etk.h>
#include "Entrance_Widgets.h"

int
ew_init(int *argc, char*** argv)
{
	return etk_init(argc, argv);
}

void 
ew_main(void)
{
	etk_main();
}

void 
ew_main_quit(void)
{
	etk_main_quit();
}

int
ew_shutdown(void)
{
	return etk_shutdown();
}

Entrance_Widget 
ew_new(void)
{
	Entrance_Widget ew = calloc(1, sizeof(*ew));
	if(ew) 
	{
		ew->owner = NULL;
		ew->box = NULL;
		ew->label = NULL;
		ew->dialog_hbox = NULL;
		ew->entry_control = NULL;
		ew->list_col = NULL;
		ew->preview_smart = NULL;
		ew->extra = NULL;
	}

	return ew;
}

void
ew_widget_destroy(Entrance_Widget ew) {
	EW_FREE(ew);
}

void
ew_widget_extra_set(Entrance_Widget w, void *extra) {
	w->extra = extra;
}

void *
ew_widget_extra_get(Entrance_Widget w) {
	return w->extra;
}

/* privates */
