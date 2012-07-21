
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "Exquisite.h"

static void _exquisite_cb_exit_done(void *data, Evas_Object *obj,
    const char *emission, const char *source);
static void _exquisite_object_del_cb(void *data, Evas *e, Evas_Object *obj,
    void *event_info);

typedef struct _Exquisite_Text_Line Exquisite_Text_Line;
struct _Exquisite_Text_Line {
  const char* message;
  const char* status_text;
  Exquisite_Status_Type status;
};

static void
_exquisite_text_update(Evas_Object *obj, Eina_List *messages, int signal)
{
   char buf[8192];
   char buf2[8192];
   char *p = buf, *s = buf2;
   Eina_List *l = NULL;
   int i;
   const char *msg, *status;
   Exquisite_Text_Line *t;

   edje_object_signal_emit(obj, "exquisite", "text_enable");

   if(!messages)
     {
        buf[0] = 0;
        buf2[0] = 0;
     }

   for(l = messages, i = 0; l && i < 8192; l = l->next)
     {
       t = (Exquisite_Text_Line*)l->data;
       snprintf(p, sizeof(buf)-strlen(buf), "<p>%s</p>", t->message);
       p = buf+strlen(buf);

       if(!t->status_text)
         snprintf(s, sizeof(buf2)-strlen(buf2), "<br>");
       else
         {
           switch(t->status) {
             case EXQUISITE_STATUS_TYPE_SUCCESS:
               snprintf(s, sizeof(buf2)-strlen(buf2), "<success>%s</success>", t->status_text);
               break;
             case EXQUISITE_STATUS_TYPE_FAILURE:
               snprintf(s, sizeof(buf2)-strlen(buf2), "<failure>%s</failure>", t->status_text);
               break;
             case EXQUISITE_STATUS_TYPE_NORMAL:
             default:
               snprintf(s, sizeof(buf2)-strlen(buf2), "%s<br/>", t->status_text);
               break;
           }
         }

       s = buf2+strlen(buf2);
     }

   if (edje_object_part_exists(obj, "textarea") &&
       edje_object_part_exists(obj, "statusarea"))
     {
        edje_object_part_text_set(obj, "textarea", buf);
        edje_object_part_text_set(obj, "statusarea", buf2);
        if(signal == 1)
          edje_object_signal_emit(obj, "exquisite", "text_set");
        else if(signal == 2)
          edje_object_signal_emit(obj, "exquisite", "status_set");

     }
}

EAPI Evas_Object *
exquisite_object_add(Evas *evas, const char *theme)
{
   Evas_Object *o;

   o = edje_object_add(evas);
   if (o)
     {
       if (!edje_object_file_set(o, theme, "exquisite/theme"))
         {
           evas_object_del (o);
           return NULL;
         }
       edje_object_signal_callback_add(o, "exquisite", "done",
           _exquisite_cb_exit_done, NULL);
       evas_object_event_callback_add(o, EVAS_CALLBACK_DEL,
           _exquisite_object_del_cb, NULL);
     }

   return o;
}


EAPI void
exquisite_object_exit_callback_set(Evas_Object *obj,
    Exquisite_Exit_Callback func, void *data)
{
   evas_object_data_set (obj, "exquisite-exit-callback", func);
   evas_object_data_set (obj, "exquisite-exit-data", data);
}

EAPI void
exquisite_object_exit(Evas_Object *obj)
{
   edje_object_signal_emit(obj, "exquisite", "exit");
}

/* set theme title text */
EAPI void
exquisite_object_title_set(Evas_Object *obj, const char *txt)
{
   if (edje_object_part_exists(obj, "exquisite.title"))
     {
	edje_object_part_text_set(obj, "exquisite.title", txt);
	edje_object_signal_emit(obj, "exquisite", "title");
     }
   else
     {
	Edje_Message_String m;
	m.str = (char *)txt;
	edje_object_message_send(obj, EDJE_MESSAGE_STRING, 0, &m);
     }
}

/* set theme title text */
EAPI void
exquisite_object_message_set(Evas_Object *obj, const char *txt)
{
   if (edje_object_part_exists(obj, "exquisite.message"))
     {
	edje_object_part_text_set(obj, "exquisite.message", txt);
	edje_object_signal_emit(obj, "exquisite", "message");
     }
   else
     {
	Edje_Message_String m;
	m.str = (char *)txt;
	edje_object_message_send(obj, EDJE_MESSAGE_STRING, 1, &m);
     }
}

