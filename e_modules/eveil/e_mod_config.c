/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   int    time_format;

   int    timer_time_h;
   int    timer_time_m;
   int    timer_time_s;
   int    timer_icon_mode;
   int    timer_detail_mode;
   int    timer_open_popup_default;
   int    timer_run_program_default;
   char  *timer_program_default;
   struct
   {
      Evas_Object *slider_h;
      Evas_Object *slider_m;
      Evas_Object *slider_s;
      Evas_Object *button_start;
      Evas_Object *button_stop;
      Evas_Object *button_zero;
   } timer_gui;

   Evas_Object *alarms_ilist;
   E_Config_Dialog *alarms_cfd;
   int    alarms_active;
   int    alarms_date_autoremove;
   int    alarms_icon_mode;
   int    alarms_detail_mode;
   int    alarms_open_popup_default;
   int    alarms_run_program_default;
   char  *alarms_program_default;
};

static void        *_create_data(E_Config_Dialog *cfd);
static void         _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void         _fill_data(E_Config_Dialog_Data *cfdata);
static void         _common_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata, Evas_Object *o);
static int          _common_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static void _cb_alarms_list(void *data);
static void _cb_alarm_add(void *data, void *data2);
static void _cb_alarm_del(void *data, void *data2);
static void _cb_alarm_config(void *data, void *data2);
static void _cb_timer_start(void *data, void *data2);
static void _cb_timer_stop(void *data, void *data2);
static void _cb_timer_zero(void *data, void *data2);

void eveil_config_module(void) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   
   v = E_NEW(E_Config_Dialog_View, 1);
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;
   
   cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
			     _("Eveil Configuration"), NULL, 0, v, NULL);
   eveil_config->config_dialog = cfd;
}

void eveil_config_refresh_alarms_ilist(E_Config_Dialog_Data *cfdata)
{
   Evas_List *l;
   int pos;
   int wmw, wmh;

   pos = e_widget_ilist_selected_get(cfdata->alarms_ilist);
   e_widget_ilist_clear(cfdata->alarms_ilist);

   for(l=eveil_config->alarms; l; l=evas_list_next(l))
     {
        Alarm *al;
        char buf[1024], bufdate[15];
        
        al = evas_list_data(l);
        
        if (al->sched.type == ALARM_SCHED_TYPE_DAY)
          {
             struct tm *st;
             time_t t;
             
             t = al->sched.date_epoch - (al->sched.hour*3600) - (al->sched.minute*60);
             st = localtime(&t);
             strftime(bufdate, sizeof(bufdate), "%m/%d", st);
             snprintf(buf, sizeof(buf), "%s (%s %.2d:%.2d)", al->name, bufdate, al->sched.hour, al->sched.minute);
          }
        else
          {
             // TODO: show the days of the week
             snprintf(buf, sizeof(buf), "%s (Weekly %.2d:%.2d)", al->name, al->sched.hour, al->sched.minute);
          }

        e_widget_ilist_append(cfdata->alarms_ilist, NULL, buf, _cb_alarms_list, cfdata, NULL);
     }

   e_widget_min_size_get(cfdata->alarms_ilist, &wmw, &wmh);
   if (evas_list_count(eveil_config->alarms) > 0) 
     e_widget_min_size_set(cfdata->alarms_ilist, wmw, 150);
   else 
     e_widget_min_size_set(cfdata->alarms_ilist, 165, 100);
   
   e_widget_ilist_go(cfdata->alarms_ilist);
   e_widget_ilist_selected_set(cfdata->alarms_ilist, pos);
}

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   free(cfdata->alarms_program_default);
   free(cfdata->timer_program_default);
   eveil_config->config_dialog = NULL;
   free(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   cfdata->time_format = eveil_config->time_format;
   cfdata->timer_time_h = eveil_config->timer_time/3600;
   cfdata->timer_time_m = (eveil_config->timer_time - cfdata->timer_time_h*3600)/60;
   cfdata->timer_time_s = eveil_config->timer_time - (cfdata->timer_time_h*3600 +
                                                      cfdata->timer_time_m*60);
   cfdata->timer_icon_mode = eveil_config->timer_icon_mode;
   cfdata->timer_detail_mode = eveil_config->timer_detail_mode;
   cfdata->timer_open_popup_default = eveil_config->timer_open_popup_default;
   cfdata->timer_run_program_default = eveil_config->timer_run_program_default;
   if (eveil_config->timer_program_default)
     cfdata->timer_program_default = strdup(eveil_config->timer_program_default);
   else
     cfdata->timer_program_default = strdup("");

   if (eveil_config->alarms_state == ALARMS_STATE_OFF)
     cfdata->alarms_active = ALARMS_STATE_OFF;
   else
     cfdata->alarms_active = ALARMS_STATE_ON;
   cfdata->alarms_date_autoremove = eveil_config->alarms_date_autoremove;
   cfdata->alarms_icon_mode = eveil_config->alarms_icon_mode;
   cfdata->alarms_detail_mode = eveil_config->alarms_detail_mode;
   cfdata->alarms_open_popup_default = eveil_config->alarms_open_popup_default;
   cfdata->alarms_run_program_default = eveil_config->alarms_run_program_default;
   if (eveil_config->alarms_program_default)
     cfdata->alarms_program_default = strdup(eveil_config->alarms_program_default);
   else
     cfdata->alarms_program_default = strdup("");
}

