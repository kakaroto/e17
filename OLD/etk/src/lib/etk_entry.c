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

/** @file etk_entry.c */
#ifdef HAVE_CONFIG_H
#define _GNU_SOURCE
#include "config.h"
#endif

#include "etk_entry.h"

#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_IMF_Evas.h>

#include "etk_editable.h"
#include "etk_event.h"
#include "etk_image.h"
#include "etk_selection.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toplevel.h"
#include "etk_utils.h"

/* TODO: what if the image is reparented?! */

/**
 * @addtogroup Etk_Entry
 * @{
 */

#define IMAGE_SIZE 16

int ETK_ENTRY_TEXT_CHANGED_SIGNAL;
int ETK_ENTRY_TEXT_ACTIVATED_SIGNAL;

enum Etk_Entry_Propery_Id
{
   ETK_ENTRY_PASSWORD_MODE_PROPERTY
};

static void _etk_entry_constructor(Etk_Entry *entry);
static void _etk_entry_destructor(Etk_Entry *entry);
static void _etk_entry_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_entry_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_entry_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_entry_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_entry_internal_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_entry_internal_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static Etk_Bool _etk_entry_internal_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_entry_internal_unrealized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _etk_entry_editable_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_entry_editable_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_entry_editable_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_entry_editable_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_entry_editable_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static Etk_Bool _etk_entry_image_mouse_in_cb(Etk_Widget *widget, Etk_Event_Mouse_In *event, void *data);
static Etk_Bool _etk_entry_image_mouse_out_cb(Etk_Widget *widget, Etk_Event_Mouse_Out *event, void *data);
static Etk_Bool _etk_entry_image_mouse_down_cb(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data);
static Etk_Bool _etk_entry_image_mouse_up_cb(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_entry_clear_button_cb(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_entry_focused_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_entry_unfocused_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_entry_enabled_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_entry_disabled_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_entry_selection_received_cb(Etk_Object *object, void *event, void *data);
static void _etk_entry_selection_copy(Etk_Entry *entry, Etk_Selection_Type selection, Etk_Bool cut);

static int _etk_entry_imf_retrieve_surrounding_cb(void *data, Ecore_IMF_Context *ctx, char **text, int *cursor_pos);
static int _etk_entry_imf_event_commit_cb(void *data, int type, void *event);
static int _etk_entry_imf_event_delete_surrounding_cb(void *data, int type, void *event);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Entry
 * @return Returns the type of an Etk_Entry
 */
Etk_Type *etk_entry_type_get(void)
{
   static Etk_Type *entry_type = NULL;

   if (!entry_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_ENTRY_TEXT_CHANGED_SIGNAL,
                                    "text-changed", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_ENTRY_TEXT_ACTIVATED_SIGNAL,
                                    "text-activated", etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      entry_type = etk_type_new("Etk_Entry", ETK_WIDGET_TYPE,
         sizeof(Etk_Entry), ETK_CONSTRUCTOR(_etk_entry_constructor),
         ETK_DESTRUCTOR(_etk_entry_destructor), signals);

      etk_type_property_add(entry_type, "password-mode", ETK_ENTRY_PASSWORD_MODE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_FALSE));

      entry_type->property_set = _etk_entry_property_set;
      entry_type->property_get = _etk_entry_property_get;
   }

   return entry_type;
}

/**
 * @brief Creates a new entry
 * @return Returns the new entry widget
 */
Etk_Widget *etk_entry_new(void)
{
   return etk_widget_new(ETK_ENTRY_TYPE, "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Sets the text of the entry
 * @param entry an entry
 * @param text the text to set
 */
void etk_entry_text_set(Etk_Entry *entry, const char *text)
{
   char *text_tmp;

   if (!entry)
      return;

   if (!text)
      text_tmp = NULL;
   else if (entry->text_limit == 0)
      text_tmp = strdup(text);
   else
   {
      size_t limit = strlen(text);
      limit = ETK_MIN(entry->text_limit, limit);
      text_tmp = malloc(limit + 1);
      text_tmp[limit] = '\0';
      strncpy(text_tmp, text, limit);
   }

   if (!entry->editable_object && entry->text != text)
   {
      free(entry->text);
      entry->text = text_tmp;
   }
   else 
   {
      etk_editable_text_set(entry->editable_object, text_tmp);
      free(text_tmp);
   }

   etk_signal_emit(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(entry));
}

/**
 * @brief Gets the text of the entry
 * @param entry an entry
 * @return Returns the text of the entry
 */
const char *etk_entry_text_get(Etk_Entry *entry)
{
   if (!entry)
      return NULL;

   if (!entry->editable_object)
      return entry->text;
   else
      return etk_editable_text_get(entry->editable_object);
}


/**
 * @brief Set the text limit of the entry
 * @param entry an entry
 * @param limit the limit of text length, 0 means no limit
 */
void etk_entry_text_limit_set(Etk_Entry *entry, size_t limit) 
{
   if (!entry) 
      return;
   if (limit >= 0)
   {
      entry->text_limit = limit;
      return;
   }
   entry->text_limit = 0;
   return;
}

/**
 * @brief Get the text limit of the entry
 * @param entry an entry
 * @return Returns the limit of text entry, 0 means no limit. 
 */ 
size_t etk_entry_text_limit_get(Etk_Entry *entry) 
{
   if (!entry) return 0;
   return entry->text_limit;
}

/**
 * @brief Clears the text of the entry
 * @param entry the entry to clear
 */
void etk_entry_clear(Etk_Entry *entry)
{
   etk_entry_text_set(entry, NULL);
}

/**
 * @brief Sets an image inside the entry
 * @param entry an entry
 * @param position the position where to place the image: ETK_ENTRY_IMAGE_PRIMARY to place it on the left, and
 * ETK_ENTRY_IMAGE_SECONDARY to place it on the right
 * @param image an image
 * @note If there was an existing image already there, it will be destroyed. To avoid this, or if you just want to
 * remove the previous image, you hav to unparent it with etk_widget_parent_set(prev_image, NULL)
 * @note By default, the new image will be highlighted on mouse-over. You can change this behavior with
 * etk_entry_image_highlight_set()
 * @note The given image will be automatically shown
 */
void etk_entry_image_set(Etk_Entry *entry, Etk_Entry_Image_Position position, Etk_Image *image)
{
   if (!entry)
      return;

   if (position == ETK_ENTRY_IMAGE_PRIMARY)
   {
      if (entry->primary_image == image)
         return;

      if (entry->primary_image)
         etk_object_destroy(ETK_OBJECT(entry->primary_image));
      entry->primary_image = image;
   }
   else if (position == ETK_ENTRY_IMAGE_SECONDARY)
   {
      if (entry->secondary_image == image)
         return;

      if (entry->secondary_image)
         etk_object_destroy(ETK_OBJECT(entry->secondary_image));
      entry->secondary_image = image;
   }
   else
      return;

   if (image)
   {
      etk_widget_parent_set(ETK_WIDGET(image), entry->internal_entry);
      etk_widget_internal_set(ETK_WIDGET(image), ETK_TRUE);
      etk_widget_show(ETK_WIDGET(image));
      etk_entry_image_highlight_set(entry, position, ETK_TRUE);
   }
   etk_widget_size_recalc_queue(ETK_WIDGET(entry));
}

/**
 * @brief Gets the image of the entry
 * @param entry an entry
 * @param position the position of the image to get: ETK_ENTRY_IMAGE_PRIMARY for the left image,
 * ETK_ENTRY_IMAGE_SECONDARY for the right image
 * @return Returns the image of the entry
 */
Etk_Image *etk_entry_image_get(Etk_Entry *entry, Etk_Entry_Image_Position position)
{
   if (!entry)
      return NULL;

   if (position == ETK_ENTRY_IMAGE_PRIMARY)
      return entry->primary_image;
   else if (position == ETK_ENTRY_IMAGE_SECONDARY)
      return entry->secondary_image;

   return NULL;
}

/**
 * @brief Adds a "Clear" button on the right of the entry
 * @param entry an entry
 */
void etk_entry_clear_button_add(Etk_Entry *entry)
{
   Etk_Widget *image;

   if (!entry)
      return;

   image = etk_image_new_from_stock(ETK_STOCK_EDIT_CLEAR, ETK_STOCK_SMALL);
   etk_entry_image_set(entry, ETK_ENTRY_IMAGE_SECONDARY, ETK_IMAGE(image));
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_CLICK_SIGNAL, ETK_OBJECT(image), ETK_CALLBACK(_etk_entry_clear_button_cb), entry);
}

/**
 * @brief Sets whether the image will be highlighted on mouse-over
 * @param entry an entry
 * @param position the position of the image to set: ETK_ENTRY_IMAGE_PRIMARY for the left image,
 * ETK_ENTRY_IMAGE_SECONDARY for the right image
 * @param highlight if @a highlight is ETK_TRUE, the image will be highlighted
 * @note By default, the image has mouse highlight turned on
 */
void etk_entry_image_highlight_set(Etk_Entry *entry, Etk_Entry_Image_Position position, Etk_Bool highlight)
{
   Etk_Image *image;

   if (!entry)
      return;

   if (position == ETK_ENTRY_IMAGE_PRIMARY)
   {
      if (!(image = entry->primary_image))
         return;
      if (entry->primary_image_highlight == highlight)
         return;

      entry->primary_image_highlight = highlight;
   }
   else if (position == ETK_ENTRY_IMAGE_SECONDARY)
   {
      if (!(image = entry->secondary_image))
         return;
      if (entry->secondary_image_highlight == highlight)
         return;

      entry->secondary_image_highlight = highlight;
   }
   else
      return;

   if (highlight)
   {
      etk_signal_connect_by_code(ETK_WIDGET_MOUSE_IN_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_in_cb), entry);
      etk_signal_connect_by_code(ETK_WIDGET_MOUSE_OUT_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_out_cb), entry);
      etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_down_cb), entry);
      etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_up_cb), entry);
   }
   else
   {
      etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_IN_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_in_cb), entry);
      etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_OUT_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_out_cb), entry);
      etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_down_cb), entry);
      etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(image),
            ETK_CALLBACK(_etk_entry_image_mouse_up_cb), entry);

      etk_widget_color_set(ETK_WIDGET(image), 255, 255, 255, 255);
   }
}