/* set progress position 0.0 -> 1.0 */
EAPI void
exquisite_object_progress_set(Evas_Object *obj, double val)
{
   if (edje_object_part_exists(obj, "exquisite.progress"))
     {
	edje_object_part_drag_value_set(obj, "exquisite.progress", val, val);
	edje_object_signal_emit(obj, "exquisite", "progress");
     }
   else
     {
	Edje_Message_Float m;
	m.val = val;
	edje_object_message_send(obj, EDJE_MESSAGE_FLOAT, 2, &m);
     }
}

/* input a tick into the theme (like a heartbeat - are things alive */
EAPI void
exquisite_object_tick(Evas_Object *obj)
{
   edje_object_signal_emit(obj, "exquisite", "tick");
}

/* Put into pulsate mode, Usplash command */
EAPI void
exquisite_object_pulsate(Evas_Object *obj)
{
   edje_object_signal_emit(obj, "exquisite", "pulsate");
}

EAPI int
exquisite_object_text_add(Evas_Object *obj, const char *txt)
{
   Exquisite_Text_Line *t = NULL;
   Eina_List *messages = NULL;

   if(!txt || (txt[0] == 0)) return -1;

   messages = evas_object_data_get (obj, "exquisite-messages");

   t = malloc(sizeof(Exquisite_Text_Line));
   t->message = eina_stringshare_add(txt);
   t->status_text = NULL;
   t->status = EXQUISITE_STATUS_TYPE_NORMAL;

   messages = eina_list_append(messages, t);
   evas_object_data_set (obj, "exquisite-messages", messages);

   /*A 1 means that a text update signal will be sent*/
   _exquisite_text_update(obj, messages, 1);

   return eina_list_count (messages) - 1;
}

EAPI void
exquisite_object_status_set(Evas_Object *obj, int text,
    const char *status, Exquisite_Status_Type type)
{
   Exquisite_Text_Line *t = NULL;
   Eina_List *messages = NULL;

   messages = evas_object_data_get (obj, "exquisite-messages");

   if(text < 0 || !status || (status[0] == 0) || !messages) return;

   t = (Exquisite_Text_Line *)eina_list_nth(messages, text);

   if (t == NULL) return;

   t->status = type;
   if(t->status_text) eina_stringshare_del(t->status_text);
   t->status_text = eina_stringshare_add(status);
   evas_object_data_set (obj, "exquisite-messages", messages);

   /*A 2 means that a status update signal will be sent*/
   _exquisite_text_update(obj, messages, 2);
}

EAPI void
exquisite_object_text_clear(Evas_Object *obj)
{
   Eina_List *messages = NULL;

   messages = evas_object_data_get (obj, "exquisite-messages");
   if (messages)
    {
      Eina_List *l = NULL;
      Exquisite_Text_Line *t = NULL;

      for(l = messages; l; l = l->next)
        {
          t = l->data;
          eina_stringshare_del(t->message);
          if(t->status_text) eina_stringshare_del(t->status_text);
          free(l->data);
        }

      eina_list_free(messages);
      evas_object_data_del (obj, "exquisite-messages");
    }

   if (edje_object_part_exists(obj, "textarea") &&
       edje_object_part_exists(obj, "statusarea"))
     {
	edje_object_signal_emit(obj, "exquisite", "text_clear");
	edje_object_part_text_set(obj, "textarea", "");
	edje_object_part_text_set(obj, "statusarea", "");
     }
}

static void
_exquisite_cb_exit_done(void *data, Evas_Object *obj,
    const char *emission, const char *source)
{
   Exquisite_Exit_Callback exit_cb = NULL;
   void *exit_data = NULL;

   exit_cb = evas_object_data_get (obj, "exquisite-exit-callback");
   exit_data = evas_object_data_get (obj, "exquisite-exit-data");

   if (exit_cb)
	exit_cb(exit_data);
}

static void
_exquisite_object_del_cb(void *data, Evas *e, Evas_Object *obj,
    void *event_info)
{
   Eina_List *messages = NULL;

   messages = evas_object_data_get (obj, "exquisite-messages");
   if (messages)
    {
      Eina_List *l = NULL;
      Exquisite_Text_Line *t = NULL;

      EINA_LIST_FREE (messages, t)
        {
          eina_stringshare_del(t->message);
          if(t->status_text)
            eina_stringshare_del(t->status_text);
        }
    }

   evas_object_data_del (obj, "exquisite-messages");
   evas_object_data_del (obj, "exquisite-exit-callback");
   evas_object_data_del (obj, "exquisite-exit-data");
}
