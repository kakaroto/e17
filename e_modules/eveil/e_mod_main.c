/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"
#include <config.h>

#define ALARM_ADD_FAIL { eveil_alarm_del(al); return NULL; }

/* module requirements */
EAPI E_Module_Api e_modapi = 
   {
      E_MODULE_API_VERSION,
      "Eveil"
   };

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, char *name, char *id, char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const E_Gadcon_Client_Class _gadcon_class =
   {
      GADCON_CLIENT_CLASS_VERSION,
      "eveil",
      {
         _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon
      }
   };

/* eveil module specifics */
typedef struct _Instance Instance;
struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *obj;
};

static int    _alarm_check_date(Alarm *al, int strict);
static double _epoch_find_date(char *date, int hour, int minute);
static double _epoch_find_next(int day_monday, int day_tuesday, int day_wenesday, int day_thursday, int day_friday, int day_saturday, int day_sunday, int hour, int minute);
static void   _button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void   _menu_cb_deactivate_post(void *data, E_Menu *m);
static void   _menu_cb_alarm_add(void *data, E_Menu *m, E_Menu_Item *mi);
static void   _menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void   _cb_edje_messages(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg);
static int    _cb_timer_etimer(void *data);
static int    _cb_timer_s_etimer(void *data);
static int    _cb_alarms_ring_etimer(void *data);

static E_Config_DD *_conf_edd = NULL;
static E_Config_DD *_alarms_edd = NULL;
Config *eveil_config = NULL;


/*
 * Gadcon functions
 */

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, char *name, char *id, char *style)
{
   char buf[4096];
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   
   inst = E_NEW(Instance, 1);
   
   o = edje_object_add(gc->evas);

   snprintf(buf, sizeof(buf), "%s/eveil.edj", e_module_dir_get(eveil_config->module));
   if (!e_theme_edje_object_set(o, "base/theme/modules/eveil", "modules/eveil/main"))
     edje_object_file_set(o, buf, "modules/eveil/main");

   edje_object_message_handler_set(o, _cb_edje_messages, inst);
   
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   
   inst->gcc = gcc;
   inst->obj = o;

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  _button_cb_mouse_down, inst);

   eveil_config->instances = evas_list_append(eveil_config->instances, inst);

   if (eveil_config->timer_icon_mode)
     eveil_edje_message_send(EDJE_MSG_SEND_TIMER_ICON_MODE,
                             eveil_config->timer_icon_mode);
   if (eveil_config->timer_detail_mode)
     eveil_edje_message_send(EDJE_MSG_SEND_TIMER_DETAIL_MODE,
                             eveil_config->timer_detail_mode);
   if (eveil_config->alarms_icon_mode)
     eveil_edje_message_send(EDJE_MSG_SEND_ALARM_ICON_MODE,
                             eveil_config->alarms_icon_mode);
   if (eveil_config->alarms_detail_mode)
     eveil_edje_message_send(EDJE_MSG_SEND_ALARM_DETAIL_MODE,
                             eveil_config->alarms_detail_mode);
   if (eveil_config->alarms_state)
     eveil_edje_message_send(EDJE_MSG_SEND_ALARM_STATE,
                             eveil_config->alarms_state);

   eveil_edje_refresh_alarm();

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   
   inst = gcc->data;
   evas_object_del(inst->obj);
   eveil_config->instances = evas_list_remove(eveil_config->instances, inst);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;
   
   inst = gcc->data;

   if (eveil_config->alarms_icon_mode)
     {
        e_gadcon_client_aspect_set(gcc, 24, 16);
        e_gadcon_client_min_size_set(gcc, 24, 16);
     }
   else
     {
        e_gadcon_client_aspect_set(gcc, 16, 16);
        e_gadcon_client_min_size_set(gcc, 16, 16);
     }
}
   
