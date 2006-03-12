/** @file etk_combobox.h */
#ifndef _ETK_COMBOBOX_H_
#define _ETK_COMBOBOX_H_

#include "etk_widget.h"
#include <stdarg.h>
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Combobox Etk_Combobox
 * @{
 */

/** @brief Gets the type of a combobox */
#define ETK_COMBOBOX_TYPE       (etk_combobox_type_get())
/** @brief Casts the object to an Etk_Combobox */
#define ETK_COMBOBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_TYPE, Etk_Combobox))
/** @brief Check if the object is an Etk_Combobox */
#define ETK_IS_COMBOBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_TYPE))

/** @brief Gets the type of a combobox item */
#define ETK_COMBOBOX_ITEM_TYPE       (etk_combobox_item_type_get())
/** @brief Casts the object to an Etk_Combobox_Item */
#define ETK_COMBOBOX_ITEM(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_ITEM_TYPE, Etk_Combobox_Item))
/** @brief Check if the object is an Etk_Combobox_Item */
#define ETK_IS_COMBOBOX_ITEM(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_ITEM_TYPE))

/* TODO: doc */
typedef enum _Etk_Combobox_Column_Type
{
   ETK_COMBOBOX_LABEL,
   ETK_COMBOBOX_IMAGE,
   ETK_COMBOBOX_OTHER
} Etk_Combobox_Column_Type;

/* TODO: doc */
struct _Etk_Combobox_Column
{
   Etk_Combobox_Column_Type type;
   Etk_Bool expand;
   int size;
};

/* TODO: doc */
struct _Etk_Combobox_Item
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Combobox *combobox;
   Etk_Widget **widgets;
   void *data;
};

/**
 * @struct Etk_Combobox
 * @param A combobox is a button that pops up a window with a list of options when you click on it
 */
struct _Etk_Combobox
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Widget *button;
   Etk_Widget *active_item_widget;
   Etk_Menu_Window *window;
   
   int num_cols;   
   Etk_Combobox_Column **cols;
   
   Evas_List *items;
   int item_height;
   
   Etk_Bool built;
};

Etk_Type *etk_combobox_type_get();
Etk_Type *etk_combobox_item_type_get();

Etk_Widget *etk_combobox_new();
Etk_Widget *etk_combobox_new_default();

void etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int size, Etk_Bool expand);
void etk_combobox_build(Etk_Combobox *combobox);

Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, void *data, ...);

/** @} */

#endif
