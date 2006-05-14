/** @file etk_statusbar.c */
#include "etk_statusbar.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_toplevel_widget.h"
#include "etk_window.h"

/**
 * @addtogroup Etk_Statusbar
 * @{
 */

typedef struct _Etk_Statusbar_Msg
{
   char *msg;
   int context_id;
   int message_id;
} Etk_Statusbar_Msg;

enum _Etk_Statusbar_Signal_Id
{
   ETK_STATUSBAR_TEXT_POPPED_SIGNAL,
   ETK_STATUSBAR_TEXT_PUSHED_SIGNAL,
   ETK_STATUSBAR_NUM_SIGNALS
};

enum _Etk_Statusbar_Property_Id
{
   ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY
};

static void _etk_statusbar_constructor(Etk_Statusbar *statusbar);
static void _etk_statusbar_destructor(Etk_Statusbar *statusbar);
static void _etk_statusbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_statusbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_statusbar_realize_cb(Etk_Object *object, void *data);
static void _etk_statusbar_resize_grip_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_statusbar_mouse_move_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_statusbar_update(Etk_Statusbar *statusbar);

static Etk_Signal *_etk_statusbar_signals[ETK_STATUSBAR_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Statusbar
 * @return Returns the type on an Etk_Statusbar
 */
Etk_Type *etk_statusbar_type_get()
{
   static Etk_Type *statusbar_type = NULL;

   if (!statusbar_type)
   {
      statusbar_type = etk_type_new("Etk_Statusbar", ETK_WIDGET_TYPE, sizeof(Etk_Statusbar), ETK_CONSTRUCTOR(_etk_statusbar_constructor), ETK_DESTRUCTOR(_etk_statusbar_destructor));

      _etk_statusbar_signals[ETK_STATUSBAR_TEXT_POPPED_SIGNAL] = etk_signal_new("text_popped", statusbar_type, -1, etk_marshaller_VOID__INT_POINTER, NULL, NULL);
      _etk_statusbar_signals[ETK_STATUSBAR_TEXT_PUSHED_SIGNAL] = etk_signal_new("text_pushed", statusbar_type, -1, etk_marshaller_VOID__INT_POINTER, NULL, NULL);
      
      etk_type_property_add(statusbar_type, "has_resize_grip", ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      
      statusbar_type->property_set = _etk_statusbar_property_set;
      statusbar_type->property_get = _etk_statusbar_property_get;
   }

   return statusbar_type;
}

/**
 * @brief Creates a new status bar
 * @return Returns the new status bar widget
 */
Etk_Widget *etk_statusbar_new()
{
   return etk_widget_new(ETK_STATUSBAR_TYPE, "theme_group", "statusbar", NULL);
}

/**
 * @brief Gets a context id corresponding to a context description
 * @param statusbar a statusbar
 * @param context the description of the context
 * @return Returns the context id corresponding to the context description, or -1 on failure
 */
int etk_statusbar_context_id_get(Etk_Statusbar *statusbar, const char *context)
{
   char *key;
   int *context_id;
   
   if (!statusbar || !context)
      return -1;
   
   key = malloc(strlen("_Etk_Statusbar::") + strlen(context) + 1);
   sprintf(key, "_Etk_Statusbar::%s", context);
   
   if (!(context_id = etk_object_data_get(ETK_OBJECT(statusbar), key)))
   {
      context_id = malloc(sizeof(int));
      *context_id = statusbar->next_context_id++;
      etk_object_data_set_full(ETK_OBJECT(statusbar), key, context_id, free);
   }
   
   free(key);
   return *context_id;
}

/**
 * @brief Pushs on the statusbar message stack a new message
 * @param statusbar a statusbar
 * @param message the message to push
 * @param context_id the context id to associate to the message. You can generate a context id with @a etk_statusbar_context_id_get()
 * @return Returns the message id of the pushed message, or -1 on failure
 */
int etk_statusbar_push(Etk_Statusbar *statusbar, const char *message, int context_id)
{
   Etk_Statusbar_Msg *new_msg;
   
   if (!statusbar || !message)
      return -1;
   
   new_msg = malloc(sizeof(Etk_Statusbar_Msg));
   new_msg->msg = strdup(message);
   new_msg->context_id = context_id;
   new_msg->message_id = statusbar->next_message_id++;
   
   statusbar->msg_stack = evas_list_prepend(statusbar->msg_stack, new_msg);
   _etk_statusbar_update(statusbar);
   
   etk_signal_emit(_etk_statusbar_signals[ETK_STATUSBAR_TEXT_PUSHED_SIGNAL], ETK_OBJECT(statusbar), NULL, new_msg->context_id, new_msg->msg);
   
   return new_msg->message_id;
}

/**
 * @brief Pops from the statusbar the first message whose context id matchs
 * @param statusbar a statusbar
 * @param context id the context id of the message to pop, specified when you've pushed the message with @a etk_statusbar_push()
 */
void etk_statusbar_pop(Etk_Statusbar *statusbar, int context_id)
{
   Evas_List *l;
   Etk_Statusbar_Msg *m;
   
   if (!statusbar)
      return;
   
   for (l = statusbar->msg_stack; l; l = l->next)
   {
      m = l->data;
      if (m->context_id == context_id)
      {
         free(m->msg);
         free(m);
         statusbar->msg_stack = evas_list_remove_list(statusbar->msg_stack, l);
         _etk_statusbar_update(statusbar);
         break;
      }
   }
   
   m = statusbar->msg_stack ? statusbar->msg_stack->data : NULL;
   etk_signal_emit(_etk_statusbar_signals[ETK_STATUSBAR_TEXT_POPPED_SIGNAL], ETK_OBJECT(statusbar), NULL, m ? m->context_id : 0, m);
}

/**
 * @brief Removes the message corresponding to the message id from the statusbar
 * @param statusbar a statusbar
 * @param message_id the message id of the message, returned when you've pushed the message with @a etk_statusbar_push()
 */
void etk_statusbar_remove(Etk_Statusbar *statusbar, int message_id)
{
   Evas_List *l;
   Etk_Statusbar_Msg *m;
   
   if (!statusbar)
      return;
   
   for (l = statusbar->msg_stack; l; l = l->next)
   {
      m = l->data;
      if (m->message_id == message_id)
      {
         free(m->msg);
         free(m);
         statusbar->msg_stack = evas_list_remove_list(statusbar->msg_stack, l);
         _etk_statusbar_update(statusbar);
         break;
      }
   }
}

/**
 * @brief Sets whether the status bar has a resize grip
 * @param statusbar a status bar
 * @param has_resize_grip @a has_resize_grip == ETK_TRUE if the statusbar should have a resize grip
 */
void etk_statusbar_has_resize_grip_set(Etk_Statusbar *statusbar, Etk_Bool has_resize_grip)
{
   if (!statusbar || statusbar->has_resize_grip == has_resize_grip)
      return;
   
   statusbar->has_resize_grip = has_resize_grip;
   etk_widget_theme_object_signal_emit(ETK_WIDGET(statusbar), statusbar->has_resize_grip ? "show_resize_grip" : "hide_resize_grip");
   etk_object_notify(ETK_OBJECT(statusbar), "has_resize_grip");
}

/**
 * @brief Gets whether the status bar has a resize grip
 * @param statusbar a status bar
 * @return Returns ETK_TRUE if the statusbar has a resize grip
 */
Etk_Bool etk_statusbar_has_resize_grip_get(Etk_Statusbar *statusbar)
{
   if (!statusbar)
      return ETK_FALSE;
   return statusbar->has_resize_grip;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_statusbar_constructor(Etk_Statusbar *statusbar)
{
   if (!statusbar)
      return;

   statusbar->has_resize_grip = ETK_TRUE;
   statusbar->msg_stack = NULL;
   statusbar->next_message_id = 0;
   statusbar->next_context_id = 0;
   
   etk_signal_connect("realize", ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_realize_cb), NULL);
}

/* Destroys the status bar */
static void _etk_statusbar_destructor(Etk_Statusbar *statusbar)
{
   Etk_Statusbar_Msg *msg;
   
   if (!statusbar)
      return;
   
   while (statusbar->msg_stack)
   {
      msg = statusbar->msg_stack->data;
      free(msg->msg);
      free(msg);
      statusbar->msg_stack = evas_list_remove_list(statusbar->msg_stack, statusbar->msg_stack);
   }
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_statusbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Statusbar *statusbar;

   if (!(statusbar = ETK_STATUSBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY:
         etk_statusbar_has_resize_grip_set(statusbar, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_statusbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Statusbar *statusbar;

   if (!(statusbar = ETK_STATUSBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY:
         etk_property_value_bool_set(value, statusbar->has_resize_grip);
         break;
      default:
         break;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the status bar is realized */
static void _etk_statusbar_realize_cb(Etk_Object *object, void *data)
{
   etk_widget_theme_object_signal_emit(ETK_WIDGET(object), ETK_STATUSBAR(object)->has_resize_grip ? "show_resize_grip" : "hide_resize_grip");
   _etk_statusbar_update(ETK_STATUSBAR(object));
   
   if (ETK_WIDGET(object)->theme_object)
      edje_object_signal_callback_add(ETK_WIDGET(object)->theme_object, "*", "resize_grip", _etk_statusbar_resize_grip_cb, object);
}

/* Called when an event occurs on the resize grip of the status bar */
static void _etk_statusbar_resize_grip_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Statusbar *statusbar;
   
   if (!(statusbar = ETK_STATUSBAR(data)))
      return;
   
   if (strcmp(emission, "mouse,in") == 0)
      etk_toplevel_widget_pointer_push(etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar)), ETK_POINTER_RESIZE_BR);
   else if (strcmp(emission, "mouse,out") == 0)
      etk_toplevel_widget_pointer_pop(etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar)), ETK_POINTER_RESIZE_BR);
   else if (strcmp(emission, "mouse,down,1") == 0)
   {
      Etk_Toplevel_Widget *window;
      
      if (!(window = etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar))) || !ETK_IS_WINDOW(window))
         return;
      
      etk_window_geometry_get(ETK_WINDOW(window), NULL, NULL, &statusbar->new_window_width, &statusbar->new_window_height);
      etk_signal_connect("mouse_move", ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_mouse_move_cb), NULL);
   }
   else if (strcmp(emission, "mouse,up,1") == 0)
      etk_signal_disconnect("mouse_move", ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_mouse_move_cb));
}