/**
 * @brief Sets whether or not the entry is in password mode
 * @param entry an entry
 * @param password_mode ETK_TRUE to turn the entry into a password entry, ETK_FALSE to turn it into a normal entry
 */
void etk_entry_password_mode_set(Etk_Entry *entry, Etk_Bool password_mode)
{
   if (!entry || entry->password_mode == password_mode)
      return;

   if (entry->editable_object)
      etk_editable_password_mode_set(entry->editable_object, password_mode);
   entry->password_mode = password_mode;
   if (entry->imf_context)
      ecore_imf_context_input_mode_set(entry->imf_context,
            password_mode ? ECORE_IMF_INPUT_MODE_FULL & ECORE_IMF_INPUT_MODE_INVISIBLE :
                            ECORE_IMF_INPUT_MODE_FULL);
   etk_object_notify(ETK_OBJECT(entry), "password-mode");
}

/**
 * @brief Gets whether or not the entry is in password mode
 * @param entry an entry
 * @return Returns ETK_TRUE if the entry is in password mode, ETK_FALSE otherwise
 */
Etk_Bool etk_entry_password_mode_get(Etk_Entry *entry)
{
   if (!entry)
      return ETK_FALSE;
   return entry->password_mode;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the entry */
static void _etk_entry_constructor(Etk_Entry *entry)
{
   Etk_Widget *widget;

   if (!entry)
      return;

   if (!(widget = ETK_WIDGET(entry)))
      return;

   entry->editable_object = NULL;
   entry->primary_image = NULL;
   entry->secondary_image = NULL;
   entry->password_mode = ETK_FALSE;
   entry->selection_dragging = ETK_FALSE;
   entry->pointer_set = ETK_FALSE;
   entry->primary_image_highlight = ETK_FALSE;
   entry->secondary_image_highlight = ETK_FALSE;
   entry->imf_context = NULL;
   entry->imf_ee_handler_commit = NULL;
   entry->imf_ee_handler_delete = NULL;
   entry->text = NULL;
   entry->text_limit = 0;

   entry->internal_entry = etk_widget_new(ETK_WIDGET_TYPE, "repeat-mouse-events", ETK_TRUE,
         "theme-group", "entry", "theme-parent", entry, "parent", entry, "internal", ETK_TRUE, NULL);
   etk_widget_show(entry->internal_entry);
   etk_object_data_set(ETK_OBJECT(entry->internal_entry), "_Etk_Entry::Entry", entry);
   entry->internal_entry->size_request = _etk_entry_internal_size_request;
   entry->internal_entry->size_allocate = _etk_entry_internal_size_allocate;

   widget->size_request = _etk_entry_size_request;
   widget->size_allocate = _etk_entry_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(entry->internal_entry),
         ETK_CALLBACK(_etk_entry_internal_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(entry->internal_entry),
         ETK_CALLBACK(_etk_entry_internal_unrealized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(entry),
         ETK_CALLBACK(_etk_entry_key_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_FOCUSED_SIGNAL, ETK_OBJECT(entry),
         ETK_CALLBACK(_etk_entry_focused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNFOCUSED_SIGNAL, ETK_OBJECT(entry),
         ETK_CALLBACK(_etk_entry_unfocused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_ENABLED_SIGNAL, ETK_OBJECT(entry),
         ETK_CALLBACK(_etk_entry_enabled_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_DISABLED_SIGNAL, ETK_OBJECT(entry),
         ETK_CALLBACK(_etk_entry_disabled_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_SELECTION_RECEIVED_SIGNAL, ETK_OBJECT(entry),
         ETK_CALLBACK(_etk_entry_selection_received_cb), NULL);

}

/* Destroys the entry */
static void _etk_entry_destructor(Etk_Entry *entry)
{
   if (!entry)
      return;
   free(entry->text);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_entry_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_ENTRY_PASSWORD_MODE_PROPERTY:
         etk_entry_password_mode_set(entry, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_entry_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_ENTRY_PASSWORD_MODE_PROPERTY:
         etk_property_value_bool_set(value, entry->password_mode);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the entry */
static void _etk_entry_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(widget)))
      return;
   etk_widget_size_request(entry->internal_entry, size);
}

/* Resizes the entry to the allocated size */
static void _etk_entry_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Entry *entry;
   Etk_Size requested_size;

   if (!(entry = ETK_ENTRY(widget)))
      return;

   etk_widget_size_request(entry->internal_entry, &requested_size);
   geometry.y = geometry.y + (geometry.h - requested_size.h) / 2;
   geometry.h = requested_size.h;
   etk_widget_size_allocate(entry->internal_entry, geometry);
}

/* Calculates the ideal size of the entry's internal widget */
static void _etk_entry_internal_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Entry *entry;

   if (!widget || !size)
      return;
   if (!(entry = ETK_ENTRY(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Entry::Entry"))))
      return;

   if (!entry->editable_object)
   {
      size->w = 0;
      size->h = 0;
   }
   else
   {
      int cw, ch;

      etk_editable_char_size_get(entry->editable_object, &cw, &ch);
      size->w = 15 * cw;
      size->h = ETK_MAX(ch, IMAGE_SIZE);

      if (entry->primary_image)
         size->w += IMAGE_SIZE + entry->image_interspace;
      if (entry->secondary_image)
         size->w += IMAGE_SIZE + entry->image_interspace;
   }
}

/* Resizes the entry's internal widget to the allocated size */
static void _etk_entry_internal_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Entry *entry;

   if (!widget || !(entry = ETK_ENTRY(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Entry::Entry"))))
      return;

   if (!entry->primary_image && !entry->secondary_image)
   {
      evas_object_move(entry->editable_object, geometry.x, geometry.y);
      evas_object_resize(entry->editable_object, geometry.w, geometry.h);
   }
   else
   {
      Etk_Image *image;
      Etk_Geometry i_geometry;

      i_geometry.w = i_geometry.h = ETK_MIN(IMAGE_SIZE, geometry.h);
      i_geometry.y = geometry.y + (geometry.h - i_geometry.h) / 2;

      if (entry->primary_image)
      {
         image = entry->primary_image;
         i_geometry.x = geometry.x;

         etk_widget_size_allocate(ETK_WIDGET(image), i_geometry);
         geometry.x += i_geometry.w + entry->image_interspace;
         geometry.w -= i_geometry.w + entry->image_interspace;
      }
      if (entry->secondary_image)
      {
         image = entry->secondary_image;
         i_geometry.x = geometry.x + geometry.w - i_geometry.w;

         etk_widget_size_allocate(ETK_WIDGET(image), i_geometry);
         geometry.w -= i_geometry.w + entry->image_interspace;
      }

      evas_object_move(entry->editable_object, geometry.x, geometry.y);
      evas_object_resize(entry->editable_object, geometry.w, geometry.h);
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the entry's internal widget is realized */
static Etk_Bool _etk_entry_internal_realized_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;
   Etk_Widget *internal_entry;
   Evas *evas;
   const char *ctx_id;
   const Ecore_IMF_Context_Info *ctx_info;
   char *text_tmp;

   if (!(internal_entry = ETK_WIDGET(object)) || !(evas = etk_widget_toplevel_evas_get(internal_entry)))
      return ETK_TRUE;
   if (!(entry = ETK_ENTRY(etk_object_data_get(object, "_Etk_Entry::Entry"))))
      return ETK_TRUE;

   ctx_id = ecore_imf_context_default_id_get();
   if (ctx_id)
   {
      ctx_info = ecore_imf_context_info_by_id_get(ctx_id);
      if (!ctx_info->canvas_type ||
          strcmp(ctx_info->canvas_type, "evas") == 0)
         entry->imf_context = ecore_imf_context_add(ctx_id);
      else
      {
         ctx_id = ecore_imf_context_default_id_by_canvas_type_get("evas");
         if (ctx_id)
            entry->imf_context = ecore_imf_context_add(ctx_id);
      }
   }
   else
      entry->imf_context = NULL;

   if (entry->imf_context)
   {
      ecore_imf_context_client_window_set(entry->imf_context,
            ecore_evas_window_get(ecore_evas_ecore_evas_get(evas)));
      ecore_imf_context_client_canvas_set(entry->imf_context, evas);
      ecore_imf_context_retrieve_surrounding_callback_set(entry->imf_context,
            _etk_entry_imf_retrieve_surrounding_cb, entry);
      entry->imf_ee_handler_commit = ecore_event_handler_add(
         ECORE_IMF_EVENT_COMMIT, _etk_entry_imf_event_commit_cb, entry);
      entry->imf_ee_handler_delete = ecore_event_handler_add(
         ECORE_IMF_EVENT_DELETE_SURROUNDING,
         _etk_entry_imf_event_delete_surrounding_cb, entry);

      ecore_imf_context_input_mode_set(entry->imf_context,
            entry->password_mode ? ECORE_IMF_INPUT_MODE_INVISIBLE :
                                   ECORE_IMF_INPUT_MODE_FULL);
   }

   entry->editable_object = etk_editable_add(evas);
   evas_object_show(entry->editable_object);
   etk_widget_member_object_add(internal_entry, entry->editable_object);

   etk_editable_theme_set(entry->editable_object, etk_widget_theme_file_get(internal_entry),
         etk_widget_theme_group_get(internal_entry));
   if (entry->text_limit == 0) 
      etk_editable_text_set(entry->editable_object, entry->text);
   else 
   {
      if (!entry->text)
         text_tmp = NULL;
      else
      {
         size_t limit = strlen(entry->text);
         limit = ETK_MIN(entry->text_limit, limit);
         text_tmp = malloc(limit+1);
         text_tmp[limit] = '\0';
         strncpy(text_tmp, entry->text, limit);
      }
      etk_editable_text_set(entry->editable_object, text_tmp);
      free(text_tmp);
   }
   etk_editable_password_mode_set(entry->editable_object, entry->password_mode);

   if (!etk_widget_is_focused(ETK_WIDGET(entry)))
   {
      etk_editable_cursor_hide(entry->editable_object);
      etk_editable_selection_hide(entry->editable_object);
   }

   if (etk_widget_disabled_get(ETK_WIDGET(entry)))
      etk_editable_disabled_set(entry->editable_object, ETK_TRUE);

   evas_object_event_callback_add(entry->editable_object, EVAS_CALLBACK_MOUSE_IN,
         _etk_entry_editable_mouse_in_cb, entry);
   evas_object_event_callback_add(entry->editable_object, EVAS_CALLBACK_MOUSE_OUT,
         _etk_entry_editable_mouse_out_cb, entry);
   evas_object_event_callback_add(entry->editable_object, EVAS_CALLBACK_MOUSE_DOWN,
         _etk_entry_editable_mouse_down_cb, entry);
   evas_object_event_callback_add(entry->editable_object, EVAS_CALLBACK_MOUSE_UP,
         _etk_entry_editable_mouse_up_cb, entry);
   evas_object_event_callback_add(entry->editable_object, EVAS_CALLBACK_MOUSE_MOVE,
         _etk_entry_editable_mouse_move_cb, entry);

   if (etk_widget_theme_data_get(internal_entry, "icon_highlight_color", "%d %d %d %d",
         &entry->highlight_color.r, &entry->highlight_color.g,
         &entry->highlight_color.b, &entry->highlight_color.a) != 4)
   {
      entry->highlight_color.r = 128;
      entry->highlight_color.g = 128;
      entry->highlight_color.b = 128;
      entry->highlight_color.a = 255;
   }
   else
   {
      evas_color_argb_premul(entry->highlight_color.a, &entry->highlight_color.r,
            &entry->highlight_color.g, &entry->highlight_color.b);
   }

   if (etk_widget_theme_data_get(internal_entry, "icon_interspace", "%d", &entry->image_interspace) != 1)
      entry->image_interspace = 5;

   if (entry->imf_context &&
       etk_widget_is_focused(ETK_WIDGET(entry)))
   {
      ecore_imf_context_reset(entry->imf_context);
      ecore_imf_context_focus_in(entry->imf_context);
   }

   return ETK_TRUE;
}

/* Called when the entry's internal widget is unrealized */
static Etk_Bool _etk_entry_internal_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;
   const char *text;

   if (!(entry = ETK_ENTRY(etk_object_data_get(object, "_Etk_Entry::Entry"))))
      return ETK_TRUE;

   if (entry->imf_context)
   {
      if (entry->imf_ee_handler_commit)
      {
         ecore_event_handler_del(entry->imf_ee_handler_commit);
         entry->imf_ee_handler_commit = NULL;
      }

      if (entry->imf_ee_handler_delete)
      {
         ecore_event_handler_del(entry->imf_ee_handler_delete);
         entry->imf_ee_handler_delete = NULL;
      }

      ecore_imf_context_del(entry->imf_context);
      entry->imf_context = NULL;
   }

   free(entry->text);
   if ((text = etk_editable_text_get(entry->editable_object)))
   { 
      if (entry->text_limit==0)
         entry->text = strdup(text);
      else 
      {
         size_t limit = strlen(text);
         limit = ETK_MIN(entry->text_limit, limit);
         entry->text = malloc(limit+1);
         entry->text[limit] = '\0';
         strncpy(entry->text, text, limit);
      }
   }
   else
      entry->text = NULL;

   evas_object_del(entry->editable_object);
   entry->editable_object = NULL;

   return ETK_TRUE;
}

/* Called when the user presses a key */
static Etk_Bool _etk_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Entry *entry;
   Evas_Object *editable;
   int cursor_pos, selection_pos;
   int start_pos, end_pos;
   Etk_Bool selecting;
   Etk_Bool changed = ETK_FALSE;
   Etk_Bool selection_changed = ETK_FALSE;
   Etk_Bool stop_signal = ETK_TRUE;

   if (!(entry = ETK_ENTRY(object)))
     return ETK_TRUE;

   editable = entry->editable_object;
   cursor_pos = etk_editable_cursor_pos_get(editable);
   selection_pos = etk_editable_selection_pos_get(editable);
   start_pos = ETK_MIN(cursor_pos, selection_pos);
   end_pos = ETK_MAX(cursor_pos, selection_pos);
   selecting = (start_pos != end_pos);

   /* Move the cursor/selection to the left */
   if (strcmp(event->keyname, "Left") == 0)
   {
      if (event->modifiers & ETK_MODIFIER_SHIFT)
      {
         etk_editable_cursor_move_left(editable);
         selection_changed = ETK_TRUE;
      }
      else if (selecting)
      {
         if (cursor_pos < selection_pos)
            etk_editable_selection_pos_set(editable, cursor_pos);
         else
            etk_editable_cursor_pos_set(editable, selection_pos);
      }
      else
      {
         etk_editable_cursor_move_left(editable);
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      }
   }
   /* Move the cursor/selection to the right */
   else if (strcmp(event->keyname, "Right") == 0)
   {
      if (event->modifiers & ETK_MODIFIER_SHIFT)
      {
         etk_editable_cursor_move_right(editable);
         selection_changed = ETK_TRUE;
      }
      else if (selecting)
      {
         if (cursor_pos > selection_pos)
            etk_editable_selection_pos_set(editable, cursor_pos);
         else
            etk_editable_cursor_pos_set(editable, selection_pos);
      }
      else
      {
         etk_editable_cursor_move_right(editable);
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      }
   }
   /* Move the cursor/selection to the start of the entry */
   else if (strcmp(event->keyname, "Home") == 0)
   {
      etk_editable_cursor_move_to_start(editable);
      if (!(event->modifiers & ETK_MODIFIER_SHIFT))
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      else
         selection_changed = ETK_TRUE;
   }
   /* Move the cursor/selection to the end of the entry */
   else if (strcmp(event->keyname, "End") == 0)
   {
      etk_editable_cursor_move_to_end(editable);
      if (!(event->modifiers & ETK_MODIFIER_SHIFT))
         etk_editable_selection_pos_set(editable, etk_editable_cursor_pos_get(editable));
      else
         selection_changed = ETK_TRUE;
   }
   /* Delete the previous character */
   else if (strcmp(event->keyname, "BackSpace") == 0)
   {
      if (selecting)
         changed = etk_editable_delete(editable, start_pos, end_pos);
      else
         changed = etk_editable_delete(editable, cursor_pos - 1, cursor_pos);
   }
   /* Delete the next character */
   else if (strcmp(event->keyname, "Delete") == 0)
   {
      if (selecting)
         changed = etk_editable_delete(editable, start_pos, end_pos);
      else
         changed = etk_editable_delete(editable, cursor_pos, cursor_pos + 1);
   }
   /* Emit text-activated signal */
   else if (strcmp(event->keyname, "Return") == 0 ||
            strcmp(event->keyname, "KP_Enter") == 0)
   {
      etk_signal_emit(ETK_ENTRY_TEXT_ACTIVATED_SIGNAL, ETK_OBJECT(entry));
   }
   /* Ctrl + A,C,X,V */
   else if (event->modifiers & ETK_MODIFIER_CTRL)
   {
      if (strcmp(event->keyname, "a") == 0)
      {
         etk_editable_select_all(editable);
         selection_changed = ETK_TRUE;
      }
      else if (strcmp(event->keyname, "x") == 0 || strcmp(event->keyname, "c") == 0)
         _etk_entry_selection_copy(entry, ETK_SELECTION_CLIPBOARD, (strcmp(event->keyname, "x") == 0));
      else if (strcmp(event->keyname, "v") == 0)
         etk_selection_text_request(ETK_SELECTION_CLIPBOARD, ETK_WIDGET(entry));
      else
         stop_signal = ETK_FALSE;
   }
   /* Otherwise, we insert the corresponding character */
   else if (event->string && *event->string && (strlen(event->string) != 1 || event->string[0] >= 0x20))
   {
      if (selecting)
         changed |= etk_editable_delete(editable, start_pos, end_pos);
      if (!entry->text_limit || 
            (etk_editable_text_length_get(editable) + strlen(event->string)) <= entry->text_limit)
         changed |= etk_editable_insert(editable, start_pos, event->string);
   }
   else
      stop_signal = ETK_FALSE;

   if (changed)
      etk_signal_emit(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(entry));
   if (selection_changed)
      _etk_entry_selection_copy(entry, ETK_SELECTION_PRIMARY, ETK_FALSE);

   if (entry->imf_context)
   {
      ecore_imf_context_reset(entry->imf_context);
      ecore_imf_context_cursor_position_set(entry->imf_context,
            etk_editable_cursor_pos_get(editable));
   }

   return (!stop_signal);
}

/* Called when the mouse enters the entry */
static void _etk_entry_editable_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(data)))
      return;

   if (entry->imf_context)
   {
      Ecore_IMF_Event_Mouse_In ev;

      ecore_imf_evas_event_mouse_in_wrap(event_info, &ev);
      if (ecore_imf_context_filter_event(entry->imf_context,
                                         ECORE_IMF_EVENT_MOUSE_IN,
                                         (Ecore_IMF_Event *) &ev))
         return;
   }

   if (!entry->pointer_set)
   {
      entry->pointer_set = ETK_TRUE;
      etk_toplevel_pointer_push(etk_widget_toplevel_parent_get(ETK_WIDGET(entry)), ETK_POINTER_TEXT_EDIT);
   }
}

/* Called when the mouse leaves the entry */
static void _etk_entry_editable_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(data)))
      return;

   if (entry->imf_context)
   {
      Ecore_IMF_Event_Mouse_Out ev;

      ecore_imf_evas_event_mouse_out_wrap(event_info, &ev);
      if (ecore_imf_context_filter_event(entry->imf_context,
                                         ECORE_IMF_EVENT_MOUSE_OUT,
                                         (Ecore_IMF_Event *) &ev))
         return;
   }

   if (entry->pointer_set)
   {
      entry->pointer_set = ETK_FALSE;
      etk_toplevel_pointer_pop(etk_widget_toplevel_parent_get(ETK_WIDGET(entry)), ETK_POINTER_TEXT_EDIT);
   }
}

/* Called when the entry is pressed by the mouse */
static void _etk_entry_editable_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Entry *entry;
   Etk_Event_Mouse_Down event;
   Evas_Coord ox, oy;
   int pos;

   if (!(entry = ETK_ENTRY(data)))
      return;

   if (entry->imf_context)
   {
      Ecore_IMF_Event_Mouse_Down ev;

      ecore_imf_evas_event_mouse_down_wrap(event_info, &ev);
      if (ecore_imf_context_filter_event(entry->imf_context,
                                         ECORE_IMF_EVENT_MOUSE_DOWN,
                                         (Ecore_IMF_Event *) &ev))
         return;
   }

   etk_event_mouse_down_wrap(ETK_WIDGET(entry), event_info, &event);
   evas_object_geometry_get(entry->editable_object, &ox, &oy, NULL, NULL);
   pos = etk_editable_pos_get_from_coords(entry->editable_object, event.canvas.x - ox, event.canvas.y - oy);
   if (event.button == 1)
   {
      if (event.flags & ETK_MOUSE_DOUBLE_CLICK)
         etk_editable_select_all(entry->editable_object);
      else
      {
         etk_editable_cursor_pos_set(entry->editable_object, pos);
         if (!(event.modifiers & ETK_MODIFIER_SHIFT))
            etk_editable_selection_pos_set(entry->editable_object, pos);

         entry->selection_dragging = ETK_TRUE;
      }
   }
   else if (event.button == 2)
   {
      etk_editable_cursor_pos_set(entry->editable_object, pos);
      etk_editable_selection_pos_set(entry->editable_object, pos);

      etk_selection_text_request(ETK_SELECTION_PRIMARY, ETK_WIDGET(entry));
   }

   if (entry->imf_context)
   {
      ecore_imf_context_reset(entry->imf_context);
      ecore_imf_context_cursor_position_set(entry->imf_context,
            etk_editable_cursor_pos_get(entry->editable_object));
   }
}

/* Called when the entry is released by the mouse */
static void _etk_entry_editable_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Entry *entry;
   Etk_Event_Mouse_Up event;

   if (!(entry = ETK_ENTRY(data)))
      return;

   if (entry->imf_context)
   {
      Ecore_IMF_Event_Mouse_Up ev;

      ecore_imf_evas_event_mouse_up_wrap(event_info, &ev);
      if (ecore_imf_context_filter_event(entry->imf_context,
                                         ECORE_IMF_EVENT_MOUSE_UP,
                                         (Ecore_IMF_Event *) &ev))
         return;
   }

   etk_event_mouse_up_wrap(ETK_WIDGET(entry), event_info, &event);
   if (event.button == 1)
   {
      entry->selection_dragging = ETK_FALSE;
      _etk_entry_selection_copy(entry, ETK_SELECTION_PRIMARY, ETK_FALSE);
   }
}

