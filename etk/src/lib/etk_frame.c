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

/** @file etk_frame.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_frame.h"

#include <stdlib.h>
#include <string.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Frame
 * @{
 */

enum Etk_Frame_Property_Id
{
   ETK_FRAME_LABEL_PROPERTY
};

static void _etk_frame_constructor(Etk_Frame *frame);
static void _etk_frame_destructor(Etk_Frame *frame);
static void _etk_frame_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_frame_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_frame_realized_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Frame
 * @return Returns the type of an Etk_Frame
 */
Etk_Type *etk_frame_type_get(void)
{
   static Etk_Type *frame_type = NULL;

   if (!frame_type)
   {
      frame_type = etk_type_new("Etk_Frame", ETK_BIN_TYPE, sizeof(Etk_Frame),
         ETK_CONSTRUCTOR(_etk_frame_constructor),
         ETK_DESTRUCTOR(_etk_frame_destructor), NULL);

      etk_type_property_add(frame_type, "label", ETK_FRAME_LABEL_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));

      frame_type->property_set = _etk_frame_property_set;
      frame_type->property_get = _etk_frame_property_get;
   }

   return frame_type;
}

/**
 * @brief Creates a new frame
 * @param label the label of the new frame
 * @return Returns the new frame widget
 */
Etk_Widget *etk_frame_new(const char *label)
{
   return etk_widget_new(ETK_FRAME_TYPE, "theme-group", "frame", "label", label, NULL);
}

/**
 * @brief Sets the label of the frame
 * @param frame a frame
 * @param label the label to set
 */
void etk_frame_label_set(Etk_Frame *frame, const char *label)
{
   if (!frame)
      return;

   if (label != frame->label)
   {
      free(frame->label);
      frame->label = label ? strdup(label) : NULL;
   }

   if (!frame->label || frame->label[0] == '\0')
   {
      etk_widget_theme_part_text_set(ETK_WIDGET(frame), "etk.text.label", "");
      etk_widget_theme_signal_emit(ETK_WIDGET(frame), "etk,action,hide,label", ETK_TRUE);
   }
   else
   {
      etk_widget_theme_part_text_set(ETK_WIDGET(frame), "etk.text.label", frame->label);
      etk_widget_theme_signal_emit(ETK_WIDGET(frame), "etk,action,show,label", ETK_TRUE);
   }

   if (label != frame->label)
      etk_object_notify(ETK_OBJECT(frame), "label");
}

/**
 * @brief Gets the label of the frame
 * @param frame a frame
 * @return Returns the label of the frame
 */
const char *etk_frame_label_get(Etk_Frame *frame)
{
   if (!frame)
      return NULL;
   return frame->label;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_frame_constructor(Etk_Frame *frame)
{
   if (!frame)
      return;

   frame->label = NULL;
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(frame), ETK_CALLBACK(_etk_frame_realized_cb), NULL);
}

/* Destroys the frame */
static void _etk_frame_destructor(Etk_Frame *frame)
{
   if (!frame)
      return;
   free(frame->label);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_frame_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Frame *frame;

   if (!(frame = ETK_FRAME(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_FRAME_LABEL_PROPERTY:
         etk_frame_label_set(frame, etk_property_value_string_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_frame_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Frame *frame;

   if (!(frame = ETK_FRAME(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_FRAME_LABEL_PROPERTY:
         etk_property_value_string_set(value, frame->label);
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

/* Called when the frame is realized */
static Etk_Bool _etk_frame_realized_cb(Etk_Object *object, void *data)
{
   Etk_Frame *frame;

   if (!(frame = ETK_FRAME(object)))
      return ETK_TRUE;
   etk_frame_label_set(frame, frame->label);
   return ETK_TRUE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Frame
 *
 * @image html widgets/frame.png
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Frame
 *
 * \par Properties:
 * @prop_name "label": The text of the frame's label
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 */
