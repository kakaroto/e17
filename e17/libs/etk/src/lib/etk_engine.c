#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dlfcn.h>

#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Evas.h>

#include "config.h"
#include "etk_engine.h"
#include "etk_utils.h"

Etk_Engine *engine = NULL;
static Evas_List *etk_engines = NULL;

Etk_Bool etk_engine_init()
{
   char dir[PATH_MAX];
   char *file;
   Ecore_List *files;
   
   snprintf(dir, sizeof(dir), "%s/%s", PACKAGE_LIB_DIR, "etk/engines");
   if (!ecore_file_exists(dir))
     return ETK_FALSE;
   
   files = ecore_file_ls(dir);
   if (!files || (ecore_list_nodes(files) == 0))
     return ETK_FALSE;
   
   ecore_list_goto_first(files);
   while ((file = ecore_list_next(files)))
   {
      int len;
      
      len = strlen(file);
      if (!strncmp(file + (len - 3), ".so", 3))
      {
	 file[len - 3] = '\0';
	 etk_engines = evas_list_append(etk_engines, strdup(file));
      }
   }
   return ETK_TRUE;
}

Evas_List *etk_engine_list_get()
{
   return etk_engines;
}

Etk_Bool etk_engine_exists(const char *engine_name)
{
   Evas_List *l;
   
   for(l = etk_engine_list_get(); l; l = l->next)   
     if(!strcmp(l->data, engine_name))
       return ETK_TRUE;

   return ETK_FALSE;
}

Etk_Engine *etk_engine_load(const char *engine_name)
{
   Etk_Engine *engine;
   Etk_Engine *(*engine_init)(void);   
   char filename[PATH_MAX];
   void *handle;
   
   snprintf(filename, sizeof(filename), "%s/etk/engines/%s.so", 
	    PACKAGE_LIB_DIR, engine_name);
   
   if(!ecore_file_exists(filename))
   {
      ETK_WARNING("Etk can not find requested engine!");
      return ETK_FALSE;
   }
   
   handle = dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
   if(!handle)
   {
      ETK_WARNING("Etk can not dlopen requested engine!");
      return ETK_FALSE;
   }

   engine_init = dlsym(handle, "engine_init");
   if(!engine_init)
     {
	ETK_WARNING("Etk can not find an initializer for this engine!");
	return ETK_FALSE;
     }
   
   if((engine = engine_init()) == NULL)
     {
      ETK_WARNING("Etk can not initialize requested engine!");
      return ETK_FALSE;
   }
   
   return engine;
}

Etk_Engine *etk_engine_get()
{
   return engine;
}

Etk_Bool etk_engine_inherit_from(Etk_Engine *engine, const char * inherit_name)
{
   if(etk_engine_exists(inherit_name))
   {
      Etk_Engine *inherit_from;
      
      if((inherit_from = etk_engine_load(inherit_name)) == NULL)
	return ETK_FALSE;
      etk_engine_inheritence_set(engine, inherit_from);
      return ETK_TRUE;
   }
   return ETK_FALSE;
}

void etk_engine_inheritence_set(Etk_Engine *inherit_to, Etk_Engine *inherit_from)
{
   inherit_to->super = inherit_from;
   
#define INHERIT(func) \
   if(!inherit_to->func) \
       inherit_to->func = inherit_from->func;
   
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

void etk_engine_window_constructor(Etk_Window *window)
{
   if (!engine || !engine->window_constructor)
     return;
   engine->window_constructor(window);
}

void etk_engine_window_destructor(Etk_Window *window)
{
   if (!engine || !engine->window_destructor)
     return;   
   engine->window_destructor(window);
}

void etk_engine_window_show(Etk_Window *window)
{
   if (!engine || !engine->window_show)
     return;   
   engine->window_show(window);
}

void etk_engine_window_hide(Etk_Window *window)
{
   if (!engine || !engine->window_hide)
     return;   
   engine->window_hide(window);
}

Evas *etk_engine_window_evas_get(Etk_Window *window)
{
   if (!engine || !engine->window_evas_get)
     return NULL;
   return engine->window_evas_get(window);
}

void etk_engine_window_title_set(Etk_Window *window, const char *title)
{
   if (!engine || !engine->window_title_set)
     return;   
   engine->window_title_set(window, title);
}

const char *etk_engine_window_title_get(Etk_Window *window)
{
   if (!engine || !engine->window_title_get)
     return NULL;   
   return engine->window_title_get(window);
}

void etk_engine_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class)
{
   if (!engine || !engine->window_wmclass_set)
     return;   
   engine->window_wmclass_set(window, window_name, window_class);
}

