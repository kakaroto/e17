#include "erss.h"
#include "parse_config.h"     /* rc, erss_parse_config_file(), erss_parse_rc_file() */
#include "parse.h"            /* erss_parse_free() */
#include "net.h"              /* erss_net_connect(), erss_net_poll() */
#include "ls.h"               /* erss_list_config_files() */
#include "gui.h"              /* erss_gui_init(), erss_gui_exit() */



Ecore_List    *config_files=NULL;
char        *theme=NULL;



#if 0
static void erss_xml_error_handler (void *ctx, const char *msg, ...) {
	fprintf (stderr, "%s xml report: %s\n", PACKAGE, msg);

	return;
}
#endif



static int erss_handler_signal_exit (void *data, int ev_type, void *ev) {
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



char *erss_time_format () {
	char    *str;
	struct  tm  *ts;
	time_t  curtime;
	
	curtime = time(NULL);
	ts = localtime(&curtime);

	str = malloc (9*sizeof(char));

	snprintf (str, 9, "%02d:%02d:%02d", 
			ts->tm_hour, ts->tm_min, ts->tm_sec);

	/*free(ts);*/
	return str;
}



static void erss_display_default_usage () {
	fprintf (stderr, "Usage: %s [OPTION] ...\n", PACKAGE);
	fprintf (stderr, "Try `%s -h` for more information\n", PACKAGE);
	exit (-1);
}



static Erss_Feed *erss_feed_dst(Erss_Feed **feed) {
	Erss_Feed *f;

	if((feed != NULL) && ((f=*feed) != NULL)) {
		*feed = NULL;

		if(f->cfg)
			erss_cfg_dst(&f->cfg);

		if(f->main_buffer)
			free(f->main_buffer);

		if(f->last_time)
			free(f->last_time);

		erss_parse_free (f);

		erss_gui_items_drop(&f->list);

		memset(f,0,sizeof(Erss_Feed));
		free(f);
	}
	return NULL;
}



static Erss_Feed *erss_feed_new(char *config_file, char *theme_file) {
	Erss_Feed *f=malloc(sizeof(Erss_Feed));

	if(f == NULL)
		return f;

	memset(f,0,sizeof(Erss_Feed));

	f->cfg = erss_parse_config_file (config_file);
	if(f->cfg == NULL) {
		fprintf (stderr, "%s error: failed parse config file '%s'!\n", PACKAGE, config_file);
		goto fail;
	}

	if(theme_file != NULL) {
		char *t = strdup(theme_file);
		if(t == NULL) {
			fprintf (stderr, "%s error: out of memory!\n", PACKAGE);
			goto fail;
		}
		if(f->cfg->theme != NULL)
			free(f->cfg->theme);
		f->cfg->theme = t;
	}
	if(f->cfg->theme == NULL) {
		fprintf (stderr, "%s error: no theme for '%s'!\n", PACKAGE, config_file);
		goto fail;
	}

	return f;

  fail:
	erss_feed_dst(&f);
	return NULL;
}



static int erss_feed_start(Erss_Feed *f) {
	erss_net_connect(f);
	erss_net_poll(f);
	ecore_timer_add (f->cfg->update_rate, erss_net_poll, f);
	return TRUE;
}






int main (int argc, char * const argv[]) {
	int          c = 0;
	int          got_config_file = FALSE;
	int          got_theme_file = FALSE;
	int          got_rc_file = FALSE;
	char         config_file[PATH_MAX];
	char         theme_file[PATH_MAX];
	char        *config = NULL;
	struct stat  statbuf;
	Erss_Feed   *f = NULL;

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
				printf ("  -t THEME  \t specify an edje theme file (.edj)\n");
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
		


	if (!got_theme_file) {
		if (!got_rc_file) 
			theme = strdup (PACKAGE_DATA_DIR"/default.edj");
		else
			theme = strdup (rc->theme);
	}
	else
		theme = strdup (theme_file);

	stat (theme, &statbuf);
	if (!S_ISREG(statbuf.st_mode)) {
		fprintf (stderr, "%s error: themefile '%s' does not exist!\n", 
				PACKAGE, theme);
		exit (-1);
	}



	if(!got_config_file) {
		if (!got_rc_file || !rc->config)
			erss_display_default_usage ();
		else
			config=rc->config;

	} else
		config=config_file;
	


	ecore_init ();
	ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT,erss_handler_signal_exit, NULL);
	if (!ecore_con_init ())
		return -1;
	ecore_app_args_set (argc, (const char **) argv);

	f=erss_feed_new(config, theme);
	if(f == NULL)
		fprintf (stderr, "%s error: failed to add feed '%s'!\n", PACKAGE, config);
	else {
		erss_gui_init(config, f->cfg);
		erss_feed_start(f);
	}
	ecore_main_loop_begin ();

	erss_gui_exit();
	ecore_shutdown ();

	return FALSE;
}