static void
_common_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata, Evas_Object *o)
{
   Evas_Object *of, *ob;

   of = e_widget_frametable_add(evas, _("Alarm"), 0);

   ob = e_widget_check_add(evas, _("Active"), &(cfdata->alarms_active));
   e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 10);

   ob = e_widget_ilist_add(evas, 80, 60, NULL);
   e_widget_ilist_selector_set(ob, 1);
   cfdata->alarms_ilist = ob;
   eveil_config_refresh_alarms_ilist(cfdata);
   e_widget_frametable_object_append(of, ob, 0, 1, 3, 1, 1, 1, 1, 1);

   ob = e_widget_button_add(evas, _("Add"), NULL, _cb_alarm_add, cfdata, NULL);
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_button_add(evas, _("Delete"), NULL, _cb_alarm_del, cfdata, NULL);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_button_add(evas, _("Configure"), NULL, _cb_alarm_config, cfdata, NULL);
   e_widget_frametable_object_append(of, ob, 2, 2, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);


   of = e_widget_frametable_add(evas, _("Timer"), 0);

   cfdata->timer_gui.slider_h =
      e_widget_slider_add(evas, 1, 0, _("%1.0f hours"),
                          (float)TIMER_TIME_MIN/3600, (float)TIMER_TIME_MAX/3600,
                          1.0, 0,
                          NULL, &(cfdata->timer_time_h), 130);
   e_widget_disabled_set(cfdata->timer_gui.slider_h, eveil_config->timer_state);
   e_widget_frametable_object_append(of, cfdata->timer_gui.slider_h,
                                     0, 0, 3, 1, 1, 0, 1, 0);
   cfdata->timer_gui.slider_m =
      e_widget_slider_add(evas, 1, 0, _("%1.0f min"),
                          0.0, 59.0,
                          1.0, 0,
                          NULL, &(cfdata->timer_time_m), 130);
   e_widget_disabled_set(cfdata->timer_gui.slider_m, eveil_config->timer_state);
   e_widget_frametable_object_append(of, cfdata->timer_gui.slider_m,
                                     0, 1, 3, 1, 1, 0, 1, 0);
   cfdata->timer_gui.slider_s =
      e_widget_slider_add(evas, 1, 0, _("%1.0f sec"),
                          0.0, 59.0,
                          1.0, 0,
                          NULL, &(cfdata->timer_time_s), 130);
   e_widget_disabled_set(cfdata->timer_gui.slider_s, eveil_config->timer_state);
   e_widget_frametable_object_append(of, cfdata->timer_gui.slider_s,
                                     0, 2, 3, 1, 1, 0, 1, 0);
   
   cfdata->timer_gui.button_start =
      e_widget_button_add(evas, _("Start !"), NULL, _cb_timer_start, cfdata, NULL);
   e_widget_disabled_set(cfdata->timer_gui.button_start, eveil_config->timer_state);
   e_widget_frametable_object_append(of, cfdata->timer_gui.button_start,
                                     0, 3, 1, 1, 1, 1, 1, 1);
   cfdata->timer_gui.button_stop =
      e_widget_button_add(evas, _("Stop !"), NULL, _cb_timer_stop, cfdata, NULL);
   e_widget_disabled_set(cfdata->timer_gui.button_stop, !eveil_config->timer_state);
   e_widget_frametable_object_append(of, cfdata->timer_gui.button_stop,
                                     1, 3, 1, 1, 1, 1, 1, 1);
   cfdata->timer_gui.button_zero =
      e_widget_button_add(evas, _("Zero"), NULL, _cb_timer_zero, cfdata, NULL);
   e_widget_disabled_set(cfdata->timer_gui.button_zero, eveil_config->timer_state);
   e_widget_frametable_object_append(of, cfdata->timer_gui.button_zero,
                                     2, 3, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 0, 1, 1, 1, 1, 1, 1, 1);
}

