/** @file etk_text_view.c */
#include "etk_text_view.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_text_buffer.h"
#include "etk_string.h"

/**
 * @addtogroup Etk_Text_View
* @{
 */

enum _Etk_Text_View_Signal_Id
{
   ETK_TEXT_VIEW_TEXT_CHANGED_SIGNAL,
   ETK_TEXT_VIEW_NUM_SIGNALS
};

static void _etk_text_view_constructor(Etk_Text_View *text_view);
static void _etk_text_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_text_view_realize_cb(Etk_Object *object, void *data);

static Etk_Signal *_etk_text_view_signals[ETK_TEXT_VIEW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Text_View
 * @return Returns the type on an Etk_Text_View
 */
Etk_Type *etk_text_view_type_get()
{
   static Etk_Type *text_view_type = NULL;

   if (!text_view_type)
   {
      /* TODO: destructor */
      text_view_type = etk_type_new("Etk_Text_View", ETK_WIDGET_TYPE, sizeof(Etk_Text_View), ETK_CONSTRUCTOR(_etk_text_view_constructor), NULL);

      _etk_text_view_signals[ETK_TEXT_VIEW_TEXT_CHANGED_SIGNAL] = etk_signal_new("text_changed", text_view_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return text_view_type;
}

/**
 * @brief Creates a new text view
 * @return Returns the new text view widget
 */
Etk_Widget *etk_text_view_new()
{
   return etk_widget_new(ETK_TEXT_VIEW_TYPE, "theme_group", "text_view", "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
}


/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the text_view */
static void _etk_text_view_constructor(Etk_Text_View *text_view)
{
   if (!text_view)
      return;

   text_view->text_buffer = etk_text_buffer_new();
   text_view->textblock_object = NULL;
   ETK_WIDGET(text_view)->size_allocate = _etk_text_view_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(text_view), ETK_CALLBACK(_etk_text_view_realize_cb), NULL);
}

/* TODO: Renders the textblock object */
static void _etk_text_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Text_View *text_view;
   
   if (!(text_view = ETK_TEXT_VIEW(widget)))
      return;
   
   printf("text_view_size_allocate\n");
   evas_object_textblock_text_markup_set(text_view->textblock_object, etk_string_get(text_view->text_buffer->formatted_buffer));
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the text_view is realized */
static void _etk_text_view_realize_cb(Etk_Object *object, void *data)
{
   Evas_Textblock_Style *text_style;
   Etk_Text_View *text_view;
   Etk_Widget *text_view_widget;
   Evas *evas;

   if (!(text_view_widget = ETK_WIDGET(object)) || !(evas = etk_widget_toplevel_evas_get(text_view_widget)))
      return;

   text_view = ETK_TEXT_VIEW(text_view_widget);
   text_view->textblock_object = evas_object_textblock_add(evas);
   text_style = evas_textblock_style_new();
   evas_textblock_style_set(text_style, "DEFAULT='font=Vera font_size=10 align=left color=#000000 wrap=word' br='\n'");
   evas_object_textblock_style_set(text_view->textblock_object, text_style);
   evas_object_show(text_view->textblock_object);
   etk_widget_theme_object_swallow(text_view_widget, "text_area", text_view->textblock_object);
   etk_widget_member_object_add(text_view_widget, text_view->textblock_object);
}

/** @} */
