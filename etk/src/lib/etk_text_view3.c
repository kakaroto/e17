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

/** @file etk_text_view3.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_text_view3.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Edje.h>

#include "etk_event.h"
#include "etk_selection.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_string.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Text_View3
 * @{
 */
static void     _etk_text_view3_constructor(Etk_Text_View3 *text_view);
static void     _etk_text_view3_destructor(Etk_Text_View3 *text_view);

static void     _etk_text_view3_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void     _etk_text_view3_scroll(Etk_Widget *widget, int x, int y);
static void     _etk_text_view3_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);

static Etk_Bool _etk_text_view3_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_text_view3_unrealized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_text_view3_focused_cb(Etk_Widget *widget, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Text_View3
 * @return Returns the type of an Etk_Text_View3
 */
Etk_Type *etk_text_view3_type_get(void)
{
   static Etk_Type *text_view3_type = NULL;

   if (!text_view3_type)
   {
      text_view3_type = etk_type_new("Etk_Text_View3", ETK_WIDGET_TYPE, sizeof(Etk_Text_View3),
            ETK_CONSTRUCTOR(_etk_text_view3_constructor), ETK_DESTRUCTOR(_etk_text_view3_destructor), NULL);
   }

   return text_view3_type;
}

/**
 * @brief Creates a new text-view
 * @return Returns the new text-view widget
 */
Etk_Widget *etk_text_view3_new(void)
{
   return etk_widget_new(ETK_TEXT_VIEW3_TYPE, "theme-group", "text_view3", "focusable", ETK_TRUE,
      "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Gets the content of the text-view
 * @param text_view a text-view
 * @return Returns the content of the text view
 */
const char *etk_text_view3_text_get(Etk_Text_View3 *text_view)
{
   Etk_Widget *w;

   if (!(w = ETK_WIDGET(text_view)))
      return NULL;

   return edje_object_part_text_get(w->theme_object, "etk.text.textblock");
}

/**
 * @brief Set the content of the text-view
 * @param text_view a text-view
 * @param text a string containing the text to show
 */
void etk_text_view3_text_set(Etk_Text_View3 *text_view, const char *text)
{
   Etk_Widget *w;

   if (!text || !(w = ETK_WIDGET(text_view)))
      return;

   edje_object_part_text_set(w->theme_object, "etk.text.textblock", text);
}

/**
 * @brief Insert the given text at the current cursor position
 * @param text_view a text-view
 * @param text a string containing the text to insert
 */
void etk_text_view3_text_insert(Etk_Text_View3 *text_view, const char *text)
{
   Etk_Widget *w;

   if (!text || !(w = ETK_WIDGET(text_view)))
      return;

   edje_object_part_text_insert(w->theme_object, "etk.text.textblock", text);
}

/**
 * @brief Get the selected text
 * @param text_view a text-view
 * @return Returns the selected text
 */
const char *etk_text_view3_selection_get(Etk_Text_View3 *text_view)
{
   Etk_Widget *w;

   if (!(w = ETK_WIDGET(text_view)))
      return NULL;

   return edje_object_part_text_selection_get(w->theme_object, "etk.text.textblock");
}

/**
 * @brief Get the evas textblock object
 * @param text_view a text-view
 * @return the evas textblock object
 */
Evas_Object *etk_text_view3_textblock_get(Etk_Text_View3 *text_view)
{
   return text_view->tbo;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/
/* Initializes the text-view */
static void _etk_text_view3_constructor(Etk_Text_View3 *text_view)
{
   Etk_Widget *w;

   if (!(w = ETK_WIDGET(text_view)))
      return;

   w->size_allocate = _etk_text_view3_size_allocate;

   ETK_WIDGET(text_view)->scroll = _etk_text_view3_scroll;
   ETK_WIDGET(text_view)->scroll_size_get = _etk_text_view3_scroll_size_get;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(w),
                              ETK_CALLBACK(_etk_text_view3_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(w),
                              ETK_CALLBACK(_etk_text_view3_unrealized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_FOCUSED_SIGNAL, ETK_OBJECT(w),
                              ETK_CALLBACK(_etk_text_view3_focused_cb), NULL);
}

/* Destroys the text-view */
static void _etk_text_view3_destructor(Etk_Text_View3 *text_view)
{
   if (!text_view)
      return;
}

/* TODO: size_allocate doc, swallow?? */
static void _etk_text_view3_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Text_View3 *text_view;

   if (!(text_view = ETK_TEXT_VIEW3(widget)))
      return;

   evas_object_move(text_view->tbo, geometry.x, geometry.y);
   evas_object_resize(text_view->tbo, geometry.w, geometry.h);
}

/* Size of all the text_view for scrolling ability. */
static void _etk_text_view3_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size)
{
   Etk_Text_View3 *text_view;

   if(!(text_view = ETK_TEXT_VIEW3(widget)) || !scroll_size )
      return;

   //etk_textblock_object_full_geometry_get( text_view->textblock_object, NULL, NULL, &(scroll_size->w), &(scroll_size->h) );
   scroll_size->w = 100;
   scroll_size->h = 10;
}

static void _etk_text_view3_scroll(Etk_Widget *widget, int x, int y)
{
   Etk_Text_View3 *text_view;

   if( !( text_view = ETK_TEXT_VIEW3(widget) ) )
      return;

   //etk_textblock_object_xoffset_set( text_view->textblock_object, x );
   //etk_textblock_object_yoffset_set( text_view->textblock_object, y );

   etk_widget_redraw_queue( widget );
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the text-view is realized */
static Etk_Bool _etk_text_view3_realized_cb(Etk_Object *object, void *data)
{
   Etk_Text_View3 *text_view;

   if (!(text_view = ETK_TEXT_VIEW3(object)))
      return ETK_TRUE;

   text_view->tbo = ETK_WIDGET(text_view)->theme_object;

   return ETK_TRUE;
}

/* Called when the text-view is unrealized */
static Etk_Bool _etk_text_view3_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Text_View3 *text_view;

   if (!(text_view = ETK_TEXT_VIEW3(object)))
      return ETK_TRUE;

   return ETK_TRUE;
}

/* Called when the text-view is focused */
static Etk_Bool _etk_text_view3_focused_cb(Etk_Widget *widget, void *data)
{
   evas_object_focus_set(widget->theme_object, 1);
   return ETK_FALSE;
}

/** @} */
