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
 
static void _etk_notebook_constructor(Etk_Notebook *notebook);
static void _etk_notebook_destructor(Etk_Notebook *notebook);
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_child_remove(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_notebook_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_notebook_tab_toggled_cb(Etk_Object *object, void *data);
static Etk_Notebook_Page *_etk_notebook_page_create(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget);
static void _etk_notebook_current_page_show(Etk_Notebook *notebook);

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

/* TODO */
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

/* TODO */
void etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num)
{
   Etk_Notebook_Page *page;
   
   if (!notebook || !(page = evas_list_nth(notebook->pages, page_num)))
      return;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(page->tab), 1);
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
   
   /* TODO */
   //tab_offset = (geometry.w - tabs_width) * 0.5;
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
   /*Etk_Notebook *notebook;

   if (!(notebook = ETK_NOTEBOOK(container)) || !widget)
      return;

   if (notebook->child != widget)
      return;

   etk_widget_parent_set(widget, NULL);
   notebook->child = NULL;
   etk_widget_size_recalc_queue(ETK_WIDGET(notebook));*/
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
   etk_widget_parent_set(new_page->tab, ETK_CONTAINER(notebook));
   etk_widget_visibility_locked_set(new_page->tab, TRUE);
   etk_widget_show(new_page->tab);
   etk_signal_connect("toggled", ETK_OBJECT(new_page->tab), ETK_CALLBACK(_etk_notebook_tab_toggled_cb), notebook);
   
   new_page->page_frame = etk_widget_new(ETK_BIN_TYPE, "theme_group", "notebook_frame", NULL);
   etk_widget_parent_set(new_page->page_frame, ETK_CONTAINER(notebook));
   etk_widget_visibility_locked_set(new_page->page_frame, TRUE);
   etk_widget_hide(new_page->page_frame);
   
   new_page->page_widget = page_widget;
   if (new_page->page_widget)
      etk_bin_child_set(ETK_BIN(new_page->page_frame), new_page->page_widget);
   
   if (!notebook->current_page)
   {
      notebook->current_page = new_page;
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
   etk_widget_member_object_raise(ETK_WIDGET(notebook), notebook->current_page->page_frame->smart_object);
   
   for (l = notebook->pages; l; l = l->next)
   {
      p = l->data;
      etk_widget_member_object_raise(ETK_WIDGET(notebook), p->tab->smart_object);
   }
   evas_object_raise(notebook->current_page->tab->smart_object);
   etk_widget_member_object_raise(ETK_WIDGET(notebook), notebook->current_page->tab->smart_object);         
   etk_widget_size_recalc_queue(ETK_WIDGET(notebook));
}

/** @} */
