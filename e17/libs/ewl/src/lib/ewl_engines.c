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
 * @param embed: The embed to lookup the engine chain for hook matching
 * @param type: The Ewl_Engine_Hook type to find the best matching function
 * @return Returns a pointer to the appropriate engine callback on success.
 * @brief Retrieves the correct engine callback for the specified type.
 */
void *
ewl_engine_hook_get(Ewl_Embed *embed, Ewl_Engine_Hook type)
{
	Ewl_Engine *caller;
	void *match = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("embed", embed, NULL);
	DCHECK_PARAM_PTR_RET("valid type", 
				(type >= 0 && type < EWL_ENGINE_MAX), NULL);
	DCHECK_TYPE_RET("embed", embed, EWL_EMBED_TYPE, NULL);

	caller = EWL_ENGINE(embed->engine);
	if (!caller->functions->engine_hooks[type] && caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_first(deps);
		while ((caller = ecore_dlist_next(deps)))
		{
			if (caller->functions->engine_hooks[type])
				break;
		}
	}

	if (caller && caller->functions->engine_hooks[type])
		match = caller->functions->engine_hooks[type];

	DRETURN_PTR(match, DLEVEL_STABLE);
}

/**
 * @param embed: The embeddow to lookup the engine chain for hook matching
 * @param type: The Ewl_Engine_Hook type to find the best matching function
 * @return Returns a list of engine hooks matching on success.
 * @brief Retrieves a list of dependent engine callbacks for the specified type.
 */
Ecore_List *
ewl_engine_hook_chain_get(Ewl_Embed *embed, Ewl_Engine_Hook type)
{
	Ewl_Engine *caller;
	Ecore_List *matches = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("embed", embed, NULL);
	DCHECK_PARAM_PTR_RET("valid type", 
				(type >= 0 && type < EWL_ENGINE_MAX), NULL);
	DCHECK_TYPE_RET("embed", embed, EWL_EMBED_TYPE, NULL);

	matches = ecore_list_new();

	caller = EWL_ENGINE(embed->engine);
	if (caller->functions->engine_hooks[type])
		ecore_list_prepend(matches,
				caller->functions->engine_hooks[type]);
	if (caller->dependancies)
	{
		Ecore_List *deps;

		deps = caller->dependancies;
		ecore_list_goto_first(deps);
		while ((caller = ecore_dlist_next(deps)))
		{
			if (caller->functions->engine_hooks[type])
				ecore_list_append(matches,
					caller->functions->engine_hooks[type]);
		}
	}

	/*
	 * Free and return NULL if no matching hooks are found
	 */
	if (ecore_list_nodes(matches) <= 0) 
	{
		ecore_list_destroy(matches);
		matches = NULL;
	}

	DRETURN_PTR(matches, DLEVEL_STABLE);
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
	Ewl_Engine_Cb_Init engine_init;
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
	engine_init = engine->functions->engine_hooks[EWL_ENGINE_INIT];
	if (engine_init)
		engine_init(engine);
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
	Ewl_Engine_Cb_Window_New window_new;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	window_new = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_NEW);
	if (window_new)
		window_new(win);

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
	Ewl_Engine_Cb_Window_Destroy window_destroy;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	window_destroy = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_DESTROY);
	if (window_destroy)
		window_destroy(win);

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
	Ewl_Engine_Cb_Window_Move window_move;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	window_move = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_MOVE);
	if (window_move)
		window_move(win);

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
	Ewl_Engine_Cb_Window_Resize window_resize;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	window_resize = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_RESIZE);
	if (window_resize)
		window_resize(win);

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
	Ewl_Engine_Cb_Window_Min_Max_Size_Set window_min_max_size_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	window_min_max_size_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_MIN_MAX_SIZE_SET);
	if (window_min_max_size_set)
		window_min_max_size_set(win);

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
	Ewl_Engine_Cb_Window_Show window_show;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	window_show = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_SHOW);
	if (window_show)
		window_show(win);

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
	Ewl_Engine_Cb_Window_Hide window_hide;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	window_hide = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_HIDE);
	if (window_hide)
		window_hide(win);

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
	Ewl_Engine_Cb_Window_Title_Set window_title_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_title_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_TITLE_SET);
	if (window_title_set)
		window_title_set(win);

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
	Ewl_Engine_Cb_Window_Name_Class_Set window_name_class_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_name_class_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_NAME_CLASS_SET);
	if (window_name_class_set)
		window_name_class_set(win);

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
	Ewl_Engine_Cb_Window_Borderless_Set window_borderless_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_borderless_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_NAME_CLASS_SET);
	if (window_borderless_set)
		window_borderless_set(win);

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
	Ewl_Engine_Cb_Window_Dialog_Set window_dialog_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_dialog_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_DIALOG_SET);
	if (window_dialog_set)
		window_dialog_set(win);

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
	Ewl_Engine_Cb_Window_Fullscreen_Set window_fullscreen_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_fullscreen_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_FULLSCREEN_SET);
	if (window_fullscreen_set)
		window_fullscreen_set(win);

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
	Ewl_Engine_Cb_Window_Transient_For window_transient_for;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_transient_for = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_TRANSIENT_FOR);
	if (window_transient_for)
		window_transient_for(win);

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
	Ewl_Engine_Cb_Window_Modal_Set window_modal_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_modal_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_MODAL_SET);
	if (window_modal_set)
		window_modal_set(win);

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
	Ewl_Engine_Cb_Window_Raise window_raise;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	window_raise = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_RAISE);
	if (window_raise)
		window_raise(win);

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
	Ewl_Engine_Cb_Window_Lower window_lower;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	window_lower = ewl_engine_hook_get(EWL_EMBED(win), 
						EWL_ENGINE_WINDOW_LOWER);
	if (window_lower)
		window_lower(win);

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
	Ewl_Engine_Cb_Keyboard_Grab keyboard_grab;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, 0);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, 0);

	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	keyboard_grab = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_KEYBOARD_GRAB);
	if (keyboard_grab)
		DRETURN_INT(keyboard_grab(win), DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the keyboard ungrab
 */
void
ewl_engine_keyboard_ungrab(Ewl_Window *win)
{
	Ewl_Engine_Cb_Keyboard_Ungrab keyboard_ungrab;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	keyboard_ungrab = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_KEYBOARD_UNGRAB);
	if (keyboard_ungrab)
		keyboard_ungrab(win);

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
	Ewl_Engine_Cb_Pointer_Grab pointer_grab;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, 0);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, 0);

	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	pointer_grab = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_POINTER_GRAB);
	if (pointer_grab)
		DRETURN_INT(pointer_grab(win), DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the pointer ungrab
 */
void
ewl_engine_pointer_ungrab(Ewl_Window *win)
{
	Ewl_Engine_Cb_Pointer_Ungrab pointer_ungrab;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!VISIBLE(win) || !REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	pointer_ungrab = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_POINTER_UNGRAB);
	if (pointer_ungrab)
		pointer_ungrab(win);

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
	Ewl_Engine_Cb_Window_Selection_Text_Set window_selection_text_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_PARAM_PTR("txt", txt);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

	window_selection_text_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_SELECTION_TEXT_SET);
	if (window_selection_text_set)
		window_selection_text_set(win, txt);

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
	Ewl_Engine_Cb_Window_Geometry_Get window_geometry_get;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	/* make sure these get set to _something_ */
	if (width) *width = 0;
	if (height) *height = 0;

	window_geometry_get = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_GEOMETRY_GET);
	if (window_geometry_get)
		window_geometry_get((root ? NULL : win), width, height);

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
	Ewl_Engine_Cb_Window_Dnd_Aware_Set window_dnd_aware_set;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (!(win->window))
		DRETURN(DLEVEL_STABLE);

	window_dnd_aware_set = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_WINDOW_DND_AWARE_SET);
	if (window_dnd_aware_set)
		window_dnd_aware_set(win);

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
	Ewl_Engine_Cb_Canvas_Setup canvas_setup;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	canvas_setup = ewl_engine_hook_get(EWL_EMBED(win),
					EWL_ENGINE_CANVAS_SETUP);
	if (canvas_setup)
		canvas_setup(win, debug);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Freezes the canvas
 */
