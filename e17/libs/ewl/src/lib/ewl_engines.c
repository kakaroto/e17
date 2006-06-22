#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

#include <dlfcn.h>

#define EWL_ENGINE_DIR "engines"

static Ecore_Hash *ewl_engines = NULL;
static void ewl_engine_free(Ewl_Engine *engine);
static void ewl_engines_cb_engine_free(void *data);

/**
 * @return Returns no value
 * @brief Initialises the engine system 
 */
int
ewl_engines_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_engines = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	if (!ewl_engines)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ecore_hash_set_free_value(ewl_engines, ewl_engines_cb_engine_free);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 *  @return Returns no value
 *  @brief Shutdown the engine system
 */
void
ewl_engines_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_hash_destroy(ewl_engines);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a list of engine names
 * @brief Retrieves a list of available engine names
 */
Ecore_List *
ewl_engine_names_get(void)
{
	char dir[PATH_MAX], *file;
	Ecore_List *files, *names;
	
	DENTER_FUNCTION(DLEVEL_STABLE);

	snprintf(dir, sizeof(dir), "%s/%s", PACKAGE_LIB_DIR, EWL_ENGINE_DIR);
	if (!ecore_file_exists(dir))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	files = ecore_file_ls(dir);
	if (!files || (ecore_list_nodes(files) == 0))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	names = ecore_list_new();
	if (!names)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ecore_list_goto_first(files);
	while ((file = ecore_list_next(files)))
	{
		int len;

		len = strlen(file);
		if (!strncmp(file + (len - 3), ".so", 3))
		{
			file[len - 3] = '\0';
			ecore_list_append(names, strdup(file));
		}
	}
	ecore_list_destroy(files);

	DRETURN_PTR(names, DLEVEL_STABLE);
}

/**
 * @param name: The name of the engine to create
 * @return Returns the Ewl_Engine or NULL on failure
 * @brief Retrieves, or creates the given Ewl_Engine, or NULL on failure
 */
Ewl_Engine *
ewl_engine_new(const char *name)
{
	Ewl_Engine *engine = NULL;
	Ewl_Engine *(*create_engine)(void);
	Ecore_List *(*dependancies)(void);
	Ecore_DList *deps = NULL;
	Ecore_List *dep_list;
	void *handle;
	char filename[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	/* see if we've created this engine already */
	engine = ecore_hash_get(ewl_engines, (char *)name);
	if (engine)
		DRETURN_PTR(engine, DLEVEL_STABLE);

	snprintf(filename, sizeof(filename), "%s/%s/%s.so", PACKAGE_LIB_DIR, 
							EWL_ENGINE_DIR, name);
	if (!ecore_file_exists(filename))
	{
		DWARNING("Given engine name dosen't exist.\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	handle = dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
	if (!handle) 
	{
		DWARNING("Unable to dlopen engine file.\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	dependancies = dlsym(handle, "ewl_engine_dependancies");
	if (!dependancies)
	{
		DWARNING("Unable to find ewl_engine_dependancies in "
				"engine file.\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	/* create all the needed parent engines for this engine */
	dep_list = dependancies();
	if (dep_list)
	{
		char *name;

		deps = ecore_dlist_new();

		ecore_list_goto_first(dep_list);
		while ((name = ecore_list_next(dep_list)))
		{
			Ewl_Engine *parent;

			parent = ewl_engine_new(name);
			ecore_list_append(deps, parent);
		}
		ecore_list_destroy(dep_list);
	}

	create_engine = dlsym(handle, "ewl_engine_create");
	if (!create_engine)
	{
		DWARNING("Unable to find ewl_engine_create in engine file.\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	engine = EWL_ENGINE(create_engine());
	if (!engine)
	{
		DWARNING("Unable to create engine.\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	engine->handle = handle;
	engine->dependancies = deps;
	engine->functions->init(engine);
	ecore_hash_set(ewl_engines, strdup(name), engine);

	DRETURN_PTR(engine, DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Creates a new window
 */
void
ewl_engine_window_new(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_new && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_new)
				break;
		}
	}

	if (!caller || !caller->functions->window_new)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_new(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Destroys the window
 */
void
ewl_engine_window_destroy(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_destroy && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_destroy)
				break;
		}
	}

	if (!caller || !caller->functions->window_destroy)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_destroy(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Moves the window
 */
void
ewl_engine_window_move(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_move && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_move)
				break;
		}
	}

	if (!caller || !caller->functions->window_move)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_move(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Resizes the window
 */
void
ewl_engine_window_resize(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_resize && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_resize)
				break;
		}
	}

	if (!caller || !caller->functions->window_resize)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_resize(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window min/max size
 */
void
ewl_engine_window_min_max_size_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_min_max_size_set 
			&& caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_min_max_size_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_min_max_size_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_min_max_size_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Shows the window
 */
void
ewl_engine_window_show(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_show && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_show)
				break;
		}
	}

	if (!caller || !caller->functions->window_show)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_show(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Hides the window
 */
void
ewl_engine_window_hide(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_hide && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_hide)
				break;
		}
	}

	if (!caller || !caller->functions->window_hide)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_hide(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the title of the window
 */
void
ewl_engine_window_title_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_title_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_title_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_title_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_title_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the name and class of the window
 */
void
ewl_engine_window_name_class_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_name_class_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_name_class_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_name_class_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_name_class_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window borderless
 */
void
ewl_engine_window_borderless_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_borderless_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_borderless_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_borderless_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_borderless_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window as a dialog
 */
void
ewl_engine_window_dialog_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_dialog_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_dialog_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_dialog_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_dialog_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window fullscreen
 */
void
ewl_engine_window_fullscreen_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_fullscreen_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_fullscreen_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_fullscreen_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_fullscreen_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window transient
 */
void
ewl_engine_window_transient_for(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_transient_for && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_transient_for)
				break;
		}
	}

	if (!caller || !caller->functions->window_transient_for)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_transient_for(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window modal
 */
void
ewl_engine_window_modal_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_modal_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_modal_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_modal_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_modal_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Raises the window
 */
void
ewl_engine_window_raise(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_raise && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_raise)
				break;
		}
	}

	if (!caller || !caller->functions->window_raise)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_raise(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Lowers the window
 */
void
ewl_engine_window_lower(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_lower && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_lower)
				break;
		}
	}

	if (!caller || !caller->functions->window_lower)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_lower(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return The keyboard grab value
 * @brief Sets the keyboard grab
 */
int
ewl_engine_keyboard_grab(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, 0);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, 0);

	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->keyboard_grab && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->keyboard_grab)
				break;
		}
	}

	if (!caller || !caller->functions->keyboard_grab)
		DRETURN_INT(0, DLEVEL_STABLE);

	DRETURN_INT(caller->functions->keyboard_grab(win), DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the keyboard ungrab
 */
void
ewl_engine_keyboard_ungrab(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->keyboard_ungrab && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->keyboard_ungrab)
				break;
		}
	}

	if (!caller || !caller->functions->keyboard_ungrab)
		DRETURN(DLEVEL_STABLE);

	caller->functions->keyboard_ungrab(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE); 
}

