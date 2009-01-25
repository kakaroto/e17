/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

#define EWL_ENGINE_DIR "engines"

#ifdef _WIN32
# define EWL_ENGINE_EXT ".dll"
#else
# define EWL_ENGINE_EXT ".so"
#endif /* _WIN32 */

enum Ewl_Engine_Hook_Type
{
        EWL_ENGINE_HOOK_TYPE_WINDOW,
        EWL_ENGINE_HOOK_TYPE_CANVAS,
        EWL_ENGINE_HOOK_TYPE_THEME,
        EWL_ENGINE_HOOK_TYPE_POINTER
};
typedef enum Ewl_Engine_Hook_Type Ewl_Engine_Hook_Type;

Ecore_Path_Group *ewl_engines_path = NULL;

static Ecore_Hash *ewl_engines = NULL;
static void ewl_engine_free(Ewl_Engine *engine);
static void **ewl_engine_hooks_get(Ewl_Engine *engine, Ewl_Engine_Hook_Type type);
static void *ewl_engine_hook_get(Ewl_Embed *embed,
                                Ewl_Engine_Hook_Type type, int hook);

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

        ecore_hash_free_key_cb_set(ewl_engines, ECORE_FREE_CB(free));
        ecore_hash_free_value_cb_set(ewl_engines, ECORE_FREE_CB(ewl_engine_free));

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

        IF_FREE_HASH(ewl_engines);
        if (ewl_engines_path)
        {
                ecore_path_group_del(ewl_engines_path);
                ewl_engines_path = 0;
        }

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

        snprintf(dir, sizeof(dir), "%s/ewl/%s", PACKAGE_LIB_DIR,
                                                EWL_ENGINE_DIR);
        if (!ecore_file_exists(dir))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        files = ecore_file_ls(dir);
        if (!files || (ecore_list_count(files) == 0))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        names = ecore_list_new();
        if (!names)
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        ecore_list_free_cb_set(names, ECORE_FREE_CB(free));

        ecore_list_first_goto(files);
        while ((file = ecore_list_next(files)))
        {
                char *ext;

                ext = strchr(file, '.');
                if (ext)
                {
                        char *lastext;
                        lastext = strrchr(file, '.');
                        if (!strncmp(lastext, EWL_ENGINE_EXT, strlen(EWL_ENGINE_EXT)) && ext == lastext) {
                                *ext = '\0';
                                ecore_list_append(names, strdup(file));
                        }
                }
        }
        IF_FREE_LIST(files);

        DRETURN_PTR(names, DLEVEL_STABLE);
}

/**
 * @param name: The name of the engine to create
 * @param argc: Arguments count
 * @param argv: Arguments
 * @return Returns the Ewl_Engine or NULL on failure
 * @brief Retrieves, or creates the given Ewl_Engine, or NULL on failure
 */