static char *
_gc_label(void)
{
   return _("Eveil");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/module.eap",
	    e_module_dir_get(eveil_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}


/*
 * Eveil functions
 */

Alarm *eveil_alarm_add(int state, char *name, int type, char *date, int day_monday, int day_tuesday, int day_wenesday, int day_thursday, int day_friday, int day_saturday, int day_sunday, int hour, int minute, int open_popup, int run_program, char *program)
{
   Alarm *al;

   if (!name)
     return NULL;
   if (!strlen(name))
     return NULL;

   al = E_NEW(Alarm, 1);

   al->state = state;
   al->name = evas_stringshare_add(name);
   al->sched.type = type;
   switch(type)
     {
     case ALARM_SCHED_TYPE_DAY:
        if ( !(al->sched.date_epoch = _epoch_find_date(date, hour, minute)) )
          ALARM_ADD_FAIL
             if (al->sched.date_epoch <= ecore_time_get())
               ALARM_ADD_FAIL
                  break;

     case ALARM_SCHED_TYPE_WEEK:
        al->sched.day_monday = day_monday;
        al->sched.day_tuesday = day_tuesday;
        al->sched.day_wenesday = day_wenesday;
        al->sched.day_thursday = day_thursday;
        al->sched.day_friday = day_friday;
        al->sched.day_saturday = day_saturday;
        al->sched.day_sunday = day_sunday;
        if ( !(al->sched.day_next_epoch = _epoch_find_next(day_monday,
                                                           day_tuesday,
                                                           day_wenesday,
                                                           day_thursday,
                                                           day_friday,
                                                           day_saturday,
                                                           day_sunday,
                                                           hour, minute)) )
          ALARM_ADD_FAIL
             }
   al->sched.hour = hour;
   al->sched.minute = minute;
   al->open_popup = open_popup;
   al->run_program = run_program;
   if (program)
     if (strlen(program))
       al->program = evas_stringshare_add(program);

   if (!_alarm_check_date(al, 1))
     ALARM_ADD_FAIL

        if (!eveil_config->alarms_ring_etimer)
          eveil_config->alarms_ring_etimer = ecore_timer_add(ALARMS_RING_TIME,
                                                             _cb_alarms_ring_etimer,
                                                             NULL);

   return al;
}

void eveil_alarm_del(Alarm *al)
{
   if (al->name)
     evas_stringshare_del(al->name);
   if (al->program)
     evas_stringshare_del(al->program);
   eveil_config->alarms = evas_list_remove(eveil_config->alarms, al);
   free(al);

   if (!evas_list_count(eveil_config->alarms))
     ecore_timer_del(eveil_config->alarms_ring_etimer);
}

int eveil_alarm_test(Alarm *al)
{
   char buf[4096];

   eveil_config->alarms_state = ALARMS_STATE_RINGING;
   eveil_edje_message_send(EDJE_MSG_SEND_ALARM_STATE,
                           ALARMS_STATE_RINGING);

   // TODO: real popups
   if ((al->open_popup == ALARM_OPEN_POPUP_YES) ||
       (al->open_popup == ALARM_OPEN_POPUP_PARENT &&
        eveil_config->alarms_open_popup_default))
     {
        char buf[4096];
        
        snprintf(buf, sizeof(buf),
                 "<hilight>(Test) %s !</hilight>"
                 "<br><br>",
                 al->name);
        e_module_dialog_show(_("Eveil Module Popup (Test)"), buf);
     }

   if ((al->run_program == ALARM_RUN_PROGRAM_OWN) ||
       (al->run_program == ALARM_RUN_PROGRAM_PARENT &&
        eveil_config->alarms_run_program_default))
     {
        Ecore_Exe *exe;
        
        if (al->run_program == ALARM_RUN_PROGRAM_PARENT)
          {
             exe = ecore_exe_pipe_run(eveil_config->alarms_program_default,
                                      ECORE_EXE_USE_SH, NULL);
          }
        else
          {
             exe = ecore_exe_pipe_run(al->program,
                                      ECORE_EXE_USE_SH, NULL);
          }
        if (exe > 0)
          {
             ecore_exe_free(exe);
             snprintf(buf, sizeof(buf),
                      "<hilight>Success !</hilight><br>"
                      "Youre program has been launch :)");
          }
        else
          {
             snprintf(buf, sizeof(buf),
                      "<hilight>Failed !</hilight><br>"
                      "Eveil couln't launch the program you specified");
          }
     }
   else
     {
        snprintf(buf, sizeof(buf),
                 "<hilight>Success !</hilight>");
     }
   
   e_module_dialog_show(_("Eveil Module Test Report"), buf);

   return 1;
}

void eveil_timer_start(void)
{
   if ((eveil_config->timer_state == TIMER_STATE_ON) ||
       (eveil_config->timer_state == TIMER_STATE_RINGING))
     eveil_timer_stop();

   eveil_config->timer_s_time = eveil_config->timer_time;

   eveil_config->timer_etimer =
      ecore_timer_add(eveil_config->timer_time, _cb_timer_etimer, NULL);
   eveil_config->timer_s_etimer = ecore_timer_add(1, _cb_timer_s_etimer, NULL);

   eveil_config->timer_state = TIMER_STATE_ON;
   eveil_edje_message_send(EDJE_MSG_SEND_TIMER_STATE,
                           eveil_config->timer_state);
}

void eveil_timer_stop(void)
{
   if (eveil_config->timer_state == TIMER_STATE_OFF)
     return;

   eveil_config->timer_state = TIMER_STATE_OFF;
   eveil_edje_message_send(EDJE_MSG_SEND_TIMER_STATE,
                           TIMER_STATE_OFF);
   ecore_timer_del(eveil_config->timer_etimer);
   ecore_timer_del(eveil_config->timer_s_etimer);

   eveil_config->timer_etimer = NULL;
}

void eveil_edje_message_send(int id, int message)
{
   Evas_List *l;

   for (l=eveil_config->instances; l; l=evas_list_next(l))
     {
        Instance *inst;

        inst = evas_list_data(l);
        edje_object_message_send(inst->obj, EDJE_MESSAGE_INT,
                                 id, &message);
     }
}

void eveil_edje_text_set(char *part, char *text)
{
   Evas_List *l;

   for (l=eveil_config->instances; l; l=evas_list_next(l))
     {
        Instance *inst;

        inst = evas_list_data(l);
        edje_object_part_text_set(inst->obj, part, text);
     }
}

void eveil_edje_refresh_alarm(void)
{
   Evas_List *l;
   Alarm *al;
   Alarm *first;
   double first_epoch;
   char buf[32];

   first = NULL;
   first_epoch = LONG_MAX;

   for (l=eveil_config->alarms; l; l=evas_list_next(l))
     {
        double epoch;

        al = evas_list_data(l);
        if (al->state == ALARM_STATE_OFF)
          continue;
        GET_NEXT_EPOCH(epoch, al);
        if (epoch < first_epoch)
          {
             first = al;
             first_epoch = epoch;
          }
     }

   if (!first)
     {
        eveil_edje_text_set("alarm_txt_line", "-- --:--");
        return;
     }
   else
     {
        struct tm *st;
        time_t t;
        char bufdate[32];
        
        t = first_epoch - (first->sched.hour*3600) - (first->sched.minute*60);
        st = localtime(&t);
        strftime(bufdate, sizeof(bufdate), "%d", st);
        snprintf(buf, sizeof(buf), "%.2s %.2d:%.2d",
                 bufdate, al->sched.hour, al->sched.minute);
        eveil_edje_text_set("alarm_txt_line", buf);
        printf("REFRESH !\n");
     }
}

void eveil_edje_refresh_timer(void)
{
   char buf[16];
   int h, m, s;
   
   h = (int)eveil_config->timer_s_time/3600;
   m = (int)(eveil_config->timer_s_time - h*3600)/60;
   s = (int)(eveil_config->timer_s_time - (h*3600 + m*60));

   snprintf(buf, sizeof(buf), "%.2d:%.2d:%.2d", h, m, s);
   eveil_edje_text_set("timer_txt_line", buf);   
}


static int
_alarm_check_date(Alarm *al, int strict)
{
   if (al->state == ALARM_STATE_OFF)
     return 1;

   switch(al->sched.type)
     {
     case ALARM_SCHED_TYPE_DAY:
        if (al->sched.date_epoch <= ecore_time_get())
          {
             if (strict)
               return 0;
             else
               {
                  if (eveil_config->alarms_date_autoremove)
                    eveil_alarm_del(al);
                  else
                    al->state = ALARM_STATE_OFF;
                  if (eveil_config->config_dialog)
                    eveil_config_refresh_alarms_ilist(eveil_config->config_dialog->cfdata);
               }
          }
        break;

     case ALARM_SCHED_TYPE_WEEK:
        if ((al->sched.day_monday || 
             al->sched.day_tuesday || 
             al->sched.day_wenesday || 
             al->sched.day_thursday || 
             al->sched.day_friday || 
             al->sched.day_saturday || 
             al->sched.day_sunday) == 0)
          return 0;

        if (al->sched.day_next_epoch <= ecore_time_get())
          {
             al->sched.day_next_epoch =
                _epoch_find_next(al->sched.day_monday,
                                 al->sched.day_tuesday,
                                 al->sched.day_wenesday,
                                 al->sched.day_thursday,
                                 al->sched.day_friday,
                                 al->sched.day_saturday,
                                 al->sched.day_sunday,
                                 al->sched.hour,
                                 al->sched.minute);
          }
        break;
     }

   return 1;
}

static double
_epoch_find_date(char *date, int hour, int minute)
{
   extern long timezone;
   extern int daylight;
   struct tm ts;
   time_t t;
   int i;

   if (!date)
     return 0;
   if (strlen(date) < 10)
     return 0;

   /* date format : "YYYY/MM/DD" */
   ts.tm_sec = 0;
   ts.tm_min = minute;
   ts.tm_hour = hour;
   sscanf(date+8, "%2d", &ts.tm_mday);
   sscanf(date+5, "%2d", &ts.tm_mon);
   ts.tm_mon--;   /* mktime works with 0-11 */
   sscanf(date, "%4d", &i);
   ts.tm_year = i - 1900;
   ts.tm_isdst = 1;

   t = mktime(&ts);

   printf("epoch find date : %f\n", (double)t);

   return (double)t;
}

static double
_epoch_find_next(int day_monday, int day_tuesday, int day_wenesday, int day_thursday, int day_friday, int day_saturday, int day_sunday, int hour, int minute)
{
   extern long timezone;
   extern int daylight;
   int day_week[7];
   struct tm ts;
   struct tm *ts_today;
   time_t t;
   int i, days_offset;
   double epoch;

   day_week[0] = day_sunday;
   day_week[1] = day_monday;
   day_week[2] = day_tuesday;
   day_week[3] = day_wenesday;
   day_week[4] = day_thursday;
   day_week[5] = day_friday;
   day_week[6] = day_saturday;
   t = time(NULL);
   ts_today = localtime(&t);

   ts.tm_sec = 0;
   ts.tm_min = minute;
   ts.tm_hour = hour;
   ts.tm_mday = ts_today->tm_mday;
   ts.tm_mon = ts_today->tm_mon;
   ts.tm_year = ts_today->tm_year;
   ts.tm_isdst = 1;
   epoch = (double)mktime(&ts);

   days_offset = 0;
   for(i=ts_today->tm_wday; i<ts_today->tm_wday+7; i++)
     {
        if(day_week[i%7])
          {
             if ((i%7) != ts_today->tm_wday)
               break;
             else
               {
                  if (ts_today->tm_hour < hour)
                    break;
                  else
                    if ((ts_today->tm_hour == hour) && (ts_today->tm_min < minute))
                      break;
               
               }
          }
        days_offset++;
     }

   epoch += days_offset*3600*24;

   return epoch;
}

static void
_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   
   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!eveil_config->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _menu_cb_deactivate_post, inst);
	eveil_config->menu = mn;
	
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Add an alarm"));
	e_menu_item_callback_set(mi, _menu_cb_alarm_add, NULL);
        mi = e_menu_item_new(mn);
        e_menu_item_separator_set(mi, 1);
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _menu_cb_configure, NULL);
	
	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
					  &cx, &cy, &cw, &ch);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      cx + ev->output.x, cy + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void
