/** @file etk_label.c */
#include "etk_label.h"
#include <stdlib.h>
#include <string.h>
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
static void _etk_label_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_label_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_label_clip_set(Etk_Widget *widget, Evas_Object *clip);
static void _etk_label_clip_unset(Etk_Widget *widget);
static void _etk_label_realize_cb(Etk_Object *object, void *data);
static void _etk_label_unrealize_cb(Etk_Object *object, void *data);

static Evas_Textblock_Style *_etk_label_style = NULL;
static int _etk_label_style_use = 0;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Label
 * @return Returns the type of an Etk_Label
 */
Etk_Type *etk_label_type_get()
{
   static Etk_Type *label_type = NULL;

   if (!label_type)
   {
      label_type = etk_type_new("Etk_Label", ETK_WIDGET_TYPE, sizeof(Etk_Label),
         ETK_CONSTRUCTOR(_etk_label_constructor), ETK_DESTRUCTOR(_etk_label_destructor));
      
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
   return etk_widget_new(ETK_LABEL_TYPE, "label", text, NULL);
}

/**
 * @brief Sets the text of the label
 * @param label a label
 * @param text the text to set
 */
void etk_label_set(Etk_Label *label, const char *text)
{
   char *old_label;

   if (!label)
      return;

   old_label = label->text;
   /* TODO: etk_label_set: " " if the length of the label is 0 */
   if (!text || *text == 0)
      label->text = strdup(" ");
   else
      label->text = strdup(text);
   free(old_label);

   if (label->text_object)
      evas_object_textblock_text_markup_set(label->text_object, label->text);
   
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

   label->text_object = NULL;
   label->clip = NULL;
   label->text = NULL;

   label->xalign = 0.0;
   label->yalign = 0.5;

   widget->size_request = _etk_label_size_request;
   widget->size_allocate = _etk_label_size_allocate;
   widget->clip_set = _etk_label_clip_set;
   widget->clip_unset = _etk_label_clip_unset;

   etk_signal_connect("realize", ETK_OBJECT(label), ETK_CALLBACK(_etk_label_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(label), ETK_CALLBACK(_etk_label_unrealize_cb), NULL);
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

/* Calculates the ideal size of the label */
static void _etk_label_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Label *label;

   if (!(label = ETK_LABEL(widget)) || !size_requisition)
      return;

   size_requisition->w = 0;
   size_requisition->h = 0;
   if (label->text_object)
      evas_object_textblock_size_native_get(label->text_object, &size_requisition->w, &size_requisition->h);
}

/* Resizes the label to the allocated size */
static void _etk_label_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Label *label;
   Etk_Size requested_size;

   if (!(label = ETK_LABEL(widget)))
      return;

   etk_widget_size_request(widget, &requested_size);
   evas_object_move(label->text_object, geometry.x + (geometry.w - requested_size.w) * label->xalign,
      geometry.y + (geometry.h - requested_size.h) * label->yalign);
   evas_object_resize(label->text_object, requested_size.w, requested_size.h);

   evas_object_move(label->clip, geometry.x, geometry.y);
   evas_object_resize(label->clip, geometry.w, geometry.h);
}

/* Clips the label against "clip" */
static void _etk_label_clip_set(Etk_Widget *widget, Evas_Object *clip)
{
   Etk_Label *label;
   
   if (!(label = ETK_LABEL(widget)) || !label->clip || !clip)
      return;
   evas_object_clip_set(label->clip, clip);
}

/* Unclips the label */
static void _etk_label_clip_unset(Etk_Widget *widget)
{
   Etk_Label *label;
   
   if (!(label = ETK_LABEL(widget)) || !label->clip)
      return;
   evas_object_clip_unset(label->clip);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the label is realized */
static void _etk_label_realize_cb(Etk_Object *object, void *data)
{
   Etk_Label *label;
   Evas *evas;

   if (!(label = ETK_LABEL(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(label))))
      return;

   if (!_etk_label_style)
   {
      _etk_label_style = evas_textblock_style_new();
      evas_textblock_style_set(_etk_label_style,
         "DEFAULT='font=Vera font_size=10 align=left color=#000000 wrap=word style=shadow shadow_color=#ffffff80'"
         "center='+ align=center'"
         "/center='- \n'"
         "right='+ align=right'"
         "/right='- \n'"
         "h1='+ font_size=20'"
	 "b='+font=Vera-Bold'"
	 "i='+font=Vera-Italic'"			       
         "glow='+ style=glow color=#fff glow2_color=#fe87 glow_color=#fa14'"
         "link='+ underline=on underline_color=#0000aa color=#0000aa'"       			       
         "red='+ color=#ff0000'"
         "br='\n'"
         "tab='\t'");
      _etk_label_style_use = 0;
   }

   label->clip = evas_object_rectangle_add(evas);
   evas_object_show(label->clip);
   etk_widget_member_object_add(ETK_WIDGET(label), label->clip);
   
   label->text_object = evas_object_textblock_add(evas);
   evas_object_textblock_style_set(label->text_object, _etk_label_style);
   evas_object_show(label->text_object);
   etk_widget_member_object_add(ETK_WIDGET(label), label->text_object);
   evas_object_clip_set(label->text_object, label->clip);
   _etk_label_style_use++;

   evas_object_textblock_text_markup_set(label->text_object, label->text);
   etk_widget_size_recalc_queue(ETK_WIDGET(label));
}

/* Called when the label is unrealized */
static void _etk_label_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Label *label;
   
   if (!(label = ETK_LABEL(object)))
      return;

   _etk_label_style_use--;
   if (_etk_label_style_use <= 0 && _etk_label_style)
   {
      evas_textblock_style_free(_etk_label_style);
      _etk_label_style = NULL;
   }
   label->text_object = NULL;
   label->clip = NULL;
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
 * You can use html-like tags to format the text of the label. For example, "<b>Text</b>" makes <b>Text</b> bold. @n
 * Here is the list of the supported tags: @n
 * TODO: [Doc] List of the tags for the label
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