void
ewl_engine_canvas_freeze(Ewl_Embed *embed)
{
	Ewl_Engine_Cb_Canvas_Freeze canvas_freeze;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	canvas_freeze = ewl_engine_hook_get(embed, EWL_ENGINE_CANVAS_FREEZE);
	if (canvas_freeze)
		canvas_freeze(embed);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Thaws the canvas
 */
void
ewl_engine_canvas_thaw(Ewl_Embed *embed)
{
	Ewl_Engine_Cb_Canvas_Thaw canvas_thaw;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	canvas_thaw = ewl_engine_hook_get(embed, EWL_ENGINE_CANVAS_THAW);
	if (canvas_thaw)
		canvas_thaw(embed);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: Embed used to lookup the current theme engine.
 * @return Returns no value
 * @brief Freeze the theme system
 */
void
ewl_engine_theme_freeze(Ewl_Embed *embed)
{
	Ewl_Engine_Cb_Theme_Freeze theme_freeze;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	theme_freeze = ewl_engine_hook_get(embed, EWL_ENGINE_THEME_FREEZE);
	if (theme_freeze)
		theme_freeze();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: Embed used to lookup the current theme engine.
 * @return Returns no value
 * @brief Thaw the theme system
 */
void
ewl_engine_theme_thaw(Ewl_Embed *embed)
{
	Ewl_Engine_Cb_Theme_Thaw theme_thaw;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	theme_thaw = ewl_engine_hook_get(embed, EWL_ENGINE_THEME_THAW);
	if (theme_thaw)
		theme_thaw();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a new object group on success, NULL on failure
 * @brief Create a grouping for theme objects.
 */
void *
ewl_engine_theme_widget_group(Ewl_Widget *w)
{
	Ewl_Engine_Cb_Theme_Widget_Group theme_widget_group;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	theme_widget_group = ewl_engine_hook_get(EWL_EMBED(w),
					EWL_ENGINE_THEME_WIDGET_GROUP);
	if (theme_widget_group)
		DRETURN_PTR(theme_widget_group(w), DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

static void
ewl_engine_free(Ewl_Engine *engine)
{
	if (engine->functions->engine_hooks[EWL_ENGINE_SHUTDOWN]) {
		Ewl_Engine_Cb_Shutdown engine_shutdown;
		engine_shutdown = engine->functions->engine_hooks[EWL_ENGINE_SHUTDOWN];
		if (engine_shutdown)
			engine_shutdown(engine);
	}

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

