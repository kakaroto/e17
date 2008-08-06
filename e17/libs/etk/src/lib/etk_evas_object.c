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

/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
/** @file etk_evas_object.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_evas_object.h"

#include <stdlib.h>
#include <string.h>

#include <Evas.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_theme.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Evas_Object
 * @{
 */

enum Etk_Evas_Object_Property_Id
{
   ETK_EVAS_OBJECT_OBJECT_PROPERTY,
};

static void _etk_evas_object_constructor(Etk_Evas_Object *etk_evas_object);
static void _etk_evas_object_destructor(Etk_Evas_Object *etk_evas_object);
static void _etk_evas_object_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_evas_object_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_evas_object_realized_cb(Etk_Object *object, void *data);
static void _etk_evas_object_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_evas_object_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_evas_object_load(Etk_Evas_Object *etk_evas_object);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Evas_Object
 * @return Returns the type of an Etk_Evas_Object
 */
Etk_Type *etk_evas_object_type_get(void)
{
   static Etk_Type *etk_evas_object_type = NULL;

   if (!etk_evas_object_type)
   {
      etk_evas_object_type = etk_type_new("Etk_Evas_Object", ETK_WIDGET_TYPE, sizeof(Etk_Evas_Object),
         ETK_CONSTRUCTOR(_etk_evas_object_constructor),
         ETK_DESTRUCTOR(_etk_evas_object_destructor), NULL);

      etk_type_property_add(etk_evas_object_type, "evas-object", ETK_EVAS_OBJECT_OBJECT_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
      etk_evas_object_type->property_set = _etk_evas_object_property_set;
      etk_evas_object_type->property_get = _etk_evas_object_property_get;
   }

   return etk_evas_object_type;
}

/**
 * @brief Creates a new empty etk evas object wrapper 
 * @return Returns the new etk evas object widget
 */
Etk_Widget *etk_evas_object_new(void)
{
   return etk_widget_new(ETK_EVAS_OBJECT_TYPE, NULL);
}

/**
 * @brief Creates a new etk evas object wrapper from the given evas object
 * @param evas_object the evas object to use for the etk evas object wrapper
 * @return Returns the new widget
 */
Etk_Widget *etk_evas_object_new_from_object(Evas_Object *evas_object)
{
   Etk_Widget *etk_evas_object;

   etk_evas_object = etk_evas_object_new();
   etk_evas_object_set_object(ETK_EVAS_OBJECT(etk_evas_object), evas_object);
   return etk_evas_object;
}

/**
 * @brief Sets the Evas_Object for the widget 
 * @param etk_evas_object an Etk_Evas_Object
 * @param evas_object the Evas object to use.
 */
void etk_evas_object_set_object(Etk_Evas_Object *etk_evas_object, Evas_Object *evas_object)
{
   if (!etk_evas_object)
      return;

   if (etk_evas_object->object != evas_object)
   {
      etk_evas_object->object = evas_object;
      etk_object_notify(ETK_OBJECT(etk_evas_object), "evas-object");
      evas_object_data_set(evas_object, "_Etk_Evas_Object::Widget", etk_evas_object);
   }

   _etk_evas_object_load(etk_evas_object);
}

/**
 * @brief Gets the Evas object used by the widget.
 * @param etk_evas_object an Etk_Evas_Object widget
 * @return Returns the Evas object
 */
Evas_Object *etk_evas_object_get(Etk_Evas_Object *etk_evas_object)
{
   if (!etk_evas_object)
      return NULL;

   return etk_evas_object->object;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the widget */
static void _etk_evas_object_constructor(Etk_Evas_Object *etk_evas_object)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(etk_evas_object)))
      return;

   etk_evas_object->object = NULL;

   widget->size_request = _etk_evas_object_size_request;
   widget->size_allocate = _etk_evas_object_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(etk_evas_object), ETK_CALLBACK(_etk_evas_object_realized_cb), NULL);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(etk_evas_object), ETK_CALLBACK(etk_callback_set_null), &etk_evas_object->object);
}

/* Destroys the widget */
static void _etk_evas_object_destructor(Etk_Evas_Object *etk_evas_object)
{
   if (!etk_evas_object)
      return;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_evas_object_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Evas_Object *etk_evas_object;

   if (!(etk_evas_object = ETK_EVAS_OBJECT(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_EVAS_OBJECT_OBJECT_PROPERTY:
         etk_evas_object_set_object(etk_evas_object, etk_property_value_pointer_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_evas_object_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Evas_Object *etk_evas_object;

   if (!(etk_evas_object = ETK_EVAS_OBJECT(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_EVAS_OBJECT_OBJECT_PROPERTY:
         etk_property_value_pointer_set(value, etk_evas_object->object);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size */
static void _etk_evas_object_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Evas_Object *etk_evas_object;

   if (!(etk_evas_object = ETK_EVAS_OBJECT(widget)) || !size || !etk_evas_object->object)
      return;

   evas_object_geometry_get(etk_evas_object->object, NULL, NULL, &size->w, &size->h);
}

/* Resizes the widget to the allocated size */
static void _etk_evas_object_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Evas_Object *etk_evas_object;

   if (!(etk_evas_object = ETK_EVAS_OBJECT(widget)) || !etk_evas_object->object)
      return;

   evas_object_move(etk_evas_object->object, geometry.x, geometry.y);
   evas_object_resize(etk_evas_object->object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the widget is realized */
static Etk_Bool _etk_evas_object_realized_cb(Etk_Object *object, void *data)
{
   Etk_Evas_Object *etk_evas_object;

   if (!(etk_evas_object = ETK_EVAS_OBJECT(object)))
      return ETK_TRUE;

   _etk_evas_object_load(etk_evas_object);
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Load the widget and its object */ 
static void _etk_evas_object_load(Etk_Evas_Object *etk_evas_object)
{
   Evas *evas;

   if (!etk_evas_object || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(etk_evas_object))))
      return;

   if (!etk_evas_object->object)
      return;

   etk_widget_member_object_add(ETK_WIDGET(etk_evas_object), etk_evas_object->object);

   evas_object_show(etk_evas_object->object);
   etk_widget_size_recalc_queue(ETK_WIDGET(etk_evas_object));
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Evas_Object
 *
 * @image html widgets/image.png
 * The Etk_Evas_Object can used to load and hold Evas objects (regular,
 * smarts, Edje objects, etc. ) in Etk. 
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Evas_Object
 *
 * \par Properties:
 * @prop_name "evas-object": A pointer to the Evas object that will be held in the widget.
 * @prop_type Pointer (Evas_Object *)
 * @prop_rw
 * @prop_val NULL
 */
