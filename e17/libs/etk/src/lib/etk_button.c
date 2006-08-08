/** @file etk_button.c */
#include "etk_button.h"
#include <stdlib.h>
#include <string.h>
#include "etk_box.h"
#include "etk_alignment.h"
#include "etk_image.h"
#include "etk_label.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Button
 * @{
 */

enum Etk_Button_Signal_Id
{
   ETK_BUTTON_PRESSED_SIGNAL,
   ETK_BUTTON_RELEASED_SIGNAL,
   ETK_BUTTON_CLICKED_SIGNAL,
   ETK_BUTTON_NUM_SIGNALS
};

enum Etk_Button_Property_Id
{
   ETK_BUTTON_LABEL_PROPERTY,
   ETK_BUTTON_IMAGE_PROPERTY,
   ETK_BUTTON_XALIGN_PROPERTY,
   ETK_BUTTON_YALIGN_PROPERTY
};

static void _etk_button_constructor(Etk_Button *button);
static void _etk_button_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_button_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_button_realize_cb(Etk_Object *object, void *data);
static void _etk_button_image_removed_cb(Etk_Object *object, Etk_Widget *child, void *data);
static void _etk_button_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_button_key_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_button_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
static void _etk_button_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
static void _etk_button_mouse_click_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
static void _etk_button_pressed_handler(Etk_Button *button);
static void _etk_button_released_handler(Etk_Button *button);
static void _etk_button_clicked_handler(Etk_Button *button);
static void _etk_button_children_create(Etk_Button *button);

static Etk_Signal *_etk_button_signals[ETK_BUTTON_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Button
 * @return Returns the type of an Etk_Button
 */
Etk_Type *etk_button_type_get()
{
   static Etk_Type *button_type = NULL;

   if (!button_type)
   {
      button_type = etk_type_new("Etk_Button", ETK_BIN_TYPE, sizeof(Etk_Button),
         ETK_CONSTRUCTOR(_etk_button_constructor), NULL);

      _etk_button_signals[ETK_BUTTON_PRESSED_SIGNAL] = etk_signal_new("pressed",
         button_type, ETK_MEMBER_OFFSET(Etk_Button, pressed), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_button_signals[ETK_BUTTON_RELEASED_SIGNAL] = etk_signal_new("released",
         button_type, ETK_MEMBER_OFFSET(Etk_Button, released), etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_button_signals[ETK_BUTTON_CLICKED_SIGNAL] = etk_signal_new("clicked",
         button_type, ETK_MEMBER_OFFSET(Etk_Button, clicked), etk_marshaller_VOID__VOID, NULL, NULL);

      etk_type_property_add(button_type, "label", ETK_BUTTON_LABEL_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      etk_type_property_add(button_type, "image", ETK_BUTTON_IMAGE_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_pointer(NULL));
      etk_type_property_add(button_type, "xalign", ETK_BUTTON_XALIGN_PROPERTY,
         ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.5));
      etk_type_property_add(button_type, "yalign", ETK_BUTTON_YALIGN_PROPERTY,
         ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.5));

      button_type->property_set = _etk_button_property_set;
      button_type->property_get = _etk_button_property_get;
   }

   return button_type;
}

/**
 * @brief Creates a new button
 * @return Returns the new button widget
 */
Etk_Widget *etk_button_new()
{
   return etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "button",
      "focusable", ETK_TRUE, "focus_on_click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new button with a label
 * @return Returns the new button widget
 */
Etk_Widget *etk_button_new_with_label(const char *label)
{
   return etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "button",
      "label", label, "focusable", ETK_TRUE, "focus_on_click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new button with a label and an icon defined by a stock id
 * @param stock_id the stock id corresponding to a label and an icon
 * @return Returns the new button widget
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
 * @brief Presses the button (the button should be released)
 * @param button a button
 */
void etk_button_press(Etk_Button *button)
{
   if (!button || button->is_pressed)
      return;
   
   button->is_pressed = ETK_TRUE;
   etk_signal_emit(_etk_button_signals[ETK_BUTTON_PRESSED_SIGNAL], ETK_OBJECT(button), NULL);
}

/**
 * @brief Releases the button (the button should be pressed)
 * @param button a button
 */
void etk_button_release(Etk_Button *button)
{
   if (!button || !button->is_pressed)
      return;
   
   button->is_pressed = ETK_FALSE;
   etk_signal_emit(_etk_button_signals[ETK_BUTTON_RELEASED_SIGNAL], ETK_OBJECT(button), NULL);
}

/**
 * @brief Clicks the button
 * @param button a button
 */
void etk_button_click(Etk_Button *button)
{
   if (!button)
      return;
   etk_signal_emit(_etk_button_signals[ETK_BUTTON_CLICKED_SIGNAL], ETK_OBJECT(button), NULL);
}

/**
 * @brief Sets the text of the label of the button
 * @param button a button
 * @param label the text to set to the label of the button
 */
void etk_button_label_set(Etk_Button *button, const char *label)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(button)) || !button->label)
      return;

   etk_label_set(ETK_LABEL(button->label), label);
   if (!label || *label == '\0')
      etk_widget_hide(button->label);
   else
      etk_widget_show(button->label);
}

/**
 * @brief Gets the text of the label of the button
 * @param button a button
 * @return Returns the text of the label of the button
 */
const char *etk_button_label_get(Etk_Button *button)
{
   if (!button || !button->label)
      return NULL;
   return etk_label_get(ETK_LABEL(button->label));;
}

/**
 * @brief Sets the image of the button
 * @param button a button
 * @param image the image to set
 */
void etk_button_image_set(Etk_Button *button, Etk_Image *image)
{
   if (!button || button->image == image)
      return;
   
   if (button->image)
   {
      etk_signal_disconnect("child_removed", ETK_OBJECT(button->hbox), ETK_CALLBACK(_etk_button_image_removed_cb));
      etk_container_remove(ETK_CONTAINER(button->hbox), ETK_WIDGET(button->image));
   }

   button->image = image;
   _etk_button_children_create(button);
   etk_object_notify(ETK_OBJECT(button), "image");
}

/**
 * @brief Gets the image of the button
 * @param button a button
 * @return Returns the image of button
 */
Etk_Image *etk_button_image_get(Etk_Button *button)
{
   if (!button)
      return NULL;
   return button->image;
}

/**
 * @brief Sets the label and the image of the button from the stock id
 * @param button a button
 * @param stock_id the stock id to use
 */
void etk_button_set_from_stock(Etk_Button *button, Etk_Stock_Id stock_id)
{
   Etk_Widget *image;
   char *label;

   if (!button)
      return;
   
   image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
   etk_widget_show(image);
   etk_widget_visibility_locked_set(image, ETK_TRUE);
   label = etk_stock_label_get(stock_id);
   etk_button_label_set(button, label);
   etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(image));
}

/**
 * @brief Sets the alignment of the child of the button (only have effect if the child is a label, or an alignment)
 * @param button a button
 * @param xalign the horizontal alignment (0.0 = left, 0.5 = center, 1.0 = right, ...)
 * @param yalign the vertical alignment (0.0 = top, 0.5 = center, 1.0 = bottom, ...)
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
      if (!etk_object_notify(ETK_OBJECT(button), "xalign"))
         return;
   }
   if (button->yalign != yalign)
   {
      button->yalign = yalign;
      if (!etk_object_notify(ETK_OBJECT(button), "yalign"))
         return;
   }
}

/**
 * @brief Gets the alignment of the child of the button
 * @param button a button
 * @param xalign the location to store the horizontal alignment
 * @param yalign the location to store the vertical alignment
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

   button->hbox = NULL;
   button->image = NULL;
   button->alignment = NULL;

   button->label = etk_label_new(NULL);
   etk_widget_visibility_locked_set(button->label, ETK_TRUE);
   etk_label_alignment_set(ETK_LABEL(button->label), 0.5, 0.5);
   etk_widget_pass_mouse_events_set(button->label, ETK_TRUE);
   etk_container_add(ETK_CONTAINER(button), button->label);

   button->pressed = _etk_button_pressed_handler;
   button->released = _etk_button_released_handler;
   button->clicked = _etk_button_clicked_handler;

   button->is_pressed = ETK_FALSE;
   button->xalign = 0.5;
   button->yalign = 0.5;

   etk_signal_connect("realize", ETK_OBJECT(button), ETK_CALLBACK(_etk_button_realize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(button), ETK_CALLBACK(_etk_button_key_down_cb), NULL);
   etk_signal_connect("key_up", ETK_OBJECT(button), ETK_CALLBACK(_etk_button_key_up_cb), NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(button), ETK_CALLBACK(_etk_button_mouse_down_cb), NULL);
   etk_signal_connect("mouse_up", ETK_OBJECT(button), ETK_CALLBACK(_etk_button_mouse_up_cb), NULL);
   etk_signal_connect("mouse_click", ETK_OBJECT(button), ETK_CALLBACK(_etk_button_mouse_click_cb), NULL);
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
         etk_button_image_set(button, ETK_IMAGE(etk_property_value_pointer_get(value)));
         break;
      case ETK_BUTTON_XALIGN_PROPERTY:
         etk_button_alignment_set(button, etk_property_value_float_get(value), button->yalign);
         break;
      case ETK_BUTTON_YALIGN_PROPERTY:
         etk_button_alignment_set(button, button->xalign, etk_property_value_float_get(value));
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
         etk_property_value_pointer_set(value, ETK_OBJECT(button->image));
         break;
      case ETK_BUTTON_XALIGN_PROPERTY:
         etk_property_value_float_set(value, button->xalign);
         break;
      case ETK_BUTTON_YALIGN_PROPERTY:
         etk_property_value_float_set(value, button->yalign);
         break;
      default:
         break;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the button is realized */
static void _etk_button_realize_cb(Etk_Object *object, void *data)
{
   Etk_Button *button;
   
   if (!(button = ETK_BUTTON(object)))
      return;
   
   if (button->is_pressed)
      etk_widget_theme_signal_emit(ETK_WIDGET(button), "pressed");
}

/* Called when the image is removed from the hbox */
static void _etk_button_image_removed_cb(Etk_Object *object, Etk_Widget *child, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(data)) || (child != ETK_WIDGET(button->image)))
      return;
   button->image = NULL;
   _etk_button_children_create(button);
}

