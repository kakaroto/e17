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

/** @file etk_button.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_button.h"

#include <stdlib.h>
#include <string.h>

#include "etk_alignment.h"
#include "etk_box.h"
#include "etk_event.h"
#include "etk_image.h"
#include "etk_label.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Button
 * @{
 */

int ETK_BUTTON_PRESSED_SIGNAL;
int ETK_BUTTON_RELEASED_SIGNAL;
int ETK_BUTTON_CLICKED_SIGNAL;

enum Etk_Button_Property_Id
{
   ETK_BUTTON_LABEL_PROPERTY,
   ETK_BUTTON_IMAGE_PROPERTY,
   ETK_BUTTON_XALIGN_PROPERTY,
   ETK_BUTTON_YALIGN_PROPERTY,
   ETK_BUTTON_STYLE_PROPERTY,
   ETK_BUTTON_STOCK_SIZE_PROPERTY
};

static void _etk_button_constructor(Etk_Button *button);
static void _etk_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_button_theme_signal_emit(Etk_Widget *widget, const char *signal, Etk_Bool size_recalc);
static Etk_Bool _etk_button_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_button_label_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_button_image_removed_cb(Etk_Object *object, Etk_Widget *child, void *data);
static Etk_Bool _etk_button_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_button_key_up_cb(Etk_Object *object, Etk_Event_Key_Up *event, void *data);
static Etk_Bool _etk_button_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
static Etk_Bool _etk_button_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_button_mouse_click_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_button_pressed_handler(Etk_Button *button);
static Etk_Bool _etk_button_released_handler(Etk_Button *button);
static Etk_Bool _etk_button_clicked_handler(Etk_Button *button);
static void _etk_button_rebuild(Etk_Button *button);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Button
 * @return Returns the type of an Etk_Button
 */
Etk_Type *etk_button_type_get(void)
{
   static Etk_Type *button_type = NULL;

   if (!button_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_HANDLER(ETK_BUTTON_PRESSED_SIGNAL,
            "pressed", Etk_Button, pressed_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_BUTTON_RELEASED_SIGNAL,
            "released", Etk_Button, released_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_BUTTON_CLICKED_SIGNAL,
            "clicked", Etk_Button, clicked_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      button_type = etk_type_new("Etk_Button", ETK_BIN_TYPE,
         sizeof(Etk_Button), ETK_CONSTRUCTOR(_etk_button_constructor), NULL,
         signals);

      etk_type_property_add(button_type, "label", ETK_BUTTON_LABEL_PROPERTY,
            ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      etk_type_property_add(button_type, "image", ETK_BUTTON_IMAGE_PROPERTY,
            ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_object(NULL));
      etk_type_property_add(button_type, "xalign", ETK_BUTTON_XALIGN_PROPERTY,
            ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.5));
      etk_type_property_add(button_type, "yalign", ETK_BUTTON_YALIGN_PROPERTY,
            ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.5));
      etk_type_property_add(button_type, "style", ETK_BUTTON_STYLE_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_BUTTON_BOTH_HORIZ));
      etk_type_property_add(button_type, "stock-size", ETK_BUTTON_STOCK_SIZE_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_STOCK_SMALL));

      button_type->property_set = _etk_button_property_set;
      button_type->property_get = _etk_button_property_get;
   }

   return button_type;
}

/**
 * @brief Creates a new button
 * @return Returns the new button
 */
