/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   int new;
   Alarm *al;

   int state;
   char *name;

   struct
   {
      int type;
      int hour;
      int minute;
      char *date;
      int day_monday;
      int day_tuesday;
      int day_wenesday;
      int day_thursday;
      int day_friday;
      int day_saturday;
      int day_sunday;
   } sched;
   struct
   {
      Evas_Object *date;
   } sched_gui;
   int autoremove;

   char *description;
   int open_popup;
   int run_program;
   char *program;
};

static void        *_create_data(E_Config_Dialog *cfd);
static void         _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void         _fill_data(E_Config_Dialog_Data *cfdata, Alarm *al);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static void _cb_alarm_test(void *data, void *data2);
static void _cb_alarm_today(void *data, void *data2);
static void _cb_alarm_tomorrow(void *data, void *data2);

void eveil_config_alarm(Alarm *al) 
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
			     _("Eveil Alarm Configuration"), NULL, 0, v, al);
}

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata, cfd->data);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   free(cfdata->name);
   free(cfdata->description);
   free(cfdata->program);
   free(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata, Alarm *al)
{
   if (al)
     {
        cfdata->new = 0;
        cfdata->al = al;
        cfdata->state = al->state;
        if (al->name)
          cfdata->name = strdup(al->name);
        else
          cfdata->name = strdup("");
        cfdata->sched.type = al->sched.type;
        cfdata->sched.hour = al->sched.hour;
        cfdata->sched.minute = al->sched.minute;
        if (al->sched.type == ALARM_SCHED_TYPE_DAY)
          {
             struct tm *st;
             time_t t;
             char buf[20];

             t = al->sched.date_epoch - (al->sched.hour*3600) - (al->sched.minute*60);
             st = localtime(&t);
             strftime(buf, sizeof(buf), "%Y/%m/%d", st);
             cfdata->sched.date = strdup(buf);
          }
        else
          cfdata->sched.date = strdup("");
        cfdata->sched.day_monday = al->sched.day_monday;
        cfdata->sched.day_tuesday = al->sched.day_tuesday;
        cfdata->sched.day_wenesday = al->sched.day_wenesday;
        cfdata->sched.day_thursday = al->sched.day_thursday;
        cfdata->sched.day_friday = al->sched.day_friday;
        cfdata->sched.day_saturday = al->sched.day_saturday;
        cfdata->sched.day_sunday = al->sched.day_sunday;
	cfdata->autoremove = al->autoremove;
	if (al->description)
	   cfdata->description = strdup(al->description);
	else
	   cfdata->description = strdup("");
        cfdata->open_popup = al->open_popup;
        cfdata->run_program = al->run_program;
        if (al->program)
          cfdata->program = strdup(al->program);
        else
          cfdata->program = strdup("");
     }
   else
     {
	struct tm *st;
	time_t t;
	char buf[20];

        cfdata->new = 1;
        cfdata->state = ALARM_STATE_ON;
        cfdata->name = strdup("");
        cfdata->sched.type = ALARM_SCHED_TYPE_DEFAULT;

	t = time(NULL);
	st = localtime(&t);
	strftime(buf, sizeof(buf), "%Y/", st);
	cfdata->sched.date = strdup(buf);

	cfdata->autoremove = ALARM_AUTOREMOVE_PARENT;
	cfdata->description = strdup("");
        cfdata->open_popup = ALARM_OPEN_POPUP_PARENT;
        cfdata->run_program = ALARM_RUN_PROGRAM_PARENT;
        cfdata->program = strdup("");
     }
}

