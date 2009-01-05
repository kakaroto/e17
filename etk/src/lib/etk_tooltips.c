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

/** @file etk_tooltips.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_tooltips.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Evas.h>

#include "etk_event.h"
#include "etk_label.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_window.h"

/**
 * @addtogroup Etk_Tooltips
 * @{
 */

static Eina_Bool _etk_tooltips_hash_free(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Etk_Bool _etk_tooltips_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In *event, void *data);
static Etk_Bool _etk_tooltips_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_Out *event, void *data);
static Etk_Bool _etk_tooltips_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data);
static Etk_Bool _etk_tooltips_mouse_down_cb(Etk_Object *object, void *event, void *data);
static Etk_Bool _etk_tooltips_mouse_wheel_cb(Etk_Object *object, void *event, void *data);
static Etk_Bool _etk_tooltips_key_down_cb(Etk_Object *object, void *event, void *data);
static Etk_Bool _etk_tooltips_widget_unrealized_cb(Etk_Object *object, void *data);
static int _etk_tooltips_timer_cb(void *data);

static Etk_Widget *_etk_tooltips_window = NULL;
static Etk_Widget *_etk_tooltips_label = NULL;
static Etk_Object *_etk_tooltips_cur_object = NULL;
static double _etk_tooltips_delay = 1.0;
static Etk_Bool _etk_tooltips_enabled = ETK_FALSE;
static Etk_Bool _etk_tooltips_initialized = ETK_FALSE;
static Ecore_Timer *_etk_tooltips_timer = NULL;
static Eina_Hash *_etk_tooltips_hash = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initialize tooltips subsystem
 */
void etk_tooltips_init()
{
   if(_etk_tooltips_initialized)
     return;


   _etk_tooltips_window = etk_widget_new(ETK_WINDOW_TYPE, "theme-group", "tooltip", "decorated", ETK_FALSE, "skip-taskbar", ETK_TRUE, "skip-pager", ETK_TRUE, NULL);

   _etk_tooltips_label = etk_label_new(NULL);
   etk_container_add(ETK_CONTAINER(_etk_tooltips_window), _etk_tooltips_label);

   _etk_tooltips_hash = eina_hash_string_superfast_new(NULL);

   _etk_tooltips_initialized = ETK_TRUE;
}

/**
 * @brief Shutdown tooltips subsystem
 */
void etk_tooltips_shutdown()
{
   if(!_etk_tooltips_initialized)
     return;

   //etk_object_destroy(_etk_tooltips_window);
   eina_hash_foreach(_etk_tooltips_hash, _etk_tooltips_hash_free, NULL);
   eina_hash_free(_etk_tooltips_hash);
   _etk_tooltips_initialized = ETK_FALSE;
}

/**
 * @brief Shutdown tooltips subsystem
 */
void etk_tooltips_enable()
{
   _etk_tooltips_enabled = ETK_TRUE;
}

/**
 * @brief Get wether tooltips are enabled or disabled
 */
Etk_Bool etk_tooltips_enabled_get()
{
   return _etk_tooltips_enabled;
}

/**
 * @brief Disable tooltips
 */
void etk_tooltips_disable()
{
   _etk_tooltips_enabled = ETK_FALSE;
}

/**
 * @brief Set tooltip text for a widget
 * @param widget the widget whose tooltip we want to set
 * @param text the text of the tooltip
 */
void etk_tooltips_tip_set(Etk_Widget *widget, const char *text)
{
   char *tip_text = NULL;
   char *key;

   key = calloc(32, sizeof(char));
   snprintf(key, 32 * sizeof(char), "%p", widget);

   if((tip_text = eina_hash_find(_etk_tooltips_hash, key)) != NULL)
   {
      if(text == NULL)
      {
	 etk_tooltips_pop_down();
	 eina_hash_del(_etk_tooltips_hash, key, tip_text);
	 etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_IN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_in_cb), NULL);
	 etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_OUT_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_out_cb), NULL);
	 etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_move_cb), NULL);
	 etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_down_cb), NULL);
	 etk_signal_disconnect_by_code(ETK_WIDGET_MOUSE_WHEEL_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_wheel_cb), NULL);
	 etk_signal_disconnect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_key_down_cb), NULL);
	 etk_signal_disconnect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_widget_unrealized_cb), NULL);
	 free(tip_text);
      }
      else
      {
	 free(tip_text);
	 tip_text = strdup(text);
	 eina_hash_modify(_etk_tooltips_hash, key, tip_text);
      }
   }
   else
   {
      if(text != NULL)
      {
	 eina_hash_add(_etk_tooltips_hash, key, strdup(text));
	 etk_signal_connect_by_code(ETK_WIDGET_MOUSE_IN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_in_cb), NULL);
	 etk_signal_connect_by_code(ETK_WIDGET_MOUSE_OUT_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_out_cb), NULL);
	 etk_signal_connect_by_code(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_move_cb), NULL);
	 etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_down_cb), NULL);
	 etk_signal_connect_by_code(ETK_WIDGET_MOUSE_WHEEL_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_wheel_cb), NULL);
	 etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_key_down_cb), NULL);
	 etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_widget_unrealized_cb), NULL);
      }
   }
   free(key);
}

/**
 * @brief Get tooltip text for a widget
 * @param widget the widget whose tooltip we want to set
 * @return The text of the tooltip of the widget or NULL if it isn't set
 */
 const char *etk_tooltips_tip_get(Etk_Widget *widget)
{
   const char *tip_text;
   char *key;

   key = calloc(32, sizeof(char));
   snprintf(key, 32 * sizeof(char), "%p", widget);
   if((tip_text = eina_hash_find(_etk_tooltips_hash, key)) != NULL)
   {
      free(key);
      return tip_text;
   }
   free(key);
   return NULL;
}

