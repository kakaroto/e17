/** @file etk_notebook.c */
#include "etk_notebook.h"
#include <stdlib.h>
#include <string.h>
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_radio_button.h"

/**
 * @addtogroup Etk_Notebook
 * @{
 */

enum Etk_Notebook_Signal_Id
{
   ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL,
   ETK_NOTEBOOK_NUM_SIGNALS
};

static void _etk_notebook_constructor(Etk_Notebook *notebook);
static void _etk_notebook_destructor(Etk_Notebook *notebook);
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_notebook_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_notebook_tab_bar_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_notebook_tab_bar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_child_remove(Etk_Container *container, Etk_Widget *widget);
static Evas_List *_etk_notebook_children_get(Etk_Container *container);

static void _etk_notebook_tab_toggled_cb(Etk_Object *object, void *data);
static void _etk_notebook_tab_bar_focused_cb(Etk_Object *object, void *data);
static void _etk_notebook_tab_bar_unfocused_cb(Etk_Object *object, void *data);
static void _etk_notebook_tab_bar_key_down_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_notebook_tab_bar_mouse_wheel_cb(Etk_Object *object, void *event_info, void *data);

static void _etk_notebook_tab_bar_create(Etk_Notebook *notebook);
static Etk_Notebook_Page *_etk_notebook_page_create(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child);
static void _etk_notebook_page_switch(Etk_Notebook *notebook, Etk_Notebook_Page *page);

static Etk_Signal *_etk_notebook_signals[ETK_NOTEBOOK_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Notebook
 * @return Returns the type of an Etk_Notebook
 */
Etk_Type *etk_notebook_type_get()
{
   static Etk_Type *notebook_type = NULL;

   if (!notebook_type)
   {
      notebook_type = etk_type_new("Etk_Notebook", ETK_CONTAINER_TYPE, sizeof(Etk_Notebook),
         ETK_CONSTRUCTOR(_etk_notebook_constructor), ETK_DESTRUCTOR(_etk_notebook_destructor));
   
      _etk_notebook_signals[ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL] = etk_signal_new("current_page_changed",
         notebook_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return notebook_type;
}

/**
 * @brief Creates a new notebook widget
 * @return Returns the new notebook
 */
Etk_Widget *etk_notebook_new()
{
   return etk_widget_new(ETK_NOTEBOOK_TYPE, "theme_group", "notebook", NULL);
}

/**
 * @brief Creates a new page and prepends it to the notebook
 * @param notebook a notebook
 * @param tab_label the text to set to the tab's label
 * @param page_child the child of the new page. This widget will be shown when the page is set as the current one
 * @return Returns the position of the new page (0 since the page is prepended), or -1 on failure
 */
int etk_notebook_page_prepend(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child)
{
   Etk_Notebook_Page *new_page;
   
   if (!notebook)
      return -1;
   
   if (!(new_page = _etk_notebook_page_create(notebook, tab_label, page_child)))
      return -1;
   notebook->pages = evas_list_prepend(notebook->pages, new_page);
   return 0;
}

/**
 * @brief Creates a new page and appends it to the notebook
 * @param notebook a notebook
 * @param tab_label the text to set to the tab's label
 * @param page_child the child of the new page. This widget will be shown when the page is set as the current one
 * @return Returns the position of the new page (starting from 0), or -1 on failure
 */
int etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child)
{
   Etk_Notebook_Page *new_page;
   
   if (!notebook)
      return -1;
   
   if (!(new_page = _etk_notebook_page_create(notebook, tab_label, page_child)))
      return -1;
   notebook->pages = evas_list_append(notebook->pages, new_page);
   return evas_list_count(notebook->pages) - 1;
}

/**
 * @brief Creates a new page and inserts it to the notebook at the given position
 * @param notebook a notebook
 * @param tab_label the text to set to the tab's label
 * @param page_child the child of the new page. This widget will be shown when the page is set as the current one
 * @param position the position (0 being the first) where the new page will be inserted. @n
 * If @a position <= 0, the page will be prepended.
 * If @a position >= etk_notebook_num_pages_get(notebook), the page will be appended.
 * @return Returns the position of the new page (i.e. @a position in this case), or -1 on failure
 */
int etk_notebook_page_insert(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child, int position)
{
   Etk_Notebook_Page *new_page;
   
   if (!notebook)
      return -1;
   
   if (position < 0)
      return etk_notebook_page_prepend(notebook, tab_label, page_child);
   else if (position >= etk_notebook_num_pages_get(notebook))
      return etk_notebook_page_append(notebook, tab_label, page_child);
   else
   {
      if (!(new_page = _etk_notebook_page_create(notebook, tab_label, page_child)))
         return -1;
      notebook->pages = evas_list_prepend_relative(notebook->pages, new_page, evas_list_nth(notebook->pages, position));
   }
   return position;
}

/**
 * @brief Removes from the notebook the page corresponding to the index
 * @param notebook a notebook
 * @param page_num the index of the page to remove
 */
void etk_notebook_page_remove(Etk_Notebook *notebook, int page_num)
{
   Evas_List *l;
   Etk_Notebook_Page *page;
   
   if (!notebook || !(l = evas_list_nth_list(notebook->pages, page_num)))
      return;
     
   page = l->data;
   etk_widget_parent_set(page->frame, NULL);
   etk_object_destroy(ETK_OBJECT(page->frame));
   etk_widget_parent_set(page->tab, NULL);
   etk_object_destroy(ETK_OBJECT(page->tab));
   free(page);
   notebook->pages = evas_list_remove_list(notebook->pages, l);
   
   if (!notebook->pages)
   {
      notebook->current_page = NULL;
      ETK_WIDGET(notebook)->focus_order = evas_list_free(ETK_WIDGET(notebook)->focus_order);
      etk_signal_emit(_etk_notebook_signals[ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL], ETK_OBJECT(notebook), NULL);
   }
}

/**
 * @brief Gets the numbers of pages of the notebook
 * @param notebook a notebook
 * @return Returns the number of pages
 */
int etk_notebook_num_pages_get(Etk_Notebook *notebook)
{
   if (!notebook)
      return -1;
   return evas_list_count(notebook->pages);
}

/**
 * @brief Sets the current page of the notebook
 * @param notebook a notebook
 * @param page_num the index of the page to set as the current one
 */
void etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(page->tab), ETK_TRUE);
}