Ewl_Engine *
ewl_engine_new(const char *name, int *argc, char ** argv)
{
        Ewl_Engine *engine = NULL;
        Ewl_Engine *(*create_engine)(int *argc, char ** argv);
        Ecore_DList *(*dependancies)(void);
        Ecore_DList *deps = NULL;
        Ecore_DList *dep_list;
        Ecore_Plugin *plugin;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(name, NULL);

        /* see if we've created this engine already */
        engine = ecore_hash_get(ewl_engines, name);
        if (engine)
                DRETURN_PTR(engine, DLEVEL_STABLE);

        if (!ewl_engines_path)
        {
                char pathname[PATH_MAX];

                ewl_engines_path = ecore_path_group_new();
                snprintf(pathname, sizeof(pathname), "%s/ewl/%s/",
                                                        PACKAGE_LIB_DIR,
                                                        EWL_ENGINE_DIR);
                ecore_path_group_add(ewl_engines_path, pathname);
        }

        plugin = ecore_plugin_load(ewl_engines_path, name, INTERFACE_CURRENT);
        if (!plugin)
        {
                fprintf(stderr, "Unable to open engine %s.\n", name);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        dependancies = ecore_plugin_symbol_get(plugin, "ewl_engine_dependancies");
        if (!dependancies)
        {
                fprintf(stderr, "Unable to find ewl_engine_dependancies for "
                                "the %s engine.\n", name);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        /* create all the needed parent engines for this engine */
        dep_list = dependancies();
        if (dep_list)
        {
                char *dep_name;

                /* this doesn't have a free callback attached to it because
                 * the engines are cached and the _shutdown() function will
                 * cleanup the cache */
                deps = ecore_dlist_new();
                while ((dep_name = ecore_list_first_remove(dep_list)))
                {
                        Ewl_Engine *parent;

                        parent = ewl_engine_new(dep_name, argc, argv);
                        if (!parent)
                        {
                                FREE(dep_name);
                                ecore_dlist_destroy(dep_list);
                                DRETURN_PTR(NULL, DLEVEL_STABLE);
                        }

                        ecore_dlist_append(deps, parent);
                        FREE(dep_name);
                }
                ecore_dlist_destroy(dep_list);
        }

        create_engine = ecore_plugin_symbol_get(plugin, "ewl_engine_create");
        if (!create_engine)
        {
                fprintf(stderr, "Unable to find ewl_engine_create for the %s engine.\n", name);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        engine = EWL_ENGINE(create_engine(argc, argv));
        if (!engine)
        {
                fprintf(stderr, "Unable to create %s engine.\n", name);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        engine->plugin = plugin;
        engine->dependancies = deps;

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        window_new = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        window_destroy = ewl_engine_hook_get(EWL_EMBED(win),
                                                EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        window_move = ewl_engine_hook_get(EWL_EMBED(win),
                                                EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        window_resize = ewl_engine_hook_get(EWL_EMBED(win),
                                                EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        window_min_max_size_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        window_show = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        window_hide = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_title_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_name_class_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_borderless_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_dialog_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DIALOG_SET);
        if (window_dialog_set)
                window_dialog_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window state values
 */
void
ewl_engine_window_states_set(Ewl_Window *win)
{
        Ewl_Engine_Cb_Window_States_Set window_states_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_states_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_STATES_SET);
        if (window_states_set)
                window_states_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets the window hints
 */
void
ewl_engine_window_hints_set(Ewl_Window *win)
{
        Ewl_Engine_Cb_Window_Hints_Set window_hints_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_hints_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_HINTS_SET);
        if (window_hints_set)
                window_hints_set(win);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_transient_for = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_TRANSIENT_FOR);
        if (window_transient_for)
                window_transient_for(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @return Returns no value
 * @brief Sets a leader for the window
 */
void
ewl_engine_window_leader_set(Ewl_Window *win)
{
        Ewl_Engine_Cb_Window_Leader_Set window_leader_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!(win->window))
                DRETURN(DLEVEL_STABLE);

        window_leader_set = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_LEADER_SET);
        if (window_leader_set)
                window_leader_set(win);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        window_raise = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        window_lower = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR_RET(win, 0);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, 0);

        if (!VISIBLE(win) || !REALIZED(win))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        keyboard_grab = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!VISIBLE(win) || !REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        keyboard_ungrab = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR_RET(win, 0);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, 0);

        if (!VISIBLE(win) || !REALIZED(win))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        pointer_grab = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (!VISIBLE(win) || !REALIZED(win))
                DRETURN(DLEVEL_STABLE);

        pointer_ungrab = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_POINTER_UNGRAB);
        if (pointer_ungrab)
                pointer_ungrab(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param emb: the embed to work with
 * @param txt: The text to set as the selection
 * @return Returns no value
 * @brief Sets the embed selection text
 */
void
ewl_engine_embed_selection_text_set(Ewl_Embed *emb, const char *txt)
{
        Ewl_Engine_Cb_Window_Selection_Text_Set embed_selection_text_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(emb);
        DCHECK_PARAM_PTR(txt);
        DCHECK_TYPE(emb, EWL_EMBED_TYPE);

        if (!REALIZED(emb))
                DRETURN(DLEVEL_STABLE);

        embed_selection_text_set = ewl_engine_hook_get(EWL_EMBED(emb),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_SELECTION_TEXT_SET);
        if (embed_selection_text_set)
                embed_selection_text_set(emb, txt);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @param root: The root window to get the height for
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        /* make sure these get set to _something_ */
        if (width) *width = 0;
        if (height) *height = 0;

        window_geometry_get = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_GEOMETRY_GET);
        if (window_geometry_get)
                window_geometry_get((root ? NULL : win), width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @return Returns no value
 * @brief Sets the dnd awareness
 */
void
ewl_engine_embed_dnd_aware_set(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Window_Dnd_Aware_Set embed_dnd_aware_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        if (!(embed->canvas_window))
                DRETURN(DLEVEL_STABLE);

        embed_dnd_aware_set = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DND_AWARE_SET);
        if (embed_dnd_aware_set)
                embed_dnd_aware_set(embed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @param types: The dnd types to set
 * @param num: The number of types
 * @return Returns no value
 * @brief Sets the dnd types provided by this embed.
 */
void
ewl_engine_embed_dnd_drag_types_set(Ewl_Embed *embed, char **types, unsigned int num)
{
        Ewl_Engine_Cb_Window_Dnd_Drag_Types_Set embed_dnd_drag_types_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        if (!(embed->canvas_window))
                DRETURN(DLEVEL_STABLE);

        embed_dnd_drag_types_set = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DND_DRAG_TYPES_SET);
        if (embed_dnd_drag_types_set)
                embed_dnd_drag_types_set(embed, types, num);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @return Returns no value
 * @brief Starts the dnd process on an embed.
 */
void
ewl_engine_embed_dnd_drag_start(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Window_Dnd_Drag_Start embed_dnd_drag_start;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        if (!(embed->canvas_window))
                DRETURN(DLEVEL_STABLE);

        embed_dnd_drag_start = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DND_DRAG_START);
        if (embed_dnd_drag_start)
                embed_dnd_drag_start(embed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @return Returns no value
 * @brief Completes the dnd process on an embed.
 */
void
ewl_engine_embed_dnd_drag_drop(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Window_Dnd_Drag_Drop embed_dnd_drag_drop;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        if (!(embed->canvas_window))
                DRETURN(DLEVEL_STABLE);

        embed_dnd_drag_drop = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DND_DRAG_DROP);
        if (embed_dnd_drag_drop)
                embed_dnd_drag_drop(embed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @param handle: The DND handle to work with
 * @param data: The data to send
 * @param size: The size of the data being sent
 * @return Returns no value
 * @brief Sends dnd data to the drop recipient
 */
int
ewl_engine_embed_dnd_drag_data_send(Ewl_Embed *embed, void *handle, void *data,
                                    int size)
{
        Ewl_Engine_Cb_Window_Dnd_Drag_Data_Send embed_dnd_drag_data_send;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, FALSE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, FALSE);

        if (!(embed->canvas_window))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        embed_dnd_drag_data_send = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DND_DRAG_DATA_SEND);
        if (embed_dnd_drag_data_send)
                DRETURN_INT(embed_dnd_drag_data_send(embed, handle, data, size),
                                DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @param w: a pointer where the width of the desktop will be stored
 * @param h: a pointer where the height pf the desktop will be stored
 * @return Returns no value
 * @brief Sets the dnd awareness
 */
void
ewl_engine_desktop_size_get(Ewl_Embed *embed, int *w, int *h)
{
        Ewl_Engine_Cb_Window_Desktop_Size_Get desktop_size_get;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        desktop_size_get = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_WINDOW,
                                        EWL_ENGINE_WINDOW_DESKTOP_SIZE_GET);
        if (desktop_size_get)
                desktop_size_get(embed, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with
 * @param debug: The debug setting
 * @return Returns no value
 * @brief Sets up the canvas
 */
void
ewl_engine_canvas_setup(Ewl_Window *win, int debug)
{
        Ewl_Engine_Cb_Canvas_Setup canvas_setup;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        canvas_setup = ewl_engine_hook_get(EWL_EMBED(win),
                                        EWL_ENGINE_HOOK_TYPE_CANVAS,
                                        EWL_ENGINE_CANVAS_SETUP);
        if (canvas_setup)
                canvas_setup(win, debug);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param emb: The embed to work with
 * @param x: The x coord to set
 * @param y: The y coord to set
 * @param width: The width to set
 * @param height: The height to set
 * @return Returns no value
 * @brief Sets up the canvas
 */
void
ewl_engine_canvas_output_set(Ewl_Embed *emb, int x, int y, int width, int height)
{
        Ewl_Engine_Cb_Canvas_Output_Set canvas_output_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(emb);
        DCHECK_TYPE(emb, EWL_EMBED_TYPE);

        canvas_output_set = ewl_engine_hook_get(emb,
                                        EWL_ENGINE_HOOK_TYPE_CANVAS,
                                        EWL_ENGINE_CANVAS_OUTPUT_SET);
        if (canvas_output_set)
                canvas_output_set(emb, x, y, width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @return Returns no value
 * @brief Renders the canvas
 */
void
ewl_engine_canvas_render(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Canvas_Render canvas_render;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        canvas_render = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_CANVAS,
                                        EWL_ENGINE_CANVAS_RENDER);
        if (canvas_render)
                canvas_render(embed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @return Returns no value
 * @brief Freezes the canvas
 */
void
ewl_engine_canvas_freeze(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Canvas_Freeze canvas_freeze;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        canvas_freeze = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_CANVAS,
                                        EWL_ENGINE_CANVAS_FREEZE);
        if (canvas_freeze)
                canvas_freeze(embed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to work with
 * @return Returns no value
 * @brief Thaws the canvas
 */
void
ewl_engine_canvas_thaw(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Canvas_Thaw canvas_thaw;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        canvas_thaw = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_CANVAS,
                                        EWL_ENGINE_CANVAS_THAW);
        if (canvas_thaw)
                canvas_thaw(embed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_engine_canvas_damage_add(Ewl_Embed *embed, int x, int y, int w, int h)
{
        Ewl_Engine_Cb_Canvas_Damage_Add damage_add;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        damage_add = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_CANVAS,
                                        EWL_ENGINE_CANVAS_DAMAGE_ADD);
        if (damage_add)
                damage_add(embed, x, y, w, h);

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
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        theme_freeze = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_FREEZE);
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
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        theme_thaw = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_THAW);
        if (theme_thaw)
                theme_thaw();

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: Embed used to lookup the current theme engine.
 * @param widget: The widget to add to the layer stack
 * @return Returns no value
 * @brief add the widget into the appropriated layer stack
 */
void
ewl_engine_theme_layer_stack_add(Ewl_Embed *embed, Ewl_Widget *w)
{
        Ewl_Engine_Cb_Theme_Layer_Stack_Add stack_add;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        stack_add = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_LAYER_STACK_ADD);
        if (stack_add)
                stack_add(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: Embed used to lookup the current theme engine.
 * @param widget: The widget to remove from the layer stack
 * @return Returns no value
 * @brief remove the widget into the appropriated layer stack
 */
void
ewl_engine_theme_layer_stack_del(Ewl_Embed *embed, Ewl_Widget *w)
{
        Ewl_Engine_Cb_Theme_Layer_Stack_Del stack_del;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        stack_del = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_LAYER_STACK_DEL);
        if (stack_del)
                stack_del(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: Embed used to lookup the current theme engine.
 * @param widget: The widget to update the layer position
 * @return Returns no value
 * @brief update the layer postion of the given widget
 */
void
ewl_engine_theme_layer_update(Ewl_Embed *embed, Ewl_Widget *w)
{
        Ewl_Engine_Cb_Theme_Layer_Update layer_update;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        layer_update = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_LAYER_UPDATE);
        if (layer_update)
                layer_update(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}
#if 0
/**
 * @param w: Widget used to lookup the current theme key.
 * @param key: The key to lookup
 * @return Returns the theme data
 * @brief Fetch data from the theme system
 */
char *
ewl_engine_theme_data_get(Ewl_Widget *w, char *key)
{
        Ewl_Embed *embed;
        Ewl_Engine_Cb_Theme_Data_Get theme_data_get;
        char *value = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        embed = ewl_embed_widget_find(w);
        if (embed) {
                theme_data_get = ewl_engine_hook_get(embed,
                                                EWL_ENGINE_HOOK_TYPE_THEME,
                                                EWL_ENGINE_THEME_DATA_GET);
                if (theme_data_get)
                        value = theme_data_get(ewl_theme_path_get(), key);
        }

        DRETURN_PTR(value, DLEVEL_STABLE);
}
#endif

/**
 * @return Returns a new theme object on success, NULL on failure
 * @brief Del a theme object.
 */
void
ewl_engine_theme_object_del(Ewl_Embed *embed, void *obj)
{
        Ewl_Engine_Cb_Theme_Object_Del theme_object_del;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        theme_object_del = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_OBJECT_DEL);
        if (theme_object_del)
                theme_object_del(obj);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Show a theme object.
 */
void
ewl_engine_theme_object_color_set(Ewl_Embed *embed, void *obj,
                Ewl_Color_Set *color)
{
        Ewl_Engine_Cb_Theme_Object_Color_Set color_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        color_set = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_OBJECT_COLOR_SET);
        if (color_set)
                color_set(obj, color);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @return Returns no value.
 * @brief Show a theme object.
 */
void
ewl_engine_theme_object_show(Ewl_Embed *embed, void *obj)
{
        Ewl_Engine_Cb_Theme_Object_Show theme_object_show;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        theme_object_show = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_OBJECT_SHOW);
        if (theme_object_show)
                theme_object_show(obj);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

#if 0
/**
 * @return Returns no value.
 * @brief Hide a theme object.
 */
void
ewl_engine_theme_object_hide(Ewl_Embed *embed, void *obj)
{
        Ewl_Engine_Cb_Theme_Object_Hide theme_object_hide;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        theme_object_hide = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_OBJECT_HIDE);
        if (theme_object_hide)
                theme_object_hide(obj);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}
#endif

/**
 * @return Returns no value.
 * @brief Move a theme object.
 */
void
ewl_engine_theme_object_move(Ewl_Embed *embed, void *obj, int x, int y)
{
        Ewl_Engine_Cb_Theme_Object_Move theme_object_move;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        theme_object_move = ewl_engine_hook_get(EWL_EMBED(embed),
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_OBJECT_MOVE);
        if (theme_object_move)
                theme_object_move(obj, x, y);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Resize a theme object.
 */
void
ewl_engine_theme_object_resize(Ewl_Embed *embed, void *obj, int width, int height)
{
        Ewl_Engine_Cb_Theme_Object_Resize theme_object_resize;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_WIDGET_TYPE);

        theme_object_resize = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_OBJECT_RESIZE);
        if (theme_object_resize)
                theme_object_resize(obj, width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @return Returns a new object group on success, NULL on failure
 * @brief Create a grouping for theme objects.
 */
void *
ewl_engine_theme_group_add(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Theme_Group_Add group_add;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        group_add = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_GROUP_ADD);
        if (group_add)
                DRETURN_PTR(group_add(embed), DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @return Returns a new theme object on success, NULL on failure
 * @brief Add a theme object.
 */
void *
ewl_engine_theme_element_add(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Theme_Element_Add element_add;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        element_add = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_ADD);
        if (element_add)
                DRETURN_PTR(element_add(embed), DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}



/**
 * @brief File set a theme element.
 */
unsigned int
ewl_engine_theme_element_file_set(Ewl_Embed *embed, void *obj, const char *file,
                                const char *group)
{
        Ewl_Engine_Cb_Theme_Element_File_Set file_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, FALSE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, FALSE);

        file_set = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_FILE_SET);
        if (file_set)
                DRETURN_INT(file_set(obj, file, group), DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @brief
 */
unsigned int
ewl_engine_theme_element_load_error_get(Ewl_Embed *embed, void *obj)
{
        Ewl_Engine_Cb_Theme_Element_Load_Error_Get error_get;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, TRUE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, TRUE);

        error_get = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_LOAD_ERROR_GET);
        if (error_get)
                DRETURN_INT(error_get(obj), DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @brief
 */
void
ewl_engine_theme_element_state_set(Ewl_Embed *embed, void *obj,
                const char *state)
{
        Ewl_Engine_Cb_Theme_Element_State_Set state_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        state_set = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_STATE_SET);
        if (state_set)
                state_set(obj, state);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief
 */
void
ewl_engine_theme_element_text_set(Ewl_Embed *embed, void *obj,
                const char *part, const char *text)
{
        Ewl_Engine_Cb_Theme_Element_Text_Set text_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        text_set = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_TEXT_SET);
        if (text_set)
                text_set(obj, part, text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief
 */
void
ewl_engine_theme_element_minimum_size_get(Ewl_Embed *embed, void *obj,
                int *w, int *h)
{
        Ewl_Engine_Cb_Theme_Element_Minimum_Size_Get min_get;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        min_get = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_MINIMUM_SIZE_GET);
        if (min_get)
                min_get(obj, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief
 */
void
ewl_engine_theme_element_minimum_size_calc(Ewl_Embed *embed, void *obj,
                int *w, int *h)
{
        Ewl_Engine_Cb_Theme_Element_Minimum_Size_Calc min_calc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        min_calc = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_MINIMUM_SIZE_CALC);
        if (min_calc)
                min_calc(obj, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief
 */
void
ewl_engine_theme_element_maximum_size_get(Ewl_Embed *embed, void *obj,
                int *w, int *h)
{
        Ewl_Engine_Cb_Theme_Element_Maximum_Size_Get max_get;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        max_get = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_MAXIMUM_SIZE_GET);
        if (max_get)
                max_get(obj, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief
 */
const char *
ewl_engine_theme_element_data_get(Ewl_Embed *embed, void *obj, const char *key)
{
        Ewl_Engine_Cb_Theme_Element_Data_Get data_get;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        data_get = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_DATA_GET);
        if (data_get)
                DRETURN_PTR(data_get(obj, key), DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @brief
 */
unsigned int
ewl_engine_theme_element_swallow(Ewl_Embed *embed, void *obj, void *sw)
{
        Ewl_Engine_Cb_Theme_Element_Swallow swallow;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, FALSE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, FALSE);

        swallow = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_SWALLOW);
        if (swallow)
                DRETURN_INT(swallow(obj, sw), DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @brief
 */
void *
ewl_engine_theme_element_unswallow(Ewl_Embed *embed, void *obj, void *swallow)
{
        Ewl_Engine_Cb_Theme_Element_Unswallow unswallow;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        unswallow = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_THEME,
                                        EWL_ENGINE_THEME_ELEMENT_UNSWALLOW);
        if (unswallow)
                DRETURN_PTR(unswallow(obj, swallow), DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}


/**
 * @return Returns a pointer id on success, zero on failure.
 * @brief Creates a new pointer from ARGB data.
 */
int
ewl_engine_pointer_data_new(Ewl_Embed *embed, unsigned int *data, int w, int h)
{
        Ewl_Engine_Cb_Pointer_Data_New pointer_data_new;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, 0);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, 0);

        pointer_data_new = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_POINTER,
                                        EWL_ENGINE_POINTER_DATA_NEW);
        if (pointer_data_new)
                DRETURN_INT(pointer_data_new(embed, data, w, h), DLEVEL_STABLE);

        DRETURN_INT(0, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Change the current pointer to the one identified.
 */
void
ewl_engine_pointer_set(Ewl_Embed *embed, int pointer)
{
        Ewl_Engine_Cb_Pointer_Set pointer_set;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        pointer_set = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_POINTER,
                                        EWL_ENGINE_POINTER_SET);
        if (pointer_set)
                pointer_set(embed, pointer);
        embed->cursor = pointer;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Free the identified pointer.
 */
void
ewl_engine_pointer_free(Ewl_Embed *embed, int pointer)
{
        Ewl_Engine_Cb_Pointer_Set pointer_free;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        pointer_free = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_POINTER,
                                        EWL_ENGINE_POINTER_FREE);
        if (pointer_free)
                pointer_free(embed, pointer);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a pointer id on success, zero on failure.
 * @brief Retrieve the currently used pointer.
 */
int
ewl_engine_pointer_get(Ewl_Embed *embed)
{
        Ewl_Engine_Cb_Pointer_Get pointer_get;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, 0);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, 0);

        pointer_get = ewl_engine_hook_get(embed,
                                        EWL_ENGINE_HOOK_TYPE_POINTER,
                                        EWL_ENGINE_POINTER_GET);
        if (pointer_get)
                DRETURN_INT(pointer_get(embed), DLEVEL_STABLE);

        DRETURN_INT(0, DLEVEL_STABLE);
}

static void **
ewl_engine_hooks_get(Ewl_Engine *engine, Ewl_Engine_Hook_Type type)
{
        void **hooks = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(engine, NULL);

        switch (type)
        {
                case EWL_ENGINE_HOOK_TYPE_WINDOW:
                        hooks = engine->functions->hooks.window;
                        break;

                case EWL_ENGINE_HOOK_TYPE_CANVAS:
                        hooks = engine->functions->hooks.canvas;
                        break;

                case EWL_ENGINE_HOOK_TYPE_THEME:
                        hooks = engine->functions->hooks.theme;
                        break;
                case EWL_ENGINE_HOOK_TYPE_POINTER:
                        hooks = engine->functions->hooks.pointer;
                        break;

                default:
                        DWARNING("Unknown hook type.");
                        DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(hooks, DLEVEL_STABLE);
}

/**
 * @param embed: The embed to lookup the engine chain for hook matching
 * @param type: The Ewl_Engine_Hook type to find the best matching function
 * @param hook: The hook to retrieve
 * @return Returns a pointer to the appropriate engine callback on success.
 * @brief Retrieves the correct engine callback for the specified type.
 */
static void *
ewl_engine_hook_get(Ewl_Embed *embed, Ewl_Engine_Hook_Type type, int hook)
{
        Ewl_Engine *caller;
        void *match = NULL;
        void **hooks = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        caller = EWL_ENGINE(embed->engine);

        /* bail if we don't have a valid engine */
        if (!caller)
        {
                DWARNING("Calling ewl_engine_hook_get without a valid engine.");
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        hooks = ewl_engine_hooks_get(caller, type);
        if ((!hooks || !hooks[hook]) && caller->dependancies)
        {
                Ecore_DList *deps;

                deps = caller->dependancies;
                ecore_dlist_first_goto(deps);
                while ((caller = ecore_dlist_next(deps)))
                {
                        hooks = ewl_engine_hooks_get(caller, type);
                        if (hooks && hooks[hook]) break;
                }
        }
        if (hooks) match = hooks[hook];

        DRETURN_PTR(match, DLEVEL_STABLE);
}

static void
ewl_engine_free(Ewl_Engine *engine)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(engine);

        if (engine->functions->shutdown)
                engine->functions->shutdown(engine);

        /* parents will be shutdown when their freed by the cleanup code */
        if (engine->dependancies)
                ecore_dlist_destroy(engine->dependancies);

        ecore_plugin_unload(engine->plugin);
        engine->plugin = NULL;

        FREE(engine);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

