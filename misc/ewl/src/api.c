#include "api.h"

void ewl_init(int *argc, char **argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	return;
}

void ewl_main_iteration()
{
	EwlObject *object;
	EwlEvent  *event;
	XEvent     xev;
	/* GET X EVENTS HERE */
	
	while (XPending(ewl_get_display()))	{
		XNextEvent(ewl_get_display(), &xev);
		event = ewl_translate(&xev);
		object = ewl_event_get_object(event);
		if (object) ewl_object_handle_event(object, event);
	}
	XFlush(ewl_get_display());

	while (ewl_events_pending())	{
		event = ewl_next_event();
		object = ewl_event_get_object(event);
		if (object) ewl_object_handle_event(object, event);
	}

	ewl_handle_timers();

	ewl_handle_renders();
	return;
}

void ewl_main()
{
	while (!ewl_done())
		ewl_main_iteration();
	return;
}

void ewl_quit()
{
	EwlObject *state = ewl_get_state();
	char      *flag = malloc(1);
	*flag = TRUE;
	ewl_set(state, "/state/flags/quit", flag);
	return;
}

char       ewl_done()
{
	EwlObject *state = ewl_get_state();
	char      *c = ewl_get(state,"/state/flags/quit");
	return c?*c:FALSE;
}

EwlObject *ewl_get_state()
{
	static EwlObject *internal_state = NULL;
	if (!internal_state)	{
		internal_state = ewl_object_new();
		ewl_load_prefs();	
	}
	return internal_state;
}


EwlList   *ewl_get_render_list()
{
	static EwlList *render_list = NULL;
	if (!render_list)	{
		render_list = ewl_list_new();
	}
	return render_list;
}

void       ewl_add_render(void *object)
{
	ewl_list_insert(ewl_get_render_list(), ewl_list_node_new(object));
	return;
}

void       ewl_remove_render(void *object)
{
	EwlListNode *node = ewl_list_find_by_value(ewl_get_render_list(), object);
	if (node)
		ewl_list_remove(ewl_get_render_list(), node);
	return;
}

void       ewl_load_prefs()
{
	EwlDB *db =NULL;
	char  *temp = NULL,
	      *home = NULL,
	      *prefs_path = NULL,
	      *prefs_name = ".ewl/preferences",
	      *prefs_list = NULL,
	      *font_list = NULL;
	int len;
	home = getenv("HOME");
	if (!home||!strlen(home))	{
		fprintf(stderr,"ERROR: The environment variable $HOME is either not "
		               "set, or set to NULL.\nApplications using EWL cannot "
		               "run correctly without this value.\n");
	} else {
		prefs_path = malloc(strlen(home)+strlen(prefs_name)+16);
		if (!prefs_path)	{
			ewl_debug("ewl_state_init", EWL_NULL_ERROR, "prefs_path");
		} else {
			sprintf(prefs_path, "%s/%s", home, prefs_name);
			db = ewl_db_open(prefs_path);
			if (!db)	{
				ewl_debug("ewl_state_init", EWL_NULL_ERROR, "db");
				fprintf(stderr,"ERROR: Couldn't open \"%s\".\n", prefs_path);
			} else {
				ewl_set_theme(ewl_db_get(db,"theme", &len));
				ewl_set(ewl_get_state(), "/state/default_font",
				         ewl_db_get(db,"default_font", &len));
				/*s->default_font = e_string_dup(temp);*/
					fprintf(stderr,"ewl_state_init(): dfeault_font = %s\n", 
					        temp?temp:"");
				
				temp = ewl_db_get(db,"render/image_cache", &len);
				if (temp)	{
					ewl_set_image_cache(atoi(temp));
				} else {
					fprintf(stderr, 
					        "WARNING: Undefined Image Cache size in %s\n"
					        "         This could mean you have an "
					        "obsolete or corrupt preferences database,\n"
					        "         or that you just forgot to run "
					        "the create preferences script.\n"
				        "         Setting Image Cache to 8388608 bytes.\n",
					        prefs_path);
						ewl_set_image_cache(8388608);
				}

				temp = ewl_db_get(db,"render/font_cache", &len);
				if (temp)	{
					ewl_set_font_cache(atoi(temp));
				} else {
					fprintf(stderr, 
					        "WARNING: Undefined Font Cache size in %s\n"
					        "         This could mean you have an "
					        "obsolete or corrupt preferences database,\n"
					        "         or that you just forgot to run "
					        "the create preferences script.\n"
					        "         Setting Font Cache to 1048576  bytes.\n",
					        prefs_path);
					ewl_set_font_cache(1048576);
				}



				temp = ewl_db_get(db,"render/method", &len);
				if (temp) {
					/*for (i=0; i<RENDER_METHOD_COUNT; i++)	{
						if (!strncasecmp(
						          temp,
						          EwlRenderMethodStringEnum[i], 
						          strlen(EwlRenderMethodStringEnum[i]))) {
							s->render_method = i;
							break;
						} 
					}*/
					ewl_set_render_method(temp);
				} else {
					fprintf(stderr,
				            "WARNING: Undefined Render Method in %s\n"
					        "         This could mean you have an "
					        "obsolete or corrupt preferences database,\n"
					        "         or that you just forgot to run "
					        "the create preferences script.\n"
					        "         Falling back to Software Rendering.\n",
					        prefs_path);
					/*s->render_method = RENDER_METHOD_ALPHA_SOFTWARE;*/
					ewl_set_render_method("alpha_software");
				}

				/*if (s->render_method==-1)	{
					fprintf(stderr,
				            "WARNING: Unknown Render Method in %s\n"
					        "         This could mean you have an obsolete or corrupt preferences database.\n"
					        "         Falling back to Software Rendering.\n",
					        prefs_path);
					s->render_method = RENDER_METHOD_ALPHA_SOFTWARE;
				}*/

				/* get config file path */
				prefs_list = ewl_db_get(db,"path", &len);
				fprintf(stderr,"ewl_state_init(): paths = %s\n",
				        prefs_list);

				for (temp=strtok(prefs_list,":"); temp;
				     temp=strtok(NULL,":"))	{
					ewl_add_path(temp);
						fprintf(stderr,"ewl_state_init(): adding path %s\n",
						temp);
				}

				/* get font path list */
				font_list = ewl_db_get(db,"font_path", &len);
					fprintf(stderr,"ewl_state_init(): font_paths = %s\n",
					        font_list);

				for (temp=strtok(font_list,":"); temp;
				     temp=strtok(NULL,":"))	{
					ewl_add_font_path(temp);
						fprintf(stderr,"ewl_state_init(): "
						        "adding font path %s\n", temp);
				}
				ewl_db_close(db);
			}
			/*free(prefs_path);*/
		}
	}
	return;
}

