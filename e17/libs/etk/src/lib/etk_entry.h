/** @file etk_entry.h */
#ifndef _ETK_ENTRY_H_
#define _ETK_ENTRY_H_

#include "etk_widget.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Entry Etk_Entry
 * @brief An Etk_Entry is a widget that allows the user to edit a single-line text
 * @{
 */

/** Gets the type of an entry */
#define ETK_ENTRY_TYPE       (etk_entry_type_get())
/** Casts the object to an Etk_Entry */
#define ETK_ENTRY(obj)       (ETK_OBJECT_CAST((obj), ETK_ENTRY_TYPE, Etk_Entry))
/** Checks if the object is an Etk_Entry */
#define ETK_IS_ENTRY(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ENTRY_TYPE))


/** @brief The position of the image in the entry */
typedef enum Etk_Entry_Image_Position
{
   ETK_ENTRY_IMAGE_PRIMARY,        /**< The image is primary, to the left of the editable object */
   ETK_ENTRY_IMAGE_SECONDARY       /**< The image is secondary, to the right of the editable object */
} Etk_Entry_Image_Position;


/**
 * @brief @widget A widget that allows the user to edit a single-line text
 * @structinfo
 */
struct Etk_Entry
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *internal_entry;
   Evas_Object *editable_object;
   Etk_Image *primary_image;
   Etk_Image *secondary_image;
   
   Etk_Bool password_mode;
   Etk_Bool selection_dragging;
   Etk_Bool pointer_set;
   Etk_Bool primary_image_highlight;
   Etk_Bool secondary_image_highlight;
   Etk_Color highlight_color;
   int image_interspace;

   char *text;
};


Etk_Type   *etk_entry_type_get(void);
Etk_Widget *etk_entry_new(void);

void        etk_entry_text_set(Etk_Entry *entry, const char *text);
const char *etk_entry_text_get(Etk_Entry *entry);
void        etk_entry_clear(Etk_Entry *entry);
void        etk_entry_image_set(Etk_Entry *entry, Etk_Entry_Image_Position position, Etk_Image *image);
Etk_Image  *etk_entry_image_get(Etk_Entry *entry, Etk_Entry_Image_Position position);
void        etk_entry_clear_button_add(Etk_Entry *entry);
void        etk_entry_image_highlight_set(Etk_Entry *entry, Etk_Entry_Image_Position position, Etk_Bool highlight);
void        etk_entry_password_mode_set(Etk_Entry *entry, Etk_Bool password_mode);
Etk_Bool    etk_entry_password_mode_get(Etk_Entry *entry);

/** @} */

#endif
