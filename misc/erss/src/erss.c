#include "erss.h"
#include "parse_config.h"
#include "net.h"
#include "ls.h"
#include "gui.h"

Ewd_List    *config_files=NULL;


#if 0
static void erss_xml_error_handler (void *ctx, const char *msg, ...)
{
	fprintf (stderr, "%s xml report: %s\n", PACKAGE, msg);

	return;
}
#endif

static int erss_handler_signal_exit (void *data, int ev_type, void *ev)
{
	Ecore_Event_Signal_Exit *e = ev;

	if (e->interrupt)
		printf ("%s exit: interrupt\n", PACKAGE);
	if (e->quit)
		printf ("%s exit: quit\n", PACKAGE);
	if (e->terminate)
		printf ("%s exit: terminate\n", PACKAGE);

	ecore_main_loop_quit ();

	return 1;
}

char *erss_time_format () 
{
	char    *str;
	struct  tm  *ts;
	time_t  curtime;
	
	curtime = time(NULL);
	ts = localtime(&curtime);

	str = malloc (11);

	snprintf (str, 20, "%02d:%02d:%02d", 
			ts->tm_hour, ts->tm_min, ts->tm_sec);

	return str;
}

static void erss_display_default_usage ()
{
	fprintf (stderr, "Usage: %s [OPTION] ...\n", PACKAGE);
	fprintf (stderr, "Try `%s -h` for more information\n", PACKAGE);
	exit (-1);
}

int main (int argc, char * const argv[])
{
	int         c = 0;
	int         got_config_file = FALSE;
	int         got_theme_file = FALSE;
	int         got_rc_file = FALSE;
	char        config_file[PATH_MAX];
	char        theme_file[PATH_MAX];
	char       *config=NULL;
	struct stat statbuf;
	erss_feed   f;

	memset(&f,0,sizeof(erss_feed));

	cfg = NULL;

	/*
	 * Disable this for now..
	 * xmlSetGenericErrorFunc (NULL, erss_xml_error_handler);
	 */
	
	while ((c = getopt (argc, argv, "cvhlt")) != -1) 
	{
		switch (c) {
			case 'l':
				erss_list_config_files (TRUE);
				exit (-1);
			case 'c':
				
				if(optind >= argc) 
					erss_display_default_usage ();
				
				got_config_file = TRUE;
				snprintf (config_file, PATH_MAX, "%s", (char *) argv[optind]);
				break;
			case 't':
				got_theme_file = TRUE;
				snprintf (theme_file, PATH_MAX, "%s", (char *) argv[optind]);
				break;
			case 'v':
				printf ("%s %s\n", PACKAGE, VERSION);
				exit (-1);
			case 'h':
				printf ("Usage: %s [OPTION] ...\n\n", PACKAGE);
	
				printf ("  -c CONFIG \t specify a config file (required)\n");
				printf ("  -l        \t list avaliable config files\n\n");
				printf ("  -t THEME  \t specify an edje theme file (.eet)\n");
				printf ("            \t else the default will be used.\n\n");
				printf ("  -h        \t display this help and exit\n");
				printf ("  -v        \t display %s version\n\n", PACKAGE);
				exit (-1);
			default:
				break;
		}
				
	}

	if (erss_parse_rc_file ()) 
		got_rc_file = TRUE;
		
	if(!got_config_file) {
		
		if (!got_rc_file || !rc->config)
			erss_display_default_usage ();
		else
			erss_parse_config_file (config=rc->config);

	} else
		erss_parse_config_file (config=config_file);
	
	if (!got_theme_file) {
		if (!got_rc_file) 
			cfg->theme = strdup (PACKAGE_DATA_DIR"/default.eet");
		else
			cfg->theme = strdup (rc->theme);
	}
	else
		cfg->theme = strdup (theme_file);

	stat (cfg->theme, &statbuf);
	if (!S_ISREG(statbuf.st_mode)) {
		printf ("%s error: themefile '%s' does not exist!\n", 
				PACKAGE, cfg->theme);
		exit (-1);
	}



	ecore_init ();
	ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT,erss_handler_signal_exit, NULL);
	if (!ecore_con_init ()) return -1;
	ecore_app_args_set (argc, (const char **) argv);
	erss_gui_init(config);

	erss_net_connect(&f);
	erss_net_poll (&f);
	ecore_timer_add (cfg->update_rate, erss_net_poll, &f);

	ecore_main_loop_begin ();

	erss_gui_exit();
	ecore_shutdown ();

	return FALSE;
}
