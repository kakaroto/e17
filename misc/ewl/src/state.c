#include "state.h"

EwlState *ewl_state_new()
{
	EwlState *s = NULL;
	FUNC_BGN("ewl_state_new");

	s = malloc(sizeof(EwlState));
	if (!s)	{
		ewl_debug("ewl_state_new",EWL_NULL_ERROR,"s");
	}
	FUNC_END("ewl_state_new");
	return s;
}

void       ewl_state_init(EwlState *s)
{
	EwlDB *db = NULL;
	char  *temp = NULL,
	      *home = NULL,
	      *prefs_path = NULL,
	      *prefs_name = ".ewl/preferences",
	      *prefs_list = NULL;
	int    i = 0,
	       len = 0;
	FUNC_BGN("ewl_state_init");
	if (!s)	{
		ewl_debug("ewl_state_init",EWL_NULL_ERROR,"s");
		FUNC_END("ewl_state_init");
		return;
	}
	
	/* sanitize data */
	s->application_name = NULL;
	s->theme_name = NULL;
	s->path_list = NULL;

	s->debug = _Ewl_Debug;
	/*s->static_registry = _Ewl_Static_Registry;*/
	/*s->num_static_registry_entries = ewl_registry_get_num_widgets();*/
	s->dynamic_registry = NULL;

	s->focused = NULL;
	s->grabbed = NULL;
	s->widget_list = NULL;

	s->timer_queue = NULL;

	s->event_queue = NULL;
	s->quit_flag =   FALSE;

	s->render_dithered = 0;
	s->render_antialiased = 0;

	s->render_method = -1; /* for sanity checks on preferences db later */
	ewl_state_translators_init(s);


	/* load user defaults */
	home = getenv("HOME");
	if (!home||!strlen(home))	{
		ewl_debug("ewl_state_init", EWL_NULL_ERROR, "home");
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
				s->theme_name = ewl_db_get(db,"theme", &len);
				if (ewl_debug_is_active())
					fprintf(stderr,"ewl_state_init(): theme_name = %s\n", 
				        s->theme_name?s->theme_name:"untitled");

				temp = ewl_db_get(db,"render/antialiased", &len);
				s->render_antialiased = e_check_bool_string(temp);
				if (ewl_debug_is_active())
					fprintf(stderr,"ewl_state_init(): antialiased = %s\n", 
					        s->render_antialiased?"true":"false");

				temp = ewl_db_get(db,"render/dithered", &len);
				s->render_dithered = e_check_bool_string(temp);
				if (ewl_debug_is_active())
					fprintf(stderr,"ewl_state_init(): dithered = %s\n", 
					        s->render_dithered?"true":"false");

				temp = ewl_db_get(db,"render/method", &len);
				if (temp) {
					for (i=0; i<RENDER_METHOD_COUNT; i++)	{
						if (!strncasecmp(
						          temp,
						          EwlRenderMethodStringEnum[i], 
						          strlen(EwlRenderMethodStringEnum[i]))) {
							s->render_method = i;
							break;
						} 
					}
				} else {
					fprintf(stderr,
				            "WARNING: Undefined Render Method in %s\n"
					        "         This could mean you have an obsolete or corrupt preferences database.\n"
					        "         Falling back to Software Rendering.\n",
					        prefs_path);
					s->render_method = RENDER_METHOD_ALPHA_SOFTWARE;
				}

				if (s->render_method==-1)	{
					fprintf(stderr,
				            "WARNING: Unknown Render Method in %s\n"
					        "         This could mean you have an obsolete or corrupt preferences database.\n"
					        "         Falling back to Software Rendering.\n",
					        prefs_path);
					s->render_method = RENDER_METHOD_ALPHA_SOFTWARE;
				}

				prefs_list = ewl_db_get(db,"path", &len);
				if (ewl_debug_is_active())
					fprintf(stderr,"ewl_state_init(): paths = %s\n",
					        prefs_list);

				for (temp=strtok(prefs_list,":"); temp;
				     temp=strtok(NULL,":"))	{
					s->path_list = ewl_ll_insert_with_data(s->path_list,
					                                       (EwlData*)
					                                       e_string_dup(temp));
					if (ewl_debug_is_active())
						fprintf(stderr,"ewl_state_init(): adding path %s\n",
						temp);
				}
				ewl_db_close(db);
			}
			/*free(prefs_path);*/
		} 
	}
	FUNC_END("ewl_state_init");
	return;
}

EwlState *ewl_state_dup(EwlState *s)
{
	EwlState *ts = NULL;
	FUNC_BGN("ewl_state_dup");
	if (!s)	{
		ewl_debug("ewl_state_dup",EWL_NULL_ERROR,"s");
		FUNC_END("ewl_state_dup");
		return NULL;
	}

	ts = ewl_state_new();
	if (!ts)	{
		ewl_debug("ewl_state_dup",EWL_NULL_ERROR,"ts");
	} else {
		fprintf(stderr,"THIS FUNCTION IS HORRIBLY BROKEN -- FOR THE LOVE OF GOD DON'T USE IT!!\n");
		ts->debug = s->debug;
		/*ts->static_registry = s->static_registry;*/
		/*ts->num_static_registry_entries = s->num_static_registry_entries;*/
		ts->dynamic_registry = s->dynamic_registry;
		ts->widget_list = s->widget_list;
		ts->event_queue = s->event_queue;
	}

	FUNC_END("ewl_state_dup");
	return ts;
}