/* Called when the mouse moves over the entry */
static void _etk_entry_editable_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Entry *entry;
   Etk_Event_Mouse_Move event;
   Evas_Coord ox, oy;
   int pos;

   if (!(entry = ETK_ENTRY(data)))
      return;

   if (entry->imf_context)
   {
      Ecore_IMF_Event_Mouse_Move ev;

      ecore_imf_evas_event_mouse_move_wrap(event_info, &ev);
      if (ecore_imf_context_filter_event(entry->imf_context,
                                         ECORE_IMF_EVENT_MOUSE_MOVE,
                                         (Ecore_IMF_Event *) &ev))
         return;
   }

   if (entry->selection_dragging)
   {
      etk_event_mouse_move_wrap(ETK_WIDGET(entry), event_info, &event);
      evas_object_geometry_get(entry->editable_object, &ox, &oy, NULL, NULL);
      pos = etk_editable_pos_get_from_coords(entry->editable_object, event.cur.canvas.x - ox, event.cur.canvas.y - oy);
      if (pos >= 0)
      {
         etk_editable_cursor_pos_set(entry->editable_object, pos);
         if (entry->imf_context)
         {
            ecore_imf_context_reset(entry->imf_context);
            ecore_imf_context_cursor_position_set(entry->imf_context, pos);
         }
      }
   }
}