_menu_cb_deactivate_post(void *data, E_Menu *m)
{
   if (!eveil_config->menu) return;
   e_object_del(E_OBJECT(eveil_config->menu));
   eveil_config->menu = NULL;
}

static void
_menu_cb_alarm_add(void *data, E_Menu *m, E_Menu_Item *mi)
{
   eveil_config_alarm(NULL);
}

static void
_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   if (!eveil_config) return;
   if (eveil_config->config_dialog) return;
   eveil_config_module();
}

static void
_cb_edje_messages(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
   Instance *inst;
   int *w;

   inst = data;

   switch (id)
     {
     case EDJE_MSG_RECV_ALARM_STATE:
        if (type == EDJE_MESSAGE_INT)
          {
             w = msg;
             eveil_config->alarms_state = *w;
             if (eveil_config->alarms_state == ALARMS_STATE_ON)
               {
                  if(!eveil_config->alarms_ring_etimer)
                    eveil_config->alarms_ring_etimer =
                       ecore_timer_add(ALARMS_RING_TIME,
                                       _cb_alarms_ring_etimer,
                                       NULL);
                  
               }
             else
               {
                  if(eveil_config->alarms_ring_etimer)
                    {
                       ecore_timer_del(eveil_config->alarms_ring_etimer);
                       eveil_config->alarms_ring_etimer = NULL;
                    }
               }
             eveil_edje_message_send(EDJE_MSG_SEND_ALARM_STATE,
                                     eveil_config->alarms_state);               
          }
        break;

     case EDJE_MSG_RECV_TIMER_STATE:
        if (type == EDJE_MESSAGE_INT)
          {
             w = msg;
             eveil_config->timer_state = *w;
             if (eveil_config->timer_state != TIMER_STATE_OFF)
               eveil_timer_start();
             else
               eveil_timer_stop();
             eveil_edje_message_send(EDJE_MSG_SEND_TIMER_STATE,
                                     eveil_config->timer_state);
          }
        break;
     }
}

