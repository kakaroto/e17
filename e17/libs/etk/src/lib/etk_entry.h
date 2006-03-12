/** @file etk_entry.h */
#ifndef _ETK_ENTRY_H_
#define _ETK_ENTRY_H_

#include "etk_widget.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Entry Etk_Entry
 * @{
 */

/** @brief Gets the type of an entry */
#define ETK_ENTRY_TYPE       (etk_entry_type_get())
/** @brief Casts the object to an Etk_Entry */
#define ETK_ENTRY(obj)       (ETK_OBJECT_CAST((obj), ETK_ENTRY_TYPE, Etk_Entry))
/** @brief Checks if the object is an Etk_Entry */
#define ETK_IS_ENTRY(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ENTRY_TYPE))

/**
 * @struct Etk_Entry
 * @brief An Etk_Entry is a widget to edit single-line text 
 */
struct _Etk_Entry
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *editable_object;
   
   Etk_Bool     is_password;
   char        *password_text;
   char        *text;
};

Etk_Type *etk_entry_type_get();
Etk_Widget *etk_entry_new();

void        etk_entry_text_set(Etk_Entry *entry, const char *text);
const char *etk_entry_text_get(Etk_Entry *entry);
void        etk_entry_password_set(Etk_Entry *entry, Etk_Bool on);
Etk_Bool    etk_entry_password_get(Etk_Entry *entry);
  
  
/** @} */

#endif
