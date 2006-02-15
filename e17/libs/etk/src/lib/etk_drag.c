/** @file etk_drag.c */
#include "etk_drag.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "etk_widget.h"
#include "etk_window.h"
#include "config.h"

Etk_Drag            *_etk_drag_widget = NULL;

static Ecore_Event_Handler *_etk_drag_mouse_move_handler;
static Ecore_Event_Handler *_etk_drag_mouse_up_handler;

static void _etk_drag_constructor(Etk_Drag *drag);
static int  _etk_drag_mouse_up_cb(void *data, int type, void *event);
static int  _etk_drag_mouse_move_cb(void *data, int type, void *event);

/**
 * @brief Gets the type of an Etk_Drag
 * @return Returns the type on an Etk_Drag
 */
Etk_Type *etk_drag_type_get()
{
   static Etk_Type *drag_type = NULL;
   
   if(!drag_type)
   {
      drag_type = etk_type_new("Etk_Drag", ETK_WINDOW_TYPE, sizeof(Etk_Drag), ETK_CONSTRUCTOR(_etk_drag_constructor), NULL);      
   }
   
   return drag_type;
}

/**
 * @brief Create a new drag widget
 * @return Returns the new drag widget
 */
Etk_Widget *etk_drag_new()
{
   return etk_widget_new(ETK_DRAG_TYPE, NULL);   
}

void etk_drag_types_set(Etk_Drag *drag, const char **types, unsigned int num_types)
{
   int i;
   
   drag->types = malloc(num_types * sizeof(char *));
   
   for (i = 0; i < num_types; i++)
     drag->types[i] = strdup(types[i]);
   
   drag->num_types = num_types;   
}

void etk_drag_data_set(Etk_Drag *drag, void *data, int size)
{
   drag->data = data;
   drag->data_size = size;   
}

void etk_drag_begin(Etk_Drag *drag)
{
   _etk_drag_widget = drag;
   
   etk_widget_show_all(ETK_WIDGET(drag));   
   ecore_evas_ignore_events_set((ETK_WINDOW(drag))->ecore_evas, 1);
   ecore_x_dnd_types_set((ETK_WINDOW(drag))->x_window, drag->types, drag->num_types);
   ecore_x_dnd_begin((ETK_WINDOW(drag))->x_window, drag->data, drag->data_size);
   _etk_drag_mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, _etk_drag_mouse_move_cb, drag);
   _etk_drag_mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, _etk_drag_mouse_up_cb, drag);   
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_drag_constructor(Etk_Drag *drag)
{
   if (!drag)
     return;
   
   etk_window_decorated_set(ETK_WINDOW(drag), ETK_FALSE);
   etk_window_shaped_set(ETK_WINDOW(drag), ETK_TRUE);
   etk_window_skip_pager_hint_set(ETK_WINDOW(drag), ETK_TRUE);
   etk_window_skip_taskbar_hint_set(ETK_WINDOW(drag), ETK_TRUE);
   ecore_x_dnd_aware_set((ETK_WINDOW(drag))->x_window, 1);   
}

static int _etk_drag_mouse_up_cb(void *data, int type, void *event)
{
   Etk_Drag *drag;
   
   drag = data;
   etk_widget_hide_all(ETK_WIDGET(drag));
   ecore_event_handler_del(_etk_drag_mouse_move_handler);
   ecore_event_handler_del(_etk_drag_mouse_up_handler);
   ecore_x_dnd_drop();   
   etk_widget_drag_end(ETK_WIDGET(drag));   
   return 1;
}

static int _etk_drag_mouse_move_cb(void *data, int type, void *event)
{
   Ecore_X_Event_Mouse_Move *ev;
   Etk_Drag *drag;
   
   drag = data;
   ev = event;
   
   etk_window_move(ETK_WINDOW(drag), ev->root.x + 2, ev->root.y + 2);
   return 1;
}

/** @} */