static int
_common_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   eveil_config->timer_time =
      cfdata->timer_time_h*3600 +
      cfdata->timer_time_m*60 +
      cfdata->timer_time_s;

   if (eveil_config->alarms_state != cfdata->alarms_active)
     {
        eveil_config->alarms_state = cfdata->alarms_active;
        eveil_edje_message_send(EDJE_MSG_SEND_ALARM_STATE,
                                cfdata->alarms_active);
     }

   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o;
   
   o = e_widget_table_add(evas, 0);

   _common_create_widgets(cfd, evas, cfdata, o);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   int ret;

   ret = _common_apply_data(cfd, cfdata);

   e_config_save_queue();
   return ret;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;

   o = e_widget_table_add(evas, 0);

   _common_create_widgets(cfd, evas, cfdata, o);


   of = e_widget_frametable_add(evas, _("Alarm Options"), 0);

   ob = e_widget_label_add(evas, _("Show icon"));
   e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 1);
   rg = e_widget_radio_group_new(&(cfdata->alarms_icon_mode));
   ob = e_widget_radio_add(evas, _("Never"), ALARMS_ICON_MODE_OFF, rg);
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("When enabled"), ALARMS_ICON_MODE_ONGO, rg);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Always"), ALARMS_ICON_MODE_ON, rg);
   e_widget_frametable_object_append(of, ob, 2, 1, 1, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Show details"));
   e_widget_frametable_object_append(of, ob, 0, 2, 2, 1, 1, 1, 1, 1);
   rg = e_widget_radio_group_new(&(cfdata->alarms_detail_mode));
   ob = e_widget_radio_add(evas, _("Never"), ALARMS_DETAIL_MODE_OFF, rg);
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("When enabled"), ALARMS_DETAIL_MODE_ONGO, rg);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Always"), ALARMS_DETAIL_MODE_ON, rg);
   e_widget_frametable_object_append(of, ob, 2, 3, 1, 1, 1, 1, 1, 1);

   ob = e_widget_check_add(evas, _("Open popup"), &(cfdata->alarms_open_popup_default));
   e_widget_frametable_object_append(of, ob, 0, 4, 2, 1, 1, 1, 1, 1);

   ob = e_widget_check_add(evas, _("Run program"), &(cfdata->alarms_run_program_default));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);

   ob = e_widget_entry_add(evas, &(cfdata->alarms_program_default));
   e_widget_min_size_set(ob, 110, 25);
   e_widget_frametable_object_append(of, ob, 1, 5, 2, 1, 1, 1, 1, 1);

   ob = e_widget_check_add(evas, _("Remove alarm after the date"), &(cfdata->alarms_date_autoremove));
   e_widget_frametable_object_append(of, ob, 0, 6, 3, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Time format"));
   e_widget_frametable_object_append(of, ob, 0, 7, 1, 1, 1, 1, 1, 1);
   rg = e_widget_radio_group_new(&(cfdata->time_format));
   ob = e_widget_radio_add(evas, _("12h"), TIME_FORMAT_12, rg);
   e_widget_frametable_object_append(of, ob, 1, 7, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("24h"), TIME_FORMAT_24, rg);
   e_widget_frametable_object_append(of, ob, 2, 7, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);


   of = e_widget_frametable_add(evas, _("Timer Options"), 0);

   ob = e_widget_label_add(evas, _("Show icon"));
   e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 1);
   rg = e_widget_radio_group_new(&(cfdata->timer_icon_mode));
   ob = e_widget_radio_add(evas, _("Never"), TIMER_ICON_MODE_OFF, rg);
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("When enabled"), TIMER_ICON_MODE_ONGO, rg);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Always"), TIMER_ICON_MODE_ON, rg);
   e_widget_frametable_object_append(of, ob, 2, 1, 1, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Show details"));
   e_widget_frametable_object_append(of, ob, 0, 2, 2, 1, 1, 1, 1, 1);
   rg = e_widget_radio_group_new(&(cfdata->timer_detail_mode));
   ob = e_widget_radio_add(evas, _("Never"), TIMER_DETAIL_MODE_OFF, rg);
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("When enabled"), TIMER_DETAIL_MODE_ONGO, rg);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Always"), TIMER_DETAIL_MODE_ON, rg);
   e_widget_frametable_object_append(of, ob, 2, 3, 1, 1, 1, 1, 1, 1);

   ob = e_widget_check_add(evas, _("Open popup"), &(cfdata->timer_open_popup_default));
   e_widget_frametable_object_append(of, ob, 0, 4, 2, 1, 1, 1, 1, 1);

   ob = e_widget_check_add(evas, _("Run program"), &(cfdata->timer_run_program_default));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);

   ob = e_widget_entry_add(evas, &(cfdata->timer_program_default));
   e_widget_min_size_set(ob, 110, 25);
   e_widget_frametable_object_append(of, ob, 1, 5, 2, 1, 1, 1, 1, 1);   

   e_widget_table_object_append(o, of, 1, 1, 1, 1, 1, 1, 1, 1);

   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   int ret;
   
   ret = _common_apply_data(cfd, cfdata);

   eveil_config->time_format = cfdata->time_format;
   if (eveil_config->timer_icon_mode != cfdata->timer_icon_mode)
     {
        eveil_config->timer_icon_mode = cfdata->timer_icon_mode;
        eveil_edje_message_send(EDJE_MSG_SEND_TIMER_ICON_MODE,
                                cfdata->timer_icon_mode);
     }
   if (eveil_config->timer_detail_mode != cfdata->timer_detail_mode)
     {
        eveil_config->timer_detail_mode = cfdata->timer_detail_mode;
        eveil_edje_message_send(EDJE_MSG_SEND_TIMER_DETAIL_MODE,
                                cfdata->timer_detail_mode);
     }
   eveil_config->timer_open_popup_default = cfdata->timer_open_popup_default;
   eveil_config->timer_run_program_default = cfdata->timer_run_program_default;
   if (eveil_config->timer_program_default)
     evas_stringshare_del(eveil_config->timer_program_default);
   eveil_config->timer_program_default = evas_stringshare_add(cfdata->timer_program_default);

   eveil_config->alarms_date_autoremove = cfdata->alarms_date_autoremove;
   if (eveil_config->alarms_icon_mode != cfdata->alarms_icon_mode)
     {
        eveil_config->alarms_icon_mode = cfdata->alarms_icon_mode;
        eveil_edje_message_send(EDJE_MSG_SEND_ALARM_ICON_MODE,
                                cfdata->alarms_icon_mode);
     }
   if (eveil_config->alarms_detail_mode != cfdata->alarms_detail_mode)
     {
        eveil_config->alarms_detail_mode = cfdata->alarms_detail_mode;
        eveil_edje_message_send(EDJE_MSG_SEND_ALARM_DETAIL_MODE,
                                cfdata->alarms_detail_mode);
     }
   eveil_config->alarms_open_popup_default = cfdata->alarms_open_popup_default;
   eveil_config->alarms_run_program_default = cfdata->alarms_run_program_default;
   if (eveil_config->alarms_program_default)
     evas_stringshare_del(eveil_config->alarms_program_default);
   eveil_config->alarms_program_default = evas_stringshare_add(cfdata->alarms_program_default);

   e_config_save_queue();
   return ret;
}