EwlList   *ewl_get_path_list()
{
	static EwlList *path_list = NULL;
	if (!path_list)	{
		path_list = ewl_list_new();
	}
	return path_list;
}

void       ewl_add_path(char *path)
{
	ewl_list_insert(ewl_get_path_list(),
	                ewl_list_node_new(ewl_string_dup(path)));
	return;
}

char       ewl_remove_path_cb(EwlListNode *node, void *data)
{
	return ewl_string_cmp(node->data, (char*) data, 0);
}


void       ewl_remove_path(char *path)
{
	EwlListNode *node = ewl_list_find(ewl_get_path_list(),
	                                  ewl_remove_path_cb, path);
	if (node)
		ewl_list_remove(ewl_get_path_list(), node);
	return;
}


char      *ewl_get_application_name()
{
	char *name = ewl_string_dup(ewl_get(ewl_get_state(),
	                            "/state/application_name"));
	if (!name)	{
		name = ewl_string_dup("default");
		ewl_set_application_name(name);
	}
	return name;
}

void       ewl_set_application_name(char *name)
{
	ewl_set(ewl_get_state(), "/state/application_name", ewl_string_dup(name));
	return;
}

char      *ewl_get_theme()
{
	char *theme = ewl_string_dup(ewl_get(ewl_get_state(), "/state/theme"));
	if (!theme)	{
		theme = ewl_string_dup("default");
		ewl_set_theme(theme);
	}
	return theme;
}

void       ewl_set_theme(char *theme)
{
	ewl_set(ewl_get_state(), "/state/theme", ewl_string_dup(theme));
	return;
}


int         ewl_get_render_method()
{
	static char *methods[] = {
		"alpha_software",
		"basic_hardware",
		"3d_hardware",
		"alpha_hardware"
	};
	char *rm = ewl_get(ewl_get_state(), "/state/render_method");
	int   i, r = 0;

	if (rm)	{
		for (i=0; i<4; i++)	{
			if (!strncasecmp(rm,methods[i], strlen(methods[i]))) {
				r = i;
				break;
			}
		}
	}
	return r;
}

void        ewl_set_render_method(char *render_method)
{
	ewl_set(ewl_get_state(), "/state/render_method",
	        ewl_string_dup(render_method));
	return;
}


int         ewl_get_image_cache()
{
	int *cache = ewl_get(ewl_get_state(), "/state/image_cache");
	return cache?*cache:EWL_DEFAULT_IMAGE_CACHE_SIZE;
}

void        ewl_set_image_cache(int cache)
{
	int *tc = malloc(sizeof(int));
	*tc = cache;
	ewl_set(ewl_get_state(), "/state/image_cache", tc);
	return;
}

int         ewl_get_font_cache()
{
	int *cache = ewl_get(ewl_get_state(), "/state/font_cache");
	return cache?*cache:EWL_DEFAULT_FONT_CACHE_SIZE;
}

void        ewl_set_font_cache(int cache)
{
	int *tc = malloc(sizeof(int));
	*tc = cache;
	ewl_set(ewl_get_state(), "/state/font_cache", tc);
	return;
}

void        ewl_handle_renders()
{
	EwlIterator *i = NULL;
	for (i=ewl_iterator_start(ewl_get_render_list()); i;
	     i=ewl_iterator_next(i))	{
		evas_render(EWL_WINDOW(i->data)->evas);
		/*fprintf(stderr,"DEBUG: rendering window %p\n", i->data);*/
	}
	return;
}