void       ewl_state_free(EwlState *s)
{
	FUNC_BGN("ewl_state_free");
	if (!s)	{
		ewl_debug("ewl_state_free",EWL_NULL_ERROR,"s");
		FUNC_END("ewl_state_free");
		return;
	}
	free(s);

	FUNC_END("ewl_state_free");
	return;
}


EwlState *ewl_state_get()
{
	EwlState *state = NULL;
	FUNC_BGN("ewl_state_get");
	state = _Ewl_Internal_State;
	if (!state)	{
		ewl_debug("ewl_state_get", EWL_NULL_ERROR, "state");
	}
	FUNC_END("ewl_state_get");
	return state;
}

void       ewl_state_set(EwlState *s)
{
	FUNC_BGN("ewl_state_set");
	if (!s)	{
		ewl_debug("ewl_state_set",EWL_NULL_ERROR,"s");
	} else {
		/* THIS ISN'T THREAD SAfE UNLESS IT BLOCKS SOMEWHERE IN
		   HERE.................... EITHER THAT OR WE NEED TO 
		   PASS A STATE STRUCT TO  PRETTY MUCH _EVERTYHIGN_ :/ */
		_Ewl_Internal_State = s;
	}

	FUNC_END("ewl_state_set");
	return;
}


void       ewl_set_application_name(char *name)
{
	FUNC_BGN("ewl_set_application_name");
	ewl_state_set_application_name(name);
	FUNC_END("ewl_set_application_name");
	return;
}

char      *ewl_get_application_name()
{
	char *temp = NULL;
	FUNC_BGN("ewl_get_application_name");
	temp = ewl_state_get_application_name();
	FUNC_END("ewl_get_application_name");
	return temp;
}

void       ewl_set_theme(char *name)
{
	FUNC_BGN("ewl_set_theme");
	ewl_state_set_theme(name);
	FUNC_END("ewl_set_theme");
	return;
}

char      *ewl_get_theme()
{
	char *temp = NULL;
	FUNC_BGN("ewl_get_theme");
	temp = ewl_state_get_theme();
	FUNC_END("ewl_get_theme");
	return temp;
}


void       ewl_insert_path(char *path)
{
	FUNC_BGN("ewl_insert_path");
	ewl_state_insert_path(path);
	FUNC_END("ewl_insert_path");
	return;
}

void       ewl_remove_path(char *path)
{
	FUNC_BGN("ewl_remove_path");
	ewl_state_remove_path(path);
	FUNC_END("ewl_remove_path");
	return;
}

EwlLL     *ewl_get_path_list()
{
	EwlLL *l = NULL;
	FUNC_BGN("ewl_get_path_list");
	l = ewl_state_get_path_list();
	FUNC_END("ewl_get_path_list");
	return l;
}


char     **ewl_get_path_strings(int *length)
{
	char **temp = NULL;
	FUNC_BGN("ewl_get_path_strings");
	temp = ewl_state_get_path_strings(length);
	FUNC_END("ewl_get_path_strings");
	return temp;
}


void       ewl_render_antialiased_set(EwlBool a)
{
	FUNC_BGN("ewl_render_antialiased_set");
	ewl_state_render_antialiased_set(a);
	FUNC_END("ewl_render_antialiased_set");
	return;
}

void       ewl_render_dithered_set(EwlBool d)
{
	FUNC_BGN("ewl_render_dithered_set");
	ewl_state_render_dithered_set(d);
	FUNC_END("ewl_render_dithered_set");
	return;
}

EwlBool    ewl_render_antialiased_get()
{
	EwlBool r = FALSE;
	FUNC_BGN("ewl_render_antialiased_get");
	r = ewl_state_render_antialiased_get();
	FUNC_END("ewl_render_antialiased_get");
	return r;
}

EwlBool    ewl_render_dithered_get()
{
	EwlBool r = FALSE;
	FUNC_BGN("ewl_render_dithered_get");
	r = ewl_state_render_dithered_get();
	FUNC_END("ewl_render_dithered_get");
	return r;
}


void       ewl_set_display(Display *disp)
{
	FUNC_BGN("ewl_set_display");
	ewl_state_set_display(disp);
	FUNC_END("ewl_set_display");
	return;
}

Display   *ewl_get_display()
{
	Display *disp = NULL;
	FUNC_BGN("ewl_get_display");
	disp = ewl_state_get_display();
	FUNC_END("ewl_get_display");
	return disp;
}


EwlRenderMethod  ewl_get_render_method()
{
	EwlState        *s = ewl_state_get();
	EwlRenderMethod  method = 0;
	FUNC_BGN("ewl_get_render_method");
	if (!s)	{
		ewl_debug("ewl_get_render_method", EWL_NULL_ERROR, "s");
	} else {
		method = s->render_method;
	}
	FUNC_END("ewl_get_render_method");
	return method;
}

void             ewl_set_render_method(EwlRenderMethod method)
{
	EwlState        *s = ewl_state_get();
	FUNC_BGN("ewl_set_render_method");
	if (!s)	{
		ewl_debug("ewl_set_render_method", EWL_NULL_ERROR, "s");
	} else if (method<0||method>RENDER_METHOD_COUNT) {
		ewl_debug("ewl_set_render_method", EWL_OUT_OF_BOUNDS_ERROR,
		          "method");
	} else {
		s->render_method = method;
	}
	FUNC_END("ewl_set_render_method");
	return;
}


void             ewl_add_window(EwlWidget *widget)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_add_window");
	if (!s)	{
		ewl_debug("ewl_add_window", EWL_NULL_ERROR, "s");
	} else if (!widget) {
		ewl_debug("ewl_add_window", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		s->window_list = ewl_ll_insert_with_data(s->window_list, widget);
	}
	FUNC_END("ewl_add_window");
	return;
}

