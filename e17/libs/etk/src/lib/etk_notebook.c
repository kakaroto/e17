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

typedef struct _Etk_Notebook_Page
{
   Etk_Widget *tab;
   Etk_Widget *page_widget;
} Etk_Notebook_Page;

static void _etk_notebook_constructor(Etk_Notebook *notebook);
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_child_remove(Etk_Container *container, Etk_Widget *widget);
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_notebook_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

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
      notebook_type = etk_type_new("Etk_Notebook", ETK_CONTAINER_TYPE, sizeof(Etk_Notebook), ETK_CONSTRUCTOR(_etk_notebook_constructor), NULL);
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
int etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label)
{
   Etk_Notebook_Page *new_page, *prev_page;
   
   if (!notebook)
      return -1;
   
   new_page = malloc(sizeof(Etk_Notebook_Page));
   prev_page = notebook->pages ? notebook->pages->data : NULL;
   new_page->tab = etk_radio_button_new_with_label_from_widget(tab_label, prev_page ? ETK_RADIO_BUTTON(prev_page->tab) : NULL);
   etk_object_properties_set(ETK_OBJECT(new_page->tab), "theme_group", "notebook_tab", NULL);
   etk_widget_parent_set(new_page->tab, ETK_CONTAINER(notebook));
   etk_widget_show(new_page->tab);
   
   notebook->pages = evas_list_append(notebook->pages, new_page);
   
   return 1;
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
   notebook->frame = etk_widget_new(ETK_BIN_TYPE, "theme_group", "notebook_frame", NULL);
   etk_widget_parent_set(notebook->frame, ETK_CONTAINER(notebook));
   etk_widget_show(ETK_WIDGET(notebook));
   
   ETK_CONTAINER(notebook)->child_add = _etk_notebook_child_add;
   ETK_CONTAINER(notebook)->child_remove = _etk_notebook_child_remove;
   //ETK_WIDGET(notebook)->size_request = _etk_notebook_size_request;
   ETK_WIDGET(notebook)->size_allocate = _etk_notebook_size_allocate;
}

/* Calculates the ideal size of the notebook */
static void _etk_notebook_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   /*Etk_Notebook *notebook;
   Etk_Container *container;

   if (!(notebook = ETK_NOTEBOOK(widget)) || !size_requisition)
      return;

   container = ETK_CONTAINER(notebook);

   if (!notebook->child)
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
   else
      etk_widget_size_request(notebook->child, size_requisition);

   size_requisition->w += 2 * etk_container_border_width_get(container);
   size_requisition->h += 2 * etk_container_border_width_get(container);*/
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

   if (!(notebook = ETK_NOTEBOOK(widget)))
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
   
   tab_offset = (geometry.w - tabs_width) * 0.5;
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
   etk_widget_size_allocate(notebook->frame, frame_geometry);
}

/* Adds a child to the notebook */
static void _etk_notebook_child_add(Etk_Container *container, Etk_Widget *widget)
{
   /*if (!container || !widget)
      return;

   etk_notebook_child_set(ETK_NOTEBOOK(container), widget);*/
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


/** @} */
