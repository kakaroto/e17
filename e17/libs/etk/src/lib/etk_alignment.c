/** @file etk_alignment.c */
#include "etk_alignment.h"
#include <stdlib.h>
#include "etk_utils.h"

/**
 * @addtogroup Etk_Alignment
 * @{
 */

enum _Etk_Alignment_Property_Id
{
   ETK_ALIGNMENT_XALIGN_PROPERTY,
   ETK_ALIGNMENT_YALIGN_PROPERTY,
   ETK_ALIGNMENT_XSCALE_PROPERTY,
   ETK_ALIGNMENT_YSCALE_PROPERTY
};

static void _etk_alignment_constructor(Etk_Alignment *alignment);
static void _etk_alignment_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_alignment_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_alignment_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Alignment
 * @return Returns the type on an Etk_Alignment
 */
Etk_Type *etk_alignment_type_get()
{
   static Etk_Type *alignment_type = NULL;

   if (!alignment_type)
   {
      alignment_type = etk_type_new("Etk_Alignment", ETK_BIN_TYPE, sizeof(Etk_Alignment), ETK_CONSTRUCTOR(_etk_alignment_constructor), NULL);

      etk_type_property_add(alignment_type, "xalign", ETK_ALIGNMENT_XALIGN_PROPERTY, ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.5));
      etk_type_property_add(alignment_type, "yalign", ETK_ALIGNMENT_YALIGN_PROPERTY, ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.5));
      etk_type_property_add(alignment_type, "xscale", ETK_ALIGNMENT_XSCALE_PROPERTY, ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(1.0));
      etk_type_property_add(alignment_type, "yscale", ETK_ALIGNMENT_YSCALE_PROPERTY, ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(1.0));
      
      alignment_type->property_set = _etk_alignment_property_set;
      alignment_type->property_get = _etk_alignment_property_get;
   }

   return alignment_type;
}

/**
 * @brief Creates a new alignment
 * @param xalign the horizontal alignment, from 0.0 (left) to 1.0 (right)
 * @param yalign the vertical alignment, from 0.0 (top) to 1.0 (bottom)
 * @param xscale the horizontal scale factor, from 0.0 (do not scale) to 1.0 (take all the available space)
 * @param yscale the vertical scale factor, from 0.0 (do not scale) to 1.0 (take all the available space)
 * @return Returns the new alignment widget
 */
Etk_Widget *etk_alignment_new(float xalign, float yalign, float xscale, float yscale)
{
   return etk_widget_new(ETK_ALIGNMENT_TYPE, "xalign", xalign, "yalign", yalign, "xscale", xscale, "yscale", yscale, NULL);
}

/**
 * @brief Sets the parameters of the alignment
 * @param alignment an alignment
 * @param xalign the horizontal alignment, from 0.0 (left) to 1.0 (right)
 * @param yalign the vertical alignment, from 0.0 (top) to 1.0 (bottom)
 * @param xscale the horizontal scale factor, from 0.0 (do not scale) to 1.0 (take all the available space)
 * @param yscale the vertical scale factor, from 0.0 (do not scale) to 1.0 (take all the available space)
 */
void etk_alignment_set(Etk_Alignment *alignment, float xalign, float yalign, float xscale, float yscale)
{
   if (!alignment)
      return;

   xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   if (xalign != alignment->xalign)
   {
      alignment->xalign = xalign;
      etk_widget_redraw_queue(ETK_WIDGET(alignment));
      etk_object_notify(ETK_OBJECT(alignment), "xalign");
   }
   yalign = ETK_CLAMP(yalign, 0.0, 1.0);
   if (yalign != alignment->yalign)
   {
      alignment->yalign = yalign;
      etk_widget_redraw_queue(ETK_WIDGET(alignment));
      etk_object_notify(ETK_OBJECT(alignment), "yalign");
   }
   xscale = ETK_CLAMP(xscale, 0.0, 1.0);
   if (xscale != alignment->xscale)
   {
      alignment->xscale = xscale;
      etk_widget_redraw_queue(ETK_WIDGET(alignment));
      etk_object_notify(ETK_OBJECT(alignment), "xscale");
   }
   yscale = ETK_CLAMP(yscale, 0.0, 1.0);
   if (yscale != alignment->yscale)
   {
      alignment->yscale = yscale;
      etk_widget_redraw_queue(ETK_WIDGET(alignment));
      etk_object_notify(ETK_OBJECT(alignment), "yscale");
   }
}