void             ewl_remove_window(EwlWidget *widget)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_add_window");
	if (!s)	{
		ewl_debug("ewl_add_window", EWL_NULL_ERROR, "s");
	} else if (!widget) {
		ewl_debug("ewl_add_window", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		s->window_list = ewl_ll_remove_by_data(s->window_list, widget);
	}
	FUNC_END("ewl_add_window");
	return;
}


void       ewl_state_set_application_name(char *name)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_set_application_name");
	if (!s)	{
		ewl_debug("ewl_state_set_application_name", EWL_NULL_ERROR, "s");
	} else if (!name)	{
		ewl_debug("ewl_state_set_application_name", EWL_NULL_ERROR, "name");
	} else {
		if (s->application_name)	{
			free(s->application_name);
			s->application_name = NULL;
		}
		s->application_name = e_string_dup(name);
	}
	FUNC_BGN("ewl_state_set_application_name");
	return;
}

char      *ewl_state_get_application_name()
{
	char     *name = NULL;
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_get_application_name");
	if (!s)	{
		ewl_debug("ewl_state_get_application_name", EWL_NULL_ERROR, "s");
	} else {
		name = e_string_dup(s->application_name);
		if (!name)	{
			ewl_debug("ewl_state_get_application_name",EWL_NULL_ERROR,"name");
		}
	}
	FUNC_BGN("ewl_state_get_application_name");
	return name;
}


void       ewl_state_set_theme(char *name)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_set_theme");
	if (!s)	{
		ewl_debug("ewl_state_set_theme", EWL_NULL_ERROR, "s");
	} else if (!name)	{
		ewl_debug("ewl_state_set_theme", EWL_NULL_ERROR, "name");
	} else {
		if (s->theme_name)	{
			free(s->theme_name);
			s->theme_name = NULL;
		}
		s->theme_name = e_string_dup(name);
	}
	FUNC_BGN("ewl_state_set_theme");
	return;
}

char      *ewl_state_get_theme()
{
	char     *name = NULL;
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_get_theme");
	if (!s)	{
		ewl_debug("ewl_state_get_theme", EWL_NULL_ERROR, "s");
	} else {
		name = e_string_dup(s->theme_name);
		if (!name)	{
			ewl_debug("ewl_state_get_theme",EWL_NULL_ERROR,"name");
		}
	}
	FUNC_BGN("ewl_state_get_theme");
	return name;
}


void       ewl_state_insert_path(char *path)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_insert_path");
	if (!s)	{
		ewl_debug("ewl_state_insert_path", EWL_NULL_ERROR, "s");
	} else if (!path) {
		ewl_debug("ewl_state_insert_path", EWL_NULL_ERROR, "path");
	} else {
		s->path_list = ewl_ll_insert_with_data(s->path_list,
		                                       (EwlData*)e_string_dup(path));
	}
	FUNC_END("ewl_state_insert_path");
	return;
}

void       ewl_state_remove_path(char *path)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_insert_path");
	if (!s)	{
		ewl_debug("ewl_state_insert_path", EWL_NULL_ERROR, "s");
	} else if (!path) {
		ewl_debug("ewl_state_insert_path", EWL_NULL_ERROR, "path");
	} else {
		fprintf(stderr,"ewl_state_remove_path(): not implemented yet\n");
	}
	FUNC_END("ewl_state_insert_path");
	return;
}

EwlBool   _cb_ewl_state_get_path_list(EwlLL *node, EwlData *data)
{
	EwlLL *l = (EwlLL*) data;
	l = ewl_ll_insert_with_data(l,(EwlData*) e_string_dup((char*)node->data));
	fprintf(stderr,"_cb_ewl_state_get_path: %s\n", (char*) node->data);
	return TRUE;
}

EwlLL     *ewl_state_get_path_list()
{
	EwlState    *s = ewl_state_get();
	EwlLL       *l = NULL;
	FUNC_BGN("ewl_state_get_path_list");
	if (!s)	{
		ewl_debug("ewl_state_get_path_list", EWL_NULL_ERROR, "s");
	} else {
		l = s->path_list;
	}
	FUNC_END("ewl_state_get_path_list");
	return l;
}

char     **ewl_state_get_path_strings(int *length)
{
	EwlState    *s = ewl_state_get();
	EwlLL       *l = NULL;
	char       **paths = NULL;
	int          len = 0, i = 0;
	FUNC_BGN("ewl_state_get_path_strings");
	if (!s)	{
		ewl_debug("ewl_state_get_path_strings", EWL_NULL_ERROR, "s");
	} else if (!(len = ewl_ll_sizeof(s->path_list))) {
		ewl_debug("ewl_state_get_path_strings", EWL_NULL_ERROR, "s");
	} else {
		paths = malloc(sizeof(char*)*len);	
		if (length) *length = len;
		for (l=s->path_list;l;l=l->next)	{
			if (l->data&&((len = strlen((char*)l->data))!=0))	{
				paths[i] = e_string_dup((char*)l->data);
				i++;
			}
		}
	}
	FUNC_BGN("ewl_state_get_path_strings");
	return paths;
}



char       ewl_state_render_antialiased_get()
{
	EwlState *s = ewl_state_get();
	char      c = 0;
	FUNC_BGN("ewl_state_render_antialiased_get");
	if (!s)	{
		ewl_debug("ewl_state_render_antialiased_get", EWL_NULL_ERROR, "s");
	} else {
		c = s->render_antialiased;
	}
	FUNC_END("ewl_state_render_antialiased_get");
	return c;
}

