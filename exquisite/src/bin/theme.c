#include "main.h"

static void _theme_cb_exit_done(void *data, Evas_Object *obj, const char *emission, const char *source);

typedef struct _Exquisite_Text_Line Exquisite_Text_Line;
struct _Exquisite_Text_Line {
  const char* message;
  const char* status_text;
  int status;
};

static Evas_Object *o_bg = NULL;
static Eina_List *messages = NULL;
static char *txt = NULL;

static void
theme_update_text(int signal)
{
   char buf[8192];
   char buf2[8192];
   char *p = buf, *s = buf2;
   Eina_List *l = NULL;
   int i;
   const char *msg, *status;
   Exquisite_Text_Line *t;

   edje_object_signal_emit(o_bg, "exquisite", "text_enable");

   if(!messages)
     {
        buf[0] = 0;
        buf2[0] = 0;
     }

   for(l = messages, i = 0; l != NULL && i < 8192; l = l->next)
     {
       t = (Exquisite_Text_Line*)l->data;
       snprintf(p, strlen(t->message)+8, "<p>%s</p>", t->message);
       p = buf+strlen(buf);
       
       if(!t->status_text)
         snprintf(s, 5, "<br>");
       else
         {
           if(t->status != -1)
             snprintf(s, strlen(t->status_text)+8, "<%i>%s</%i>", t->status, t->status_text, t->status);
           else
             snprintf(s, strlen(t->status_text)+5, "%s<br>", t->status_text);
         }
       
       s = buf2+strlen(buf2);
     }

   if (edje_object_part_exists(o_bg, "textarea") &&
       edje_object_part_exists(o_bg, "statusarea"))
     {          
        if(signal == 1)
          edje_object_signal_emit(o_bg, "exquisite", "text_set");  
        else if(signal == 2)
          edje_object_signal_emit(o_bg, "exquisite", "status_set");
        
        edje_object_part_text_set(o_bg, "textarea", buf);
        edje_object_part_text_set(o_bg, "statusarea", buf2);
     }
}

void
theme_init(const char *theme)
{
   Evas_Object *o;
   char buf[4096];
   
   if (!theme) theme = "default";
   o = edje_object_add(evas);
   if ((theme) && 
       (theme[0] != '/') && 
       (!!strncmp(theme, "./", 2)) &&
	(!!strncmp(theme, "../", 3)))
     snprintf(buf, sizeof(buf), THEMES"/%s.edj", theme);
   else
     snprintf(buf, sizeof(buf), "%s", theme);
   edje_object_file_set(o, buf, "exquisite/theme");
   evas_object_move(o, 0, 0);
   evas_object_resize(o, scr_w, scr_h);
   edje_object_signal_callback_add(o, "exquisite", "done", _theme_cb_exit_done, NULL);
   evas_object_show(o);
   o_bg = o;
}

void
theme_shutdown(void)
{
   evas_object_del(o_bg);
   o_bg = NULL;
}

void
theme_resize(void)
{
   evas_object_resize(o_bg, scr_w, scr_h);
}

/* emit to theme to shut down - when done call callback func */
static void (*_theme_exit_func) (void *data) = NULL;
static void *theme_exit_data = NULL;
void
theme_exit(void (*func) (void *data), void *data)
{
   quitting = 1;
   edje_object_signal_emit(o_bg, "exquisite", "exit");
   _theme_exit_func = func;
   theme_exit_data = data;
}

/* if theme_exit() was called - this aborts calling the callback at the end */
void
theme_exit_abort(void)
{
   _theme_exit_func = NULL;
   theme_exit_data = NULL;
}

/* set theme title text */
void
theme_title_set(const char *txt)
{
   if (edje_object_part_exists(o_bg, "exquisite.title"))
     {
	edje_object_part_text_set(o_bg, "exquisite.title", txt);
	edje_object_signal_emit(o_bg, "exquisite", "title");
     }
   else
     {
	Edje_Message_String m;
	m.str = (char *)txt;
	edje_object_message_send(o_bg, EDJE_MESSAGE_STRING, 0, &m);
     }
}

/* set theme title text */
void
theme_message_set(const char *txt)
{
   if (edje_object_part_exists(o_bg, "exquisite.message"))
     {
	edje_object_part_text_set(o_bg, "exquisite.message", txt);
	edje_object_signal_emit(o_bg, "exquisite", "message");
     }
   else
     {
	Edje_Message_String m;
	m.str = (char *)txt;
	edje_object_message_send(o_bg, EDJE_MESSAGE_STRING, 1, &m);
     }
}

/* set progress position 0.0 -> 1.0 */
void
theme_progress_set(double val)
{	
   if (edje_object_part_exists(o_bg, "exquisite.progress"))
     {
	edje_object_part_drag_value_set(o_bg, "exquisite.progress", val, val);
	edje_object_signal_emit(o_bg, "exquisite", "progress");
     }
   else
     {
	Edje_Message_Float m;
	m.val = val;
	edje_object_message_send(o_bg, EDJE_MESSAGE_FLOAT, 2, &m);
     }
}

/* input a tick into the theme (like a heartbeat - are things alive */
void
theme_tick(void)
{
   edje_object_signal_emit(o_bg, "exquisite", "tick");
}

/* Put into pulsate mode, Usplash command */
void
theme_pulsate(void)
{
   edje_object_signal_emit(o_bg, "exquisite", "pulsate");
}

void 
theme_text_add(const char *txt)
{
   Exquisite_Text_Line *t = NULL;

   t = malloc(sizeof(Exquisite_Text_Line));

   if(!txt || (txt[0] == 0)) return;

   t->message = evas_stringshare_add(txt);
   t->status_text = NULL;
   t->status = -1;

   messages = eina_list_append(messages, t);
   
   /*A 1 means that a text update signal will be sent*/
   theme_update_text(1);
}

void 
theme_status_set(const char *txt, int type)
{
   Exquisite_Text_Line *t = NULL;

   if(!txt || (txt[0] == 0) || !messages) return;

   t = (Exquisite_Text_Line *)(eina_list_last(messages)->data);
   
   t->status = type;
   if(t->status_text) evas_stringshare_del(t->status_text);
   t->status_text = evas_stringshare_add(txt);
   
   /*A 2 means that a status update signal will be sent*/
   theme_update_text(2);
}

void
theme_text_clear()
{
  if (messages)
    {
      Eina_List *l = NULL;
      Exquisite_Text_Line *t = NULL;
   
      for(l = messages; l != NULL; l = l->next)
        {
          t = l->data;
          evas_stringshare_del(t->message);
          if(t->status_text) evas_stringshare_del(t->status_text);
          free(l->data);
        }
      
      eina_list_free(messages);
      messages = NULL;
    }  

   if (edje_object_part_exists(o_bg, "textarea") &&
       edje_object_part_exists(o_bg, "statusarea"))
     {
	edje_object_signal_emit(o_bg, "exquisite", "text_clear");
	edje_object_part_text_set(o_bg, "textarea", "");
	edje_object_part_text_set(o_bg, "statusarea", "");
     }
}

static void
_theme_cb_exit_done(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   if (_theme_exit_func)
     {
	_theme_exit_func(theme_exit_data);
	_theme_exit_func = NULL;
	theme_exit_data = NULL;
     }
   
   theme_text_clear();
}