/* Called when the mouse is over the image */
static Etk_Bool _etk_entry_image_mouse_in_cb(Etk_Widget *widget, Etk_Event_Mouse_In *event, void *data)
{
   Etk_Entry *entry;
   Etk_Image *image;

   if (!(entry = ETK_ENTRY(data)) || !(image = ETK_IMAGE(widget)))
      return ETK_TRUE;
   etk_widget_color_set(ETK_WIDGET(image),
         entry->highlight_color.r, entry->highlight_color.g,
         entry->highlight_color.b, entry->highlight_color.a);
   return ETK_TRUE;
}

/* Called when the mouse moves out of the image */
static Etk_Bool _etk_entry_image_mouse_out_cb(Etk_Widget *widget, Etk_Event_Mouse_Out *event, void *data)
{
   Etk_Entry *entry;
   Etk_Image *image;

   if (!(entry = ETK_ENTRY(data)))
      return ETK_TRUE;
   if (!(image = ETK_IMAGE(widget)))
      return ETK_TRUE;
   etk_widget_color_set(ETK_WIDGET(image), 255, 255, 255, 255);

   return ETK_TRUE;
}

/* Called when the mouse is pressed over the image */
static Etk_Bool _etk_entry_image_mouse_down_cb(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Entry *entry;
   Etk_Image *image;

   if (!(entry = ETK_ENTRY(data)) || !(image = ETK_IMAGE(widget)))
      return ETK_TRUE;
   etk_widget_color_set(ETK_WIDGET(image), 255, 255, 255, 255);

   return ETK_TRUE;
}