char       ewl_state_render_dithered_get()
{
	EwlState *s = ewl_state_get();
	char      c = 0;
	FUNC_BGN("ewl_state_render_dithered_get");
	if (!s)	{
		ewl_debug("ewl_state_render_dithered_get", EWL_NULL_ERROR, "s");
	} else {
		c = s->render_dithered;
	}
	FUNC_END("ewl_state_render_dithered_get");
	return c;
}

void       ewl_state_render_antialiased_set(char a)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_render_antialiased_set");
	if (!s)	{
		ewl_debug("ewl_state_render_antialiased_get", EWL_NULL_ERROR, "s");
	} else {
		s->render_antialiased = a;
	}
	FUNC_END("ewl_state_render_antialiased_set");
	return;
}

void       ewl_state_render_dithered_set(char d)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_render_dithered_set");
	if (!s)	{
		ewl_debug("ewl_state_render_dithered_get", EWL_NULL_ERROR, "s");
	} else {
		s->render_dithered = d;
	}
	FUNC_END("ewl_state_render_dithered_set");
	return;
}


void       ewl_state_set_display(Display *disp)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_state_set_display");
	if (!s) {
		ewl_debug("ewl_state_set_display", EWL_NULL_ERROR, "s");
	} else if (!disp) {
		ewl_debug("ewl_state_set_display", EWL_NULL_ERROR, "disp");
	} else {
		s->disp = disp;
	}
	FUNC_END("ewl_state_set_display");
	return;
}

Display   *ewl_state_get_display()
{
	EwlState *s = ewl_state_get();
	Display  *disp = NULL;
	FUNC_BGN("ewl_state_get_display");
	if (!s) {
		ewl_debug("ewl_state_get_display", EWL_NULL_ERROR, "s");
	} else {
		disp = s->disp;
	}
	FUNC_END("ewl_state_get_display");
	return disp;
}

/* EwlState_Result ewl_state_dynreg_insert(); */
/* EwlState_Result ewl_state_dynreg_remove(); */
/* int              ewl_state_dynreg_count();  */

char       ewl_state_widget_add(EwlWidget *widget)
{
	EwlState *s = NULL;
	char       r = 0;
	FUNC_BGN("ewl_state_widget_insert");
	if (!widget)	{
		ewl_debug("ewl_state_widget_insert", EWL_NULL_ERROR, "widget");
	} else {
		s = ewl_state_get();
		if (!s)	{
			ewl_debug("ewl_state_widget_insert", EWL_NULL_ERROR, "s");
		} else {
			s->widget_list = ewl_ll_insert_with_data(s->widget_list,
			                                         (EwlData*)widget);
			r = 1;
		}
	}

	FUNC_END("ewl_state_widget_insert");
	return r;
}

char       ewl_state_widget_remove(EwlWidget *widget)
{
	EwlState *s = NULL;
	char       r = 0;
	FUNC_BGN("ewl_state_widget_remove");
	if (!widget)	{
		ewl_debug("ewl_state_widget_remove", EWL_NULL_ERROR, "widget");
	} else {
		s = ewl_state_get();
		if (!s)	{
			ewl_debug("ewl_state_widget_remove", EWL_NULL_ERROR, "s");
		} else {
			s->widget_list = ewl_ll_remove_by_data(s->widget_list,
			                                       (EwlData*)widget);
			r = 1;
		}
	}

	FUNC_END("ewl_state_widget_remove");
	return r;
}

int              ewl_state_widget_count()
{
	EwlState *s = NULL;
	int        r = 0;
	FUNC_BGN("ewl_state_widget_count");
	s = ewl_state_get();
	if (!s)	{
		ewl_debug("ewl_state_widget_count", EWL_NULL_ERROR, "s");
		r = -1;
	} else {
		r = ewl_ll_sizeof(s->widget_list);
	}
	FUNC_END("ewl_state_widget_count");
	return r;
}


char       ewl_state_event_add(EwlEvent *event)
{
	EwlState *s = NULL;
	char       r = 0;
	FUNC_BGN("ewl_state_event_insert");
	if (!event)	{
		ewl_debug("ewl_state_event_insert", EWL_NULL_ERROR, "event");
	} else {
		s = ewl_state_get();
		if (!s)	{
			ewl_debug("ewl_state_event_insert", EWL_NULL_ERROR, "s");
		} else {
			s->event_queue = (EwlEvent*)ewl_ll_insert((EwlLL*)s->event_queue,
			                                          (EwlLL*)event);
			r = 1;
		}
	}

	FUNC_END("ewl_state_event_insert");
	return r;
}

char       ewl_state_event_remove(EwlEvent *event)
{
	EwlState *s = NULL;
	char       r = 0;
	FUNC_BGN("ewl_state_event_remove");
	if (!event)	{
		ewl_debug("ewl_state_event_remove", EWL_NULL_ERROR, "event");
	} else {
		s = ewl_state_get();
		if (!s)	{
			ewl_debug("ewl_state_event_remove", EWL_NULL_ERROR, "s");
		} else {
			s->event_queue = (EwlEvent*)ewl_ll_remove((EwlLL*)s->event_queue,
			                                          (EwlLL*)event);
			r = 1;
		}
	}

	FUNC_END("ewl_state_event_remove");
	return r;
}

int              ewl_state_event_count()
{
	EwlState *s = NULL;
	int        r = 0;
	FUNC_BGN("ewl_state_event_count");
	s = ewl_state_get();
	if (!s)	{
		ewl_debug("ewl_state_event_count", EWL_NULL_ERROR, "s");
		r = -1;
	} else {
		r = ewl_ll_sizeof((EwlLL*)s->event_queue);
	}
	FUNC_END("ewl_state_event_count");
	return r;
}

