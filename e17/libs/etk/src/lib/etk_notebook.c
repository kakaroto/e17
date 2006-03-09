/** @file etk_notebook.c */
#include "etk_notebook.h"
#include <stdlib.h>
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_radio_button.h"

/**
 * @addtogroup Etk_Notebook
 * @{
 */

enum _Etk_Notebook_Signal_Id
{
   ETK_NOTEBOOK_CURRENT_PAGE_CHANGED_SIGNAL,
   ETK_NOTEBOOK_NUM_SIGNALS
};

static void _etk_notebook_constructor(Etk_Notebook *notebook);
static void _etk_notebook_destructor(Etk_Notebook *notebook);
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_child_remove(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_notebook_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_notebook_tab_toggled_cb(Etk_Object *object, void *data);
static Etk_Notebook_Page *_etk_notebook_page_create(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget);
static void _etk_notebook_current_page_show(Etk_Notebook *notebook);

static Etk_Signal *_etk_notebook_signals[ETK_NOTEBOOK_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Notebook
 * @return Returns the type on an Etk_Notebook
 */
Etk_Type *etk_notebook_type_get()
{
   static Etk_Type *notebook_type = NULL;

   if (!notebook_type)
   {
      notebook_type = etk_type_new("Etk_Notebook", ETK_CONTAINER_TYPE, sizeof(Etk_Notebook), ETK_CONSTRUCTOR(_etk_notebook_constructor), ETK_DESTRUCTOR(_etk_notebook_destructor));
   
      _etk_notebook_signals[ETK_NOTEBOOK_CURRENT_PAGE_CHANGED_SIGNAL] = etk_signal_new("current_page_changed", notebook_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return notebook_type;
}

/**
 * @brief Creates a new notebook widget
 * @return Returns the new notebook
 */
Etk_Widget *etk_notebook_new()
{
   return etk_widget_new(ETK_NOTEBOOK_TYPE, NULL);
}

/**
 * @brief Creates a new page and appends it to the notebook
 * @param notebook a notebook
 * @param tab_label the text to display in the tab
 * @param page_widget the widget to display when the tab of the page is activated
 * @return Returns the place of the new page (starting from 0), or -1 if it failed
 */
int etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget)
{
   Etk_Notebook_Page *new_page;
   
   if (!notebook)
      return -1;
   
   if (!(new_page = _etk_notebook_page_create(notebook, tab_label, page_widget)))
      return -1;
   notebook->pages = evas_list_append(notebook->pages, new_page);
   return evas_list_count(notebook->pages) - 1;
}

/**
 * @brief Creates a new page and prepends it to the notebook
 * @param notebook a notebook
 * @param tab_label the text to display in the tab
 * @param page_widget the widget to display when the tab of the page is activated
 * @return Returns the place of the new page (will be always 0 since the page is prepended), or -1 if it failed
 */
int etk_notebook_page_prepend(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget)
{
   Etk_Notebook_Page *new_page;
   
   if (!notebook)
      return -1;
   
   if (!(new_page = _etk_notebook_page_create(notebook, tab_label, page_widget)))
      return -1;
   notebook->pages = evas_list_prepend(notebook->pages, new_page);
   return 0;
}

/**
 * @brief Delete the specified page of the notebook
 * @param notebook a notebook
 * @param page_num the number of the page to delete to
 */
void etk_notebook_page_remove(Etk_Notebook *notebook, int page_num)
{
   Evas_List *l;
   Etk_Notebook_Page *page;
   
   if (!notebook || !(l = evas_list_nth_list(notebook->pages, page_num)))
      return;
     
   page = l->data;
   etk_widget_parent_set(page->page_frame, NULL);
   etk_object_destroy(ETK_OBJECT(page->page_frame));
   etk_widget_parent_set(page->tab, NULL);
   etk_object_destroy(ETK_OBJECT(page->tab));
   free(page);
   notebook->pages = evas_list_remove_list(notebook->pages, l);
}

/**
 * @brief Gets the numbers of pages of the notebok
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
 * @brief Gets the label of the tab of a page of the notebook
 * @param notebook a notebook
 * @param page_num the number of the page to get the tab label to
 * @return Returns the label
 */
const char *etk_notebook_page_tab_label_get(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;

   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
     return NULL;
   if(!ETK_IS_BUTTON(page->tab))
     return NULL;       
   return etk_button_label_get(ETK_BUTTON(page->tab));
}

/**
 * @brief Sets the label of the tab of a page of the notebook
 * @param notebook a notebook
 * @param page_num the number of the page to set the tab label to
 * @param tab_label the new label of the tab
 */
void etk_notebook_page_tab_label_set(Etk_Notebook *notebook, int page_num, const char *tab_label)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   if(!ETK_IS_BUTTON(page->tab))
     return;   
   etk_button_label_set(ETK_BUTTON(page->tab), tab_label);
}

/**
 * @brief Sets the widget to use as child of a tab of the notebook. @n
 * You can for example set a hbox containing an icon, a text label and a close button
 * @param notebook a notebook
 * @param page_num the number of the page to set the tab widget to
 * @param tab_widget the new widget to use as child of the tab
 */
void etk_notebook_page_tab_widget_set(Etk_Notebook *notebook, int page_num, Etk_Widget *tab_widget)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_bin_child_set(ETK_BIN(page->tab), tab_widget);
}

/**
 * @brief Gets the widget used as child of a tab of the notebook.
 * @param notebook a notebook
 * @param page_num the number of the page to set the tab widget to
 * @return The widget used as child of the tab
 */
Etk_Widget *etk_notebook_page_tab_widget_get(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return NULL;
   return etk_bin_child_get(ETK_BIN(page->tab));
}

/**
 * @brief Sets the widget to display when the corresponding tab is activated
 * @param notebook a notebook
 * @param page_num the number of the page to set the page widget to
 * @param child the new widget to display when the corresponding tab is activated
 */
void etk_notebook_page_child_set(Etk_Notebook *notebook, int page_num, Etk_Widget *child)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_bin_child_set(ETK_BIN(page->page_frame), child);
}