/**
 * @brief Gets the index of the current page of the notebook
 * @param notebook a notebook
 * @return Returns the index of the current page of the notebook, or -1 on failure
 */
int etk_notebook_current_page_get(Etk_Notebook *notebook)
{
   int i;
   Evas_List *l;
   
   if (!notebook || !notebook->current_page)
      return -1;
   
   for (l = notebook->pages, i = 0; l; l = l->next, i++)
   {
      if (notebook->current_page == (Etk_Notebook_Page *)l->data)
         return i;
   }
   return -1;
}

/**
 * @brief Gets the index of the page containing the child widget
 * @param notebook a notebook
 * @param child the child widget of the page which we want to get the index of
 * @return Returns the index of the page or -1 if the page is not found
 */
int etk_notebook_page_index_get(Etk_Notebook *notebook, Etk_Widget *child)
{
   Etk_Notebook_Page *page;
   Evas_List *l;
   int i;
   
   if (!notebook)
     return -1;
   
   for (l = notebook->pages, i = 0; l; l = l->next, i++)
   {      
      page = l->data;
      if (etk_bin_child_get(ETK_BIN(page->frame)) == child)
	return i;
   }
   
   return -1;
}

/**
 * @brief Switches to the previous page of the notebook
 * @param notebook a notebook
 * @return Returns the index of the previous page of the notebook (which is now the current page), or -1 on failure
 */
int etk_notebook_page_prev(Etk_Notebook *notebook)
{
   int current;
   
   if (!notebook || !notebook->current_page)
     return -1;
   
   current = etk_notebook_current_page_get(notebook);
   if (current >= 1)
   {
      etk_notebook_current_page_set(notebook, current - 1);
      return current - 1;
   }
   return -1;
}

/**
 * @brief Switches to the next page of the notebook
 * @param notebook a notebook
 * @return Returns the index of the next page of the notebook (which is now the current page), or -1 on failure
 */
int etk_notebook_page_next(Etk_Notebook *notebook)
{
   int current;
   
   if (!notebook || !notebook->current_page)
     return -1;
   
   current = etk_notebook_current_page_get(notebook);
   if (current < etk_notebook_num_pages_get(notebook) - 1)
   {
      etk_notebook_current_page_set(notebook, current + 1);
      return current + 1;
   }
   return -1;   
}

