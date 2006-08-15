/** @file etk_notebook.h */
#ifndef _ETK_NOTEBOOK_H_
#define _ETK_NOTEBOOK_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/** TODO/FIXME list:
 * - Improve the default theme of the tabs
 * - Make the tab bar "swallowable"
 * - Fix focus problems
 * - Find a way to access clipped tabs when there are two many tabs (popup menus ?)
 */

/**
 * @defgroup Etk_Notebook Etk_Notebook
 * @brief The Etk_Notebook widget is a container that can contain several widgets in different pages
  * accessible through tabs
 * @{
 */

/** Gets the type of a notebook */
#define ETK_NOTEBOOK_TYPE       (etk_notebook_type_get())
/** Casts the object to an Etk_Notebook */
#define ETK_NOTEBOOK(obj)       (ETK_OBJECT_CAST((obj), ETK_NOTEBOOK_TYPE, Etk_Notebook))
/** Checks if the object is an Etk_Notebook */
#define ETK_IS_NOTEBOOK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_NOTEBOOK_TYPE))

/** @brief A page of a notebook */
typedef struct Etk_Notebook_Page
{
   /* private: */
   Etk_Widget *tab;
   Etk_Widget *frame;
} Etk_Notebook_Page;

/**
 * @brief @widget A container that can contain several widgets in different pages accessible through tabs
 * @structinfo
 */
struct Etk_Notebook
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;
   
   Etk_Widget *tab_bar;
   Etk_Bool tab_bar_focused;
   Etk_Bool tab_bar_visible;
   
   Evas_List *pages;
   Etk_Notebook_Page *current_page;
};

Etk_Type *etk_notebook_type_get();
Etk_Widget *etk_notebook_new();

int  etk_notebook_page_prepend(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child);
int  etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child);
int  etk_notebook_page_insert(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child, int position);
void etk_notebook_page_remove(Etk_Notebook *notebook, int page_num);

int  etk_notebook_num_pages_get(Etk_Notebook *notebook);
void etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num);
int  etk_notebook_current_page_get(Etk_Notebook *notebook);
int  etk_notebook_page_index_get(Etk_Notebook *notebook, Etk_Widget *child);

int etk_notebook_page_prev(Etk_Notebook *notebook);
int etk_notebook_page_next(Etk_Notebook *notebook);

void        etk_notebook_page_tab_label_set(Etk_Notebook *notebook, int page_num, const char *tab_label);
const char *etk_notebook_page_tab_label_get(Etk_Notebook *notebook, int page_num);
void        etk_notebook_page_tab_widget_set(Etk_Notebook *notebook, int page_num, Etk_Widget *tab_widget);
Etk_Widget *etk_notebook_page_tab_widget_get(Etk_Notebook *notebook, int page_num);
void        etk_notebook_page_child_set(Etk_Notebook *notebook, int page_num, Etk_Widget *child);
Etk_Widget *etk_notebook_page_child_get(Etk_Notebook *notebook, int page_num);

void     etk_notebook_tabs_visible_set(Etk_Notebook *notebook, Etk_Bool tabs_visible);
Etk_Bool etk_notebook_tabs_visible_get(Etk_Notebook *notebook);

/** @} */

#endif
