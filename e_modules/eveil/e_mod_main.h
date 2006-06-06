/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define CONFIG_VERSION 7

#define TIME_FORMAT_12 0
#define TIME_FORMAT_24 1
#define TIME_FORMAT_DEFAULT 1

#define TIMER_STATE_OFF 0
#define TIMER_STATE_ON 1
#define TIMER_STATE_RINGING 2
#define TIMER_TIME_MIN 1
#define TIMER_TIME_MAX (3600*24)
#define TIMER_TIME_DEFAULT (60*9)
#define TIMER_ICON_MODE_OFF 0
#define TIMER_ICON_MODE_ON 1
#define TIMER_ICON_MODE_ONGO 2
#define TIMER_ICON_MODE_DEFAULT 2
#define TIMER_DETAIL_MODE_OFF 0
#define TIMER_DETAIL_MODE_ON 1
#define TIMER_DETAIL_MODE_ONGO 2
#define TIMER_DETAIL_MODE_DEFAULT 2
#define TIMER_OPEN_POPUP_DEFAULT 1
#define TIMER_RUN_PROGRAM_DEFAULT 0

#define ALARMS_STATE_OFF 0
#define ALARMS_STATE_ON 1
#define ALARMS_STATE_RINGING 2
#define ALARMS_DATE_AUTOREMOVE_DEFAULT 0
#define ALARMS_ICON_MODE_OFF 0
#define ALARMS_ICON_MODE_ON 1
#define ALARMS_ICON_MODE_ONGO 2
#define ALARMS_ICON_MODE_DEFAULT 1
#define ALARMS_DETAIL_MODE_OFF 0
#define ALARMS_DETAIL_MODE_ON 1
#define ALARMS_DETAIL_MODE_ONGO 2
#define ALARMS_DETAIL_MODE_DEFAULT 2
#define ALARMS_OPEN_POPUP_DEFAULT 1
#define ALARMS_RUN_PROGRAM_DEFAULT 0
#define ALARMS_RING_TIME (60)

#define ALARM_STATE_OFF 0
#define ALARM_STATE_ON 1
#define ALARM_STATE_RINGING 2
#define ALARM_STATE_SNOOZED 3
#define ALARM_SCHED_TYPE_DAY 0
#define ALARM_SCHED_TYPE_WEEK 1
#define ALARM_SCHED_TYPE_DEFAULT 1
#define ALARM_AUTOREMOVE_NO 0
#define ALARM_AUTOREMOVE_YES 1
#define ALARM_AUTOREMOVE_PARENT 2
#define ALARM_OPEN_POPUP_NO 0
#define ALARM_OPEN_POPUP_PARENT 1
#define ALARM_OPEN_POPUP_YES 2
#define ALARM_RUN_PROGRAM_NO 0
#define ALARM_RUN_PROGRAM_PARENT 1
#define ALARM_RUN_PROGRAM_OWN 2
#define ALARM_RUN_PROGRAM_DEFAULT 1
#define ALARM_SNOOZE_HOUR_DEFAULT 0
#define ALARM_SNOOZE_MINUTE_DEFAULT 15
#define ALARM_ADD_ERROR_UNKNOWN 0
#define ALARM_ADD_ERROR_NAME 1
#define ALARM_ADD_ERROR_SCHED_WEEK 2
#define ALARM_ADD_ERROR_SCHED_DAY 3
#define ALARM_ADD_ERROR_SCHED_BEFORE 4

#define RING_TEST_ALARM 0
#define RING_TEST_TIMER 1

#define THEME_IN_E "base/theme/modules/eveil"
#define THEME_MAIN "modules/eveil/main"
#define THEME_ICON_SNOOZE "modules/eveil/icon/alarm/snooze"
#define THEME_ICON_ALARM_ON "modules/eveil/icon/alarm/on"
#define THEME_ICON_ALARM_OFF "modules/eveil/icon/alarm/off"
#define THEME_ICON_TIMER_ON "modules/eveil/icon/timer/on"
#define THEME_ICON_TIMER_OFF "modules/eveil/icon/timer/off"

#define EDJE_MSG_RECV_ALARM_STATE 0
#define EDJE_MSG_RECV_TIMER_STATE 1

#define EDJE_MSG_SEND_ALARM_STATE 0
#define EDJE_MSG_SEND_ALARM_ICON_MODE 1
#define EDJE_MSG_SEND_ALARM_DETAIL_MODE 2
#define EDJE_MSG_SEND_TIMER_STATE 3
#define EDJE_MSG_SEND_TIMER_ICON_MODE 4
#define EDJE_MSG_SEND_TIMER_DETAIL_MODE 5

#define GET_NEXT_EPOCH(epoch, al) \
        if (al->sched.type == ALARM_SCHED_TYPE_DAY) \
          epoch = al->sched.date_epoch; \
        else \
          epoch = al->sched.day_next_epoch;

typedef struct _Config Config;
typedef struct _Alarm Alarm;

struct _Alarm
{
   int         state;
   const char  *name;

   struct
   {
      int type;
      double date_epoch;
      int hour;
      int minute;
      int day_monday;
      int day_tuesday;
      int day_wenesday;
      int day_thursday;
      int day_friday;
      int day_saturday;
      int day_sunday;
      double day_next_epoch;
   } sched;
   int autoremove;

   struct
   {
      int hour;
      int minute;
      int remember;
      E_Dialog *dia;
      Ecore_Timer *etimer;
   } snooze;

   const char *description;
   int   open_popup;
   int   run_program;
   const char *program;
};

struct _Config
{
   int              time_format;

   int              timer_state;
   double           timer_time;
   Ecore_Timer     *timer_etimer;
   double           timer_s_time;
   Ecore_Timer     *timer_s_etimer;
   int              timer_icon_mode;
   int              timer_detail_mode;
   int              timer_open_popup_default;
   int              timer_run_program_default;
   const char      *timer_program_default;

   int              alarms_state;
   Evas_List       *alarms;
   Ecore_Timer     *alarms_ring_etimer;
   int              alarms_date_autoremove_default;
   int              alarms_icon_mode;
   int              alarms_detail_mode;
   int              alarms_open_popup_default;
   int              alarms_run_program_default;
   const char      *alarms_program_default;

   int config_version;
   char *theme;
   E_Module        *module;
   E_Config_Dialog *config_dialog;
   Evas_List       *instances;
   E_Menu          *menu;
};

EAPI extern E_Module_Api e_modapi;
extern Config *eveil_config;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

Alarm *eveil_alarm_add(int state, char *name, int type, char *date, int day_monday, int day_tuesday, int day_wenesday, int day_thursday, int day_friday, int day_saturday, int day_sunday, int hour, int minute, int autoremove, char *description, int open_popup, int run_program, char *program, int *error);
void   eveil_alarm_del(Alarm *al);
int    eveil_alarm_ring(Alarm *al, int test);
void   eveil_timer_start(void);
void   eveil_timer_stop(void);
void   eveil_edje_message_send(int id, int message, void *data);
void   eveil_edje_text_set(char *part, char *text);
void   eveil_edje_refresh_alarm(void);
void   eveil_edje_refresh_timer(void);

void   eveil_config_module(void);
void   eveil_config_refresh_alarms_ilist(E_Config_Dialog_Data *cfdata);
void   eveil_config_alarm(Alarm *al);

#endif