void ewl_state_translators_init(EwlState *s)
{
	int i = 0;
	FUNC_BGN("ewl_state_translators_init");

	if (!s)	{
		ewl_debug("ewl_state_translators_init", EWL_NULL_ERROR, "s");
		FUNC_END("ewl_state_translators_init");
		return;
	}

	for (i=0; i<EWL_MAX_EVENT_TRANSLATORS; i++)
		s->translators[i] = NULL;

	s->translators[KeyPress] = ewl_translator_new(
	                           cb_ewl_event_keypress_translate,
	                           "EWL_EVENT_KEYDOWN", EWL_EVENT_KEYDOWN);
	s->translators[KeyRelease] = ewl_translator_new(
	                             cb_ewl_event_keyrelease_translate,
	                             "EWL_EVENT_KEYUP", EWL_EVENT_KEYUP);
	s->translators[ButtonPress] = ewl_translator_new(
	                              cb_ewl_event_buttonpress_translate, 
	                              "EWL_EVENT_MOUSEDOWN", EWL_EVENT_MOUSEDOWN);
	s->translators[ButtonRelease] = ewl_translator_new(
	                              cb_ewl_event_buttonrelease_translate, 
	                              "EWL_EVENT_MOUSEUP", EWL_EVENT_MOUSEUP);
	s->translators[MotionNotify] = ewl_translator_new(
	                              cb_ewl_event_motion_translate, 
	                              "EWL_EVENT_MOUSEMOVE", EWL_EVENT_MOUSEMOVE);
	s->translators[EnterNotify] = ewl_translator_new(
	                              cb_ewl_event_enter_translate, 
	                              "EWL_EVENT_ENTER", EWL_EVENT_ENTER);
	s->translators[LeaveNotify] = ewl_translator_new(
	                              cb_ewl_event_leave_translate, 
	                              "EWL_EVENT_LEAVE", EWL_EVENT_LEAVE);
	s->translators[FocusIn] = ewl_translator_new(
	                              cb_ewl_event_focusin_translate, 
	                              "EWL_EVENT_FOCUSIN", EWL_EVENT_FOCUSIN);
	s->translators[FocusOut] = ewl_translator_new(
	                              cb_ewl_event_focusout_translate, 
	                              "EWL_EVENT_FOCUSOUT", EWL_EVENT_FOCUSOUT);
	s->translators[Expose] = ewl_translator_new(
	                              cb_ewl_event_expose_translate, 
	                              "EWL_EVENT_EXPOSE", EWL_EVENT_EXPOSE);
	s->translators[VisibilityNotify] = ewl_translator_new(
	                              cb_ewl_event_visibility_translate, 
	                              "EWL_EVENT_VISIBILITY", EWL_EVENT_VISIBILITY);
	s->translators[CreateNotify] = ewl_translator_new(
	                              cb_ewl_event_create_translate, 
	                              "EWL_EVENT_CREATE", EWL_EVENT_CREATE);
	s->translators[DestroyNotify] = ewl_translator_new(
	                              cb_ewl_event_destroy_translate, 
	                              "EWL_EVENT_DESTROY", EWL_EVENT_DESTROY);
	s->translators[UnmapNotify] = ewl_translator_new(
	                              cb_ewl_event_unmapnotify_translate, 
	                              "EWL_EVENT_HIDE", EWL_EVENT_HIDE);
	s->translators[MapNotify] = ewl_translator_new(
	                              cb_ewl_event_mapnotify_translate, 
	                              "EWL_EVENT_SHOW", EWL_EVENT_SHOW);
	s->translators[MapRequest] = ewl_translator_new(
	                              cb_ewl_event_maprequest_translate, 
	                              "EWL_EVENT_SHOW", EWL_EVENT_SHOW);
	s->translators[ReparentNotify] = ewl_translator_new(
	                              cb_ewl_event_reparentnotify_translate, 
	                              "EWL_EVENT_REPARENT", EWL_EVENT_REPARENT);
	s->translators[ConfigureNotify] = ewl_translator_new(
	                              cb_ewl_event_configurenotify_translate, 
	                              "EWL_EVENT_CONFIGURE", EWL_EVENT_CONFIGURE);
	s->translators[ConfigureRequest] = ewl_translator_new(
	                              cb_ewl_event_configurerequest_translate, 
	                              "EWL_EVENT_CONFIGURE", EWL_EVENT_CONFIGURE);
	s->translators[CirculateNotify] = ewl_translator_new(
	                              cb_ewl_event_circulatenotify_translate, 
	                              "EWL_EVENT_CIRCULATE", EWL_EVENT_CIRCULATE);
	s->translators[CirculateRequest] = ewl_translator_new(
	                              cb_ewl_event_circulaterequest_translate, 
	                              "EWL_EVENT_CIRCULATE", EWL_EVENT_CIRCULATE);
	s->translators[PropertyNotify] = ewl_translator_new(
	                              cb_ewl_event_property_translate, 
	                              "EWL_EVENT_PROPERTY", EWL_EVENT_PROPERTY);
	s->translators[ColormapNotify] = ewl_translator_new(
	                              cb_ewl_event_colormap_translate, 
	                              "EWL_EVENT_COLORMAP", EWL_EVENT_COLORMAP);
	s->translators[ClientMessage] = ewl_translator_new(
	                              cb_ewl_event_clientmessage_translate, 
	                              "EWL_EVENT_CLIENT", EWL_EVENT_CLIENT);
	s->translators[SelectionNotify] = ewl_translator_new(
	                              cb_ewl_event_selectionnotify_translate, 
	                              "EWL_EVENT_SELECTION", EWL_EVENT_SELECTION);
	s->translators[SelectionRequest] = ewl_translator_new(
	                              cb_ewl_event_selectionrequest_translate, 
	                              "EWL_EVENT_SELECTION", EWL_EVENT_SELECTION);

	FUNC_END("ewl_state_translators_init");
	return;
}

