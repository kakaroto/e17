
#include <Ewl.h>
#include "getopt.h"

void ewl_idle_render(void *data);

extern Ewd_List *ewl_window_list;

char *xdisplay = NULL;

static void ewl_init_parse_options(int argc, char **argv);
static void ewl_parse_option_array(int argc, char **argv);

void ewl_reread_config(int val, void *data);


void
ewl_init(int argc, char **argv)
{
	char *xdisplay = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_init_parse_options(argc, argv);

	if (!e_display_init(xdisplay))
	  {
		  fprintf(stderr, "ERRR: Cannot connect to X display!\n");
		  exit(-1);
	  }

	e_event_filter_init();
	e_ev_signal_init();
	e_ev_x_init();

	if ((ewl_config_init()) == -1)
		DERROR("Couldn't init config data.. Exiting....");

	if ((ewl_ev_init()) == -1)
		DERROR("Couldn't init event data.. Exiting....");

	if ((ewl_fx_init()) == -1)
		DERROR("Couldn't init fx data.. Exiting....");

	if ((ewl_theme_init()) == -1)
		DERROR("Couldn't init theme data.. Exiting....");

	ewl_window_list = ewd_list_new();
	e_event_filter_idle_handler_add(ewl_idle_render, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_main(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_reread_config(0, NULL);

	ewl_idle_render(NULL);
	e_event_loop();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_idle_render(void *data)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_window_list)
	  {
		  DERROR("FATAL ERROR: EWL has not been initialized\n");
		  exit(-1);
	  }

	if (ewd_list_is_empty(ewl_window_list))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(ewl_window_list);

	while ((w = EWL_WIDGET(ewd_list_next(ewl_window_list))) != NULL)
	  {

		  /*
		   * If we have any unrealized windows at this point, we want to
		   * realize and configure them to layout the children correct.
		   */
		  if (!REALIZED(w))
		    {
			    ewl_widget_realize(w);
			    ewl_widget_configure(w);
		    }

		  if (w->evas)
			  evas_render(w->evas);
	  }

	DRETURN(DLEVEL_STABLE);
	data = NULL;
}

void
ewl_main_quit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	e_event_loop_quit();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_init_parse_options(int argc, char **argv)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_parse_option_array(argc, argv);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_parse_option_array(int argc, char **argv)
{
	char stropts[] =
		"a:A:b:BcC:dD:e:f:Fg:hH:iIklL:mM:nNo:O:pPqQrR:sS:tT:uUvVwW:xXy:zZ1:2:3:4:56:78:90:";

	static struct option lopts[] = {
		{"ewl_display", 1, 0, '$'},
		{0, 0, 0, 0}
	};
	int optch = 0, cmdx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	while ((optch =
		getopt_long_only(argc, argv, stropts, lopts, &cmdx)) != EOF)
	  {
		  switch (optch)
		    {
		    case 0:
			    break;
		    case '$':
			    xdisplay = optarg;
			    break;
		    default:
			    break;
		    }
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_reread_config(int val, void *data)
{
	ewl_config_reread_and_apply();

	e_add_event_timer("reread_config_timer", 0.5,
			  ewl_reread_config, 0, NULL);
}
