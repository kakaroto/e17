#include "e.h"
#include "e_mod_main.h"

typedef struct _calendar_cfdata CFData;

struct _calendar_cfdata
{
   char *size;
   char *size1;
   char *size2;
   int temp_ImageYes;
   int temp_UserCS;
   int allow_overlap;
   Calendar *calendar;
};

//static Evas_Object      *_create_widgets(E_Config_Dialog *cfd, Evas *evas, Config *cfdata);
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, void *data);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                                          void *data);
static int _basic_apply_data(E_Config_Dialog *cfd, void *data);
static Evas_Object *_color_edit_advanced_create_widgets(E_Config_Dialog *cfd,
                                                        Evas *evas, void *data);
static int _color_edit_advanced_apply_data(E_Config_Dialog *cfd, void *data);

/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
*****************************************************/
void
e_int_config_calendar(void *con, void *calendar)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   if (v)
     {
        /* methods */
        v->create_cfdata = _create_data;
        v->free_cfdata = _free_data;
        v->basic.apply_cfdata = _basic_apply_data;
        v->basic.create_widgets = _basic_create_widgets;
        v->advanced.apply_cfdata = _color_edit_advanced_apply_data;
        v->advanced.create_widgets = _color_edit_advanced_create_widgets;

        /* create config diaolg for NULL object/data */
        cfd =
           e_config_dialog_new(con, _("Calendar Settings"), NULL, 0, v,
                               calendar);
     }
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
*****************************************************/
static void *
_create_data(E_Config_Dialog *cfd)
{
   CFData *cfdata;

   cfdata = E_NEW(CFData, 1);
   cfdata->calendar = cfd->data;
   cfdata->temp_ImageYes = cfdata->calendar->conf->ImageYes;
   cfdata->temp_UserCS = cfdata->calendar->conf->UserCS;

   return cfdata;
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
*****************************************************/
static void
_free_data(E_Config_Dialog *cfd, void *data)
{
   /* Free the cfdata */
   free(data);
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
*****************************************************/
static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, void *data)
{
   /* generate the core widget layout for a basic dialog */
   Evas_Object *o, *of, *ob, *of2, *of1, *check;
   E_Radio_Group *rg;

   CFData *cfdata;

   cfdata = data;

   o = e_widget_table_add(evas, 0);

   of1 = e_widget_framelist_add(evas, _("Color/Text Scheme"), 0);
   check = e_widget_check_add(evas, _("User"), &(cfdata->temp_UserCS));
   e_widget_framelist_object_append(of1, check);
   e_widget_table_object_append(o, of1, 0, 0, 1, 1, 1, 1, 1, 1);

   of2 = e_widget_framelist_add(evas, _("Backgroud Image"), 0);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->ImageYes));
   ob = e_widget_radio_add(evas, _("With Top Image"), 0, rg);
   e_widget_framelist_object_append(of2, ob);
   ob = e_widget_radio_add(evas, _("Without Top Image"), 1, rg);
   e_widget_framelist_object_append(of2, ob);
   e_widget_table_object_append(o, of2, 0, 1, 1, 1, 1, 1, 1, 1);

   of = e_widget_framelist_add(evas, _("First Day of Week"), 0);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->DayofWeek_Start));
   ob = e_widget_radio_add(evas, _("Sunday"), 0, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Monday"), 1, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Tuesday"), 2, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Wednesday"), 3, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Thursday"), 4, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Friday"), 5, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Saturday"), 6, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_table_object_append(o, of, 0, 2, 1, 1, 1, 1, 1, 1);

   of = e_widget_framelist_add(evas, _("Extras"), 0);
   ob = e_widget_check_add(evas, _("Allow windows to overlap this gadget"), &(cfdata->calendar->conf->allow_overlap));
   e_widget_framelist_object_append(of, ob);
   e_widget_table_object_append(o, of, 0, 3, 1, 1, 1, 1, 1, 1);

   return o;
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
*****************************************************/
/**--APPLY--**/
static int
_basic_apply_data(E_Config_Dialog *cfd, void *data)
{
   CFData *cfdata;

   cfdata = data;
   /* Actually take our cfdata settings and apply them in real life */
   e_border_button_bindings_ungrab_all();
   Calendar *calendar;

   calendar = cfd->data;
   int SwitchImage = 0;

   if ((calendar->conf->ImageYes != cfdata->temp_ImageYes)
       || (calendar->conf->UserCS != cfdata->temp_UserCS))
     {
        SwitchImage = 1;
        calendar->conf->ImageYes = cfdata->calendar->conf->ImageYes;
        if (calendar->conf->ImageYes != cfdata->temp_ImageYes)
          {
             if (cfdata->temp_ImageYes == 0)
                cfdata->temp_ImageYes = 1;
             else
                cfdata->temp_ImageYes = 0;
          }
        if (calendar->conf->UserCS != cfdata->temp_UserCS)
          {
             calendar->conf->UserCS = cfdata->temp_UserCS;
             if (cfdata->temp_UserCS)
                cfdata->temp_UserCS = TRUE;
             else
                cfdata->temp_UserCS = FALSE;
          }
     }

   e_config_save_queue();
   redraw_calendar(calendar, SwitchImage);
   e_border_button_bindings_grab_all();

   return 1;                    /* Apply was OK */
}
/***************************************************
/ Function: 
/ Purpose:  Move data from dialog to config object 
/           after "advanced" apply button presesed.
/
*****************************************************/
static int
_color_edit_advanced_apply_data(E_Config_Dialog *cfd, void *data)
{
   CFData *cfdata;

   cfdata = data;
   /* Actually take our cfdata settings and apply them in real life */
   e_border_button_bindings_ungrab_all();
   Calendar *calendar;

   calendar = cfd->data;
   int SwitchImage = 0;

   if ((calendar->conf->ImageYes != cfdata->temp_ImageYes)
       || (calendar->conf->UserCS != cfdata->temp_UserCS))
     {
        SwitchImage = 1;
        calendar->conf->ImageYes = cfdata->calendar->conf->ImageYes;
        if (calendar->conf->ImageYes != cfdata->temp_ImageYes)
          {
             if (cfdata->temp_ImageYes == 0)
                cfdata->temp_ImageYes = 1;
             else
                cfdata->temp_ImageYes = 0;
          }
        if (calendar->conf->UserCS != cfdata->temp_UserCS)
          {
             if (cfdata->temp_UserCS)
                cfdata->temp_UserCS = FALSE;
             else
                cfdata->temp_UserCS = TRUE;
          }
     }

   e_config_save_queue();
   redraw_calendar(calendar, SwitchImage);
   e_border_button_bindings_grab_all();

   return 1;                    /* Apply was OK */
}
/***************************************************
/ Function: 
/ Purpose:  create widgets for advanced dialog
/
/
*****************************************************/
static Evas_Object *
_color_edit_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                                    void *data)
{
   /* generate the core widget layout for a basic dialog */
   Evas_Object *o, *of, *ob, *of2, *of_y;
   Evas_Object *font_title;
   Evas_Object *start_table;
   Evas_Object *start_tableB, *ofB;
   Evas_Object *entry, *s_table;
   E_Radio_Group *rg;
   E_Manager *man;
   E_Container *con;
   Config *conf;
   Calendar *calendar;

   CFData *cfdata;

   cfdata = data;
   conf = cfdata->calendar->conf;
   calendar = cfdata->calendar;

   CalFonts *YTC_Ptr = cfdata->calendar->conf->YM_text_class->data;
   CalFonts *DTCs_Ptr = cfdata->calendar->conf->Day_text_class->data;
   CalFonts *TC_Ptr = cfdata->calendar->conf->text_class->data;

   man = e_manager_current_get();
   if (!man)
      return NULL;
   con = e_container_current_get(man);
   if (!con)
      return NULL;
   if (con == NULL)
      return NULL;

   if (cfdata->temp_UserCS != cfdata->calendar->conf->UserCS)
     {
        calendar->conf->UserCS = cfdata->temp_UserCS;
        redraw_calendar(calendar, 1);
     }

   o = e_widget_table_add(evas, 0);
   s_table = e_widget_table_add(evas, 0);

   of2 = e_widget_framelist_add(evas, _("Top Image"), 1);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->ImageYes));
   ob = e_widget_radio_add(evas, _("With"), 0, rg);
   e_widget_framelist_object_append(of2, ob);
   ob = e_widget_radio_add(evas, _("Without"), 1, rg);
   e_widget_framelist_object_append(of2, ob);
   e_widget_table_object_append(s_table, of2, 0, 0, 1, 1, 1, 1, 1, 1);

   of = e_widget_framelist_add(evas, _("Extras"), 0);
   ob = e_widget_check_add(evas, _("Allow windows to overlap this gadget"), &(cfdata->calendar->conf->allow_overlap));
   e_widget_framelist_object_append(of, ob);
   e_widget_table_object_append(s_table, of, 0, 1, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, s_table, 0, 0, 1, 1, 1, 1, 1, 1);

   start_table = e_widget_table_add(evas, 0);
   of = e_widget_framelist_add(evas, _("First Day of Week"), 0);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->DayofWeek_Start));
   ob = e_widget_radio_add(evas, _("Sunday"), 0, rg);
   e_widget_table_object_append(start_table, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Monday"), 1, rg);
   e_widget_table_object_append(start_table, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Tuesday"), 2, rg);
   e_widget_table_object_append(start_table, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Wednesday"), 3, rg);
   e_widget_table_object_append(start_table, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Thursday"), 4, rg);
   e_widget_table_object_append(start_table, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Friday"), 5, rg);
   e_widget_table_object_append(start_table, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Saturday"), 6, rg);
   e_widget_table_object_append(start_table, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   e_widget_framelist_object_append(of, start_table);
   e_widget_table_object_append(o, of, 0, 1, 1, 1, 1, 1, 1, 1);

   if (cfdata->calendar->conf->UserCS)
     {

     /*************  Other Fonts  *************/
        font_title = e_widget_framelist_add(evas, _("Fonts"), 0);
        of_y = e_widget_table_add(evas, 1);
        //label on top
        e_widget_table_object_append(of_y, e_widget_label_add(evas, "Font"),
                                     1, 0, 1, 1, 1, 1, 1, 1);
        e_widget_table_object_append(of_y, e_widget_label_add(evas, "Size"),
                                     2, 0, 1, 1, 1, 1, 1, 1);
        //Font row
        e_widget_table_object_append(of_y, e_widget_label_add(evas, "Year/Mon"),
                                     0, 1, 1, 1, 1, 1, 1, 1);
        {
           entry = e_widget_entry_add(evas, &YTC_Ptr->font);
           e_widget_min_size_set(entry, 100, 1);
           e_widget_table_object_append(of_y, entry, 1, 1, 1, 1, 1, 1, 1, 1);
        }
        e_widget_table_object_append(of_y,
                                     e_widget_slider_add(evas, 1, 0, _("%1.2f"),
                                                         4, 48, 1, 0, NULL,
                                                         &(YTC_Ptr->size), 60),
                                     2, 1, 1, 1, 1, 1, 1, 1);

        //label for 2nd set
        e_widget_table_object_append(of_y, e_widget_label_add(evas, "Day"),
                                     0, 2, 1, 1, 1, 1, 1, 1);
        //Font row
        e_widget_table_object_append(of_y,
                                     e_widget_entry_add(evas, &DTCs_Ptr->font),
                                     1, 2, 1, 1, 1, 1, 1, 1);

        e_widget_table_object_append(of_y,
                                     e_widget_slider_add(evas, 1, 0, _("%1.2f"),
                                                         4, 48, 1, 0, NULL,
                                                         &(DTCs_Ptr->size), 60),
                                     2, 2, 1, 1, 1, 1, 1, 1);
        //label for 3rd set
        e_widget_table_object_append(of_y, e_widget_label_add(evas, "Date"),
                                     0, 3, 1, 1, 1, 1, 1, 1);
        //Font row
        e_widget_table_object_append(of_y,
                                     e_widget_entry_add(evas, &TC_Ptr->font), 1,
                                     3, 1, 1, 1, 1, 1, 1);
        e_widget_table_object_append(of_y,
                                     e_widget_slider_add(evas, 1, 0, _("%1.2f"),
                                                         4, 48, 1, 0, NULL,
                                                         &(TC_Ptr->size), 60),
                                     2, 3, 1, 1, 1, 1, 1, 1);

        e_widget_framelist_object_append(font_title, of_y);
        e_widget_table_object_append(o, font_title, 1, 0, 1, 1, 0, 0, 0, 0);

        start_tableB = e_widget_table_add(evas, 0);
        ofB = e_widget_framelist_add(evas, _("Configure Colors"), 0);
        e_widget_table_object_append(start_tableB,
                                     e_widget_button_add(evas, "Today", NULL,
                                                         e_int_config_calendar_today,
                                                         con, calendar), 0, 0,
                                     1, 1, 0, 0, 0, 0);
        e_widget_table_object_append(start_tableB,
                                     e_widget_button_add(evas, "Other", NULL,
                                                         e_int_config_calendar_other,
                                                         con, calendar), 0, 1,
                                     1, 1, 0, 0, 0, 0);
        e_widget_table_object_append(start_tableB,
                                     e_widget_button_add(evas, "Weekend", NULL,
                                                         e_int_config_calendar_weekend,
                                                         con, calendar), 1, 0,
                                     1, 1, 0, 0, 0, 0);
        e_widget_table_object_append(start_tableB,
                                     e_widget_button_add(evas, "Weekday", NULL,
                                                         e_int_config_calendar_weekday,
                                                         con, calendar), 1, 1,
                                     1, 1, 0, 0, 0, 0);
        e_widget_framelist_object_append(ofB, start_tableB);
        e_widget_table_object_append(o, ofB, 1, 1, 1, 1, 1, 1, 1, 1);
     }

//Add option to read config file
/*  	mi = e_menu_item_new(face->menu);
   	e_menu_item_label_set(mi, _("Read Conf Files"));
   	e_menu_item_callback_set(mi, read_conf_files, calendar); 

*/
   return o;
}
