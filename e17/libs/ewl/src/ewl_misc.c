
#include <Ewl.h>

void ewl_idle_render(void * data);

extern Ewd_List * ewl_window_list;

Ewl_Options ewl_options;

static void ewl_init_parse_options(int argc, char **argv);
static void ewl_parse_option_array(int argc, char **argv);

void
ewl_init(int argc, char ** argv)
{
	ewl_init_parse_options(argc, argv);

	if (!e_display_init(NULL))
	  {
		fprintf(stderr, "ERRR: Cannot connect to X display!\n");
		exit(-1);
	  }

	e_event_filter_init();
	e_ev_signal_init();
	e_ev_x_init();

	ewl_prefs_init();
	ewl_theme_init();
	ewl_ev_init();
	ewl_fx_init();

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

	while ((widget = EWL_WIDGET(ewd_list_next(ewl_window_list))) != NULL)
	  {
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

static void
ewl_init_parse_options(int argc, char **argv)
{
	memset(&ewl_options, 0, sizeof(Ewl_Options));
	ewl_parse_option_array(argc, argv);
}

static void
ewl_parse_option_array(int argc, char ** argv)
{
	char stropts[] =
		"a:A:b:BcC:dD:e:f:Fg:hH:iIklL:mM:nNo:O:pPqQrR:sS:tT:uUvVwW:xXy:zZ1:2:3:4:56:78:90:";

	struct option lopts[] = {
		{"ewl_debug-level", 1, 0, '@'},
		{0, 0, 0, 0}
	};
	int optch = 0, cmdx = 0;

	while ((optch = getopt_long_only(argc, argv, stropts, lopts, &cmdx)) != EOF)
	  {
		switch (optch)
		  {
			case 0:
			  break;

			case '@':
			  ewl_options.debug_level = 0;
			  break;

			default:
			  break;
		  }
	  }
}