EwlWidget *ewl_widget_grabbed_get()
{
	EwlState  *s = ewl_state_get();
	EwlWidget *w = NULL;
	FUNC_BGN("ewl_widget_grabbed_get");
	if (!s)	{
		ewl_debug("ewl_widget_grabbed_get", EWL_NULL_ERROR,"s");
	} else {
		w = s->grabbed;
	}
	FUNC_END("ewl_widget_grabbed_get");
	return w;
}

EwlWidget *ewl_widget_focused_get()
{
	EwlState  *s = ewl_state_get();
	EwlWidget *w = NULL;
	FUNC_BGN("ewl_widget_focused_get");
	if (!s)	{
		ewl_debug("ewl_widget_focused_get", EWL_NULL_ERROR,"s");
	} else {
		if (!(w = ewl_widget_grabbed_get()))
			w = s->focused;
	}
	FUNC_END("ewl_widget_focused_get");
	return w;
}

void       ewl_widget_grab(EwlWidget *w)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_widget_grab");
	if (!s)	{
		ewl_debug("ewl_widget_grab", EWL_NULL_ERROR, "s");
		ewl_debug("ewl_widget_grab", EWL_GRAB_ERROR, "w");
	} else if (!w) {
		ewl_debug("ewl_widget_grab", EWL_NULL_WIDGET_ERROR, "w");
		ewl_debug("ewl_widget_grab", EWL_GRAB_ERROR, "w");
	} else {
		if (s->grabbed)	{
			ewl_widget_ungrab(s->grabbed);
		}
		ewl_widget_focus(w);
		s->grabbed = w;
		ewl_widget_set_flag(w,GRABBED, 1);
	}
	FUNC_END("ewl_widget_grab");
	return;
}

void       ewl_widget_ungrab(EwlWidget *w)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_widget_ungrab");
	if (!s)	{
		ewl_debug("ewl_widget_ungrab", EWL_NULL_ERROR, "s");
	} else if (!w) {
		ewl_debug("ewl_widget_ungrab", EWL_NULL_WIDGET_ERROR, "w");
	} else if (w!=ewl_widget_grabbed_get()) {
		/* widget isn't ungrabbed */
	} else {
		ewl_widget_set_flag(w,GRABBED, 0);
		s->grabbed = NULL;
	}

	FUNC_END("ewl_widget_ungrab");
	return;
}

void       ewl_widget_focus(EwlWidget *w)
{
	EwlState *s = ewl_state_get();
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_widget_focus");
	if (!s)	{
		ewl_debug("ewl_widget_focus", EWL_NULL_ERROR, "s");
	} else if (!w) {
		ewl_debug("ewl_widget_focus", EWL_NULL_WIDGET_ERROR, "w");
	} else if (ewl_widget_grabbed_get()&&(w != ewl_widget_grabbed_get())) {
		ewl_debug("ewl_widget_focus", EWL_GRAB_ERROR, "w");
	} else {
		if (s->focused)	{
			ewl_widget_unfocus(s->focused);
		}
		s->focused = w;
		ewl_widget_set_flag(w,FOCUSED, 1);
		ev = ewl_event_new();
		if (ev)	{
			ev->type = EWL_EVENT_FOCUSIN;
			ev->widget = w;
			ewl_event_queue(ev);
		} else {
			ewl_debug("ewl_widget_focus", EWL_NULL_ERROR, "ev");
		}
	}
	FUNC_END("ewl_widget_focus");
	return;
}

void       ewl_widget_unfocus(EwlWidget *w)
{
	EwlState *s = ewl_state_get();
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_widget_unfocus");
	if (!s)	{
		ewl_debug("ewl_widget_unfocus", EWL_NULL_ERROR, "s");
	} else if (!w) {
		ewl_debug("ewl_widget_unfocus", EWL_NULL_WIDGET_ERROR, "w");
	} else if (ewl_widget_grabbed_get&&(w!=ewl_widget_grabbed_get())) {
		ewl_debug("ewl_widget_unfocus", EWL_GRAB_ERROR, "w");
	} else if (ewl_widget_focused_get&&(w!=ewl_widget_focused_get())) {
		ewl_debug("ewl_widget_unfocus", EWL_NULL_ERROR, "w");
	} else {
		ewl_widget_set_flag(w,FOCUSED, 0);
		s->focused = 0;
		ev = ewl_event_new();
		if (ev)	{
			ev->type = EWL_EVENT_FOCUSOUT;
			ev->widget = w;
			ewl_event_queue(ev);
		} else {
			ewl_debug("ewl_widget_unfocus", EWL_NULL_ERROR, "ev");
		}
	}
	FUNC_END("ewl_widget_unfocus");
	return;
}

