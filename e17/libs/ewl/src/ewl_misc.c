
#include <Ewl.h>
#include "getopt.h"

void ewl_idle_render (void *data);

extern Ewd_List *ewl_window_list;

Ewl_Options ewl_options;

static void ewl_init_parse_options (int argc, char **argv);
static void ewl_parse_option_array (int argc, char **argv);

void
ewl_init (int argc, char **argv)
{
	char *xdisplay = NULL;

	DENTER_FUNCTION;

	ewl_init_parse_options (argc, argv);

	if (ewl_options.xdisplay)
		xdisplay = ewl_options.xdisplay;

	if (!e_display_init (xdisplay))
	  {
		  fprintf (stderr, "ERRR: Cannot connect to X display!\n");
		  exit (-1);
	  }

	e_event_filter_init ();
	e_ev_signal_init ();
	e_ev_x_init ();

	if ((ewl_config_init ()) == -1)
		DERROR ("Couldn't init config data.. Exiting....");

	if ((ewl_ev_init ()) == -1)
		DERROR ("Couldn't init event data.. Exiting....");

	if ((ewl_fx_init ()) == -1)
		DERROR ("Couldn't init fx data.. Exiting....");

	if ((ewl_theme_init ()) == -1)
		DERROR ("Couldn't init theme data.. Exiting....");

	ewl_window_list = ewd_list_new ();
	e_event_filter_idle_handler_add (ewl_idle_render, NULL);

	DLEAVE_FUNCTION;
}

void
ewl_main ()
{
	DENTER_FUNCTION;

	ewl_idle_render (NULL);
	e_event_loop ();

	DLEAVE_FUNCTION;
}

void
ewl_idle_render (void *data)
{
	Ewl_Widget *widget;

	DENTER_FUNCTION;

	if (!ewl_window_list)
	  {
		  DERROR ("FATAL ERROR: EWL has not been initialized\n");
		  exit (1);
	  }

	if (ewd_list_is_empty (ewl_window_list))
		DRETURN;

	ewd_list_goto_first (ewl_window_list);

	while ((widget =
		EWL_WIDGET (ewd_list_next (ewl_window_list))) != NULL)
	  {

		  /*
		   * If we have any unrealized windows at this point, we want to
		   * realize and configure them to layout the children correct.
		   */
		  if (!REALIZED (widget))
		    {
			    ewl_widget_realize (widget);
			    ewl_widget_configure (widget);
		    }

		  if (widget->evas)
			  evas_render (widget->evas);
	  }

	DRETURN;
	data = NULL;
}

void
ewl_main_quit ()
{
	Ewl_Widget *widget;

	DENTER_FUNCTION;

	if (ewl_window_list)
	  {
		  ewd_list_goto_first (ewl_window_list);

		  while ((widget = ewd_list_next (ewl_window_list)) != NULL)
			  ewl_widget_destroy_recursive (widget);
	  }

	exit (-1);
}

static void
ewl_init_parse_options (int argc, char **argv)
{
	DENTER_FUNCTION;

	memset (&ewl_options, 0, sizeof (Ewl_Options));
	ewl_parse_option_array (argc, argv);

	DLEAVE_FUNCTION;
}

static void
ewl_parse_option_array (int argc, char **argv)
{
	char stropts[] =
		"a:A:b:BcC:dD:e:f:Fg:hH:iIklL:mM:nNo:O:pPqQrR:sS:tT:uUvVwW:xXy:zZ1:2:3:4:56:78:90:";

	static struct option lopts[] = {
		{"ewl_debug-level", 1, 0, '@'},
		{"ewl_display", 1, 0, '$'},
		{0, 0, 0, 0}
	};
	int optch = 0, cmdx = 0;

	DENTER_FUNCTION;

	while ((optch =
		getopt_long_only (argc, argv, stropts, lopts, &cmdx)) != EOF)
	  {
		  switch (optch)
		    {
		    case 0:
			    break;

		    case '@':
			    ewl_options.debug_level = atoi (optarg);
			    D (0,
			       ("Setting debug level to %i",
				ewl_options.debug_level));
			    break;

		    case '$':
			    ewl_options.xdisplay = optarg;
			    break;

		    default:
			    break;
		    }
	  }

	DLEAVE_FUNCTION;
}