/**
 * @brief Gets the widget displayed when the corresponding tab is activated
 * @param notebook a notebook
 * @param page_num the number of the page to set the page widget to
 * @return Widget to displayed when the corresponding tab is activated
 */
Etk_Widget *etk_notebook_page_child_get(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return NULL;
   return etk_bin_child_get(ETK_BIN(page->page_frame));
}

/**
 * @brief Gets the number of the page corresponding to the child widget
 * @param notebook a notebook
 * @param child the child widget of the page who's number we wish to get
 * @return The number of the page or -1 if the page is not found
 */
int etk_notebook_page_num_get(Etk_Notebook *notebook, Etk_Widget *child)
{
   Etk_Notebook_Page *page;
   Evas_List *l;
   int i = 0;
   
   if (!notebook)
     return -1;
   
   for(l = notebook->pages; l; l = l->next)
   {      
      page = l->data;
      if(etk_bin_child_get(ETK_BIN(page->page_frame)) == child)
	return i;
      ++i;
   }
   
   return -1;
}

/**
 * @brief Sets the current page of the notebook
 * @param notebook a notebook
 * @param page_num the number of the page to set as current
 */
void etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(page->tab), 1);
}

/**
 * @brief Gets the current page of the notebook
 * @param notebook a notebook
 * @return Returns the number of the current page of the notebook
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

int etk_notebook_prev_page(Etk_Notebook *notebook)
{
   int current;
   
   if (!notebook || !notebook->current_page)
     return -1;
   
   current = etk_notebook_current_page_get(notebook);
   if(current - 1 >= 0)
   {
      etk_notebook_current_page_set(notebook, current - 1);
      return current - 1;
   }
   return -1;
}

int etk_notebook_next_page(Etk_Notebook *notebook)
{
   int current;
   
   if (!notebook || !notebook->current_page)
     return -1;
   
   current = etk_notebook_current_page_get(notebook);
   if(current + 1 < etk_notebook_num_pages_get(notebook))
   {
      etk_notebook_current_page_set(notebook, current + 1);
      return current + 1;
   }
   return -1;   
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the notebook */
static void _etk_notebook_constructor(Etk_Notebook *notebook)
{
   if (!notebook)
      return;

   notebook->pages = NULL;
   notebook->current_page = NULL;
   
   ETK_CONTAINER(notebook)->child_add = _etk_notebook_child_add;
   ETK_CONTAINER(notebook)->child_remove = _etk_notebook_child_remove;
   ETK_WIDGET(notebook)->size_request = _etk_notebook_size_request;
   ETK_WIDGET(notebook)->size_allocate = _etk_notebook_size_allocate;
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
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Evas_List *l;
   Etk_Notebook *notebook;
   Etk_Notebook_Page *page;
   Etk_Size tab_requisition;
   Etk_Size frame_requisition;

   if (!(notebook = ETK_NOTEBOOK(widget)) || !size_requisition)
      return;
   
   size_requisition->w = 0;
   size_requisition->h = 0;
   
   for (l = notebook->pages; l; l = l->next)
   {
      page = l->data;
      etk_widget_size_request(page->tab, &tab_requisition);
      size_requisition->w += tab_requisition.w;
      if (size_requisition->h < tab_requisition.h)
         size_requisition->h = tab_requisition.h;
   }
   if (notebook->current_page)
   {
      etk_widget_size_request(notebook->current_page->page_frame, &frame_requisition);
      if (size_requisition->w < frame_requisition.w)
         size_requisition->w = frame_requisition.w;
      size_requisition->h += frame_requisition.h;
   }

   size_requisition->w += 2 * etk_container_border_width_get(ETK_CONTAINER(notebook));
   size_requisition->h += 2 * etk_container_border_width_get(ETK_CONTAINER(notebook));
}