Etk_Widget *etk_button_new(void)
{
   return etk_widget_new(ETK_BUTTON_TYPE, "theme-group", "button",
         "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new button with a label
 * @return Returns the new button
 */
Etk_Widget *etk_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_BUTTON_TYPE, "theme-group", "button", "label", label,
         "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new button with a label and an icon defined by a stock-id
 * @param stock_id the stock-id corresponding to the label and the icon to use
 * @return Returns the new button
 * @note For some stock-id, the label is empty
 * @see Etk_Stock
 */
Etk_Widget *etk_button_new_from_stock(Etk_Stock_Id stock_id)
{
   Etk_Widget *button;

   button = etk_button_new();
   etk_button_set_from_stock(ETK_BUTTON(button), stock_id);

   return button;
}

/**
 * @brief Presses the button if it wasn't already pressed
 * @param button a button
 */
void etk_button_press(Etk_Button *button)
{
   if (!button || button->is_pressed)
      return;

   button->is_pressed = ETK_TRUE;
   etk_signal_emit(ETK_BUTTON_PRESSED_SIGNAL, ETK_OBJECT(button));
}

/**
 * @brief Releases the button if it was pressed
 * @param button a button
 */
void etk_button_release(Etk_Button *button)
{
   if (!button || !button->is_pressed)
      return;

   button->is_pressed = ETK_FALSE;
   etk_signal_emit(ETK_BUTTON_RELEASED_SIGNAL, ETK_OBJECT(button));
}

/**
 * @brief Clicks on the button
 * @param button a button
 */
void etk_button_click(Etk_Button *button)
{
   if (!button)
      return;
   etk_signal_emit(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button));
}

/**
 * @brief Sets the text of the button's label
 * @param button a button
 * @param label the text to set to the button's label
 */
void etk_button_label_set(Etk_Button *button, const char *label)
{
   Etk_Widget *parent;
   Etk_Bool rebuild;

   if (!button)
      return;

   etk_label_set(ETK_LABEL(button->label), label);

   /* Rebuild the button only if necessary (i.e. if the label is currently not packed into the button) */
   rebuild = ETK_TRUE;
   for (parent = etk_widget_parent_get(button->label); parent; parent = etk_widget_parent_get(parent))
   {
      if (parent == ETK_WIDGET(button))
      {
         rebuild = ETK_FALSE;
         break;
      }
   }

   if (rebuild)
      _etk_button_rebuild(button);
   else
   {
      if (!label || *label == '\0')
         etk_widget_hide(button->label);
      else
         etk_widget_show(button->label);
   }

   etk_object_notify(ETK_OBJECT(button), "label");
}

/**
 * @brief Gets the text of the button's label
 * @param button a button
 * @return Returns the text of the button's label
 */
const char *etk_button_label_get(Etk_Button *button)
{
   if (!button || !button->label)
      return NULL;
   return etk_label_get(ETK_LABEL(button->label));
}

/**
 * @brief Sets the image of the button
 * @param button a button
 * @param image the image to set
 * @note If the button already has an image, the current image will just be unpacked, it will not be destroyed
 * @note The image will be automatically shown, but you can still manually hide it with calling etk_widget_hide()
 * after having called etk_button_image_set()
 */
void etk_button_image_set(Etk_Button *button, Etk_Image *image)
{
   if (!button || button->image == image)
      return;

   if (button->image)
   {
      button->ignore_image_remove = ETK_TRUE;
      etk_widget_parent_set(ETK_WIDGET(button->image), NULL);
      button->ignore_image_remove = ETK_FALSE;
   }

   button->image = image;
   _etk_button_rebuild(button);
   etk_object_notify(ETK_OBJECT(button), "image");
}

/**
 * @brief Gets the image of the button
 * @param button a button
 * @return Returns the image of the button, or NULL if the button has no image
 */
Etk_Image *etk_button_image_get(Etk_Button *button)
{
   if (!button)
      return NULL;
   return button->image;
}

/**
 * @brief Sets the label and the image of the button from a stock-id
 * @param button a button
 * @param stock_id the stock-id to use
 * @note For some stock-id, the label is empty
 */
void etk_button_set_from_stock(Etk_Button *button, Etk_Stock_Id stock_id)
{
   Etk_Widget *image;
   const char *label;

   if (!button)
      return;

   label = etk_stock_label_get(stock_id);
   etk_button_label_set(button, label);

   image = etk_image_new_from_stock(stock_id, button->stock_size);
   etk_widget_internal_set(image, ETK_TRUE);
   etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(image));
}

