
#include <Ewl.h>

void ewl_idle_render(void * data);

extern Ewd_List * ewl_window_list;

void
ewl_init(int argc, char ** argv)
{
	if (!e_display_init(NULL)) {
		fprintf(stderr, "ERRR: Cannot connect to X display!\n");
		exit(-1);
	}

	e_event_filter_init();
	e_ev_signal_init();
	e_ev_x_init();

	ewl_prefs_init();
	ewl_theme_init();
	ewl_ev_init();

	e_event_filter_idle_handler_add(ewl_idle_render, NULL);
}

void
ewl_main()
{
	e_event_loop();
}

void
ewl_idle_render(void * data)
{
	Ewl_Widget * widget;

	ewd_list_goto_first(ewl_window_list);

	while ((widget = EWL_WIDGET(ewd_list_next(ewl_window_list))) != NULL) {
		if (widget->evas)
			evas_render(widget->evas);
	}

	return;
	data = NULL;
}

void
ewl_main_quit()
{
	ewd_list_goto_first(ewl_window_list);

	exit(-1);
}
