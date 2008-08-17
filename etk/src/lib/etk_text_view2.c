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

/** @file etk_text_view2.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_text_view2.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "etk_event.h"
#include "etk_selection.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_string.h"
#include "etk_textblock2.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Text_View2
 * @{
 */

static void     _etk_text_view2_constructor(Etk_Text_View2 *text_view);
static void     _etk_text_view2_destructor(Etk_Text_View2 *text_view);
static void     _etk_text_view2_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static Etk_Bool _etk_text_view2_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_text_view2_unrealized_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Text_View2
 * @return Returns the type of an Etk_Text_View2
 */
Etk_Type *etk_text_view2_type_get(void)
{
   static Etk_Type *text_view2_type = NULL;

   if (!text_view2_type)
   {
      text_view2_type = etk_type_new("Etk_Text_View2", ETK_WIDGET_TYPE, sizeof(Etk_Text_View2),
            ETK_CONSTRUCTOR(_etk_text_view2_constructor), ETK_DESTRUCTOR(_etk_text_view2_destructor), NULL);
   }

   return text_view2_type;
}

/**
 * @brief Creates a new text-view
 * @return Returns the new text-view widget
 */
Etk_Widget *etk_text_view2_new(void)
{
   return etk_widget_new(ETK_TEXT_VIEW2_TYPE, "theme-group", "text_view", "focusable", ETK_TRUE,
      "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Gets the textblock used by the text-view
 * @param text_view a text-view
 * @return Returns the textblock used by the text-view
 */
Etk_Textblock2 *etk_text_view2_textblock_get(Etk_Text_View2 *text_view)
{
   if (!text_view)
      return NULL;
   return text_view->textblock;
}

/**
 * @brief Gets the cursor's iterator of the text-view
 * @param text_view a text-view
 * @param Returns the cursor's iterator of the text-view
 * @warning The text-view has to be realized, otherwise it will return NULL
 */
Etk_Textblock2_Iter *etk_text_view2_cursor_get(Etk_Text_View2 *text_view)
{
   if (!text_view || !text_view->tbo)
      return NULL;
   return etk_textblock2_object_cursor_get(text_view->tbo);
}

/**
 * @brief Gets the selection-bound's iterator of the text-view
 * @param text_view a text-view
 * @param Returns the selection-bound's iterator of the text-view
 * @warning The text-view has to be realized, otherwise it will return NULL
 */
Etk_Textblock2_Iter *etk_text_view2_selection_bound_get(Etk_Text_View2 *text_view)
{
   if (!text_view || !text_view->tbo)
      return NULL;
   return etk_textblock2_object_selection_bound_get(text_view->tbo);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the text-view */
static void _etk_text_view2_constructor(Etk_Text_View2 *text_view)
{
   if (!text_view)
      return;

   text_view->textblock = etk_textblock2_new();
   text_view->tbo = NULL;

   ETK_WIDGET(text_view)->size_allocate = _etk_text_view2_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(text_view),
         ETK_CALLBACK(_etk_text_view2_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(text_view),
         ETK_CALLBACK(_etk_text_view2_unrealized_cb), NULL);
}

/* Destroys the text-view */
static void _etk_text_view2_destructor(Etk_Text_View2 *text_view)
{
   if (!text_view)
      return;
   etk_object_destroy(ETK_OBJECT(text_view->textblock));
}

/* TODO: size_allocate doc, swallow?? */
static void _etk_text_view2_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Text_View2 *text_view;

   if (!(text_view = ETK_TEXT_VIEW2(widget)))
      return;

   evas_object_move(text_view->tbo, geometry.x, geometry.y);
   evas_object_resize(text_view->tbo, geometry.w, geometry.h);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the text-view is realized */
static Etk_Bool _etk_text_view2_realized_cb(Etk_Object *object, void *data)
{
   Etk_Text_View2 *text_view;
   Evas *evas;

   if (!(text_view = ETK_TEXT_VIEW2(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(text_view))))
      return ETK_TRUE;

   text_view->tbo = etk_textblock2_object_add(text_view->textblock, evas);
   etk_widget_member_object_add(ETK_WIDGET(text_view), text_view->tbo);
   evas_object_show(text_view->tbo);

   return ETK_TRUE;
}

/* Called when the text-view is unrealized */
static Etk_Bool _etk_text_view2_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Text_View2 *text_view;

   if (!(text_view = ETK_TEXT_VIEW2(object)))
      return ETK_TRUE;

   if (text_view->tbo)
   {
      evas_object_del(text_view->tbo);
      text_view->tbo = NULL;
   }

   return ETK_TRUE;
}

/** @} */
