/** @file etk_tooltips.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Evas.h>
#include "etk_tooltips.h"
#include "etk_window.h"
#include "etk_label.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

#include "config.h"

/**
 * @addtogroup Etk_Tooltips
 * @{
 */

static Evas_Bool _etk_tooltips_hash_free(Evas_Hash *hash, const char *key, void *data, void *fdata);
static void _etk_tooltips_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_tooltips_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data);
static void _etk_tooltips_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data);
static void _etk_tooltips_mouse_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tooltips_mouse_wheel_cb(Etk_Object *object, void *event, void *data);
static void _etk_tooltips_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tooltips_widget_unrealize_cb(Etk_Object *object, void *data);
static int _etk_tooltips_timer_cb(void *data);
  
static Etk_Widget *_etk_tooltips_window = NULL;
static Etk_Widget *_etk_tooltips_label = NULL;
static Etk_Object *_etk_tooltips_cur_object = NULL;
static double _etk_tooltips_delay = 1.0;
static Etk_Bool _etk_tooltips_enabled = ETK_FALSE;
static Etk_Bool _etk_tooltips_initialized = ETK_FALSE;
static Ecore_Timer *_etk_tooltips_timer = NULL;
static Evas_Hash *_etk_tooltips_hash = NULL;

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

   
   _etk_tooltips_window = etk_widget_new(ETK_WINDOW_TYPE, "theme_group", "tooltip", "decorated", ETK_FALSE, "skip_taskbar", ETK_TRUE, "skip_pager", ETK_TRUE, NULL);   
   
   _etk_tooltips_label = etk_label_new(NULL);
   etk_container_add(ETK_CONTAINER(_etk_tooltips_window), _etk_tooltips_label);
   
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
   evas_hash_foreach(_etk_tooltips_hash, _etk_tooltips_hash_free, NULL);
   evas_hash_free(_etk_tooltips_hash);
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

   if((tip_text = evas_hash_find(_etk_tooltips_hash, key)) != NULL)
   {
      if(text == NULL)
      {
	 _etk_tooltips_hash = evas_hash_del(_etk_tooltips_hash, key, tip_text);
	 etk_signal_disconnect("mouse_in", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_in_cb));
	 etk_signal_disconnect("mouse_out", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_out_cb));
	 etk_signal_disconnect("mouse_move", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_move_cb));
	 etk_signal_disconnect("mouse_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_down_cb));
	 etk_signal_disconnect("mouse_wheel", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_wheel_cb));
	 etk_signal_disconnect("key_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_key_down_cb));
	 etk_signal_disconnect("unrealize", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_widget_unrealize_cb));
	 free(tip_text);	 
      }
      else
      {
	 free(tip_text);	 
	 tip_text = strdup(text);
	 _etk_tooltips_hash = evas_hash_modify(_etk_tooltips_hash, key, tip_text);
      }
   }
   else
   {
      if(text != NULL)
      {
	 _etk_tooltips_hash = evas_hash_add(_etk_tooltips_hash, key, strdup(text));
	 etk_signal_connect("mouse_in", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_in_cb), NULL);
	 etk_signal_connect("mouse_out", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_out_cb), NULL);
	 etk_signal_connect("mouse_move", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_move_cb), NULL);
	 etk_signal_connect("mouse_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_down_cb), NULL);
	 etk_signal_connect("mouse_wheel", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_mouse_wheel_cb), NULL);
	 etk_signal_connect("key_down", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_key_down_cb), NULL);
	 etk_signal_connect("unrealize", ETK_OBJECT(widget), ETK_CALLBACK(_etk_tooltips_widget_unrealize_cb), NULL);
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
   if((tip_text = evas_hash_find(_etk_tooltips_hash, key)) != NULL)
   {
      free(key);
      return tip_text;
   }
   free(key);
   return NULL;
}

/**
 * @brief Instantly pop up the tooltip if applicable
 * @param widget the widget we want to pop up on
 */