/* Resizes the notebook to the size allocation */
static void _etk_notebook_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Evas_List *l;
   Etk_Notebook *notebook;
   Etk_Container *notebook_container;
   Etk_Notebook_Page *page;
   int max_tab_height = 0;
   int tabs_width = 0;
   Etk_Size tab_requisition;
   Etk_Geometry tab_geometry, frame_geometry;
   int tab_offset;

   if (!(notebook = ETK_NOTEBOOK(widget)) || !notebook->current_page)
      return;

   notebook_container = ETK_CONTAINER(widget);
   geometry.x += etk_container_border_width_get(notebook_container);
   geometry.y += etk_container_border_width_get(notebook_container);
   geometry.w -= 2 * etk_container_border_width_get(notebook_container);
   geometry.h -= 2 * etk_container_border_width_get(notebook_container);
   
   for (l = notebook->pages; l; l = l->next)
   {
      page = l->data;
      etk_widget_size_request(page->tab, &tab_requisition);
      tabs_width += tab_requisition.w;
      if (max_tab_height < tab_requisition.h);
         max_tab_height = tab_requisition.h;
   }
   
   tab_offset = 0;
   tab_geometry.y = geometry.y;
   tab_geometry.h = max_tab_height;
   for (l = notebook->pages; l; l = l->next)
   {
      page = l->data;
      etk_widget_size_request(page->tab, &tab_requisition);
      tab_geometry.x = geometry.x + tab_offset;
      tab_geometry.w = tab_requisition.w;
      etk_widget_size_allocate(page->tab, tab_geometry);
      tab_offset += tab_geometry.w;
   }
   
   frame_geometry.x = geometry.x;
   frame_geometry.y = geometry.y + max_tab_height;
   frame_geometry.w = geometry.w;
   frame_geometry.h = geometry.h - max_tab_height;
   etk_widget_size_allocate(notebook->current_page->page_frame, frame_geometry);
}

/* Adds a child to the notebook */
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget)
{
   if (!container || !widget)
      return;
   etk_notebook_page_append(ETK_NOTEBOOK(container), "Tab", widget);
}