/**
 * @brief Sets the style of the button (icon, text, both vertically, both horizontally)
 * @param button a button
 * @param style the style to give to the button
 */
void etk_button_style_set(Etk_Button *button, Etk_Button_Style style)
{
   if (!button || button->style == style)
      return;

   button->style = style;
   if (button->box)
   {
      button->ignore_image_remove = ETK_TRUE;
      etk_container_remove_all(ETK_CONTAINER(button->box));
      etk_object_destroy(ETK_OBJECT(button->box));
      button->ignore_image_remove = ETK_FALSE;
      button->box = NULL;
   }

   _etk_button_rebuild(button);
   etk_object_notify(ETK_OBJECT(button), "style");
}

/**
 * @brief Gets the style of the button
 * @param button a button
 * @return Returns the button's style
 */
Etk_Button_Style etk_button_style_get(Etk_Button *button)
{
   if (!button)
      return ETK_FALSE;
   return button->style;
}

/**
 * @brief Sets the stock-size of the button's image
 * @param button a button
 * @param size the stock-size
 */
void etk_button_stock_size_set(Etk_Button *button, Etk_Stock_Size size)
{
   Etk_Stock_Id stock_id;

   if (!button || button->stock_size == size)
      return;

   button->stock_size = size;

   if (button->image)
   {
      etk_image_stock_get(button->image, &stock_id, NULL);
      if (stock_id != ETK_STOCK_NO_STOCK)
         etk_image_set_from_stock(button->image, stock_id, size);
   }

   etk_object_notify(ETK_OBJECT(button), "stock-size");
}

/**
 * @brief Gets the stock-size of the button's image
 * @param button a button
 * @return Returns the stock-size of the button's image
 */
Etk_Stock_Size etk_button_stock_size_get(Etk_Button *button)
{
   if (!button)
      return ETK_STOCK_SMALL;
   return button->stock_size;
}

/**
 * @brief Sets the alignment of the child of the button
 * @param button a button
 * @param xalign the horizontal alignment (0.0 = left, 0.5 = center, 1.0 = right, ...)
 * @param yalign the vertical alignment (0.0 = top, 0.5 = center, 1.0 = bottom, ...)
 * @note It only has effect if the child is a label or an alignment
 */
void etk_button_alignment_set(Etk_Button *button, float xalign, float yalign)
{
   Etk_Widget *child;

   if (!button)
      return;

   if ((child = etk_bin_child_get(ETK_BIN(button))))
   {
      if (ETK_IS_LABEL(child))
         etk_label_alignment_set(ETK_LABEL(child), xalign, yalign);
      else if (ETK_IS_ALIGNMENT(child))
      {
         Etk_Alignment *child_alignment = ETK_ALIGNMENT(child);
         float xscale, yscale;

         etk_alignment_get(child_alignment, NULL, NULL, &xscale, &yscale);
         etk_alignment_set(child_alignment, xalign, yalign, xscale, yscale);
      }
   }

   if (button->xalign != xalign)
   {
      button->xalign = xalign;
      etk_object_notify(ETK_OBJECT(button), "xalign");
   }
   if (button->yalign != yalign)
   {
      button->yalign = yalign;
      etk_object_notify(ETK_OBJECT(button), "yalign");
   }
}

/**
 * @brief Gets the alignment of the button's child
 * @param button a button
 * @param xalign the location where to store the horizontal alignment
 * @param yalign the location where to store the vertical alignment
 */