static void
_common_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata, Evas_Object *o)
{
   Evas_Object *of, *ob;
   E_Radio_Group *rg;

   of = e_widget_frametable_add(evas, _("Informations"), 0);

   ob = e_widget_check_add(evas, _("Active"), &(cfdata->state));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Name"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->name));
   e_widget_min_size_set(ob, 200, 25);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Description"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->description));
   e_widget_min_size_set(ob, 250, 25);
   e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 1, 1, 1);
   

   e_widget_list_object_append(o, of, 1, 1, 0.5);


   of = e_widget_frametable_add(evas, _("Schedule"), 0);

   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f hour"), 0.0, 23.0, 1.0, 0,
                            NULL, &(cfdata->sched.hour), 130);
   e_widget_frametable_object_append(of, ob, 0, 0, 4, 1, 1, 0, 1, 0);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f minute"), 0.0, 59.0, 1.0, 0,
                            NULL, &(cfdata->sched.minute), 130);
   e_widget_frametable_object_append(of, ob, 0, 1, 4, 1, 1, 0, 1, 0);

   rg = e_widget_radio_group_new(&(cfdata->sched.type));

   ob = e_widget_radio_add(evas, _("Days of the week"), ALARM_SCHED_TYPE_WEEK, rg);
   e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Monday"), &(cfdata->sched.day_monday));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Tuesday"), &(cfdata->sched.day_tuesday));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Wenesday"), &(cfdata->sched.day_wenesday));
   e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Thursday"), &(cfdata->sched.day_thursday));
   e_widget_frametable_object_append(of, ob, 0, 7, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Friday"), &(cfdata->sched.day_friday));
   e_widget_frametable_object_append(of, ob, 0, 8, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Saturday"), &(cfdata->sched.day_saturday));
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Sunday"), &(cfdata->sched.day_sunday));
   e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);

   ob = e_widget_radio_add(evas, _("One day"), ALARM_SCHED_TYPE_DAY, rg);
   e_widget_frametable_object_append(of, ob, 3, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Date (YYYY/MM/DD)"));
   e_widget_frametable_object_append(of, ob, 3, 4, 1, 1, 1, 1, 1, 1);
   cfdata->sched_gui.date = e_widget_entry_add(evas, &(cfdata->sched.date));
   e_widget_min_size_set(cfdata->sched_gui.date, 100, 25);
   e_widget_frametable_object_append(of, cfdata->sched_gui.date, 3, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_button_add(evas, _("Today"), NULL, _cb_alarm_today, cfdata, NULL);
   e_widget_frametable_object_append(of, ob, 3, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_button_add(evas, _("Tomorow"), NULL, _cb_alarm_tomorrow, cfdata, NULL);
   e_widget_frametable_object_append(of, ob, 3, 7, 1, 1, 1, 1, 1, 1);

   e_widget_list_object_append(o, of, 1, 1, 0.5);
}

static int
_common_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Alarm *al;

   al = eveil_alarm_add(cfdata->state, cfdata->name,
                        cfdata->sched.type,
                        cfdata->sched.date,
                        cfdata->sched.day_monday,
                        cfdata->sched.day_tuesday,
                        cfdata->sched.day_wenesday,
                        cfdata->sched.day_thursday,
                        cfdata->sched.day_friday,
                        cfdata->sched.day_saturday,
                        cfdata->sched.day_sunday,
                        cfdata->sched.hour, cfdata->sched.minute,
			cfdata->autoremove,
			cfdata->description,
                        cfdata->open_popup,
                        cfdata->run_program,
                        cfdata->program);

   if (!al)
     {
        char buf[4096];
        snprintf(buf, sizeof(buf),
                 "<hilight>Alarm not added !</hilight><br>"
                 "information / schedule you gave are not correct");
        e_module_dialog_show(_("Eveil Module Error"), buf);
        return 0;
     }

   if (!cfdata->new)
     eveil_config->alarms = evas_list_remove(eveil_config->alarms, cfdata->al);

   eveil_config->alarms = evas_list_append(eveil_config->alarms, al);
   cfdata->new = 0;
   cfdata->al = al;

   if (eveil_config->config_dialog)
     {
        E_Config_Dialog_Data *mcda;

        mcda = eveil_config->config_dialog->cfdata;
        eveil_config_refresh_alarms_ilist(mcda);
     }

   eveil_edje_refresh_alarm();

   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *ob;

   o = e_widget_list_add(evas, 0, 0);

   _common_create_widgets(cfd, evas, cfdata, o);

   ob = e_widget_button_add(evas, _("Test this alarm"), NULL, _cb_alarm_test, cfd, cfdata);

   e_widget_list_object_append(o, ob, 1, 1, 0.5);

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

   o = e_widget_list_add(evas, 0, 0);

   _common_create_widgets(cfd, evas, cfdata, o);

   of = e_widget_frametable_add(evas, _("Ring Options"), 0);

   ob = e_widget_label_add(evas, _("Remove alarm"));
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);

   rg = e_widget_radio_group_new(&(cfdata->autoremove));

   ob = e_widget_radio_add(evas, _("No"), ALARM_AUTOREMOVE_NO, rg);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Use general settings"), ALARM_AUTOREMOVE_PARENT, rg);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Yes"), ALARM_AUTOREMOVE_YES, rg);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Open popup"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);

   rg = e_widget_radio_group_new(&(cfdata->open_popup));

   ob = e_widget_radio_add(evas, _("No"), ALARM_OPEN_POPUP_NO, rg);
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Use general settings"), ALARM_OPEN_POPUP_PARENT, rg);
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Yes"), ALARM_OPEN_POPUP_YES, rg);
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Run a program"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);

   rg = e_widget_radio_group_new(&(cfdata->run_program));

   ob = e_widget_radio_add(evas, _("No"), ALARM_RUN_PROGRAM_NO, rg);
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Use general settings"), ALARM_RUN_PROGRAM_PARENT, rg);
   e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Run this program"), ALARM_RUN_PROGRAM_OWN, rg);
   e_widget_frametable_object_append(of, ob, 0, 7, 1, 1, 1, 1, 1, 1);

   ob = e_widget_entry_add(evas, &(cfdata->program));
   e_widget_min_size_set(ob, 150, 25);
   e_widget_frametable_object_append(of, ob, 1, 7, 1, 1, 1, 1, 1, 1);

   e_widget_list_object_append(o, of, 1, 1, 0.5);


   ob = e_widget_button_add(evas, _("Test this alarm"), NULL, _cb_alarm_test, cfd, cfdata);

   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   int ret;

   ret = _common_apply_data(cfd, cfdata);

   e_config_save_queue();
   return ret;
}


static void _cb_alarm_test(void *data, void *data2)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;

   cfd = data;
   cfdata = data2;

   if(!_basic_apply_data(cfd, cfdata))
     return;

   eveil_alarm_test(cfdata->al);
}

static void _cb_alarm_today(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   struct tm *st;
   time_t t;
   char buf[20];

   cfdata = data;
   t = time(NULL);
   st = localtime(&t);
   strftime(buf, sizeof(buf), "%Y/%m/%d", st);

   e_widget_entry_text_set(cfdata->sched_gui.date, buf);
}

static void _cb_alarm_tomorrow(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   struct tm *st;
   time_t t;
   char buf[20];

   cfdata = data;
   t = time(NULL) + 3600*24;
   st = localtime(&t);
   strftime(buf, sizeof(buf), "%Y/%m/%d", st);

   e_widget_entry_text_set(cfdata->sched_gui.date, buf);
}