/* Removes the child from the notebook */
static void _etk_notebook_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Notebook *notebook;
   Evas_List *l;
   Etk_Notebook_Page *page;

   if (!(notebook = ETK_NOTEBOOK(container)) || !widget)
      return;

   for (l = notebook->pages; l; l = l->next)
   {
      page = l->data;
      if (widget == etk_bin_child_get(ETK_BIN(page->page_frame)))
      {
         etk_widget_parent_set(widget, NULL);
         free(page);
         notebook->pages = evas_list_remove_list(notebook->pages, l);
         return;
      }
   }
   etk_widget_size_recalc_queue(ETK_WIDGET(notebook));
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
      {
         if (notebook->current_page)
            etk_widget_hide(notebook->current_page->page_frame);
         notebook->current_page = page;
         etk_signal_emit(_etk_notebook_signals[ETK_NOTEBOOK_CURRENT_PAGE_CHANGED_SIGNAL], ETK_OBJECT(notebook), NULL);
         _etk_notebook_current_page_show(notebook);
      }
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new page to the notebook */
static Etk_Notebook_Page *_etk_notebook_page_create(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget)
{
   Etk_Notebook_Page *new_page, *prev_page;
   
   if (!notebook)
      return NULL;
   
   new_page = malloc(sizeof(Etk_Notebook_Page));
   prev_page = notebook->pages ? notebook->pages->data : NULL;
   new_page->tab = etk_widget_new(ETK_RADIO_BUTTON_TYPE, "theme_group", "notebook_tab", "label", tab_label,
      "group", prev_page ? etk_radio_button_group_get(ETK_RADIO_BUTTON(prev_page->tab)) : NULL, NULL);
   etk_object_data_set(ETK_OBJECT(new_page->tab), "_Etk_Notebook::Page", new_page);
   etk_widget_parent_set(new_page->tab, ETK_WIDGET(notebook));
   etk_widget_visibility_locked_set(new_page->tab, ETK_TRUE);
   etk_widget_show(new_page->tab);
   etk_signal_connect("toggled", ETK_OBJECT(new_page->tab), ETK_CALLBACK(_etk_notebook_tab_toggled_cb), notebook);
   
   new_page->page_frame = etk_widget_new(ETK_BIN_TYPE, "theme_group", "notebook_frame", NULL);
   etk_widget_parent_set(new_page->page_frame, ETK_WIDGET(notebook));
   etk_widget_visibility_locked_set(new_page->page_frame, ETK_TRUE);
   etk_widget_hide(new_page->page_frame);
   
   etk_bin_child_set(ETK_BIN(new_page->page_frame), page_widget);
   
   if (!notebook->current_page)
   {
      notebook->current_page = new_page;
      etk_signal_emit(_etk_notebook_signals[ETK_NOTEBOOK_CURRENT_PAGE_CHANGED_SIGNAL], ETK_OBJECT(notebook), NULL);
      _etk_notebook_current_page_show(notebook);
   }
   etk_widget_size_recalc_queue(ETK_WIDGET(notebook));
   
   return new_page;
}

/* Shows the active page */
static void _etk_notebook_current_page_show(Etk_Notebook *notebook)
{
   Evas_List *l;
   Etk_Notebook_Page *p;
   
   if (!notebook || !notebook->current_page)
      return;
   
   etk_widget_show(notebook->current_page->page_frame);
   /* TODO: access to event object to raise */
   etk_widget_member_object_raise(ETK_WIDGET(notebook), notebook->current_page->page_frame->event_object);
   
   for (l = notebook->pages; l; l = l->next)
   {
      p = l->data;
      /* TODO: access to event object to raise */
      etk_widget_member_object_raise(ETK_WIDGET(notebook), p->tab->event_object);
   }
   /* TODO: access to event object to raise */
   etk_widget_member_object_raise(ETK_WIDGET(notebook), notebook->current_page->tab->event_object);         
   etk_widget_size_recalc_queue(ETK_WIDGET(notebook));
}

/** @} */