void etk_button_alignment_get(Etk_Button *button, float *xalign, float *yalign)
{
   if (xalign)
      *xalign = button ? button->xalign : 0;
   if (yalign)
      *yalign = button ? button->yalign : 0;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the button */
static void _etk_button_constructor(Etk_Button *button)
{
   if (!button)
      return;

   button->box = NULL;
   button->image = NULL;
   button->alignment = NULL;
   button->style = ETK_BUTTON_BOTH_HORIZ;
   button->stock_size = ETK_STOCK_SMALL;

   button->label = etk_label_new(NULL);
   etk_label_alignment_set(ETK_LABEL(button->label), 0.5, 0.5);
   etk_widget_internal_set(button->label, ETK_TRUE);
   etk_widget_theme_parent_set(button->label, ETK_WIDGET(button));
   etk_widget_pass_mouse_events_set(button->label, ETK_TRUE);
   etk_container_add(ETK_CONTAINER(button), button->label);

   button->ignore_image_remove = ETK_FALSE;
   button->is_pressed = ETK_FALSE;
   button->xalign = 0.5;
   button->yalign = 0.5;

   ETK_WIDGET(button)->theme_signal_emit = _etk_button_theme_signal_emit;
   button->pressed_handler = _etk_button_pressed_handler;
   button->released_handler = _etk_button_released_handler;
   button->clicked_handler = _etk_button_clicked_handler;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_button_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(button->label), ETK_CALLBACK(_etk_button_label_realized_cb), button);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_button_key_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_UP_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_button_key_up_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_button_mouse_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_button_mouse_up_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_CLICK_SIGNAL, ETK_OBJECT(button), ETK_CALLBACK(_etk_button_mouse_click_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_BUTTON_LABEL_PROPERTY:
         etk_button_label_set(button, etk_property_value_string_get(value));
         break;
      case ETK_BUTTON_IMAGE_PROPERTY:
         etk_button_image_set(button, ETK_IMAGE(etk_property_value_object_get(value)));
         break;
      case ETK_BUTTON_XALIGN_PROPERTY:
         etk_button_alignment_set(button, etk_property_value_float_get(value), button->yalign);
         break;
      case ETK_BUTTON_YALIGN_PROPERTY:
         etk_button_alignment_set(button, button->xalign, etk_property_value_float_get(value));
         break;
      case ETK_BUTTON_STYLE_PROPERTY:
         etk_button_style_set(button, etk_property_value_int_get(value));
         break;
      case ETK_BUTTON_STOCK_SIZE_PROPERTY:
         etk_button_stock_size_set(button, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_BUTTON_LABEL_PROPERTY:
         etk_property_value_string_set(value, etk_button_label_get(button));
         break;
      case ETK_BUTTON_IMAGE_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(button->image));
         break;
      case ETK_BUTTON_XALIGN_PROPERTY:
         etk_property_value_float_set(value, button->xalign);
         break;
      case ETK_BUTTON_YALIGN_PROPERTY:
         etk_property_value_float_set(value, button->yalign);
         break;
      case ETK_BUTTON_STYLE_PROPERTY:
         etk_property_value_int_set(value, button->style);
         break;
      case ETK_BUTTON_STOCK_SIZE_PROPERTY:
         etk_property_value_int_set(value, button->stock_size);
         break;
      default:
         break;
   }
}

/* Called when a theme-signal is emitted by the widget: we use this to make the button's label receive
 * the same theme-signals as the button */
static void _etk_button_theme_signal_emit(Etk_Widget *widget, const char *signal, Etk_Bool size_recalc)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(widget)))
      return;
   etk_widget_theme_signal_emit(button->label, signal, size_recalc);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the button is realized */
static Etk_Bool _etk_button_realized_cb(Etk_Object *object, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return ETK_TRUE;

   if (button->is_pressed)
      etk_widget_theme_signal_emit(ETK_WIDGET(button), "etk,state,pressed", ETK_FALSE);

   return ETK_TRUE;
}

/* Called when the button's label is realized */
static Etk_Bool _etk_button_label_realized_cb(Etk_Object *object, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(data)))
      return ETK_TRUE;

   if (button->is_pressed)
      etk_widget_theme_signal_emit(button->label, "etk,state,pressed", ETK_FALSE);
   if (etk_widget_disabled_get(ETK_WIDGET(button)))
      etk_widget_theme_signal_emit(button->label, "etk,state,disabled", ETK_FALSE);
   if (etk_widget_is_focused(ETK_WIDGET(button)))
      etk_widget_theme_signal_emit(button->label, "etk,state,focused", ETK_FALSE);

   return ETK_TRUE;
}

