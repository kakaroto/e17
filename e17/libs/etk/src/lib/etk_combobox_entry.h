/** @file etk_combobox_entry.h */
#ifndef _ETK_COMBOBOX_ENTRY_H_
#define _ETK_COMBOBOX_ENTRY_H_

#include <stdarg.h>

#include "etk_widget.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - combobox_entry item separator!
 * - selected/unselected signal for items
 * - Should items could have children (i.e sub-windows) ?
 * - make sure the items' children are not reparented
 * - etk_combobox_entry_items_height_set() should maybe use theme-data to calculate the right item-height
 */

/**
 * @defgroup Etk_Combobox_Entry Etk_Combobox_Entry
 * @brief The Etk_Combobox_Entry widget is made up of a button that shows a popup menu-when it is clicked,
 * allowing the user to choose an item from a list
 * @{
 */

/** Gets the type of a combobox_entry */
#define ETK_COMBOBOX_ENTRY_TYPE       (etk_combobox_entry_type_get())
/** Casts the object to an Etk_Combobox_Entry */
#define ETK_COMBOBOX_ENTRY(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_ENTRY_TYPE, Etk_Combobox_Entry))
/** Check if the object is an Etk_Combobox_Entry */
#define ETK_IS_COMBOBOX_ENTRY(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_ENTRY_TYPE))

/** Gets the type of a combobox_entry item */
#define ETK_COMBOBOX_ENTRY_ITEM_TYPE       (etk_combobox_entry_item_type_get())
/** Casts the object to an Etk_Combobox_Entry_Item */
#define ETK_COMBOBOX_ENTRY_ITEM(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_ENTRY_ITEM_TYPE, Etk_Combobox_Entry_Item))
/** Check if the object is an Etk_Combobox_Entry_Item */
#define ETK_IS_COMBOBOX_ENTRY_ITEM(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_ENTRY_ITEM_TYPE))

extern int ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_CHANGED_SIGNAL;

/** @brief The different types of widgets that can be contained by  a combobox_entry's column */
typedef enum
{
   ETK_COMBOBOX_ENTRY_LABEL,          /**< The column's widget is a label */
   ETK_COMBOBOX_ENTRY_IMAGE,          /**< The column's widget is an image */
   ETK_COMBOBOX_ENTRY_OTHER           /**< The column's widget is any other type of widget */
} Etk_Combobox_Entry_Column_Type;

/** @brief Indicates how a widget of a combobox_entry-column should fill and expand */
typedef enum
{
   ETK_COMBOBOX_ENTRY_NONE = 0,                       /* TODOC */
   ETK_COMBOBOX_ENTRY_EXPAND = 1 << 0,
   ETK_COMBOBOX_ENTRY_FILL = 1 << 1,
   ETK_COMBOBOX_ENTRY_EXPAND_FILL = ETK_COMBOBOX_ENTRY_EXPAND | ETK_COMBOBOX_ENTRY_FILL
} Etk_Combobox_Entry_Fill_Policy;


/**
 * @internal
 * @brief A column of a combobox_entry: a combobox_entry should have at least a column,
 * and each column contains a specific type of widget (see Etk_Combobox_Entry_Column_Type)
 */
struct Etk_Combobox_Entry_Column
{
   /* private: */
   Etk_Combobox_Entry_Column_Type type;
   Etk_Combobox_Entry_Fill_Policy fill_policy;
   int width;
   float align;
};

/**
 * @brief @widget A combobox_entry's item made up of a button that shows a popup-menu when
 * it is clicked, allowing the user to choose an item from a list
 * @structinfo
 */
struct Etk_Combobox_Entry_Item
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Combobox_Entry *combobox_entry;
   Etk_Combobox_Entry_Item *prev;
   Etk_Combobox_Entry_Item *next;

   Etk_Widget **widgets;
   void *data;
   void (*data_free_cb)(void *data);
};

/**
 * @brief @widget A combobox_entry is a widget
 * @structinfo
 */
struct Etk_Combobox_Entry
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *button;
   Etk_Popup_Window *window;
   int popup_offset_x;
   int popup_offset_y;
   int popup_extra_w;

   int num_cols;
   Etk_Combobox_Entry_Column **cols;

   Etk_Combobox_Entry_Item *first_item;
   Etk_Combobox_Entry_Item *last_item;

   Etk_Combobox_Entry_Item *selected_item;
   Etk_Combobox_Entry_Item *active_item;
   Etk_Widget *entry;
   Etk_Widget *hbox;

   int items_height;
   Etk_Bool built:1;
   
   int autosearch_enable;
   int autosearch_column;
};