/* Called when the mouse released over the image */
static Etk_Bool _etk_entry_image_mouse_up_cb(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Entry *entry;
   Etk_Image *image;

   if (!(entry = ETK_ENTRY(data)) || !(image = ETK_IMAGE(widget)))
      return ETK_TRUE;

   etk_widget_color_set(ETK_WIDGET(image),
         entry->highlight_color.r, entry->highlight_color.g,
         entry->highlight_color.b, entry->highlight_color.a);
   return ETK_TRUE;
}

/* Called when the clear button is pressed */
static Etk_Bool _etk_entry_clear_button_cb(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(data)))
      return ETK_TRUE;
   etk_entry_clear(entry);

   return ETK_TRUE;
}

/* Called when the entry is focused */
static Etk_Bool _etk_entry_focused_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)) || !entry->editable_object)
      return ETK_TRUE;

   etk_editable_cursor_show(entry->editable_object);
   etk_editable_selection_show(entry->editable_object);
   etk_widget_theme_signal_emit(entry->internal_entry, "etk,state,focused", ETK_FALSE);
   if (entry->imf_context)
   {
      ecore_imf_context_reset(entry->imf_context);
      ecore_imf_context_focus_in(entry->imf_context);
   }

   return ETK_TRUE;
}

/* Called when the entry is unfocused */
static Etk_Bool _etk_entry_unfocused_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)) || !entry->editable_object)
      return ETK_TRUE;

   etk_editable_cursor_move_to_end(entry->editable_object);
   if (entry->imf_context)
   {
      ecore_imf_context_reset(entry->imf_context);
      ecore_imf_context_cursor_position_set(entry->imf_context,
            etk_editable_cursor_pos_get(entry->editable_object));
   }
   etk_editable_selection_move_to_end(entry->editable_object);
   etk_editable_cursor_hide(entry->editable_object);
   etk_editable_selection_hide(entry->editable_object);
   etk_widget_theme_signal_emit(entry->internal_entry, "etk,state,unfocused", ETK_FALSE);
   if (entry->imf_context)
   {
      ecore_imf_context_reset(entry->imf_context);
      ecore_imf_context_focus_out(entry->imf_context);
   }

   return ETK_TRUE;
}