static int
_cb_timer_etimer(void *data)
{
   eveil_config->timer_state = TIMER_STATE_RINGING;
   eveil_edje_message_send(EDJE_MSG_SEND_TIMER_STATE,
                           TIMER_STATE_RINGING);
   eveil_config->timer_s_time = eveil_config->timer_time;
   eveil_edje_refresh_timer();

   // TODO: real popups
   if (eveil_config->timer_open_popup_default)
     {
        char buf[4096];
        
        snprintf(buf, sizeof(buf),
                 "<hilight>Timer elapsed !</hilight>"
                 "Time for pastas :D<br><br>"
                 "Real popups like in dEvian comming soon");
        e_module_dialog_show(_("Eveil Module Popup"), buf);
     }

   if (eveil_config->timer_run_program_default)
     {
        Ecore_Exe *exe;
      
        exe = ecore_exe_pipe_run(eveil_config->timer_program_default,
                                 ECORE_EXE_USE_SH, NULL);
        if (exe > 0)
          ecore_exe_free(exe);
     }

   ecore_timer_del(eveil_config->timer_s_etimer);
   eveil_config->timer_s_etimer = NULL;
   eveil_config->timer_etimer = NULL;
   return 0;
}

static int
_cb_timer_s_etimer(void *data)
{
   eveil_config->timer_s_time--;

   eveil_edje_refresh_timer();
   return 1;
}