void etk_tooltips_pop_up(Etk_Widget *widget)
{
#if HAVE_ECORE_X   
   const char *text = NULL;
   int x, y;
   char *key;

   if(!ETK_IS_OBJECT(widget))
     return;
   
   key = calloc(32, sizeof(char));
   snprintf(key, 32 * sizeof(char), "%p", ETK_OBJECT(widget));
   if((text = evas_hash_find(_etk_tooltips_hash, key)) == NULL)
   {
      free(key);
      _etk_tooltips_timer = NULL;      
      return;
   } 
   
     {
	/* We are doing this because if the label / window grow, then are not shrinking anymore */
	etk_object_destroy(ETK_OBJECT(_etk_tooltips_window));
	_etk_tooltips_window = etk_widget_new(ETK_WINDOW_TYPE, "theme_group", "tooltip", "decorated", ETK_FALSE, "skip_taskbar", ETK_TRUE, "skip_pager", ETK_TRUE, NULL);

	_etk_tooltips_label = etk_label_new(NULL);
	etk_container_add(ETK_CONTAINER(_etk_tooltips_window), _etk_tooltips_label);
     }
   
   etk_label_set(ETK_LABEL(_etk_tooltips_label), text);   
   ecore_x_pointer_xy_get(ecore_x_window_root_first_get(), &x, &y);
   /* TODO: if tooltip window is outside screen, fix its place */
   etk_window_move(ETK_WINDOW(_etk_tooltips_window), x + 5, y + 5);
   etk_widget_show_all(_etk_tooltips_window);
   free(key);
#endif  
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
   if(_etk_tooltips_timer)
     ecore_timer_del(_etk_tooltips_timer);
   _etk_tooltips_cur_object = NULL;
}

/* Callback for when the mouse enters a widget */
static void _etk_tooltips_mouse_in_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
#if HAVE_ECORE_X
   if(!_etk_tooltips_enabled || !ETK_IS_OBJECT(object))
     return;

   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
#endif   
}

/* Timer callback, pops up the tooltip */
static int _etk_tooltips_timer_cb(void *data)
{
   etk_tooltips_pop_up(ETK_WIDGET(_etk_tooltips_cur_object));
   return 0;
}

/* Callback for when the mouse leave the widget */
static void _etk_tooltips_mouse_out_cb(Etk_Object *object, Etk_Event_Mouse_In_Out *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return;
   
   etk_tooltips_pop_down();
}

/* Callback for when the mouse moves on the widget */
static void _etk_tooltips_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return;
   
   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
}

/* Callback for when the mouse clicks the widget */
static void _etk_tooltips_mouse_down_cb(Etk_Object *object, void *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return;
   
   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;   
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
}

/* Callback for when the mouse wheel is moved on the widget */
static void _etk_tooltips_mouse_wheel_cb(Etk_Object *object, void *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return;
   
   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;   
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
}

/* Callback for when the a key is pressed on the widget */
static void _etk_tooltips_key_down_cb(Etk_Object *object, void *event, void *data)
{
   if(!_etk_tooltips_enabled)
     return;
   
   etk_tooltips_pop_down();
   _etk_tooltips_cur_object = object;   
   _etk_tooltips_timer = ecore_timer_add(_etk_tooltips_delay, _etk_tooltips_timer_cb, NULL);
}

/* Callback for when the widget is unrealized */
static void _etk_tooltips_widget_unrealize_cb(Etk_Object *object, void *data)
{
   char *key;
   void *value = NULL;
   
   key = calloc(32, sizeof(char));
   snprintf(key, 32 * sizeof(char), "%p", ETK_WIDGET(object));
      
   if((value = evas_hash_find(_etk_tooltips_hash, key)) == NULL)
   {
      free(key);
      return;
   }
   
   _etk_tooltips_hash = evas_hash_del(_etk_tooltips_hash, key, value);
   free(key);
}

/* free hash items */
static Evas_Bool _etk_tooltips_hash_free(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   hash = evas_hash_del(hash, key, data);   
   if(data)
     free(data);
   return 1;
}

/** @} */