/* Called when the entry gets enabled */
static Etk_Bool _etk_entry_enabled_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)))
      return ETK_TRUE;

   etk_widget_disabled_set(entry->internal_entry, ETK_FALSE);
   etk_editable_disabled_set(entry->editable_object, ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the entry gets disabled */
static Etk_Bool _etk_entry_disabled_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;

   if (!(entry = ETK_ENTRY(object)))
      return ETK_TRUE;

   etk_widget_disabled_set(entry->internal_entry, ETK_TRUE);
   etk_editable_disabled_set(entry->editable_object, ETK_TRUE);

   return ETK_TRUE;
}

/* Called when the selection/clipboard content is received */
static Etk_Bool _etk_entry_selection_received_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Entry *entry;
   Evas_Object *editable;
   Etk_Selection_Event *ev = event;
   const char *text;

   if (!(entry = ETK_ENTRY(object)) || !(editable = entry->editable_object))
      return ETK_TRUE;

   if (ev->type == ETK_SELECTION_TEXT && (text = ev->data.text) && text[0] != '\0'
         && (strlen(text) != 1 || text[0] >= 0x20))
   {
      int cursor_pos, selection_pos;
      int start_pos, end_pos;
      Etk_Bool selecting;
      Etk_Bool changed = ETK_FALSE;

      cursor_pos = etk_editable_cursor_pos_get(editable);
      selection_pos = etk_editable_selection_pos_get(editable);
      start_pos = ETK_MIN(cursor_pos, selection_pos);
      end_pos = ETK_MAX(cursor_pos, selection_pos);
      selecting = (start_pos != end_pos);

      if (selecting)
         changed |= etk_editable_delete(editable, start_pos, end_pos);
      if (!entry->text_limit || 
            etk_editable_text_length_get(editable) + strlen(text) <= entry->text_limit)
         changed |= etk_editable_insert(editable, start_pos, text);

      if (changed)
         etk_signal_emit(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(entry));
   }

   return ETK_TRUE;
}

