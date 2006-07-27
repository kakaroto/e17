/** @file etk_engine.c */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dlfcn.h>

#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>

#include "etk_engine.h"
#include "etk_utils.h"
#include "config.h"

/**
 * @addtogroup Etk_Engine
 * @{
 */
 
#define INHERIT(func) \
   if (!inherit_to->func) \
      inherit_to->func = inherit_from->func;

static void _etk_engine_inheritance_set(Etk_Engine *inherit_to, Etk_Engine *inherit_from);
 
static Etk_Engine *_engine = NULL;
static Evas_List *_loaded_engines = NULL;
static Evas_List *_etk_engines = NULL;

/**************************
 *
 * Implementation
 *
 **************************/
 
/**
 * @internal
 * @brief Initializes the engine system
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 * @warning You should not call it manually, it's called by etk_init()
 */
Etk_Bool etk_engine_init()
{
   char *file;
   Ecore_List *files;
   
   if (!ecore_file_exists(PACKAGE_LIB_DIR "/etk/engines"))
      return ETK_FALSE;
   
   files = ecore_file_ls(PACKAGE_LIB_DIR "/etk/engines");
   if (!files || (ecore_list_nodes(files) == 0))
     return ETK_FALSE;
   
   ecore_list_goto_first(files);
   while ((file = ecore_list_next(files)))
   {
      int len;
      
      len = strlen(file);
      if (len > 3 && strncmp(&file[len - 3], ".so", 3) == 0)
      {
	 file[len - 3] = '\0';
	 _etk_engines = evas_list_append(_etk_engines, strdup(file));
      }
   }
   ecore_list_destroy(files);
   
   return ETK_TRUE;
}

/**
 * @internal
 * @brief Shutdowns the engine system
 * @warning You should not call it manually, it's called by etk_shutdown()
 */
void etk_engine_shutdown()
{
   /* Shutdowns all the loaded engines */
   while (_loaded_engines)
   {
      Etk_Engine *engine;
      void *(*engine_close)();   
      engine = _loaded_engines->data;
      _loaded_engines = evas_list_remove(_loaded_engines, engine);
      
      if (engine->engine_shutdown)
         engine->engine_shutdown();

      if (engine->handle)
      {
         if ((engine_close = dlsym(engine->handle, "engine_close")))
            engine_close();
         dlclose(engine->handle);
      }
   }
   
   while (_etk_engines)
   {
      free(_etk_engines->data);
      _etk_engines = evas_list_remove_list(_etk_engines, _etk_engines);
   }
   
   _engine = NULL;
}

/**
 * @brief Gets the list of the available engines
 * @return Returns the list of the available engines
 * @warning The returned list should not be modified or freed
 */
Evas_List *etk_engine_list_get()
{
   return _etk_engines;
}

/**
 * @brief Checks whether the engine exists
 * @param engine_name the name of the engine to find
 * @return Return ETK_TRUE if the engine exists, ETK_FALSE otherwise
 */
Etk_Bool etk_engine_exists(const char *engine_name)
{
   Evas_List *l;
   
   for (l = _etk_engines; l; l = l->next)   
   {
      if (strcmp(l->data, engine_name) == 0)
         return ETK_TRUE;
   }

   return ETK_FALSE;
}

/**
 * @brief Gets the engine used by Etk
 * @return Returns the engine used by Etk or NULL if no engine is loaded yet
 */
Etk_Engine *etk_engine_get()
{
   return _engine;
}

/**
 * @internal
 * @brief Loads an engine. The loaded engine will automatically become the engine used by Etk
 * @param engine_name the name of the engine to load
 * @return Returns the loaded engine, or NULL if the engine could not be loaded
 * @warning You should not call this function yourself, etk_init() calls it automatically
 */
Etk_Engine *etk_engine_load(const char *engine_name)
{
   Etk_Engine *engine;
   Etk_Engine *(*engine_open)();   
   char filename[PATH_MAX];
   void *handle;
   
   if (!engine_name)
      return NULL;
   
   snprintf(filename, sizeof(filename), "%s/etk/engines/%s.so", PACKAGE_LIB_DIR, engine_name);
   
   if (!ecore_file_exists(filename))
   {
      ETK_WARNING("Etk can not find requested engine!");
      return NULL;
   }
   
   handle = dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
   if (!handle)
   {
      ETK_WARNING("Etk can not dlopen the requested engine!");
      return NULL;
   }

   engine_open = dlsym(handle, "engine_open");
   if (!engine_open)
   {
      ETK_WARNING("Etk can not find an open method for this engine!");
      dlclose(handle);
      return NULL;
   }
   
   if (!(engine = engine_open()))
   {
      ETK_WARNING("Etk can not open the requested engine!");
      dlclose(handle);
      return NULL;
   }
   
   if (engine->engine_init && !engine->engine_init())
   {
      ETK_WARNING("Etk can not initialize the requested engine!");
      dlclose(handle);
      return NULL;
   }
   
   _loaded_engines = evas_list_prepend(_loaded_engines, engine);
   engine->handle = handle;
   _engine = engine;
   
   return engine;
}

