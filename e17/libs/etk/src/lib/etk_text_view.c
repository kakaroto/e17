/** @file etk_text_view.c */
#include "etk_text_view.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_textblock.h"

/**
 * @addtogroup Etk_Text_View
 * @{
 */

enum Etk_Text_View_Signal_Id
{
   ETK_TEXT_VIEW_TEXT_CHANGED_SIGNAL,
   ETK_TEXT_VIEW_NUM_SIGNALS
};

static void _etk_text_view_constructor(Etk_Text_View *text_view);
static void _etk_text_view_destructor(Etk_Text_View *text_view);
static void _etk_text_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_text_view_realize_cb(Etk_Object *object, void *data);
static void _etk_text_view_unrealize_cb(Etk_Object *object, void *data);
static void _etk_text_view_key_down_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);

static Etk_Signal *_etk_text_view_signals[ETK_TEXT_VIEW_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Text_View
 * @return Returns the type of an Etk_Text_View
 */
Etk_Type *etk_text_view_type_get()
{
   static Etk_Type *text_view_type = NULL;

   if (!text_view_type)
   {
      text_view_type = etk_type_new("Etk_Text_View", ETK_WIDGET_TYPE, sizeof(Etk_Text_View),
         ETK_CONSTRUCTOR(_etk_text_view_constructor), ETK_DESTRUCTOR(_etk_text_view_destructor));

      _etk_text_view_signals[ETK_TEXT_VIEW_TEXT_CHANGED_SIGNAL] = etk_signal_new("text_changed",
         text_view_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
   }

   return text_view_type;
}

/**
 * @brief Creates a new text view
 * @return Returns the new text view widget
 */
Etk_Widget *etk_text_view_new()
{
   return etk_widget_new(ETK_TEXT_VIEW_TYPE, "theme_group", "text_view", "focusable", ETK_TRUE,
      "focus_on_press", ETK_TRUE, NULL);
}

/**
 * @brief Gets the textblock of the text view
 * @return Returns the textblock of the textview
 */
Etk_Textblock *etk_text_view_textblock_get(Etk_Text_View *text_view)
{
   if (!text_view)
      return NULL;
   return text_view->textblock;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the text view */
static void _etk_text_view_constructor(Etk_Text_View *text_view)
{
   if (!text_view)
      return;

   text_view->textblock = etk_textblock_new();
   text_view->textblock_object = NULL;
   
   ETK_WIDGET(text_view)->size_allocate = _etk_text_view_size_allocate;

   etk_signal_connect("realize", ETK_OBJECT(text_view), ETK_CALLBACK(_etk_text_view_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(text_view), ETK_CALLBACK(_etk_text_view_unrealize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(text_view), ETK_CALLBACK(_etk_text_view_key_down_cb), NULL);
}

/* Destroys the text view */
static void _etk_text_view_destructor(Etk_Text_View *text_view)
{
   if (!text_view)
      return;
   etk_object_destroy(ETK_OBJECT(text_view->textblock));
}

/* TODO: size_allocate doc, swallow?? */
static void _etk_text_view_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Text_View *text_view;
   
   if (!(text_view = ETK_TEXT_VIEW(widget)))
      return;
   
   evas_object_move(text_view->textblock_object, geometry.x, geometry.y);
   evas_object_resize(text_view->textblock_object, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the text view is realized */
static void _etk_text_view_realize_cb(Etk_Object *object, void *data)
{
   Etk_Text_View *text_view;
   Evas *evas;

   if (!(text_view = ETK_TEXT_VIEW(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(text_view))))
      return;

   text_view->textblock_object = etk_textblock_object_add(text_view->textblock, evas);
   etk_widget_member_object_add(ETK_WIDGET(text_view), text_view->textblock_object);
   evas_object_repeat_events_set(text_view->textblock_object, 1);
   evas_object_show(text_view->textblock_object);
}

/* Called when the text view is unrealized */
static void _etk_text_view_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Text_View *text_view;

   if (!(text_view = ETK_TEXT_VIEW(object)))
      return;
   
   if (text_view->textblock_object)
   {
      etk_widget_member_object_del(ETK_WIDGET(text_view), text_view->textblock_object);
      evas_object_del(text_view->textblock_object);
      text_view->textblock_object = NULL;
   }
}

/* Called when a key is pressed */
static void _etk_text_view_key_down_cb(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data)
{
   Etk_Text_View *text_view;
   Etk_Textblock_Iter *cursor;
   Etk_Textblock_Iter *selection;
   
   if (!(text_view = ETK_TEXT_VIEW(object)) || !event || !text_view->textblock_object)
      return;
   
   cursor = etk_textblock_object_cursor_get(text_view->textblock_object);
   selection = etk_textblock_object_selection_bound_get(text_view->textblock_object);
   
   if (strcmp(event->key, "Left") == 0)
   {
      etk_textblock_iter_backward_char(cursor);
      if (!evas_key_modifier_is_set(event->modifiers, "Shift"))
         etk_textblock_iter_copy(selection, cursor);
   }
   else if (strcmp(event->key, "Right") == 0)
   {
      etk_textblock_iter_forward_char(cursor);
      if (!evas_key_modifier_is_set(event->modifiers, "Shift"))
         etk_textblock_iter_copy(selection, cursor);
   }
   else if (strcmp(event->key, "Return") == 0 || strcmp(event->key, "KP_Enter") == 0)
      etk_textblock_text_insert(text_view->textblock, cursor, "\n", -1);
   else if (event->string && !(strlen(event->string) == 1 && event->string[0] < 0x20))
      etk_textblock_text_insert(text_view->textblock, cursor, event->string, -1);
}

/** @} */