EwlEvent *ewl_event_translate(EwlEvent *queue, XEvent *xev)
{
	EwlState *s = ewl_state_get();
	EwlEvent *ev = NULL;
	FUNC_BGN("ewl_event_translate");

	/*fprintf(stderr," xev->type == %d in ewl_event_translate().\n",
		xev->type);*/
	if (!s)		{
		ewl_debug("ewl_event_translate", EWL_NULL_ERROR, "s");
		FUNC_END("ewl_event_translate");
		return queue;
	} else if (!xev)	{
		ewl_debug("ewl_event_translate", EWL_NULL_ERROR, "xev");
		FUNC_END("ewl_event_translate");
		return queue;
	} else if (xev->type<0||xev->type>EWL_MAX_EVENT_TRANSLATORS)	{
		ewl_debug("ewl_event_translate", EWL_OUT_OF_BOUNDS_ERROR, "xev");
		FUNC_END("ewl_event_translate");
		return queue;
	}

	if (!s->translators[xev->type])	{
		ewl_debug("ewl_event_translate", EWL_NULL_ERROR,
			"s->translators[xev->type]");
		fprintf(stderr,"xev->type == %d\n", xev->type);
		FUNC_END("ewl_event_translate");
		return queue;
	} else if (!s->translators[xev->type]->cb) {
		ewl_debug("ewl_event_translate", EWL_NULL_ERROR,
			"s->translators[xev->type]->cb");
		fprintf(stderr,"xev->type == %d\n", xev->type);
		FUNC_END("ewl_event_translate");
		return queue;
	}
	if (ewl_debug_func_is_active())
		fprintf(stderr,"EWL_DEBUG: ewl_event_translate(): cb 0x%08x BGN.\n",
		        (unsigned int)s->translators[xev->type]->cb);
	ev = ((s->translators[xev->type]->cb))(s->translators[xev->type], xev);
	if (ewl_debug_func_is_active())
		fprintf(stderr,"EWL_DEBUG: ewl_event_translate(): cb 0x%08x END.\n",
		        (unsigned int)s->translators[xev->type]->cb);
	if (!ev)	{
		ewl_debug("ewl_event_translate", EWL_NULL_ERROR, "ev");
		ev = queue;
	} else { 
		ev->type = s->translators[xev->type]->type;
		ev->ll.next = (EwlLL*) queue;
	}
	FUNC_END("ewl_event_translate");
	return ev;
}

void       ewl_event_queue(EwlEvent *ev)
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_event_queue");
	if (!s)	{
		ewl_debug("ewl_event_queue", EWL_NULL_ERROR, "s");
	} else if (!ev) {
		ewl_debug("ewl_event_queue", EWL_NULL_ERROR, "ev");
	} else {
		s->event_queue = (EwlEvent*) ewl_ll_insert((EwlLL*) s->event_queue,
		                                           (EwlLL*) ev);
	}
	FUNC_END("ewl_event_queue");
	return;
}

EwlBool       ewl_events_pending()
{
	EwlBool   r = FALSE;
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_events_pending");
	r = (s->event_queue)?TRUE:FALSE;
	FUNC_END("ewl_events_pending");
	return r;
}

void       ewl_event_propagate(EwlEvent *ev)
{
	EwlState  *s   = ewl_state_get();
	EwlWidget *wid = NULL;
	FUNC_BGN("ewl_event_propagate");
	if (!s)	{
		ewl_debug("ewl_event_propagate", EWL_NULL_ERROR, "s");
	} else if (!ev)	{
		ewl_debug("ewl_event_propagate", EWL_NULL_ERROR, "ev");
	} else if (!ev->widget) {
		if (!ewl_widget_grabbed_get())	{
			ewl_debug("ewl_event_propagate", EWL_NULL_ERROR, "ev->widget");
		} else {
			ev->widget = ewl_widget_grabbed_get();
		}
	} else if (ewl_widget_get_flag(ev->widget,HAS_CHILDREN)) {
		/* if it has children, then it's derived from ewl_container */
		for (; ewl_widget_get_flag(ev->widget, HAS_CHILDREN) &&
		         (wid!=ev->widget);
		       ewl_container_event_propagate(ev->widget, ev) )	{
			wid = ev->widget;
		}
		
	} else {
		/* ev->widget == widget */
	}
	FUNC_END("ewl_event_propagate");
	return;
}

EwlEvent  *ewl_event_handle(EwlEvent *ev)
{
	EwlEvent *next = NULL;
	FUNC_BGN("ewl_event_handle");
	if (!ev)	{
		ewl_debug("ewl_event_handle", EWL_NULL_ERROR, "ev");
		next = NULL;
	} else {
		ewl_event_propagate(ev);
		if (!ev->widget)	{
			/* HANDLE GLOBAL EVENTS HERE */
			ewl_debug("ewl_event_handle", EWL_NULL_ERROR, "ev->widget");
			/*next = (EwlEvent*) ev->ll.next;*/
		} else {
			if (ewl_debug_is_active())	{
				fprintf(stderr,
				        "ewl_event_handle(): widget = %s 0x%08x, ev = %s\n",
				        ewl_widget_get_type_string(ev->widget),
				        (unsigned int) ev->widget,
				        ewl_event_get_type_string(ev));
			}
			ewl_widget_handle_event((EwlWidget*)ev->widget, ev, ev->data);
		}
		next = (EwlEvent*) ev->ll.next;
		ewl_event_free(ev);
	}
	FUNC_END("ewl_event_handle");
	return next;
}

void       ewl_widget_add(EwlWidget *widget)
{
	FUNC_BGN("ewl_widget_add");
	ewl_state_widget_add(widget);
	FUNC_END("ewl_widget_add");
	return;
}