Etk_Type          *etk_combobox_entry_type_get(void);
Etk_Type          *etk_combobox_entry_item_type_get(void);

Etk_Widget        *etk_combobox_entry_new(void);
Etk_Widget        *etk_combobox_entry_new_default(void);
Etk_Widget        *etk_combobox_entry_entry_get(Etk_Combobox_Entry *combobox_entry);
void               etk_combobox_entry_items_height_set(Etk_Combobox_Entry *combobox_entry, int items_height);
int                etk_combobox_entry_items_height_get(Etk_Combobox_Entry *combobox_entry);

void               etk_combobox_entry_column_add(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Column_Type col_type, int width, Etk_Combobox_Entry_Fill_Policy fill_policy, float align);
void               etk_combobox_entry_build(Etk_Combobox_Entry *combobox_entry);

Etk_Combobox_Entry_Item *etk_combobox_entry_item_prepend(Etk_Combobox_Entry *combobox_entry, ...);
inline Etk_Combobox_Entry_Item *etk_combobox_entry_item_prepend_empty(Etk_Combobox_Entry *combobox_entry);
Etk_Combobox_Entry_Item *etk_combobox_entry_item_append(Etk_Combobox_Entry *combobox_entry, ...);
inline Etk_Combobox_Entry_Item *etk_combobox_entry_item_append_empty(Etk_Combobox_Entry *combobox_entry);
Etk_Combobox_Entry_Item *etk_combobox_entry_item_insert(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *after, ...);
Etk_Combobox_Entry_Item *etk_combobox_entry_item_insert_empty(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *after);
Etk_Combobox_Entry_Item *etk_combobox_entry_item_insert_valist(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *after, va_list args);
void               etk_combobox_entry_item_remove(Etk_Combobox_Entry_Item *item);
void               etk_combobox_entry_clear(Etk_Combobox_Entry *combobox_entry);

void               etk_combobox_entry_item_fields_set(Etk_Combobox_Entry_Item *item, ...);
void               etk_combobox_entry_item_fields_set_valist(Etk_Combobox_Entry_Item *item, va_list args);
void               etk_combobox_entry_item_field_set(Etk_Combobox_Entry_Item *item, int column, void * value);
void               etk_combobox_entry_item_fields_get(Etk_Combobox_Entry_Item *item, ...);
void               etk_combobox_entry_item_fields_get_valist(Etk_Combobox_Entry_Item *item, va_list args);
void *             etk_combobox_entry_item_field_get(Etk_Combobox_Entry_Item *item, int column);

void               etk_combobox_entry_item_data_set(Etk_Combobox_Entry_Item *item, void *data);
void               etk_combobox_entry_item_data_set_full(Etk_Combobox_Entry_Item *item, void *data, void (*free_cb)(void *data));
void              *etk_combobox_entry_item_data_get(Etk_Combobox_Entry_Item *item);

void               etk_combobox_entry_active_item_set(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *item);
Etk_Combobox_Entry_Item *etk_combobox_entry_active_item_get(Etk_Combobox_Entry *combobox_entry);
int                etk_combobox_entry_active_item_num_get(Etk_Combobox_Entry *combobox_entry);

Etk_Combobox_Entry_Item *etk_combobox_entry_first_item_get(Etk_Combobox_Entry *combobox_entry);
Etk_Combobox_Entry_Item *etk_combobox_entry_last_item_get(Etk_Combobox_Entry *combobox_entry);
Etk_Combobox_Entry_Item *etk_combobox_entry_item_prev_get(Etk_Combobox_Entry_Item *item);
Etk_Combobox_Entry_Item *etk_combobox_entry_item_next_get(Etk_Combobox_Entry_Item *item);
Etk_Combobox_Entry_Item *etk_combobox_entry_nth_item_get(Etk_Combobox_Entry *combobox_entry, int index);
Etk_Combobox_Entry      *etk_combobox_entry_item_combobox_entry_get(Etk_Combobox_Entry_Item *item);

Etk_Bool etk_combobox_entry_is_popped_up(Etk_Combobox_Entry *combobox_entry);
void etk_combobox_entry_pop_up(Etk_Combobox_Entry *combobox_entry);
void etk_combobox_entry_pop_down(Etk_Combobox_Entry *combobox_entry);
void etk_combobox_entry_popup_feed(Etk_Combobox_Entry *combobox_entry, Etk_Window *window);

void etk_combobox_entry_autosearch_enable_set(Etk_Combobox_Entry *combobox_entry, int enable);
void etk_combobox_entry_autosearch_column_set(Etk_Combobox_Entry *combobox_entry, int col_num);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