/**
 * @brief Sets the label of a tab of the notebook
 * @param notebook a notebook
 * @param page_num the index of the page that you want you set the tab label
 * @param tab_label the new label of the tab
 */
void etk_notebook_page_tab_label_set(Etk_Notebook *notebook, int page_num, const char *tab_label)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;   
   etk_button_label_set(ETK_BUTTON(page->tab), tab_label);
}

/**
 * @brief Gets the label of a tab of the notebook
 * @param notebook a notebook
 * @param page_num the index of the page that you want to get the tab label
 * @return Returns the label, or NULL on failure
 */
const char *etk_notebook_page_tab_label_get(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;

   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
     return NULL;      
   return etk_button_label_get(ETK_BUTTON(page->tab));
}

/**
 * @brief Sets the child of a tab of the notebook.
 * For example, you can set a hbox containing an icon, a text label and a close button
 * @param notebook a notebook
 * @param page_num the index of the page you want to set the tab widget
 * @param tab_widget the new widget to use as the child of the tab
 */
void etk_notebook_page_tab_widget_set(Etk_Notebook *notebook, int page_num, Etk_Widget *tab_widget)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_bin_child_set(ETK_BIN(page->tab), tab_widget);
}

/**
 * @brief Gets the child of a tab of the notebook
 * @param notebook a notebook
 * @param page_num the index of the page you want to get the tab widget
 * @return Returns the child of the tab, or NULL on failure
 */
Etk_Widget *etk_notebook_page_tab_widget_get(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return NULL;
   return etk_bin_child_get(ETK_BIN(page->tab));
}

/**
 * @brief Sets the child of a page of the notebook. This widget will be shown when the page is set as the current one
 * @param notebook a notebook
 * @param page_num the number of the page that you want to set the child
 * @param child the new widget to display when the corresponding tab is activated
 */
void etk_notebook_page_child_set(Etk_Notebook *notebook, int page_num, Etk_Widget *child)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_bin_child_set(ETK_BIN(page->frame), child);
}

/**
 * @brief Gets the child of a page of the notebook
 * @param notebook a notebook
 * @param page_num the number of the page that you want to get the child
 * @return Returns the child of the corresponding page, or NULL on failure
 */
Etk_Widget *etk_notebook_page_child_get(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return NULL;
   return etk_bin_child_get(ETK_BIN(page->frame));
}

/**
 * @brief Sets the visibility of the tab bar
 * @param notebook a notebook
 * @param show_tabs if show_tabs is ETK_FALSE, then the tab bar will be hidden
 */
void etk_notebook_show_tabs_set(Etk_Notebook *notebook, Etk_Bool show_tabs)
{
   if (show_tabs == ETK_FALSE)
     {
       etk_widget_hide(notebook->tab_bar);
     }
   else if (show_tabs == ETK_TRUE)
     {
       etk_widget_show(notebook->tab_bar);
     }
}

/**
 * @brief Gets the visibility of the tab bar
 * @param notebook a notebook
 * @return Returns ETK_TRUE if the tab bar is visible, ETK_FALSE otherwise
 */
Etk_Bool etk_notebook_show_tabs_get(Etk_Notebook *notebook)
{
   return etk_widget_is_visible(ETK_NOTEBOOK(notebook)->tab_bar);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the notebook */
static void _etk_notebook_constructor(Etk_Notebook *notebook)
{
   if (!notebook)
      return;

   notebook->pages = NULL;
   notebook->current_page = NULL;
   notebook->tab_bar_focused = ETK_FALSE;
   
   _etk_notebook_tab_bar_create(notebook);
   
   ETK_CONTAINER(notebook)->child_add = _etk_notebook_child_add;
   ETK_CONTAINER(notebook)->child_remove = _etk_notebook_child_remove;
   ETK_CONTAINER(notebook)->children_get = _etk_notebook_children_get;
   ETK_WIDGET(notebook)->size_request = _etk_notebook_size_request;
   ETK_WIDGET(notebook)->size_allocate = _etk_notebook_size_allocate;
   ETK_WIDGET(notebook)->use_focus_order = ETK_TRUE;
}

/* Destroys the notebook */
static void _etk_notebook_destructor(Etk_Notebook *notebook)
{
   Evas_List *l;
   Etk_Notebook_Page *page;
   
   if (!notebook)
      return;
   
   for (l = notebook->pages; l; l = l->next)
   {
      page = l->data;
      etk_signal_disconnect("toggled", ETK_OBJECT(page->tab), ETK_CALLBACK(_etk_notebook_tab_toggled_cb));
      free(page);
   }
}

/* Calculates the ideal size of the notebook */
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Notebook *notebook;
   Etk_Size frame_size;

   if (!(notebook = ETK_NOTEBOOK(widget)) || !size)
      return;
   
   etk_widget_size_request(notebook->tab_bar, size);
   if (notebook->current_page)
   {
      etk_widget_size_request(notebook->current_page->frame, &frame_size);
      if (size->w < frame_size.w)
         size->w = frame_size.w;
      size->h += frame_size.h;
   }

   size->w += 2 * etk_container_border_width_get(ETK_CONTAINER(notebook));
   size->h += 2 * etk_container_border_width_get(ETK_CONTAINER(notebook));
}