/* Called when the image is removed from the box */
static Etk_Bool _etk_button_image_removed_cb(Etk_Object *object, Etk_Widget *child, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(data)) || (child != ETK_WIDGET(button->image)))
      return ETK_TRUE;

   if (!button->ignore_image_remove)
   {
      button->image = NULL;
      /* We rebuild the button only if the button's child is not a user-defined widget */
      if (button->alignment == etk_bin_child_get(ETK_BIN(button)))
         _etk_button_rebuild(button);
      etk_object_notify(ETK_OBJECT(button), "image");
   }

   return ETK_TRUE;
}

/* Called when the user presses a key */
static Etk_Bool _etk_button_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "space") == 0)
   {
      etk_button_press(button);
      return ETK_FALSE;
   }

   return ETK_TRUE;
}

/* Called when the user releases a key */
static Etk_Bool _etk_button_key_up_cb(Etk_Object *object, Etk_Event_Key_Up *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "space") == 0)
   {
      if (button->is_pressed)
      {
         etk_button_release(button);
         etk_button_click(button);
      }
      return ETK_FALSE;
   }

   return ETK_TRUE;
}

/* Called when the user presses the button with the mouse */
static Etk_Bool _etk_button_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return ETK_TRUE;

   etk_widget_focus(ETK_WIDGET(button));
   if (event->button == 1)
      etk_button_press(button);
   return ETK_TRUE;
}

/* Called when the user releases the button */
static Etk_Bool _etk_button_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return ETK_TRUE;

   if (event->button == 1)
      etk_button_release(button);

   return ETK_TRUE;
}

/* Called when the uses clicks on the button with the mouse */
static Etk_Bool _etk_button_mouse_click_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return ETK_TRUE;

   if (event->button == 1)
      etk_button_click(button);

   return ETK_TRUE;
}

/* Default handler for the "pressed" signal */
static Etk_Bool _etk_button_pressed_handler(Etk_Button *button)
{
   if (!button)
      return ETK_TRUE;

   etk_widget_theme_signal_emit(ETK_WIDGET(button), "etk,state,pressed", ETK_FALSE);
   etk_widget_theme_signal_emit(button->label, "etk,state,pressed", ETK_FALSE);

   return ETK_TRUE;
}

/* Default handler for the "released" signal */
static Etk_Bool _etk_button_released_handler(Etk_Button *button)
{
   if (!button)
      return ETK_TRUE;

   etk_widget_theme_signal_emit(ETK_WIDGET(button), "etk,state,released", ETK_FALSE);
   etk_widget_theme_signal_emit(button->label, "etk,state,released", ETK_FALSE);

   return ETK_TRUE;
}

