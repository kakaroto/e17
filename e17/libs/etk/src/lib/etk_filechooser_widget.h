/** @file etk_filechooser_widget.h */
#ifndef _ETK_FILECHOOSER_WIDGET_H_
#define _ETK_FILECHOOSER_WIDGET_H_

#include <Evas.h>
#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Filechooser_Widget Etk_Filechooser_Widget
 * @{
 */

/** @brief Gets the type of a status bar */
#define ETK_FILECHOOSER_WIDGET_TYPE       (etk_filechooser_widget_type_get())
/** @brief Casts the object to an Etk_Filechooser_Widget */
#define ETK_FILECHOOSER_WIDGET(obj)       (ETK_OBJECT_CAST((obj), ETK_FILECHOOSER_WIDGET_TYPE, Etk_Filechooser_Widget))
/** @brief Checks if the object is an Etk_Filechooser_Widget */
#define ETK_IS_FILECHOOSER_WIDGET(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_FILECHOOSER_WIDGET_TYPE))

/**
 * @struct Etk_Filechooser_Widget
 * @brief TODO
 */
struct _Etk_Filechooser_Widget
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;
   
   Etk_Widget *dir_tree;
   Etk_Tree_Col *dir_col;
   
   Etk_Widget *fav_tree;
   Etk_Tree_Col *fav_col;
   
   Etk_Widget *files_tree;
   Etk_Tree_Col *files_name_col;
   Etk_Tree_Col *files_date_col;
   
   char *current_folder;
};

Etk_Type *etk_filechooser_widget_type_get();
Etk_Widget *etk_filechooser_widget_new();

void etk_filechooser_widget_current_folder_set(Etk_Filechooser_Widget *filechooser_widget, const char *folder);

/** @} */

#endif
