/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_notebook.h */
#ifndef _ETK_NOTEBOOK_H_
#define _ETK_NOTEBOOK_H_

#include <Evas.h>

#include "etk_container.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

extern int ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL;

/**
 * @internal
 * @brief A page of a notebook
 * @structinfo
 */
typedef struct Etk_Notebook_Page
{
   /* private: */
   Etk_Widget *tab;
   Etk_Widget *frame;
   Etk_Widget *child;
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

   Evas_List *pages;
   Etk_Notebook_Page *current_page;

   Etk_Bool tab_bar_focused:1;
   Etk_Bool tab_bar_visible:1;
   Etk_Bool tab_bar_homogeneous:1;
};


Etk_Type   *etk_notebook_type_get(void);
Etk_Widget *etk_notebook_new(void);

int         etk_notebook_page_prepend(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child);
int         etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child);
int         etk_notebook_page_insert(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child, int position);
Etk_Widget *etk_notebook_page_remove(Etk_Notebook *notebook, int page_num);

int         etk_notebook_num_pages_get(Etk_Notebook *notebook);
void        etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num);
int         etk_notebook_current_page_get(Etk_Notebook *notebook);
int         etk_notebook_page_index_get(Etk_Notebook *notebook, Etk_Widget *child);

int         etk_notebook_page_prev(Etk_Notebook *notebook);
int         etk_notebook_page_next(Etk_Notebook *notebook);

void        etk_notebook_page_tab_label_set(Etk_Notebook *notebook, int page_num, const char *tab_label);
const char *etk_notebook_page_tab_label_get(Etk_Notebook *notebook, int page_num);
void        etk_notebook_page_tab_widget_set(Etk_Notebook *notebook, int page_num, Etk_Widget *tab_widget);
Etk_Widget *etk_notebook_page_tab_widget_get(Etk_Notebook *notebook, int page_num);
void        etk_notebook_page_child_set(Etk_Notebook *notebook, int page_num, Etk_Widget *child);
Etk_Widget *etk_notebook_page_child_get(Etk_Notebook *notebook, int page_num);

void        etk_notebook_tabs_visible_set(Etk_Notebook *notebook, Etk_Bool tabs_visible);
Etk_Bool    etk_notebook_tabs_visible_get(Etk_Notebook *notebook);
void        etk_notebook_tabs_homogeneous_set(Etk_Notebook *notebook, Etk_Bool tabs_homogeneous);
Etk_Bool    etk_notebook_tabs_homogeneous_get(Etk_Notebook *notebook);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