/**
 * @brief Makes the engine inherit from another exising engine: the methods of the base engine
 * will be used by the inheriting engine
 * @param engine the engine which will inherit from the methods of the base engine
 * @param inherit_name the name of the engine from which @a engine will inherit
 * @return Returns ETK_TRUE on success, ETK_FALSE otherwise
 */
Etk_Bool etk_engine_inherit_from(Etk_Engine *engine, const char *inherit_name)
{
   Etk_Engine *inherit_from;
   
   if (!engine || !inherit_name)
      return ETK_FALSE;
   
   if (!(inherit_from = etk_engine_load(inherit_name)))
     return ETK_FALSE;
   
   _etk_engine_inheritance_set(engine, inherit_from);
   return ETK_TRUE;
}

/**************************
 *
 * Etk Engine's methods
 *
 **************************/

void etk_engine_window_constructor(Etk_Window *window)
{
   if (!_engine || !_engine->window_constructor)
      return;
   _engine->window_constructor(window);
}

void etk_engine_window_destructor(Etk_Window *window)
{
   if (!_engine || !_engine->window_destructor)
      return;   
   _engine->window_destructor(window);
}

void etk_engine_window_show(Etk_Window *window)
{
   if (!_engine || !_engine->window_show)
      return;   
   _engine->window_show(window);
}

void etk_engine_window_hide(Etk_Window *window)
{
   if (!_engine || !_engine->window_hide)
      return;   
   _engine->window_hide(window);
}

Evas *etk_engine_window_evas_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_evas_get)
      return NULL;
   return _engine->window_evas_get(window);
}

void etk_engine_window_title_set(Etk_Window *window, const char *title)
{
   if (!_engine || !_engine->window_title_set)
      return;   
   _engine->window_title_set(window, title);
}

const char *etk_engine_window_title_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_title_get)
      return NULL;   
   return _engine->window_title_get(window);
}

void etk_engine_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class)
{
   if (!_engine || !_engine->window_wmclass_set)
      return;   
   _engine->window_wmclass_set(window, window_name, window_class);
}

void etk_engine_window_move(Etk_Window *window, int x, int y)
{
   if (!_engine || !_engine->window_move)
      return;   
   _engine->window_move(window, x, y);
}

void etk_engine_window_resize(Etk_Window *window, int w, int h)
{
   if (!_engine || !_engine->window_resize)
      return;   
   _engine->window_resize(window, w, h);
}

void etk_engine_window_size_min_set(Etk_Window *window, int w, int h)
{
   if (!_engine || !_engine->window_size_min_set)
      return;   
   _engine->window_size_min_set(window, w, h);
}

void etk_engine_window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
{
   if (!_engine || !_engine->window_geometry_get)
      return;   
   _engine->window_geometry_get(window, x, y, w, h);
}

void etk_engine_window_center_on_window(Etk_Window *window_to_center, Etk_Window *window)
{
   if (!_engine || !_engine->window_center_on_window)
      return;   
   _engine->window_center_on_window(window_to_center, window);
}

void etk_engine_window_move_to_mouse(Etk_Window *window)
{
   if (!_engine || !_engine->window_move_to_mouse)
      return;   
   _engine->window_move_to_mouse(window);
}

void etk_engine_window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window)
{
   if (!_engine || !_engine->window_modal_for_window)
      return;   
   _engine->window_modal_for_window(window_to_modal, window);
}

void etk_engine_window_iconify(Etk_Window *window)
{
   if (!_engine || !_engine->window_iconify)
      return;   
   _engine->window_iconify(window);
}

void etk_engine_window_deiconify(Etk_Window *window)
{
   if (!_engine || !_engine->window_deiconify)
      return;   
   _engine->window_deiconify(window);
}

Etk_Bool etk_engine_window_is_iconified(Etk_Window *window)
{
   if (!_engine || !_engine->window_is_iconified)
      return ETK_FALSE;
   return _engine->window_is_iconified(window);
}

void etk_engine_window_maximize(Etk_Window *window)
{
   if (!_engine || !_engine->window_maximize)
      return;   
   _engine->window_maximize(window);
}

void etk_engine_window_unmaximize(Etk_Window *window)
{
   if (!_engine || !_engine->window_unmaximize)
      return;   
   _engine->window_unmaximize(window);
}

