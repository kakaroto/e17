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

/** @file etk_label.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_label.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Label
 * @{
 */

enum Etk_Label_Property_Id
{
   ETK_LABEL_LABEL_PROPERTY,
   ETK_LABEL_XALIGN_PROPERTY,
   ETK_LABEL_YALIGN_PROPERTY
};

static void _etk_label_constructor(Etk_Label *label);
static void _etk_label_destructor(Etk_Label *label);
static void _etk_label_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_label_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_label_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static Etk_Bool _etk_label_realized_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Label
 * @return Returns the type of an Etk_Label
 */
Etk_Type *etk_label_type_get(void)
{
   static Etk_Type *label_type = NULL;

   if (!label_type)
   {
      label_type = etk_type_new("Etk_Label", ETK_WIDGET_TYPE, sizeof(Etk_Label),
         ETK_CONSTRUCTOR(_etk_label_constructor),
         ETK_DESTRUCTOR(_etk_label_destructor), NULL);

      etk_type_property_add(label_type, "label", ETK_LABEL_LABEL_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(label_type, "xalign", ETK_LABEL_XALIGN_PROPERTY,
         ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_float(0.0));
      etk_type_property_add(label_type, "yalign", ETK_LABEL_YALIGN_PROPERTY,
         ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_float(0.5));

      label_type->property_set = _etk_label_property_set;
      label_type->property_get = _etk_label_property_get;
   }

   return label_type;
}

/**
 * @brief Creates a new label
 * @param text the text to set to the label
 * @return Returns the new label widget
 */
Etk_Widget *etk_label_new(const char *text)
{
   return etk_widget_new(ETK_LABEL_TYPE, "label", text, "theme-group", "label", NULL);
}

/**
 * @brief Sets the text of the label
 * @param label a label
 * @param text the text to set
 */
void etk_label_set(Etk_Label *label, const char *text)
{
   if (!label)
      return;

   if (text != label->text)
   {
      free(label->text);
      label->text = (text && *text != '\0') ? strdup(text) : NULL;
   }

   if (!label->text)
      etk_widget_theme_part_text_set(ETK_WIDGET(label), "etk.text.label", "");
   else
      etk_widget_theme_part_text_set(ETK_WIDGET(label), "etk.text.label", label->text);

   etk_widget_size_recalc_queue(ETK_WIDGET(label));
}

/**
 * @brief Gets the text of the label
 * @param label a label
 * @return Returns the text of the label
 */
const char *etk_label_get(Etk_Label *label)
{
   if (!label)
      return NULL;
   return label->text;
}

/**
 * @brief Sets the alignment of the label
 * @param label a label
 * @param xalign the horizontal alignment (0.0 = left, 0.5 = center, 1.0 = right, ...)
 * @param yalign the vertical alignment (0.0 = top, 0.5 = center, 1.0 = bottom, ...)
 */
void etk_label_alignment_set(Etk_Label *label, float xalign, float yalign)
{
   Etk_Bool need_redraw = ETK_FALSE;

   if (!label)
      return;

   xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   yalign = ETK_CLAMP(yalign, 0.0, 1.0);

   if (label->xalign != xalign)
   {
      label->xalign = xalign;
      need_redraw = ETK_TRUE;
      etk_object_notify(ETK_OBJECT(label), "xalign");
   }
   if (label->yalign != yalign)
   {
      label->yalign = yalign;
      need_redraw = ETK_TRUE;
      etk_object_notify(ETK_OBJECT(label), "yalign");
   }

   if (need_redraw)
      etk_widget_redraw_queue(ETK_WIDGET(label));
}

/**
 * @brief Gets the alignment of the label
 * @param label a label
 * @param xalign the location to store the horizontal alignment
 * @param yalign the location to store the vertical alignment
 */
void etk_label_alignment_get(Etk_Label *label, float *xalign, float *yalign)
{
   if (!label)
      return;

   if (xalign)
      *xalign = label->xalign;
   if (yalign)
      *yalign = label->yalign;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the label */
static void _etk_label_constructor(Etk_Label *label)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(label)))
      return;

   label->text = NULL;

   label->xalign = 0.0;
   label->yalign = 0.5;

   widget->size_allocate = _etk_label_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(label), ETK_CALLBACK(_etk_label_realized_cb), NULL);
}