/*
 * @brief Get wether the toolip is hidden / visible
 * @return True is it is, false otherwise
 */
Etk_Bool etk_tooltips_tip_visible()
{
   if(!_etk_tooltips_enabled || !ETK_IS_WINDOW(_etk_tooltips_window))
     return ETK_FALSE;

   return etk_widget_is_visible(_etk_tooltips_window);
}

/**
 * @brief Instantly pop up the tooltip if applicable
 * @param widget the widget we want to pop up on
 */
void etk_tooltips_pop_up(Etk_Widget *widget)
{
   const char *text = NULL;
   int x, y;
   char *key;
   Etk_Toplevel *toplevel;
   Evas *evas;
   int pt_x, pt_y;
   int win_x, win_w;

   if(!ETK_IS_OBJECT(widget))
     return;

   key = calloc(32, sizeof(char));
   snprintf(key, 32 * sizeof(char), "%p", ETK_OBJECT(widget));
   if((text = eina_hash_find(_etk_tooltips_hash, key)) == NULL)
   {
      free(key);
      _etk_tooltips_timer = NULL;
      return;
   }

     {
	/* We are doing this because if the label / window grow, then are not shrinking anymore */
	etk_object_destroy(ETK_OBJECT(_etk_tooltips_window));
	_etk_tooltips_window = etk_widget_new(ETK_WINDOW_TYPE, "theme-group", "tooltip", "decorated", ETK_FALSE, "skip-taskbar", ETK_TRUE, "skip-pager", ETK_TRUE, NULL);

	_etk_tooltips_label = etk_label_new(NULL);
	etk_container_add(ETK_CONTAINER(_etk_tooltips_window), _etk_tooltips_label);
     }

   etk_label_set(ETK_LABEL(_etk_tooltips_label), text);

   etk_engine_mouse_position_get(&x, &y);

   /* TODO: if tooltip window is outside screen, fix its place */
   etk_window_move(ETK_WINDOW(_etk_tooltips_window), x - 10, y + 20);
   etk_widget_show_all(_etk_tooltips_window);
   free(key);

   _etk_tooltips_timer = NULL;
}

/**
 * @brief Instantly pop down the tooltip if applicable
 */
void etk_tooltips_pop_down()
{
   if(!_etk_tooltips_enabled)
     return;

   etk_widget_hide(_etk_tooltips_window);
   _etk_tooltips_timer = NULL;
   _etk_tooltips_cur_object = NULL;
}

/* Callback for when the mouse enters a widget */
static Etk_Bool _etk_tooltips_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In *event, void *data)
{
   if(!_etk_tooltips_enabled || !ETK_IS_OBJECT(object))
     return ETK_TRUE;

   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
   return ETK_TRUE;
}

/* Timer callback, pops up the tooltip */
static int _etk_tooltips_timer_cb(void *data)
{
   if(!_etk_tooltips_timer || !_etk_tooltips_cur_object ||!_etk_tooltips_label)
     return 0;

   etk_tooltips_pop_up(ETK_WIDGET(_etk_tooltips_cur_object));
   return 0;
}

/* Callback for when the mouse leave the widget */
static Etk_Bool _etk_tooltips_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_Out *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return ETK_TRUE;

   if(_etk_tooltips_timer != NULL)
     ecore_timer_del(_etk_tooltips_timer);

   etk_tooltips_pop_down();
   return ETK_TRUE;
}

/* Callback for when the mouse moves on the widget */
static Etk_Bool _etk_tooltips_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return ETK_TRUE;

   if(!ETK_IS_WINDOW(_etk_tooltips_window))
     return ETK_TRUE;

   if(etk_widget_is_visible(_etk_tooltips_window))
     return ETK_TRUE;

   if(_etk_tooltips_timer != NULL)
     ecore_timer_del(_etk_tooltips_timer);

   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
   return ETK_TRUE;
}

/* Callback for when the mouse clicks the widget */
static Etk_Bool _etk_tooltips_mouse_down_cb(Etk_Object *object, void *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return ETK_TRUE;

   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
   return ETK_TRUE;
}

/* Callback for when the mouse wheel is moved on the widget */
static Etk_Bool _etk_tooltips_mouse_wheel_cb(Etk_Object *object, void *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return ETK_TRUE;

   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);

   return ETK_TRUE;
}

/* Callback for when the a key is pressed on the widget */
static Etk_Bool _etk_tooltips_key_down_cb(Etk_Object *object, void *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return ETK_TRUE;

   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);

   return ETK_TRUE;
}

/* Callback for when the widget is unrealized */
static Etk_Bool _etk_tooltips_widget_unrealized_cb(Etk_Object *object, void *data)
{
   char *key;
   void *value = NULL;

   key = calloc(32, sizeof(char));
   snprintf(key, 32 * sizeof(char), "%p", ETK_WIDGET(object));

   if((value = eina_hash_find(_etk_tooltips_hash, key)) == NULL)
   {
      free(key);
      return ETK_TRUE;
   }

   eina_hash_del(_etk_tooltips_hash, key, value);
   free(key);
   return ETK_TRUE;
}

/* free hash items */
static Eina_Bool _etk_tooltips_hash_free(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
   if(data)
     free(data);
   return 1;
}

/** @} */
