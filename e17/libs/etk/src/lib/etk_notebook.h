/** @file etk_notebook.h */
#ifndef _ETK_NOTEBOOK_H_
#define _ETK_NOTEBOOK_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Notebook Etk_Notebook
 * @{
 */

/** @brief Gets the type of a notebook */
#define ETK_NOTEBOOK_TYPE       (etk_notebook_type_get())
/** @brief Casts the object to an Etk_Notebook */
#define ETK_NOTEBOOK(obj)       (ETK_OBJECT_CAST((obj), ETK_NOTEBOOK_TYPE, Etk_Notebook))
/** @brief Checks if the object is an Etk_Notebook */
#define ETK_IS_NOTEBOOK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_NOTEBOOK_TYPE))

/* A page of the notebook */
typedef struct _Etk_Notebook_Page
{
   Etk_Widget *tab;
   Etk_Widget *page_frame;
} Etk_Notebook_Page;

/**
 * @struct Etk_Notebook
 * @brief TODO
 */
struct _Etk_Notebook
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;
   
   Evas_List *pages;
   Etk_Notebook_Page *current_page;
};

Etk_Type *etk_notebook_type_get();
Etk_Widget *etk_notebook_new();

int etk_notebook_page_prepend(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget);
int etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_widget);
void etk_notebook_page_remove(Etk_Notebook *notebook, int page_num);

int etk_notebook_num_pages_get(Etk_Notebook *notebook);
int etk_notebook_prev_page(Etk_Notebook *notebook);
int etk_notebook_next_page(Etk_Notebook *notebook);

void etk_notebook_page_tab_label_set(Etk_Notebook *notebook, int page_num, const char *tab_label);
const char *etk_notebook_page_tab_label_get(Etk_Notebook *notebook, int page_num);

void etk_notebook_page_tab_widget_set(Etk_Notebook *notebook, int page_num, Etk_Widget *tab_widget);
Etk_Widget *etk_notebook_page_tab_widget_get(Etk_Notebook *notebook, int page_num);

void etk_notebook_page_child_set(Etk_Notebook *notebook, int page_num, Etk_Widget *child);

void etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num);
int etk_notebook_current_page_get(Etk_Notebook *notebook);

/** @} */

#endif
