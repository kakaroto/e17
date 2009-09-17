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

/** @file etk_entry.h */
#ifndef _ETK_ENTRY_H_
#define _ETK_ENTRY_H_

#include <Ecore.h>
#include <Ecore_IMF.h>
#include <Evas.h>

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

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
typedef enum
{
   ETK_ENTRY_IMAGE_PRIMARY,        /**< The image is primary, to the left of the editable object */
   ETK_ENTRY_IMAGE_SECONDARY       /**< The image is secondary, to the right of the editable object */
} Etk_Entry_Image_Position;

extern int ETK_ENTRY_TEXT_CHANGED_SIGNAL;
extern int ETK_ENTRY_TEXT_ACTIVATED_SIGNAL;

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

   Ecore_IMF_Context *imf_context;

   char *text;
   size_t text_limit;

   Etk_Color highlight_color;
   int image_interspace;

   Ecore_Event_Handler *imf_ee_handler_commit;
   Ecore_Event_Handler *imf_ee_handler_delete;

   Etk_Bool password_mode:1;
   Etk_Bool selection_dragging:1;
   Etk_Bool pointer_set:1;
   Etk_Bool primary_image_highlight:1;
   Etk_Bool secondary_image_highlight:1;
};


Etk_Type   *etk_entry_type_get(void);
Etk_Widget *etk_entry_new(void);

void        etk_entry_text_set(Etk_Entry *entry, const char *text);
const char *etk_entry_text_get(Etk_Entry *entry);
void        etk_entry_text_limit_set(Etk_Entry *entry, size_t limit);
size_t      etk_entry_text_limit_get(Etk_Entry *entry);
void        etk_entry_clear(Etk_Entry *entry);
void        etk_entry_image_set(Etk_Entry *entry, Etk_Entry_Image_Position position, Etk_Image *image);
Etk_Image  *etk_entry_image_get(Etk_Entry *entry, Etk_Entry_Image_Position position);
void        etk_entry_clear_button_add(Etk_Entry *entry);
void        etk_entry_image_highlight_set(Etk_Entry *entry, Etk_Entry_Image_Position position, Etk_Bool highlight);
void        etk_entry_password_mode_set(Etk_Entry *entry, Etk_Bool password_mode);
Etk_Bool    etk_entry_password_mode_get(Etk_Entry *entry);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