/* Called when the user presses a key */
static void _etk_button_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *key_event = event;
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return;

   if (strcmp(key_event->key, "space") == 0)
   {
      etk_button_press(button);
      etk_signal_stop();
   }
}

/* Called when the user releases a key */
static void _etk_button_key_up_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *key_event = event;
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return;

   if (strcmp(key_event->key, "space") == 0)
   {
      if (button->is_pressed)
      {
         etk_button_release(button);
         etk_button_click(button);
      }
      etk_signal_stop();
   }
}

/* Called when the user presses the button with the mouse */
static void _etk_button_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return;

   etk_widget_focus(ETK_WIDGET(button));
   if (event->button == 1)
      etk_button_press(button);
}

/* Called when the user releases the button */
static void _etk_button_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return;

   if (event->button == 1)
      etk_button_release(button);
}

/* Called when the uses clicks on the button with the mouse */
static void _etk_button_mouse_click_cb(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data)
{
   Etk_Button *button;

   if (!(button = ETK_BUTTON(object)))
      return;

   if (event->button == 1)
      etk_button_click(button);
}

/* Default handler for the "pressed" signal */
static void _etk_button_pressed_handler(Etk_Button *button)
{
   if (!button)
      return;
   etk_widget_theme_signal_emit(ETK_WIDGET(button), "pressed");
}