void etk_engine_window_move(Etk_Window *window, int x, int y)
{
   if (!engine || !engine->window_move)
     return;   
   engine->window_move(window, x, y);
}

void etk_engine_window_resize(Etk_Window *window, int w, int h)
{
   if (!engine || !engine->window_resize)
     return;   
   engine->window_resize(window, w, h);
}

void etk_engine_window_size_min_set(Etk_Window *window, int w, int h)
{
   if (!engine || !engine->window_size_min_set)
     return;   
   engine->window_size_min_set(window, w, h);
}

void etk_engine_window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
{
   if (!engine || !engine->window_geometry_get)
     return;   
   engine->window_geometry_get(window, x, y, w, h);
}

void etk_engine_window_center_on_window(Etk_Window *window_to_center, Etk_Window *window)
{
   if (!engine || !engine->window_center_on_window)
     return;   
   engine->window_center_on_window(window_to_center, window);
}

void etk_engine_window_move_to_mouse(Etk_Window *window)
{
   if (!engine || !engine->window_move_to_mouse)
     return;   
   engine->window_move_to_mouse(window);
}

void etk_engine_window_modal_for_window(Etk_Window *window_to_modal, Etk_Window *window)
{
   if (!engine || !engine->window_modal_for_window)
     return;   
   engine->window_modal_for_window(window_to_modal, window);
}

void etk_engine_window_iconify(Etk_Window *window)
{
   if (!engine || !engine->window_iconify)
     return;   
   engine->window_iconify(window);
}

void etk_engine_window_deiconify(Etk_Window *window)
{
   if (!engine || !engine->window_deiconify)
     return;   
   engine->window_deiconify(window);
}

Etk_Bool etk_engine_window_is_iconified(Etk_Window *window)
{
   if (!engine || !engine->window_is_iconified)
     return ETK_FALSE;
   return engine->window_is_iconified(window);
}

void etk_engine_window_maximize(Etk_Window *window)
{
   if (!engine || !engine->window_maximize)
     return;   
   engine->window_maximize(window);
}

void etk_engine_window_unmaximize(Etk_Window *window)
{
   if (!engine || !engine->window_unmaximize)
     return;   
   engine->window_unmaximize(window);
}

Etk_Bool etk_engine_window_is_maximized(Etk_Window *window)
{
   if (!engine || !engine->window_is_maximized)
     return ETK_FALSE;   
   return engine->window_is_maximized(window);
}

void etk_engine_window_fullscreen(Etk_Window *window)
{
   if (!engine || !engine->window_fullscreen)
     return;   
   engine->window_fullscreen(window);
}

void etk_engine_window_unfullscreen(Etk_Window *window)
{
   if (!engine || !engine->window_unfullscreen)
     return;   
   engine->window_unfullscreen(window);
}

Etk_Bool etk_engine_window_is_fullscreen(Etk_Window *window)
{
   if (!engine || !engine->window_is_fullscreen)
     return ETK_FALSE;   
   return engine->window_is_fullscreen(window);
}

void etk_engine_window_raise(Etk_Window *window)
{
   if (!engine || !engine->window_raise)
     return;   
   engine->window_raise(window);
}

void etk_engine_window_lower(Etk_Window *window)
{
   if (!engine || !engine->window_lower)
     return;   
   engine->window_lower(window);
}

void etk_engine_window_sticky_set(Etk_Window *window, Etk_Bool on)
{
   if (!engine || !engine->window_sticky_set)
     return;   
   engine->window_sticky_set(window, on);
}

Etk_Bool etk_engine_window_sticky_get(Etk_Window *window)
{
   if (!engine || !engine->window_sticky_get)
     return;   
   return engine->window_sticky_get(window);
}

void etk_engine_window_focus(Etk_Window *window)
{
   if (!engine || !engine->window_focus)
     return;   
   engine->window_focus(window);
}

void etk_engine_window_unfocus(Etk_Window *window)
{
   if (!engine || !engine->window_unfocus)
     return;   
   engine->window_unfocus(window);
}

Etk_Bool etk_engine_window_is_focused(Etk_Window *window)
{
   if (!engine || !engine->window_is_focused)
     return ETK_FALSE;   
   return engine->window_is_focused(window);
}

void etk_engine_window_decorated_set(Etk_Window *window, Etk_Bool decorated)
{
   if (!engine || !engine->window_decorated_set)
     return;   
   engine->window_decorated_set(window, decorated);
}