/**
 * @brief Gets the parameters of the alignment
 * @param alignment an alignment
 * @param xalign the location to store the horizontal alignment
 * @param yalign the location to store the vertical alignment
 * @param xscale the location to store the horizontal scale factor
 * @param yscale the location to store the vertical scale factor
 */
void etk_alignment_get(Etk_Alignment *alignment, float *xalign, float *yalign, float *xscale, float *yscale)
{
   if (xalign)
      *xalign = alignment ? alignment->xalign : 0;
   if (yalign)
      *yalign = alignment ? alignment->yalign : 0;
   if (xscale)
      *xscale = alignment ? alignment->xscale : 0;
   if (yscale)
      *yscale = alignment ? alignment->yscale : 0;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_alignment_constructor(Etk_Alignment *alignment)
{
   if (!alignment)
      return;

   alignment->xalign = 0.5;
   alignment->yalign = 0.5;
   alignment->xscale = 1.0;
   alignment->yscale = 1.0;
   ETK_WIDGET(alignment)->size_allocate = _etk_alignment_size_allocate;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_alignment_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Alignment *alignment;

   if (!(alignment = ETK_ALIGNMENT(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_ALIGNMENT_XALIGN_PROPERTY:
         etk_alignment_set(alignment, etk_property_value_float_get(value), alignment->yalign, alignment->xscale, alignment->yscale);
         break;
      case ETK_ALIGNMENT_YALIGN_PROPERTY:
         etk_alignment_set(alignment, alignment->xalign, etk_property_value_float_get(value), alignment->xscale, alignment->yscale);
         break;
      case ETK_ALIGNMENT_XSCALE_PROPERTY:
         etk_alignment_set(alignment, alignment->xalign, alignment->yalign, etk_property_value_float_get(value), alignment->yscale);
         break;
      case ETK_ALIGNMENT_YSCALE_PROPERTY:
         etk_alignment_set(alignment, alignment->xalign, alignment->yalign, alignment->xscale, etk_property_value_float_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_alignment_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Alignment *alignment;

   if (!(alignment = ETK_ALIGNMENT(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_ALIGNMENT_XALIGN_PROPERTY:
         etk_property_value_float_set(value, alignment->xalign);
         break;
      case ETK_ALIGNMENT_YALIGN_PROPERTY:
         etk_property_value_float_set(value, alignment->yalign);
         break;
      case ETK_ALIGNMENT_XSCALE_PROPERTY:
         etk_property_value_float_set(value, alignment->xscale);
         break;
      case ETK_ALIGNMENT_YSCALE_PROPERTY:
         etk_property_value_float_set(value, alignment->yscale);
         break;
      default:
         break;
   }
}

/* Resizes the alignment to the size allocation */
static void _etk_alignment_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Alignment *alignment;
   Etk_Widget *child;
   Etk_Container *container;
   Etk_Size child_requisition;

   if (!(alignment = ETK_ALIGNMENT(widget)))
      return;

   container = ETK_CONTAINER(widget);

   if ((child = etk_bin_child_get(ETK_BIN(alignment))))
   {
      etk_widget_size_request(child, &child_requisition);

      geometry.x += etk_container_border_width_get(container);
      geometry.y += etk_container_border_width_get(container);
      geometry.w -= 2 * etk_container_border_width_get(container);
      geometry.h -= 2 * etk_container_border_width_get(container);

      if (geometry.w > child_requisition.w)
      {
         child_requisition.w += alignment->xscale * (geometry.w - child_requisition.w);
         geometry.x += alignment->xalign * (geometry.w - child_requisition.w);
         geometry.w = child_requisition.w;
      }
      if (geometry.h > child_requisition.h)
      {
         child_requisition.h += alignment->yscale * (geometry.h - child_requisition.h);
         geometry.y += alignment->yalign * (geometry.h - child_requisition.h);
         geometry.h = child_requisition.h;
      }

      etk_widget_size_allocate(child, geometry);
   }
}

/** @} */