/**
 * @param win: the window to work with
 * @return Returns the pointer grab value
 * @brief Sets the pointer grab
 */
int
ewl_engine_pointer_grab(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, 0);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, 0);
	
	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->pointer_grab && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->pointer_grab)
				break;
		}
	}

	if (!caller || !caller->functions->pointer_grab)
		DRETURN_INT(0, DLEVEL_STABLE);

	DRETURN_INT(caller->functions->pointer_grab(win), DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the pointer ungrab
 */
void
ewl_engine_pointer_ungrab(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->pointer_ungrab && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->pointer_ungrab)
				break;
		}
	}

	if (!caller || !caller->functions->pointer_ungrab)
		DRETURN(DLEVEL_STABLE);

	caller->functions->pointer_ungrab(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE); 
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window selection text
 */
void
ewl_engine_window_selection_text_set(Ewl_Window *win, const char *txt)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_PARAM_PTR("txt", txt);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_selection_text_set 
					&& caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_selection_text_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_selection_text_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_selection_text_set(win, txt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @param width: Where to store the width
 * @param height: Where to store the height
 * @return Returns no value
 * @brief Retrieves the window geometry
 * 
 * We dont' check win == null in case their querying after the root window 
 */
void
ewl_engine_window_geometry_get(Ewl_Window *win, int root, int *width, int *height)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/* make sure these get set to _something_ */
	if (width) *width = 0;
	if (height) *height = 0;

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_geometry_get && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_geometry_get)
				break;
		}
	}

	if (!caller || !caller->functions->window_geometry_get)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_geometry_get(
				(root ? NULL : win), width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the dnd awareness
 */
void
ewl_engine_window_dnd_aware_set(Ewl_Window *win)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->window_dnd_aware_set && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->window_dnd_aware_set)
				break;
		}
	}

	if (!caller || !caller->functions->window_dnd_aware_set)
		DRETURN(DLEVEL_STABLE);

	caller->functions->window_dnd_aware_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets up the canvas
 */
void
ewl_engine_canvas_setup(Ewl_Window *win, int debug)
{
	Ewl_Engine *caller;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);
	
	caller = EWL_ENGINE(win->engine);
	if (!caller->functions->canvas_setup && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_last(deps);
		while ((caller = ecore_dlist_previous(deps)))
		{
			if (caller->functions->canvas_setup)
				break;
		}
	}

	if (!caller || !caller->functions->canvas_setup)
		DRETURN(DLEVEL_STABLE);

	caller->functions->canvas_setup(win, debug);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_engine_free(Ewl_Engine *engine)
{
	if (engine->functions->shutdown)
		engine->functions->shutdown(engine);

	/* parents will be shutdown when their freed by the cleanup code */
	if (engine->dependancies)
		ecore_dlist_destroy(engine->dependancies);

	dlclose(engine->handle);
	engine->handle = NULL;

	IF_FREE(engine->name);
	FREE(engine);
}

static void
ewl_engines_cb_engine_free(void *data)
{
	Ewl_Engine *engine;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	engine = data;
	ewl_engine_free(engine);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