Etk_Bool etk_engine_window_is_maximized(Etk_Window *window)
{
   if (!_engine || !_engine->window_is_maximized)
      return ETK_FALSE;   
   return _engine->window_is_maximized(window);
}

void etk_engine_window_fullscreen(Etk_Window *window)
{
   if (!_engine || !_engine->window_fullscreen)
      return;   
   _engine->window_fullscreen(window);
}

void etk_engine_window_unfullscreen(Etk_Window *window)
{
   if (!_engine || !_engine->window_unfullscreen)
      return;   
   _engine->window_unfullscreen(window);
}

Etk_Bool etk_engine_window_is_fullscreen(Etk_Window *window)
{
   if (!_engine || !_engine->window_is_fullscreen)
      return ETK_FALSE;   
   return _engine->window_is_fullscreen(window);
}

void etk_engine_window_raise(Etk_Window *window)
{
   if (!_engine || !_engine->window_raise)
      return;   
   _engine->window_raise(window);
}

void etk_engine_window_lower(Etk_Window *window)
{
   if (!_engine || !_engine->window_lower)
      return;   
   _engine->window_lower(window);
}

void etk_engine_window_sticky_set(Etk_Window *window, Etk_Bool on)
{
   if (!_engine || !_engine->window_sticky_set)
      return;   
   _engine->window_sticky_set(window, on);
}

Etk_Bool etk_engine_window_sticky_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_sticky_get)
      return ETK_FALSE;
   return _engine->window_sticky_get(window);
}

void etk_engine_window_focus(Etk_Window *window)
{
   if (!_engine || !_engine->window_focus)
      return;   
   _engine->window_focus(window);
}

void etk_engine_window_unfocus(Etk_Window *window)
{
   if (!_engine || !_engine->window_unfocus)
      return;   
   _engine->window_unfocus(window);
}

Etk_Bool etk_engine_window_is_focused(Etk_Window *window)
{
   if (!_engine || !_engine->window_is_focused)
      return ETK_FALSE;   
   return _engine->window_is_focused(window);
}

void etk_engine_window_decorated_set(Etk_Window *window, Etk_Bool decorated)
{
   if (!_engine || !_engine->window_decorated_set)
      return;   
   _engine->window_decorated_set(window, decorated);
}

Etk_Bool etk_engine_window_decorated_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_decorated_get)
      return ETK_FALSE;   
   return _engine->window_decorated_get(window);
}

void etk_engine_window_shaped_set(Etk_Window *window, Etk_Bool shaped)
{
   if (!_engine || !_engine->window_shaped_set)
      return;   
   _engine->window_shaped_set(window, shaped);
}

Etk_Bool etk_engine_window_shaped_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_shaped_get)
      return ETK_FALSE;   
   return _engine->window_shaped_get(window);
}

void etk_engine_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint)
{
   if (!_engine || !_engine->window_skip_taskbar_hint_set)
      return;   
   _engine->window_skip_taskbar_hint_set(window, skip_taskbar_hint);
}

Etk_Bool etk_engine_window_skip_taskbar_hint_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_skip_taskbar_hint_get)
      return ETK_FALSE;   
   return _engine->window_skip_taskbar_hint_get(window);
}

void etk_engine_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint)
{
   if (!_engine || !_engine->window_skip_pager_hint_set)
      return;   
   _engine->window_skip_pager_hint_set(window, skip_pager_hint);
}

Etk_Bool etk_engine_window_skip_pager_hint_get(Etk_Window *window)
{
   if (!_engine || !_engine->window_skip_pager_hint_get)
      return ETK_FALSE;   
   return _engine->window_skip_pager_hint_get(window);
}

void etk_engine_window_dnd_aware_set(Etk_Window *window, Etk_Bool on)
{
   if (!_engine || !_engine->window_dnd_aware_set)
      return;   
   _engine->window_dnd_aware_set(window, on);
}

void etk_engine_window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
   if (!_engine || !_engine->window_pointer_set)
      return;   
   _engine->window_pointer_set(toplevel_widget, pointer_type);
}

void etk_engine_popup_window_constructor(Etk_Popup_Window *popup_window)
{
   if (!_engine || !_engine->popup_window_constructor)
      return;   
   _engine->popup_window_constructor(popup_window);
}

void etk_engine_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
{
   if (!_engine || !_engine->popup_window_popup_at_xy)
      return;   
   _engine->popup_window_popup_at_xy(popup_window, x, y);
}

void etk_engine_popup_window_popup(Etk_Popup_Window *popup_window)
{
   if (!_engine || !_engine->popup_window_popup)
      return;   
   _engine->popup_window_popup(popup_window);
}

