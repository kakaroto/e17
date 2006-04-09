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

/**
 * @enum Etk_Combobox_Column_Type
 * @brief The type of widget that can contain a column of a combobox
 */
typedef enum _Etk_Combobox_Column_Type
{
   ETK_COMBOBOX_LABEL,
   ETK_COMBOBOX_IMAGE,
   ETK_COMBOBOX_OTHER
} Etk_Combobox_Column_Type;

/**
 * @struct Etk_Combobox_Column
 * @brief A column of a combobox: a combobox should have at least a column, and each column contains a specific type of widgets
 */
struct _Etk_Combobox_Column
{
   Etk_Combobox_Column_Type type;
   Etk_Bool expand;
   Etk_Bool hfill;
   Etk_Bool vfill;
   int size;
   float xalign;
   float yalign;
};

/**
 * @struct Etk_Combobox_Item
 * @brief An item of a combobox
 */
struct _Etk_Combobox_Item
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Combobox *combobox;
   Etk_Widget **widgets;
   void *data;
   void (*data_free_cb)(void *data);
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
   Etk_Popup_Window *window;
   
   Etk_Combobox_Item *selected_item;
   Etk_Combobox_Item *active_item;
   Etk_Widget *active_item_widget;
   Etk_Widget **active_item_children;
   
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

void etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int size, Etk_Bool expand, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign);
void etk_combobox_build(Etk_Combobox *combobox);

void etk_combobox_active_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item);
Etk_Combobox_Item *etk_combobox_active_item_get(Etk_Combobox *combobox);

Etk_Combobox_Item *etk_combobox_item_prepend(Etk_Combobox *combobox, ...);
Etk_Combobox_Item *etk_combobox_item_prepend_valist(Etk_Combobox *combobox, va_list args);
Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, ...);
Etk_Combobox_Item *etk_combobox_item_append_valist(Etk_Combobox *combobox, va_list args);

Etk_Combobox_Item *etk_combobox_nth_item_get(Etk_Combobox *combobox, int index);
void etk_combobox_item_remove(Etk_Combobox *combobox, Etk_Combobox_Item *item);
void etk_combobox_clear(Etk_Combobox *combobox);

void etk_combobox_item_data_set(Etk_Combobox_Item *item, void *data);
void etk_combobox_item_data_set_full(Etk_Combobox_Item *item, void *data, void (*free_cb)(void *data));
void *etk_combobox_item_data_get(Etk_Combobox_Item *item);
void etk_combobox_item_activate(Etk_Combobox_Item *item);

/** @} */

#endif
