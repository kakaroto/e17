/** @file etk_filechooser_widget.h */
#ifndef _ETK_FILECHOOSER_WIDGET_H_
#define _ETK_FILECHOOSER_WIDGET_H_

#include <Evas.h>
#include "etk_widget.h"
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
 * @brief An Etk_Filechooser_Widget is a widget used to select several files
 */
struct _Etk_Filechooser_Widget
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Widget *hpaned;
   
   Etk_Widget *places_tree;
   Etk_Tree_Col *places_col;
   
   Etk_Widget *fav_tree;
   Etk_Tree_Col *fav_col;
   
   Etk_Widget *files_tree;
   Etk_Tree_Col *files_name_col;
   Etk_Tree_Col *files_date_col;   

   Etk_Bool select_multiple;
   Etk_Bool show_hidden;
   
   char *current_folder;
};

Etk_Type *etk_filechooser_widget_type_get();
Etk_Widget *etk_filechooser_widget_new();

void etk_filechooser_widget_select_multiple_set(Etk_Filechooser_Widget *filechooser_widget, Etk_Bool select_multiple);
Etk_Bool etk_filechooser_widget_select_multiple_get(Etk_Filechooser_Widget *filechooser_widget);

void etk_filechooser_widget_show_hidden_set(Etk_Filechooser_Widget *filechooser_widget, Etk_Bool show_hidden);
Etk_Bool etk_filechooser_widget_show_hidden_get(Etk_Filechooser_Widget *filechooser_widget);

void etk_filechooser_widget_current_folder_set(Etk_Filechooser_Widget *filechooser_widget, const char *folder);
const char *etk_filechooser_widget_current_folder_get(Etk_Filechooser_Widget *filechooser_widget);

/** @} */

#endif