/* Default handler for the "released" signal */
static void _etk_button_released_handler(Etk_Button *button)
{
   if (!button)
      return;
   etk_widget_theme_signal_emit(ETK_WIDGET(button), "released");
}

/* Default handler for the "clicked" signal */
static void _etk_button_clicked_handler(Etk_Button *button)
{
   if (!button)
      return;
   etk_widget_theme_signal_emit(ETK_WIDGET(button), "clicked");
}

/* Creates the children of the button */
static void _etk_button_children_create(Etk_Button *button)
{
   if (!button)
      return;

   if (button->image)
   {
      if (!button->alignment)
      {
         button->alignment = etk_alignment_new(button->xalign, button->yalign, 0.0, 0.0);
         etk_widget_visibility_locked_set(button->alignment, ETK_TRUE);
         etk_widget_pass_mouse_events_set(button->alignment, ETK_TRUE);
         etk_widget_show(button->alignment);
         etk_container_add(ETK_CONTAINER(button), button->alignment);
      }

      if (!button->hbox)
      {
         button->hbox = etk_hbox_new(ETK_FALSE, 8);
         etk_widget_visibility_locked_set(button->hbox, ETK_TRUE);
         etk_widget_pass_mouse_events_set(button->hbox, ETK_TRUE);
         etk_widget_show(button->hbox);
         etk_container_add(ETK_CONTAINER(button->alignment), button->hbox);
         etk_signal_connect("child_removed", ETK_OBJECT(button->hbox), ETK_CALLBACK(_etk_button_image_removed_cb), button);
         
         etk_label_alignment_set(ETK_LABEL(button->label), 0.0, 0.5);
         etk_box_pack_end(ETK_BOX(button->hbox), button->label, ETK_FALSE, ETK_FALSE, 0);
      }

      etk_box_pack_start(ETK_BOX(button->hbox), ETK_WIDGET(button->image), ETK_FALSE, ETK_FALSE, 0);
      etk_widget_pass_mouse_events_set(ETK_WIDGET(button->image), ETK_TRUE);
   }
   else
   {
      if (button->alignment)
      {
         etk_object_destroy(ETK_OBJECT(button->alignment));
         button->alignment = NULL;
      }
      if (button->hbox)
      {
         etk_object_destroy(ETK_OBJECT(button->hbox));
         button->hbox = NULL;
      }

      etk_label_alignment_set(ETK_LABEL(button->label), button->xalign, button->yalign);
      etk_container_add(ETK_CONTAINER(button), button->label);
   }
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
 * An Etk_Button usually contains only a label and an icon, but it can contain any type of widgets.
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
 * @signal_cb void callback(Etk_Button *button, void *data)
 * @signal_arg button: the button which has been pressed
 * @signal_data
 * \par
 * @signal_name "released": Emitted when the button is released
 * @signal_cb void callback(Etk_Button *button, void *data)
 * @signal_arg button: the button which has been released
 * @signal_data
 * \par
 * @signal_name "clicked": Emitted when the button is clicked
 * (i.e. when the button is released, and if the mouse is still above it)
 * @signal_cb void callback(Etk_Button *button, void *data)
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