/* Resizes the notebook to the allocated size */
static void _etk_notebook_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Notebook *notebook;
   Etk_Container *notebook_container;
   Etk_Size tab_bar_size;
   Etk_Geometry tab_bar_geometry, frame_geometry;

   if (!(notebook = ETK_NOTEBOOK(widget)) || !notebook->current_page)
      return;

   notebook_container = ETK_CONTAINER(widget);
   geometry.x += etk_container_border_width_get(notebook_container);
   geometry.y += etk_container_border_width_get(notebook_container);
   geometry.w -= 2 * etk_container_border_width_get(notebook_container);
   geometry.h -= 2 * etk_container_border_width_get(notebook_container);
   
   etk_widget_size_request(notebook->tab_bar, &tab_bar_size);
   tab_bar_geometry.x = geometry.x;
   tab_bar_geometry.y = geometry.y;
   tab_bar_geometry.w = geometry.w;
   tab_bar_geometry.h = tab_bar_size.h;
   etk_widget_size_allocate(notebook->tab_bar, tab_bar_geometry);
   
   frame_geometry.x = geometry.x;
   frame_geometry.y = geometry.y + tab_bar_size.h;
   frame_geometry.w = geometry.w;
   frame_geometry.h = geometry.h - tab_bar_size.h;
   etk_widget_size_allocate(notebook->current_page->frame, frame_geometry);
}

/* Calculates the ideal size of the notebook's tab bar */
static void _etk_notebook_tab_bar_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Notebook *notebook;
   Etk_Notebook_Page *p;
   Evas_List *l;
   Etk_Size tab_size;
   
   if (!widget || !size)
      return;
   if (!(notebook = ETK_NOTEBOOK(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Notebook::Notebook"))))
      return;
   
   size->w = 0;
   size->h = 0;
   for (l = notebook->pages; l; l = l->next)
   {
      p = l->data;
      etk_widget_size_request(p->tab, &tab_size);
      size->w += tab_size.w;
      size->h = ETK_MAX(size->h, tab_size.h);
   }
}

/* Resizes the notebook's tab bar to the allocated size */
static void _etk_notebook_tab_bar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Notebook *notebook;
   Etk_Size requested_size;
   Etk_Size tab_size;
   Etk_Geometry tab_geometry;
   float ratio;
   Etk_Notebook_Page *p;
   Evas_List *l;
   
   if (!widget || !(notebook = ETK_NOTEBOOK(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Notebook::Notebook"))))
      return;
   
   etk_widget_size_request(widget, &requested_size);
   if (geometry.w >= requested_size.w)
      ratio = 1.0;
   else
      ratio = (float)requested_size.w / geometry.w;
   
   tab_geometry.x = geometry.x;
   tab_geometry.y = geometry.y;
   tab_geometry.h = geometry.h;
   
   for (l = notebook->pages; l; l = l->next)
   {
      p = l->data;
      etk_widget_size_request(p->tab, &tab_size);
      tab_geometry.w = tab_size.w * ratio;
      etk_widget_size_allocate(p->tab, tab_geometry);
      tab_geometry.x += tab_geometry.w;
   }
}

/* Adds a child to the notebook */
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget)
      return;
   etk_notebook_page_append(ETK_NOTEBOOK(container), _("Tab"), widget);
}

/* Removes a child from the notebook */
static void _etk_notebook_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Notebook *notebook;
   int page_index;

   if (!(notebook = ETK_NOTEBOOK(container)) || !widget)
      return;

   page_index = etk_notebook_page_index_get(notebook, widget);
   if (page_index >= 0)
      etk_notebook_page_remove(notebook, page_index);
}

/* Gets the children of the notebook */
static Evas_List *_etk_notebook_children_get(Etk_Container *container)
{
   Etk_Notebook *notebook;
   Etk_Notebook_Page *page;
   Evas_List *children, *l;
   
   if (!(notebook = ETK_NOTEBOOK(container)))
      return NULL;
   
   children = NULL;
   for (l = notebook->pages; l; l = l->next)
   {
      page = l->data;
      children = evas_list_append(children, etk_bin_child_get(ETK_BIN(page->frame)));
   }
   return children;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when a tab is toggled (activated or deactivated) */
static void _etk_notebook_tab_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Widget *tab;
   Etk_Notebook *notebook;
   Etk_Notebook_Page *page;
   
   if (!(tab = ETK_WIDGET(object)) || !(notebook = ETK_NOTEBOOK(data)))
      return;
   
   if (etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(tab)))
   {
      if ((page = etk_object_data_get(object, "_Etk_Notebook::Page")))
         _etk_notebook_page_switch(notebook, page);
   }
}

