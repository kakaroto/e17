/** @file etk_statusbar.c */
#include "etk_statusbar.h"
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

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
      
      etk_type_property_add(statusbar_type, "has_resize_grip", ETK_STATUSBAR_HAS_RESIZE_GRIP_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(TRUE));
      
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

/* TODO doc */
int etk_statusbar_context_id_get(Etk_Statusbar *statusbar, const char *context)
{
   char *key;
   int *context_id;
   
   if (!statusbar || !context)
      return -1;
   
   key = malloc(strlen("_Etk_Status_Bar:") + strlen(context) + 1);
   sprintf(key, "_Etk_Status_Bar:%s", context);
   
   if (!(context_id = etk_object_data_get(ETK_OBJECT(statusbar), key)))
   {
      context_id = malloc(sizeof(int));
      *context_id = statusbar->next_context_id++;
      etk_object_data_set_full(ETK_OBJECT(statusbar), key, context_id, free);
   }
   
   free(key);
   return *context_id;
}

/* TODO doc */
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

/* TODO doc */
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

/* TODO doc */
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

   statusbar->has_resize_grip = TRUE;
   statusbar->msg_stack = NULL;
   statusbar->next_message_id = 0;
   statusbar->next_context_id = 0;
   
   etk_signal_connect_after("realize", ETK_OBJECT(statusbar), ETK_CALLBACK(_etk_statusbar_realize_cb), NULL);
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
   _etk_statusbar_update(ETK_STATUSBAR(object));
}

/**************************
 *
 * Private functions
 *
 **************************/

/* TODO: doc */
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