static void
_cb_alarms_list(void *data)
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
}

static void _cb_alarm_add(void *data, void *data2)
{
   eveil_config_alarm(NULL);
}

static void _cb_alarm_del(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   Alarm *al;

   cfdata = data;
   al = evas_list_nth(eveil_config->alarms,
                      e_widget_ilist_selected_get(cfdata->alarms_ilist));
   if (!al)
     return;

   eveil_alarm_del(al);
   e_config_save_queue();
   eveil_config_refresh_alarms_ilist(cfdata);
   eveil_edje_refresh_alarm();
}

static void _cb_alarm_config(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   Alarm *al;

   cfdata = data;
   al = evas_list_nth(eveil_config->alarms,
                      e_widget_ilist_selected_get(cfdata->alarms_ilist));
   if (!al)
     return;

   eveil_config_alarm(al);
}

static void _cb_timer_start(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = data;

   e_widget_disabled_set(cfdata->timer_gui.slider_h, 1);
   e_widget_disabled_set(cfdata->timer_gui.slider_m, 1);
   e_widget_disabled_set(cfdata->timer_gui.slider_s, 1);
   e_widget_disabled_set(cfdata->timer_gui.button_start, 1);
   e_widget_disabled_set(cfdata->timer_gui.button_stop, 0);
   e_widget_disabled_set(cfdata->timer_gui.button_zero, 1);

   eveil_config->timer_time =
      cfdata->timer_time_h*3600 +
      cfdata->timer_time_m*60 +
      cfdata->timer_time_s;

   eveil_timer_start();
}

static void _cb_timer_stop(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;

   e_widget_disabled_set(cfdata->timer_gui.slider_h, 0);
   e_widget_disabled_set(cfdata->timer_gui.slider_m, 0);
   e_widget_disabled_set(cfdata->timer_gui.slider_s, 0);
   e_widget_disabled_set(cfdata->timer_gui.button_start, 0);
   e_widget_disabled_set(cfdata->timer_gui.button_stop, 1);
   e_widget_disabled_set(cfdata->timer_gui.button_zero, 0);

   eveil_timer_stop();
}

static void _cb_timer_zero(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;

   e_widget_slider_value_int_set(cfdata->timer_gui.slider_h, 0);
   e_widget_slider_value_int_set(cfdata->timer_gui.slider_m, 0);
   e_widget_slider_value_int_set(cfdata->timer_gui.slider_s, 0);
}