/* Called when the tab bar is focused */
static void _etk_notebook_tab_bar_focused_cb(Etk_Object *object, void *data)
{
   Etk_Notebook *notebook;
   
   if (!(notebook = ETK_NOTEBOOK(data)))
      return;
   
   if (notebook->current_page)
      etk_widget_theme_object_signal_emit(notebook->current_page->tab, "focus");
   notebook->tab_bar_focused = ETK_TRUE;
}

/* Called when the tab bar is unfocused */
static void _etk_notebook_tab_bar_unfocused_cb(Etk_Object *object, void *data)
{
   Etk_Notebook *notebook;
   
   if (!(notebook = ETK_NOTEBOOK(data)))
      return;
   
   if (notebook->current_page)
      etk_widget_theme_object_signal_emit(notebook->current_page->tab, "unfocus");
   notebook->tab_bar_focused = ETK_FALSE;
}

/* Called when a key is pressed, if the tab bar is focused */
static void _etk_notebook_tab_bar_key_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Notebook *notebook;
   Etk_Event_Key_Up_Down *event;
   
   if (!(notebook = ETK_NOTEBOOK(data)) || !(event = event_info))
      return;
   
   if (strcmp(event->keyname, "Left") == 0)
   {
      etk_notebook_page_prev(notebook);
      etk_signal_stop();
   }
   else if (strcmp(event->keyname, "Right") == 0)
   {
      etk_notebook_page_next(notebook);
      etk_signal_stop();
   }
}

/* Called when the user uses the mouse wheel over the tab bar */
static void _etk_notebook_tab_bar_mouse_wheel_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Notebook *notebook;
   Etk_Event_Mouse_Wheel *event;
   int page_index;
   
   if (!(notebook = ETK_NOTEBOOK(data)) || !(event = event_info))
      return;
   
   page_index = etk_notebook_current_page_get(notebook);
   page_index += event->z;
   page_index = ETK_CLAMP(page_index, 0, etk_notebook_num_pages_get(notebook) - 1);
   etk_notebook_current_page_set(notebook, page_index);
   etk_signal_stop();
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates the tab bar of the notebook */
static void _etk_notebook_tab_bar_create(Etk_Notebook *notebook)
{
   if (!notebook)
      return;
   
   notebook->tab_bar = etk_widget_new(ETK_WIDGET_TYPE, "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
   etk_widget_parent_set(notebook->tab_bar, ETK_WIDGET(notebook));
   etk_widget_visibility_locked_set(notebook->tab_bar, ETK_TRUE);
   etk_widget_show(notebook->tab_bar);
   
   etk_signal_connect("focus", ETK_OBJECT(notebook->tab_bar),
      ETK_CALLBACK(_etk_notebook_tab_bar_focused_cb), notebook);
   etk_signal_connect("unfocus", ETK_OBJECT(notebook->tab_bar),
      ETK_CALLBACK(_etk_notebook_tab_bar_unfocused_cb), notebook);
   etk_signal_connect("key_down", ETK_OBJECT(notebook->tab_bar),
      ETK_CALLBACK(_etk_notebook_tab_bar_key_down_cb), notebook);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(notebook->tab_bar),
      ETK_CALLBACK(_etk_notebook_tab_bar_mouse_wheel_cb), notebook);
   
   etk_object_data_set(ETK_OBJECT(notebook->tab_bar), "_Etk_Notebook::Notebook", notebook);
   notebook->tab_bar->size_request = _etk_notebook_tab_bar_size_request;
   notebook->tab_bar->size_allocate = _etk_notebook_tab_bar_size_allocate;
}