/* Called when mouse presses the resize grip and when the mouse is moved */
static void _etk_statusbar_mouse_move_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Statusbar *statusbar;
   Etk_Toplevel_Widget *window;
   Etk_Event_Mouse_Move *event = event_info;
   
   if (!(statusbar = ETK_STATUSBAR(object)) || !(window = etk_widget_toplevel_parent_get(ETK_WIDGET(statusbar))) || !ETK_IS_WINDOW(window))
      return;
   
   statusbar->new_window_width += event->cur.widget.x - event->prev.widget.x;
   statusbar->new_window_height += event->cur.widget.y - event->prev.widget.y;
   etk_window_resize(ETK_WINDOW(window), statusbar->new_window_width, statusbar->new_window_height);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Updates the messages displayed in the statusbar */
static void _etk_statusbar_update(Etk_Statusbar *statusbar)
{
   Etk_Statusbar_Msg *msg;
   
   if (!statusbar)
      return;
   
   if (statusbar->msg_stack && (msg = statusbar->msg_stack->data) && msg->msg)
      etk_widget_theme_object_part_text_set(ETK_WIDGET(statusbar), "message", msg->msg);
   else
      etk_widget_theme_object_part_text_set(ETK_WIDGET(statusbar), "message", "");
}

/** @} */