Etk_Bool etk_engine_window_decorated_get(Etk_Window *window)
{
   if (!engine || !engine->window_decorated_get)
     return ETK_FALSE;   
   return engine->window_decorated_get(window);
}

void etk_engine_window_shaped_set(Etk_Window *window, Etk_Bool shaped)
{
   if (!engine || !engine->window_shaped_set)
     return;   
   engine->window_shaped_set(window, shaped);
}

Etk_Bool etk_engine_window_shaped_get(Etk_Window *window)
{
   if (!engine || !engine->window_shaped_get)
     return ETK_FALSE;   
   return engine->window_shaped_get(window);
}

void etk_engine_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint)
{
   if (!engine || !engine->window_skip_taskbar_hint_set)
     return;   
   engine->window_skip_taskbar_hint_set(window, skip_taskbar_hint);
}

Etk_Bool etk_engine_window_skip_taskbar_hint_get(Etk_Window *window)
{
   if (!engine || !engine->window_skip_taskbar_hint_get)
     return ETK_FALSE;   
   return engine->window_skip_taskbar_hint_get(window);
}

void etk_engine_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint)
{
   if (!engine || !engine->window_skip_pager_hint_set)
     return;   
   engine->window_skip_pager_hint_set(window, skip_pager_hint);
}

Etk_Bool etk_engine_window_skip_pager_hint_get(Etk_Window *window)
{
   if (!engine || !engine->window_skip_pager_hint_get)
     return ETK_FALSE;   
   return engine->window_skip_pager_hint_get(window);
}

void etk_engine_window_dnd_aware_set(Etk_Window *window, Etk_Bool on)
{
   if (!engine || !engine->window_dnd_aware_set)
     return;   
   engine->window_dnd_aware_set(window, on);
}

void etk_engine_window_pointer_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
   if (!engine || !engine->window_pointer_set)
     return;   
   engine->window_pointer_set(toplevel_widget, pointer_type);
}

void etk_engine_popup_window_constructor(Etk_Popup_Window *popup_window)
{
   if (!engine || !engine->popup_window_constructor)
     return;   
   engine->popup_window_constructor(popup_window);
}

void etk_engine_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
{
   if (!engine || !engine->popup_window_popup_at_xy)
     return;   
   engine->popup_window_popup_at_xy(popup_window, x, y);
}

void etk_engine_popup_window_popup(Etk_Popup_Window *popup_window)
{
   if (!engine || !engine->popup_window_popup)
     return;   
   engine->popup_window_popup(popup_window);
}

void etk_engine_popup_window_popdown(Etk_Popup_Window *popup_window)
{
   if (!engine || !engine->popup_window_popdown)
     return;   
   engine->popup_window_popdown(popup_window);
}

Evas_List **etk_engine_popup_window_popped_get()
{
   if (!engine || !engine->popup_window_popped_get)
     return NULL;   
   return engine->popup_window_popped_get();
}

void etk_engine_drag_constructor(Etk_Drag *drag)
{
   if (!engine || !engine->drag_constructor)
     return;
   return engine->drag_constructor(drag);   
}
  
void etk_engine_drag_begin(Etk_Drag *drag)
{
   if (!engine || !engine->drag_begin)
     return;
   return engine->drag_begin(drag);
}

Etk_Bool etk_engine_dnd_init()
{
   if (!engine || !engine->dnd_init)
          return ETK_FALSE;
   return engine->dnd_init();
}

void etk_engine_dnd_shutdown()
{
   if (!engine || !engine->dnd_shutdown)
     return;
   engine->dnd_shutdown();
}

void etk_engine_clipboard_text_request(Etk_Widget *widget)
{
   if (!engine || !engine->clipboard_text_request)
     return;
   engine->clipboard_text_request(widget);
}

void etk_engine_clipboard_text_set(Etk_Widget *widget, const char *text, int length)
{
   if (!engine || !engine->clipboard_text_set)
     return;
   engine->clipboard_text_set(widget, text, length);
}

void etk_engine_selection_text_request(Etk_Widget *widget)
{
   if (!engine || !engine->clipboard_text_request)
     return;
   engine->selection_text_request(widget);
}

void etk_engine_selection_text_set(Etk_Widget *widget, const char *text, int length)
{
   if (!engine || !engine->clipboard_text_set)
     return;
   engine->selection_text_set(widget, text, length);
}

void etk_engine_selection_clear()
{
   if (!engine || !engine->selection_clear)
     return;
   engine->selection_clear();
}