/**************************
 *
 * Private function
 *
 **************************/

/* Copies the selected text of the entry to the given selection */
static void _etk_entry_selection_copy(Etk_Entry *entry, Etk_Selection_Type selection, Etk_Bool cut)
{
   Evas_Object *editable;
   int cursor_pos, selection_pos;
   int start_pos, end_pos;
   Etk_Bool selecting;
   char *range;

   if (!entry)
     return;

   editable = entry->editable_object;
   cursor_pos = etk_editable_cursor_pos_get(editable);
   selection_pos = etk_editable_selection_pos_get(editable);
   start_pos = ETK_MIN(cursor_pos, selection_pos);
   end_pos = ETK_MAX(cursor_pos, selection_pos);
   selecting = (start_pos != end_pos);

   if (!selecting)
      return;

   range = etk_editable_text_range_get(editable, start_pos, end_pos);
   if (range)
   {
      etk_selection_text_set(selection, range);
      free(range);
      if (cut)
      {
         if (etk_editable_delete(editable, start_pos, end_pos))
            etk_signal_emit(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(entry));
      }
   }
}

static int _etk_entry_imf_retrieve_surrounding_cb(void *data, Ecore_IMF_Context *ctx, char **text, int *cursor_pos)
{
   Etk_Entry *entry;
   Evas_Object *editable;
   const char *str;

   if (!(entry = ETK_ENTRY(data)))
      return 0;

   if (text)
   {
      str = etk_entry_text_get(entry);
      *text = str ? strdup(str) : strdup("");
   }

   if (cursor_pos)
   {
      editable = entry->editable_object;
      *cursor_pos = etk_editable_cursor_pos_get(editable);
   }

   return 1;
}