static int
_cb_alarms_ring_etimer(void *data)
{
   Evas_List *l;
   double tt;
   double t;

   if (eveil_config->alarms_state == ALARMS_STATE_OFF)
     {
        eveil_config->alarms_ring_etimer = NULL;
        return 0;
     }

   tt = ecore_time_get();

   for(l=eveil_config->alarms; l; l=evas_list_next(l))
     {
        Alarm *al;

        al = evas_list_data(l);
        if (al->state == ALARM_STATE_OFF)
          continue;

        if (al->sched.type == ALARM_SCHED_TYPE_DAY)
          t = al->sched.date_epoch;
        else
          t = al->sched.day_next_epoch;

        printf("Testing al %s : %f (now %f)\n", al->name, t, tt);
        
        if (t <= tt)
          {
             eveil_config->alarms_state = ALARMS_STATE_RINGING;
             eveil_edje_message_send(EDJE_MSG_SEND_ALARM_STATE,
                                     ALARMS_STATE_RINGING);

             // TODO: real popups
             if ((al->open_popup == ALARM_OPEN_POPUP_YES) ||
                 (al->open_popup == ALARM_OPEN_POPUP_PARENT &&
                  eveil_config->alarms_open_popup_default))
               {
                  char buf[4096];
                  
                  snprintf(buf, sizeof(buf),
                           "<hilight>%s !</hilight>"
                           "<br><br>"
                           "Real popups like in dEvian comming soon",
                           al->name);
                  e_module_dialog_show(_("Eveil Module Popup"), buf);
               }

             if ((al->run_program == ALARM_RUN_PROGRAM_OWN) ||
                 (al->run_program == ALARM_RUN_PROGRAM_PARENT &&
                  eveil_config->alarms_run_program_default))
               {
                  Ecore_Exe *exe;
                
                  if (al->run_program == ALARM_RUN_PROGRAM_PARENT)
                    {
                       exe = ecore_exe_pipe_run(eveil_config->alarms_program_default,
                                                ECORE_EXE_USE_SH, NULL);
                    }
                  else
                    {
                       exe = ecore_exe_pipe_run(al->program,
                                                ECORE_EXE_USE_SH, NULL);
                    }
                  if (exe > 0)
                    ecore_exe_free(exe);
               }
             
             _alarm_check_date(al, 0);
             eveil_edje_refresh_alarm();
          }
     }

   return 1;
}