/* Destroys the label */
static void _etk_label_destructor(Etk_Label *label)
{
   if (!label)
      return;
   free(label->text);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_label_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Label *label;

   if (!(label = ETK_LABEL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_LABEL_LABEL_PROPERTY:
         etk_label_set(label, etk_property_value_string_get(value));
         break;
      case ETK_LABEL_XALIGN_PROPERTY:
         etk_label_alignment_set(label, etk_property_value_float_get(value), label->yalign);
         break;
      case ETK_LABEL_YALIGN_PROPERTY:
         etk_label_alignment_set(label, label->xalign, etk_property_value_float_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_label_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Label *label;

   if (!(label = ETK_LABEL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_LABEL_LABEL_PROPERTY:
         etk_property_value_string_set(value, label->text);
         break;
      case ETK_LABEL_XALIGN_PROPERTY:
         etk_property_value_float_set(value, label->xalign);
         break;
      case ETK_LABEL_YALIGN_PROPERTY:
         etk_property_value_float_set(value, label->yalign);
         break;
      default:
         break;
   }
}

/* Resizes the label to the allocated size */
static void _etk_label_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Label *label;
   Evas_Object *theme_object;
   Etk_Size requested_size;

   if (!(label = ETK_LABEL(widget)))
      return;

   if ((theme_object = widget->theme_object))
   {
      int x = 0;
      int y = 0;

      etk_widget_size_request(widget, &requested_size);
      if (requested_size.w < geometry.w)
         x = (geometry.w - requested_size.w) * label->xalign;
      if (requested_size.h < geometry.h)
         y = (geometry.h - requested_size.h) * label->yalign;

      evas_object_move(theme_object, geometry.x + x, geometry.y + y);
      evas_object_resize(theme_object, ETK_MIN(geometry.w, requested_size.w), ETK_MIN(geometry.h, requested_size.h));
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the label is realized */
static Etk_Bool _etk_label_realized_cb(Etk_Object *object, void *data)
{
   Etk_Label *label;

   if (!(label = ETK_LABEL(object)))
      return ETK_TRUE;

   if (!label->text)
      etk_widget_theme_part_text_set(ETK_WIDGET(label), "etk.text.label", "");
   else
      etk_widget_theme_part_text_set(ETK_WIDGET(label), "etk.text.label", label->text);
   etk_widget_size_recalc_queue(ETK_WIDGET(label));

   return ETK_TRUE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Label
 *
 * @image html widgets/label.png
 * You can use html-like tags to format the text of the label. For example, "<b>Text</b>" makes
 * <b>Text</b> bold. @n
 * Here is the list of the supported tags:
 * - <b>"<left>Text</left>":</b> Align left
 * - <b>"<right>Text</right>":</b> Align right
 * - <b>"<center>Text</center>":</b> Align center
 * - <b>"<b>Text</b>":</b> Bold
 * - <b>"<b>Text</b>":</b> Italic
 * - <b>"<bi>Text</bi>":</b> Bold-Italic
 * - <b>"<color=#rrggbbaa>Text</>":</b> Set the color of the text
 * - <b>"<font_size=16>Text</>":</b> Set the size of the text
 * - <b>"<title>Text</title>":</b> Style for titles
 * - <b>"<subtitle>Text</subtitle>":</b> Style for subtitles
 * - <b>"<glow>Text</glow>":</b> Make the the text glow
 * - <b>"<description>Text</description>":</b> Style for descriptions
 * - <b>"<br>":</b> End of line
 * - <b>"<tab>":</b> Add a tab @n @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Label
 *
 * \par Properties:
 * @prop_name "label": The text of the label widget
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "xalign": The horizontal alignment of the text of the label,
 * from 0.0 (left-aligned) to 1.0 (right-aligned)
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "yalign": The vertical alignment of the text of the label,
 * from 0.0 (top-aligned) to 1.0 (bottom-aligned)
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.5
 */
