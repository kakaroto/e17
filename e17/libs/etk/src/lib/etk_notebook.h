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
   Etk_Widget *frame;
};

Etk_Type *etk_notebook_type_get();
Etk_Widget *etk_notebook_new();

int etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label);

/** @} */

#endif