/*
 * Module functions
 */

EAPI void *
e_modapi_init(E_Module *m)
{
   _alarms_edd = E_CONFIG_DD_NEW("Eveil_Alarm", Alarm);
#undef T
#undef D
#define T Alarm
#define D _alarms_edd
   E_CONFIG_VAL(D, T, state, SHORT);
   E_CONFIG_VAL(D, T, name, STR);
   E_CONFIG_VAL(D, T, sched.type, SHORT);
   E_CONFIG_VAL(D, T, sched.date_epoch, DOUBLE);
   E_CONFIG_VAL(D, T, sched.day_monday, SHORT);
   E_CONFIG_VAL(D, T, sched.day_tuesday, SHORT);
   E_CONFIG_VAL(D, T, sched.day_wenesday, SHORT);
   E_CONFIG_VAL(D, T, sched.day_thursday, SHORT);
   E_CONFIG_VAL(D, T, sched.day_friday, SHORT);
   E_CONFIG_VAL(D, T, sched.day_saturday, SHORT);
   E_CONFIG_VAL(D, T, sched.day_sunday, SHORT);
   E_CONFIG_VAL(D, T, sched.hour, SHORT);
   E_CONFIG_VAL(D, T, sched.minute, SHORT);
   E_CONFIG_VAL(D, T, sched.day_next_epoch, DOUBLE);
   E_CONFIG_VAL(D, T, open_popup, SHORT);
   E_CONFIG_VAL(D, T, run_program, SHORT);
   E_CONFIG_VAL(D, T, program, STR);
   _conf_edd = E_CONFIG_DD_NEW("Eveil_Config", Config);
#undef T
#undef D
#define T Config
#define D _conf_edd
   E_CONFIG_VAL(D, T, time_format, SHORT);
   E_CONFIG_VAL(D, T, timer_time, DOUBLE);
   E_CONFIG_VAL(D, T, timer_icon_mode, SHORT);
   E_CONFIG_VAL(D, T, timer_detail_mode, SHORT);
   E_CONFIG_VAL(D, T, timer_open_popup_default, SHORT);
   E_CONFIG_VAL(D, T, timer_run_program_default, SHORT);
   E_CONFIG_VAL(D, T, timer_program_default, STR);
   E_CONFIG_VAL(D, T, alarms_state, SHORT);
   E_CONFIG_LIST(D, T, alarms, _alarms_edd);
   E_CONFIG_VAL(D, T, alarms_date_autoremove, SHORT);
   E_CONFIG_VAL(D, T, alarms_icon_mode, SHORT);
   E_CONFIG_VAL(D, T, alarms_detail_mode, SHORT);
   E_CONFIG_VAL(D, T, alarms_open_popup_default, SHORT);
   E_CONFIG_VAL(D, T, alarms_run_program_default, SHORT);
   E_CONFIG_VAL(D, T, alarms_program_default, STR);
   E_CONFIG_VAL(D, T, config_version, SHORT);
   eveil_config = e_config_domain_load("module.eveil", _conf_edd);
   if (eveil_config)
     {
        if (eveil_config->config_version < CONFIG_VERSION)
          {
             char buf[4096];

             snprintf(buf, sizeof(buf),
                      _("<hilight>Configuration Upgraded</hilight><br><br>"
                        "Your configuration of eveil module<br>"
                        "has been upgraded<br>"
                        "Your settings were removed<br>"
                        "Sorry for the inconvenience<br><br>"
                        "(%d -> %d)"), eveil_config->config_version, CONFIG_VERSION);
             e_module_dialog_show(_("Eveil Module version " MODULE_VERSION), buf);
             eveil_config = NULL;
          }
        else
          {
             if (eveil_config->config_version > CONFIG_VERSION)
               {
                  char buf[4096];

                  snprintf(buf, sizeof(buf),
                           _("<hilight>Configuration Downgraded</hilight><br><br>"
                             "Your configuration of Eveil module<br>"
                             "has been downgraded<br>"
                             "Your settings were removed<br>"
                             "Sorry for the inconvenience<br><br>"
                             "(%d ->%d)"), eveil_config->config_version, CONFIG_VERSION);
                  e_module_dialog_show(_("Eveil Module version " MODULE_VERSION), buf);
                  eveil_config = NULL;
               }
          }
     }
   if (!eveil_config)
     {
	eveil_config = E_NEW(Config, 1);
        eveil_config->time_format = TIME_FORMAT_DEFAULT;
	eveil_config->timer_time = TIMER_TIME_DEFAULT;
        eveil_config->timer_icon_mode = TIMER_ICON_MODE_DEFAULT;
        eveil_config->timer_detail_mode = TIMER_DETAIL_MODE_DEFAULT;
        eveil_config->timer_open_popup_default = TIMER_OPEN_POPUP_DEFAULT;
        eveil_config->timer_run_program_default = TIMER_RUN_PROGRAM_DEFAULT;
        eveil_config->alarms_state = ALARMS_STATE_OFF;
        eveil_config->alarms_date_autoremove = ALARMS_DATE_AUTOREMOVE_DEFAULT;
        eveil_config->alarms_icon_mode = ALARMS_ICON_MODE_DEFAULT;
        eveil_config->alarms_detail_mode = ALARMS_DETAIL_MODE_DEFAULT;
        eveil_config->alarms_open_popup_default = ALARMS_OPEN_POPUP_DEFAULT;
        eveil_config->alarms_run_program_default = ALARMS_RUN_PROGRAM_DEFAULT;
        eveil_config->config_version = CONFIG_VERSION;
     }
   E_CONFIG_LIMIT(eveil_config->time_format, TIME_FORMAT_12, TIME_FORMAT_24);
   E_CONFIG_LIMIT(eveil_config->timer_time, TIMER_TIME_MIN, TIMER_TIME_MAX);
   E_CONFIG_LIMIT(eveil_config->timer_icon_mode, 0, 2);
   E_CONFIG_LIMIT(eveil_config->timer_detail_mode, 0, 2);
   E_CONFIG_LIMIT(eveil_config->timer_open_popup_default, 0, 1);
   E_CONFIG_LIMIT(eveil_config->timer_run_program_default, 0, 1);
   E_CONFIG_LIMIT(eveil_config->alarms_date_autoremove, 0, 1);
   E_CONFIG_LIMIT(eveil_config->alarms_icon_mode, 0, 2);
   E_CONFIG_LIMIT(eveil_config->alarms_detail_mode, 0, 2);
   E_CONFIG_LIMIT(eveil_config->alarms_open_popup_default, 0, 1);
   E_CONFIG_LIMIT(eveil_config->alarms_run_program_default, 0, 1);

   eveil_config->timer_state = TIMER_STATE_OFF;
   /* set alarms state and check dates */
   if (eveil_config->alarms)
     {
        Evas_List *l;
        for (l=eveil_config->alarms; l; l=evas_list_next(l))
          {
             Alarm *al;
             al = evas_list_data(l);
             _alarm_check_date(al, 0);
          }
        eveil_config->alarms_ring_etimer = ecore_timer_add(ALARMS_RING_TIME,
                                                           _cb_alarms_ring_etimer,
                                                           NULL);
     }

   eveil_config->module = m;
   
   e_gadcon_provider_register((E_Gadcon_Client_Class *)&_gadcon_class);

   return eveil_config;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_gadcon_provider_unregister((E_Gadcon_Client_Class *)&_gadcon_class);
   
   if (eveil_config->timer_etimer)
     eveil_timer_stop();
   if (eveil_config->alarms)
     {
        Evas_List *l;
        Alarm *al;
        for (l=eveil_config->alarms; l; l=evas_list_next(l))
          {
             al = evas_list_data(l);
             eveil_alarm_del(al);
          }
        evas_list_free(eveil_config->alarms);
     }
   if (eveil_config->alarms_ring_etimer)
     ecore_timer_del(eveil_config->alarms_ring_etimer);

   if (eveil_config->timer_program_default)
     evas_stringshare_del(eveil_config->timer_program_default);
   if (eveil_config->alarms_program_default)
     evas_stringshare_del(eveil_config->alarms_program_default);

   if (eveil_config->config_dialog) 
     e_object_del(E_OBJECT(eveil_config->config_dialog));
   if (eveil_config->menu)
     {
	e_menu_post_deactivate_callback_set(eveil_config->menu , NULL, NULL);
	e_object_del(E_OBJECT(eveil_config->menu));
	eveil_config->menu = NULL;
     }

   E_FREE(eveil_config);
   E_CONFIG_DD_FREE(_alarms_edd);
   E_CONFIG_DD_FREE(_conf_edd);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.eveil", _conf_edd, eveil_config);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/module_icon.png", e_module_dir_get(m));
   m->icon_file = strdup(buf);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(_("Eveil Module version " MODULE_VERSION),
			_("A module to set timers and alarms to alert you<br>"
                          " when youre pastas are ready<br>"
			  "or when you need to wake up<br><br>"
			  "http://oooo.zapto.org<br>"
			  "ooookiwi@free.fr"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   if (!eveil_config) return 0;
   if (eveil_config->config_dialog) return 0;
   eveil_config_module();
   return 1;
}