void etk_engine_popup_window_popdown(Etk_Popup_Window *popup_window)
{
   if (!_engine || !_engine->popup_window_popdown)
      return;   
   _engine->popup_window_popdown(popup_window);
}

Evas_List **etk_engine_popup_window_popped_get()
{
   if (!_engine || !_engine->popup_window_popped_get)
      return NULL;   
   return _engine->popup_window_popped_get();
}

void etk_engine_drag_constructor(Etk_Drag *drag)
{
   if (!_engine || !_engine->drag_constructor)
      return;
   return _engine->drag_constructor(drag);   
}
  
void etk_engine_drag_begin(Etk_Drag *drag)
{
   if (!_engine || !_engine->drag_begin)
      return;
   return _engine->drag_begin(drag);
}

Etk_Bool etk_engine_dnd_init()
{
   if (!_engine || !_engine->dnd_init)
           return ETK_FALSE;
   return _engine->dnd_init();
}

void etk_engine_dnd_shutdown()
{
   if (!_engine || !_engine->dnd_shutdown)
      return;
   _engine->dnd_shutdown();
}

void etk_engine_clipboard_text_request(Etk_Widget *widget)
{
   if (!_engine || !_engine->clipboard_text_request)
      return;
   _engine->clipboard_text_request(widget);
}

void etk_engine_clipboard_text_set(Etk_Widget *widget, const char *text, int length)
{
   if (!_engine || !_engine->clipboard_text_set)
      return;
   _engine->clipboard_text_set(widget, text, length);
}

void etk_engine_selection_text_request(Etk_Widget *widget)
{
   if (!_engine || !_engine->clipboard_text_request)
      return;
   _engine->selection_text_request(widget);
}

void etk_engine_selection_text_set(Etk_Widget *widget, const char *text, int length)
{
   if (!_engine || !_engine->clipboard_text_set)
      return;
   _engine->selection_text_set(widget, text, length);
}

void etk_engine_selection_clear()
{
   if (!_engine || !_engine->selection_clear)
      return;
   _engine->selection_clear();
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Copies the methods of "inherit_from" to "inherit_to" */
static void _etk_engine_inheritance_set(Etk_Engine *inherit_to, Etk_Engine *inherit_from)
{
   if (!inherit_to || !inherit_from)
      return;
   
   inherit_to->super = inherit_from;
   
   INHERIT(window_constructor);
   INHERIT(window_destructor);
   INHERIT(window_show);
   INHERIT(window_hide);
   INHERIT(window_evas_get);
   INHERIT(window_title_set);
   INHERIT(window_title_get);
   INHERIT(window_wmclass_set);
   INHERIT(window_move);
   INHERIT(window_resize);
   INHERIT(window_size_min_set);
   INHERIT(window_geometry_get);
   INHERIT(window_center_on_window);
   INHERIT(window_move_to_mouse);
   INHERIT(window_modal_for_window);
   INHERIT(window_iconify);
   INHERIT(window_deiconify);
   INHERIT(window_is_iconified);
   INHERIT(window_maximize);
   INHERIT(window_unmaximize);
   INHERIT(window_is_maximized);
   INHERIT(window_fullscreen);
   INHERIT(window_unfullscreen);
   INHERIT(window_is_fullscreen);
   INHERIT(window_raise);
   INHERIT(window_lower);   
   INHERIT(window_sticky_set);
   INHERIT(window_sticky_get);
   INHERIT(window_focus);
   INHERIT(window_unfocus);
   INHERIT(window_is_focused);
   INHERIT(window_decorated_set);
   INHERIT(window_decorated_get);
   INHERIT(window_shaped_set);
   INHERIT(window_shaped_get);
   INHERIT(window_skip_taskbar_hint_set);
   INHERIT(window_skip_taskbar_hint_get);
   INHERIT(window_skip_pager_hint_set);
   INHERIT(window_skip_pager_hint_get);
   INHERIT(window_dnd_aware_set);
   INHERIT(window_pointer_set);
   
   INHERIT(popup_window_constructor);
   INHERIT(popup_window_popup_at_xy);
   INHERIT(popup_window_popup);
   INHERIT(popup_window_popdown);
   INHERIT(popup_window_popped_get);
   
   INHERIT(drag_constructor);
   INHERIT(drag_begin);
   
   INHERIT(dnd_init);
   INHERIT(dnd_shutdown);
   
   INHERIT(clipboard_text_request);
   INHERIT(clipboard_text_set);
   
   INHERIT(selection_text_request);
   INHERIT(selection_text_set);
   INHERIT(selection_clear);
}

/** @} */
