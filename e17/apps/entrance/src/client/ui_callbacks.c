#include "ui_callbacks.h"
#include "sys_callbacks.h"
#include "entrance_fx.h"

void
hide_session_list(Entrance_Session e)
{
   Evas_List *l;
   Evas_Object *li;

   if (!e || !(e->listbox) || !(e->listitems))
      return;

   for (l = e->listitems; l; l = l->next)
   {
      li = (Evas_Object *) evas_list_data(l);
      fx_fade_out(li, 0.1);
   }

//    evas_object_hide(e->listbox);
   fx_fade_out(e->listbox, 0.2);
}

void
show_session_list(Entrance_Session e)
{
   Evas_List *l;
   Evas_Object *li;

   if (evas_object_visible_get(e->listbox))
      return;

   if (!e || !(e->listbox) || !(e->listitems))
      return;

   fx_fade_in(e->listbox, 0.1);

   for (l = e->listitems; l; l = l->next)
   {
      li = (Evas_Object *) evas_list_data(l);
      fx_fade_in(li, 0.24);
   }

}

void
update_login_face(Entrance_Session e, char *name)
{
   char buf[PATH_MAX];

   snprintf(buf, PATH_MAX, "%s/%s.png", PACKAGE_DATA_DIR "/data/images/users",
            name);
   evas_object_image_file_set(e->face, buf, NULL);
   evas_object_image_reload(e->face);
   if (evas_object_image_load_error_get(e->face))
   {
      printf("Could not load %s\n", buf);
      evas_object_hide(e->face_shadow);
      return;
   }
   THEME_IMAGE_RESIZE(e->face, e->theme->face);
   fx_fade_in(e->face_shadow, 0.15);
   fx_fade_in(e->face, 0.15);
}

void
session_selector_clicked_cb(void *_data, Evas * e, Evas_Object * _o,
                            void *event_info)
{
   Entrance_Session es;
   Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *) event_info;

   if (_data && ev->button == 1)
   {
      es = (Entrance_Session) _data;
      show_session_list(es);
   }
}

void
session_selector_mousein_cb(void *_data, Evas * e, Evas_Object * _o,
                            void *event_info)
{
   Entrance_Session es;

   if (_data)
   {
      es = (Entrance_Session) _data;
      THEME_SETCOLOR(_o, es->theme->selected_session.text.hicolor);
   }
}

void
session_selector_mouseout_cb(void *_data, Evas * e, Evas_Object * _o,
                             void *event_info)
{
   Entrance_Session es;

   if (_data)
   {
      es = (Entrance_Session) _data;
      THEME_SETCOLOR(_o, es->theme->selected_session.text.color);
   }
}

void
session_list_clicked_cb(void *_data, Evas * e, Evas_Object * _o,
                        void *event_info)
{
   Entrance_Session es;
   Entrance_Session_Type *session = NULL;
   Evas_List *l;
   Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *) event_info;
   const char *object_text;

   if (_data && ev->button == 1)
   {
      es = (Entrance_Session) _data;
      for (l = es->config->sessions; l; l = l->next)
      {
         session = (Entrance_Session_Type *) evas_list_data(l);
         object_text = evas_object_text_text_get(_o);
         if (!strcmp(session->name, object_text))
         {
            entrance_select_named_session(es, session->name);

            /* Update selected session on display */
            evas_object_text_text_set(es->sessiontext, session->name);
            evas_object_image_file_set(es->sessionicon, session->icon, NULL);
            evas_object_image_reload(es->sessionicon);
            THEME_IMAGE_RESIZE(es->sessionicon,
                               es->theme->selected_session.icon);
            evas_object_show(es->sessionicon);
            hide_session_list(es);
            return;
         }
      }
      fprintf(stderr, "Unexpected error: Could not match selected session!");
   }
}

void
session_list_mousein_cb(void *_data, Evas * e, Evas_Object * _o,
                        void *event_info)
{
   Entrance_Session es;

   if (_data)
   {
      es = (Entrance_Session) _data;
      THEME_SETCOLOR(_o, es->theme->session_list.sel_color);
   }
}

void
session_list_mouseout_cb(void *_data, Evas * e, Evas_Object * _o,
                         void *event_info)
{
   Entrance_Session es;

   if (_data)
   {
      es = (Entrance_Session) _data;
      THEME_SETCOLOR(_o, es->theme->session_list.color);
   }
}

void
entrance_update_time_cb(int val, void *data)
{
   Evas_Object *o = (Evas_Object *) data;
   char buf[PATH_MAX];
   char *str;
   struct tm *current;
   time_t _t = time(NULL);

   if (!data)
      return;

   /* Fetch and format time/date */
   current = localtime(&_t);
   if (val)
      strftime(buf, PATH_MAX, "%l:%M:%S %p", current);
   else
      strftime(buf, PATH_MAX, "%A %B %e, %Y", current);

   /* Strip off leading space if necessary (for time) */
   str = buf;
   if (buf[0] == ' ')
      str++;

   evas_object_text_text_set(o, str);
   if (val)
   {
      ecore_del_event_timer("time_update");
      ecore_add_event_timer("time_update", 0.11, entrance_update_time_cb, val,
                            data);
   }
   else
   {
      ecore_del_event_timer("date_update");
      ecore_add_event_timer("date_update", 0.5, entrance_update_time_cb, val,
                            data);
   }
//    snprintf(timer, PATH_MAX, "%s timer", str);
//    ecore_add_event_timer(timer, 0.1, entrance_update_time_cb, val, data);
}