static int _etk_entry_imf_event_commit_cb(void *data, int type, void *event)
{
   Etk_Entry *entry;
   Ecore_IMF_Event_Commit *ev = event;
   Evas_Object *editable;
   int cursor_pos, selection_pos;
   int start_pos, end_pos;
   Etk_Bool selecting;
   Etk_Bool changed = ETK_FALSE;

   if (!(entry = ETK_ENTRY(data)))
      return 1;

   if (entry->imf_context != ev->ctx)
      return 1;

   editable = entry->editable_object;
   cursor_pos = etk_editable_cursor_pos_get(editable);
   selection_pos = etk_editable_selection_pos_get(editable);
   start_pos = ETK_MIN(cursor_pos, selection_pos);
   end_pos = ETK_MAX(cursor_pos, selection_pos);
   selecting = (start_pos != end_pos);

   if (selecting)
      changed |= etk_editable_delete(editable, start_pos, end_pos);
   if (!entry->text_limit ||
         etk_editable_text_length_get(editable) + strlen(ev->str) <= entry->text_limit)
      changed |= etk_editable_insert(editable, start_pos, ev->str);

   if (changed)
      etk_signal_emit(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(entry));

   return 0;
}

static int _etk_entry_imf_event_delete_surrounding_cb(void *data, int type, void *event)
{
   Etk_Entry *entry;
   Ecore_IMF_Event_Delete_Surrounding *ev = event;
   int cursor_pos;

   if (!(entry = ETK_ENTRY(data)))
      return 1;

   if (entry->imf_context != ev->ctx)
      return 1;

   if (entry->editable_object)
   {
      Evas_Object *editable;

      editable = entry->editable_object;
      cursor_pos = etk_editable_cursor_pos_get(editable);
      etk_editable_delete(editable,
            cursor_pos + ev->offset,
            cursor_pos + ev->offset + ev->n_chars);
   }

   return 0;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Entry
 *
 * @image html widgets/entry.png
 * You can add an empty entry with etk_entry_new(). @n
 * You can change the text of the entry with etk_entry_text_set() or etk_entry_clear(),
 * and get the text with etk_entry_text_get(). @n
 * An entry can work in two modes: the normal mode (the text is visible) and the password mode
 * (the text is replaced by '*'). To change the mode of the entry, use etk_entry_password_mode_set().
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Entry
 *
 * \par Signals:
 * @signal_name "text-changed": Emitted when the text of the entry is changed
 * @signal_cb Etk_Bool callback(Etk_Entry *entry, void *data)
 * @signal_arg entry: the entry whose text has been changed
 * @signal_data
 *
 * \par Properties:
 * @prop_name "password-mode": The height of an item of the combobox (should be > 0)
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 */