/* Creates a new page for the notebook */
static Etk_Notebook_Page *_etk_notebook_page_create(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child)
{
   Etk_Notebook_Page *new_page, *prev_page;
   
   if (!notebook)
      return NULL;
   
   new_page = malloc(sizeof(Etk_Notebook_Page));
   prev_page = notebook->pages ? notebook->pages->data : NULL;
   new_page->tab = etk_widget_new(ETK_RADIO_BUTTON_TYPE,
      "theme_group", "tab", "label", tab_label, "repeat_mouse_events", ETK_TRUE,
      "group", prev_page ? etk_radio_button_group_get(ETK_RADIO_BUTTON(prev_page->tab)) : NULL, NULL);
   etk_object_data_set(ETK_OBJECT(new_page->tab), "_Etk_Notebook::Page", new_page);
   etk_widget_parent_set(new_page->tab, ETK_WIDGET(notebook->tab_bar));
   etk_widget_visibility_locked_set(new_page->tab, ETK_TRUE);
   etk_widget_show(new_page->tab);
   etk_signal_connect("toggled", ETK_OBJECT(new_page->tab), ETK_CALLBACK(_etk_notebook_tab_toggled_cb), notebook);
   
   new_page->frame = etk_widget_new(ETK_BIN_TYPE, "theme_group", "frame", NULL);
   etk_widget_parent_set(new_page->frame, ETK_WIDGET(notebook));
   etk_widget_visibility_locked_set(new_page->frame, ETK_TRUE);
   etk_widget_hide(new_page->frame);
   
   etk_bin_child_set(ETK_BIN(new_page->frame), page_child);
   
   if (!notebook->current_page)
      _etk_notebook_page_switch(notebook, new_page);
   etk_widget_size_recalc_queue(ETK_WIDGET(notebook));
   
   return new_page;
}

/* Changes the active page of the notebook */
static void _etk_notebook_page_switch(Etk_Notebook *notebook, Etk_Notebook_Page *page)
{
   Evas_List *l;
   
   if (!notebook || !page || notebook->current_page == page)
      return;
   
   if (notebook->current_page)
   {
      etk_widget_hide(notebook->current_page->frame);
      if (notebook->tab_bar_focused)
         etk_widget_theme_object_signal_emit(notebook->current_page->tab, "unfocus");
   }
   
   ETK_WIDGET(notebook)->focus_order = evas_list_free(ETK_WIDGET(notebook)->focus_order);
   ETK_WIDGET(notebook)->focus_order = evas_list_append(ETK_WIDGET(notebook)->focus_order, notebook->tab_bar);
   ETK_WIDGET(notebook)->focus_order = evas_list_append(ETK_WIDGET(notebook)->focus_order, page->frame);
   
   /* Shows the page and updates the stacking of the tabs */
   etk_widget_show(page->frame);
   for (l = evas_list_last(notebook->pages); l; l = l->prev)
      etk_widget_raise(((Etk_Notebook_Page *)(l->data))->tab);
   etk_widget_raise(page->tab);
   
   if (notebook->tab_bar_focused)
      etk_widget_theme_object_signal_emit(page->tab, "focus");
   
   notebook->current_page = page;
   etk_signal_emit(_etk_notebook_signals[ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL], ETK_OBJECT(notebook), NULL);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Notebook
 *
 * @image html widgets/notebook.png
 * You can add a new page to the notebook with etk_notebook_page_prepend(),
 * etk_notebook_page_append() and etk_notebook_page_insert(). @n
 * A page can then be removed from the notebook with etk_notebook_page_remove().
 * 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Notebook
 *
 * \par Signals:
 * @signal_name "current_page_changed": Emitted when the active page of the notebook is changed
 * @signal_cb void callback(Etk_Notebook *notebook, void *data)
 * @signal_arg notebook: the notebook whose active page has been changed
 * @signal_data
 */