void       ewl_widget_show(EwlWidget *widget)
{
	EwlEvent *ev = NULL;
	if (!widget)	{
		ewl_debug("ewl_widget_show", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		ewl_widget_set_flag(widget, VISIBLE, TRUE);
		ewl_widget_set_needs_resize(widget);
		ev = ewl_event_new_by_type(EWL_EVENT_SHOW);
		if (!ev)	{
			ewl_debug("ewl_widget_show", EWL_NULL_ERROR, "ev");
		} else {
			ev->widget = widget;
			ewl_event_queue(ev);
		}
		ev = ewl_event_new_by_type(EWL_EVENT_RESIZE);
		if (!ev)	{
			ewl_debug("ewl_widget_show", EWL_NULL_ERROR, "ev");
		} else {
			ev->widget = widget;
			ewl_event_queue(ev);
		}
	}
	return;
}

EwlBool    ewl_timers_pending()
{
	EwlBool   r = 0;
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_timers_pending");
	r = s->timer_queue?TRUE:FALSE;
	FUNC_END("ewl_timers_pending");
	return r;
}

EwlTimer  *ewl_timer_handle(EwlTimer *t)
{
	FUNC_BGN("ewl_timer_handle");
	FUNC_END("ewl_timer_handle");
	return NULL;
}


void       ewl_widget_hide(EwlWidget *widget)
{
	EwlEvent *ev = NULL;
	if (!widget)	{
		ewl_debug("ewl_widget_hide", EWL_NULL_WIDGET_ERROR, "widget");
	} else {
		ewl_widget_set_flag(widget, VISIBLE, FALSE);
		ev = ewl_event_new_by_type(EWL_EVENT_HIDE);
		if (!ev)	{
			ewl_debug("ewl_widget_hide", EWL_NULL_ERROR, "ev");
		} else {
			ev->widget = widget;
			ewl_event_queue(ev);
		}
		ev = ewl_event_new_by_type(EWL_EVENT_RESIZE);
		if (!ev)	{
			ewl_debug("ewl_widget_show", EWL_NULL_ERROR, "ev");
		} else {
			ev->widget = widget;
			ewl_event_queue(ev);
		}
	}
	return;
}

char       ewl_main_iteration()
{
	EwlState  *s   = ewl_state_get();
	EwlEvent  *tev = NULL;
	EwlLL     *l   = NULL;
	XEvent     xev;
	FUNC_BGN("ewl_main_iteration");
	if (!s)	{
		ewl_debug("ewl_main_iteration", EWL_NULL_ERROR, "s");
		FUNC_END("ewl_main_iteration");
		return 1;
	}

	/* handle X events */
	/* by synthesizing EwlEvents and handling them before the event queue */
	if (s->widget_list)		{
		while (XPending(s->disp))	{
			XNextEvent(s->disp, &xev);
			tev = ewl_event_translate(NULL, &xev);
			tev = ewl_event_handle(tev);
		}
		XFlush(s->disp);
	}
	
	/* gotta stick this in or else the beast eats 100% cpu =) */
	usleep(1);
	
	/* handle internal EwlEvents */
	while (ewl_events_pending())	{
		s->event_queue = ewl_event_handle(s->event_queue);
	}

	/* handle timer queue */
	while (ewl_timers_pending())	{
		fprintf(stderr,"WHY THE FUCK ARE WE IN HERE GOD DAMMIT?\n");
		s->timer_queue = ewl_timer_handle(s->timer_queue);
	}

	/* render thread -- thank goodness for evas! */
	for (l=s->window_list; l; l=l->next)	{
		fprintf(stderr,"ewl_main_iteration(): rendering window 0x%08x\n",
		        (unsigned int) l->data);
		ewl_window_render((EwlWidget*)l->data);
	}

	/*fprintf(stderr,"end ewl_main_iteration().\n");*/
	return (!s->quit_flag);

	FUNC_END("ewl_main_iteration");
	return 1;
}

void       ewl_init(int *argc, char ***argv)
{
	EwlState *s = ewl_state_new();
	FUNC_BGN("ewl_init");
	if (!s)	{
		ewl_debug("ewl_init", EWL_NULL_ERROR, "s");
		FUNC_END("ewl_init");
		return;
	}
	ewl_state_init(s);
	ewl_state_set(s);

	ewl_option_parse(argc, argv);
	ewl_x_init();

	FUNC_END("ewl_init");
	return;
}

void       ewl_x_init()
{
	char     *t[2] = {NULL,NULL};
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_x_init");
	if (!s)	{
		ewl_debug("ewl_x_init", EWL_NULL_ERROR, "s");
		FUNC_END("ewl_x_init");
		return;
	}
	s->disp = XOpenDisplay(NULL);
	if (!s->disp)	{
		t[0] = getenv("DISPLAY");
		t[1] = malloc(1024);
		sprintf(t[1], "Could not open X Display (DISPLAY=\"%s\").",
		         t[0]?t[0]:"null");
		ewl_debug("ewl_x_init", EWL_NULL_ERROR, "s->disp");
		ewl_fatal(t[1]);
	}

	FUNC_END("ewl_x_init");
	return;
}

void       ewl_quit()
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_quit");
	if (!s)	{
		ewl_debug("ewl_quit", EWL_NULL_ERROR, "s");
	} else {
		s->quit_flag = 1;
	}
	FUNC_END("ewl_quit");
	return;
}

void       ewl_main()
{
	EwlState *s = ewl_state_get();
	FUNC_BGN("ewl_main");
	if (!s)	{
		ewl_debug("ewl_main", EWL_NULL_ERROR, "s");
	} else {
		while (!s->quit_flag)
			ewl_main_iteration();
	}
	FUNC_END("ewl_main");
	return;
}