/* Default handler for the "clicked" signal */
static Etk_Bool _etk_button_clicked_handler(Etk_Button *button)
{
   if (!button)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(ETK_WIDGET(button), "etk,state,clicked", ETK_FALSE);
   etk_widget_theme_signal_emit(button->label, "etk,state,clicked", ETK_FALSE);

   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Rebuilds the default layout for the button */
static void _etk_button_rebuild(Etk_Button *button)
{
   const char *text;

   if (!button)
      return;

   button->ignore_image_remove = ETK_TRUE;

   /* Rebuild the containers containing the label and image */
   if (button->image)
   {
      if (!button->alignment)
      {
         button->alignment = etk_alignment_new(button->xalign, button->yalign, 0.0, 0.0);
         etk_widget_internal_set(button->alignment, ETK_TRUE);
         etk_container_add(ETK_CONTAINER(button), button->alignment);
         etk_widget_show(button->alignment);
      }

      if (!button->box)
      {
         if (button->style == ETK_BUTTON_BOTH_VERT)
            button->box = etk_vbox_new(ETK_FALSE, 2);
         else
            button->box = etk_hbox_new(ETK_FALSE, 8);
         etk_widget_internal_set(button->box, ETK_TRUE);
         etk_container_add(ETK_CONTAINER(button->alignment), button->box);
         etk_widget_show(button->box);
         etk_signal_connect_by_code(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(button->box),
               ETK_CALLBACK(_etk_button_image_removed_cb), button);
      }
      else
         etk_container_remove_all(ETK_CONTAINER(button->box));

      etk_box_append(ETK_BOX(button->box), ETK_WIDGET(button->image), ETK_BOX_START, ETK_BOX_NONE, 0);
      etk_box_append(ETK_BOX(button->box), button->label, ETK_BOX_START, ETK_BOX_NONE, 0);
      etk_widget_pass_mouse_events_set(ETK_WIDGET(button->image), ETK_TRUE);
   }
   else
   {
      etk_container_remove_all(ETK_CONTAINER(button->box));
      etk_object_destroy(ETK_OBJECT(button->box));
      etk_object_destroy(ETK_OBJECT(button->alignment));
      button->alignment = NULL;
      button->box = NULL;

      etk_label_alignment_set(ETK_LABEL(button->label), button->xalign, button->yalign);
      etk_container_add(ETK_CONTAINER(button), button->label);
   }

   /* Show/hide the label and the image */
   text = etk_label_get(ETK_LABEL(button->label));
   if (button->style != ETK_BUTTON_ICON && text && *text != '\0')
      etk_widget_show(button->label);
   else
      etk_widget_hide(button->label);

   if (button->style != ETK_BUTTON_TEXT)
      etk_widget_show(ETK_WIDGET(button->image));
   else
      etk_widget_hide(ETK_WIDGET(button->image));

   button->ignore_image_remove = ETK_FALSE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Button
 *
 * @image html widgets/button.png
 * An Etk_Button usually contains only a label and an icon, but it can contain any type of widgets. @n
 * You can change the label of the button with etk_button_label_set(), and you can change its icon with
 * etk_button_image_set(). You can also pack your own widget with etk_bin_child_set()
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Button
 *
 * \par Signals:
 * @signal_name "pressed": Emitted when the button is pressed
 * @signal_cb Etk_Bool callback(Etk_Button *button, void *data)
 * @signal_arg button: the button which has been pressed
 * @signal_data
 * \par
 * @signal_name "released": Emitted when the button is released
 * @signal_cb Etk_Bool callback(Etk_Button *button, void *data)
 * @signal_arg button: the button which has been released
 * @signal_data
 * \par
 * @signal_name "clicked": Emitted when the button is clicked
 * (i.e. when the button is released, and if the mouse is still above it)
 * @signal_cb Etk_Bool callback(Etk_Button *button, void *data)
 * @signal_arg button: the button which has been clicked
 * @signal_data
 *
 * \par Properties:
 * @prop_name "label": The text of the label of the button
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "image": The image packed on the left of the label, inside the button
 * @prop_type Pointer (Etk_Image *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "style": The style of the button (icon, text, both vertically, both horizontally)
 * @prop_type Integer (Etk_Button_Style)
 * @prop_rw
 * @prop_val ETK_BUTTON_BOTH_HORIZ
 * \par
 * @prop_name "stock_size": The stock-size of the image of the button
 * @prop_type Integer (Etk_Stock_Size)
 * @prop_rw
 * @prop_val ETK_STOCK_SMALL
 * \par
 * @prop_name "xalign": The horizontal alignment of the child of the button,
 * from 0.0 (left aligned) to 1.0 (right aligned)
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.5
 * \par
 * @prop_name "yalign": The vertical alignment of the child of the button,
 * from 0.0 (top aligned) to 1.0 (bottom aligned)
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.5
 */
