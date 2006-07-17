#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   char *size;
   char *size1;
   char *size2;
   int temp_ImageYes;
   int temp_UserCS;

   Calendar *calendar;
};

//static Evas_Object      *_create_widgets(E_Config_Dialog *cfd, Evas *evas, Config *cfdata);
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static Evas_Object *_color_edit_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data);
static int _color_edit_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);

/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
*****************************************************/
void
e_int_config_calendar_other(void *con, void *calendar)
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
        cfd = e_config_dialog_new(con, D_("Calendar Settings"), NULL, 0, v, calendar);
     }
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
*****************************************************/
static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);

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
*****************************************************/
static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data)
{
   free(data);
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
****************************************************/
static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data)
{
   /* generate the core widget layout for a basic dialog */
   Evas_Object *o;
   Evas_Object *s_r_tf, *s_g_tf, *s_a_tf, *s_b_tf;
   Evas_Object *ym, *day_text;

   c_array *YNC_Ptr = data->calendar->conf->YearMon_numb_colors->data;
   c_array *DTC_Ptr = data->calendar->conf->DayWeek_text_colors->data;

   o = e_widget_table_add(evas, 0);

   // Year/month Font colors
   Evas_Object *tablef_ym;

   tablef_ym = e_widget_table_add(evas, 0);
   ym = e_widget_framelist_add(evas, D_("Year/Month Font"), 1);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Red:")), 0, 0, 1, 1, 0, 0, 0, 0);
   s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->red), 50);
   e_widget_table_object_append(tablef_ym, s_r_tf, 1, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Blue:")), 2, 0, 1, 1, 0, 0, 0, 0);
   s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_ym, s_b_tf, 3, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Green:")), 0, 1, 1, 1, 0, 0, 0, 0);
   s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->green), 50);
   e_widget_table_object_append(tablef_ym, s_g_tf, 1, 1, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Alpha:")), 2, 1, 1, 1, 0, 0, 0, 0);
   s_a_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_ym, s_a_tf, 3, 1, 1, 1, 0, 0, 0, 0);
   e_widget_framelist_object_append(ym, tablef_ym);
   e_widget_table_object_append(o, ym, 2, 3, 1, 1, 1, 1, 1, 1);
//////////////////////////////////////////////////////////////////////////////
   // Day Font colors
   Evas_Object *tablef_day;

   tablef_day = e_widget_table_add(evas, 0);
   day_text = e_widget_framelist_add(evas, D_("Day Font"), 1);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Red:")), 0, 0, 1, 1, 0, 0, 0, 0);
   s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->red), 50);
   e_widget_table_object_append(tablef_day, s_r_tf, 1, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Blue:")), 2, 0, 1, 1, 0, 0, 0, 0);
   s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_day, s_b_tf, 3, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Green:")), 0, 1, 1, 1, 0, 0, 0, 0);
   s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->green), 50);
   e_widget_table_object_append(tablef_day, s_g_tf, 1, 1, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Alpha:")), 2, 1, 1, 1, 0, 0, 0, 0);
   s_a_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_day, s_a_tf, 3, 1, 1, 1, 0, 0, 0, 0);
   e_widget_framelist_object_append(day_text, tablef_day);
   e_widget_table_object_append(o, day_text, 1, 3, 1, 1, 1, 1, 1, 1);
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
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data)
{
   /* Actually take our cfdata settings and apply them in real life */
   e_border_button_bindings_ungrab_all();
   Calendar *calendar;

   calendar = cfd->data;
   int SwitchImage = 0;

   e_config_save_queue();
   redraw_calendar(calendar, SwitchImage);
   e_border_button_bindings_grab_all();

   return 1;                    /* Apply was OK */
}
/***************************************************
/ Function: 
/ Purpose:  Move data from dialog to config object 
/           after "advanced" apply button presesed.
*****************************************************/
static int
_color_edit_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data)
{
   /* Actually take our cfdata settings and apply them in real life */
   e_border_button_bindings_ungrab_all();
   Calendar *calendar;

   calendar = cfd->data;
   int SwitchImage = 0;

   e_config_save_queue();
   redraw_calendar(calendar, SwitchImage);
   e_border_button_bindings_grab_all();

   return 1;                    /* Apply was OK */
}
/***************************************************
/ Function: 
/ Purpose:  create widgets for advanced dialog
/
*****************************************************/
static Evas_Object *
_color_edit_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data)
{
   /* generate the core widget layout for a basic dialog */
   Evas_Object *o;
   Evas_Object *s_r_tf, *s_g_tf, *s_a_tf, *s_b_tf, *s_r_tf_o, *s_b_tf_o,
      *s_g_tf_o, *s_a_tf_o, *s_r_tf_s, *s_b_tf_s, *s_g_tf_s, *s_a_tf_s;
   Evas_Object *ym, *day_text;

   c_array *YNC_Ptr = data->calendar->conf->YearMon_numb_colors->data;
   c_array *DTC_Ptr = data->calendar->conf->DayWeek_text_colors->data;

   o = e_widget_table_add(evas, 0);

   // Year/month Font colors
   Evas_Object *tablef_ym;

   tablef_ym = e_widget_table_add(evas, 0);
   ym = e_widget_framelist_add(evas, D_("Year/Month Font"), 1);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Red:")), 0, 0, 1, 1, 0, 0, 0, 0);
   s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->red), 50);
   e_widget_table_object_append(tablef_ym, s_r_tf, 1, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Blue:")), 2, 0, 1, 1, 0, 0, 0, 0);
   s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_ym, s_b_tf, 3, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Green:")), 0, 1, 1, 1, 0, 0, 0, 0);
   s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->green), 50);
   e_widget_table_object_append(tablef_ym, s_g_tf, 1, 1, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("Alpha:")), 2, 1, 1, 1, 0, 0, 0, 0);
   s_a_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_ym, s_a_tf, 3, 1, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("R Out:")), 0, 2, 1, 1, 0, 0, 0, 0);
   s_r_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->red), 50);
   e_widget_table_object_append(tablef_ym, s_r_tf_o, 1, 2, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("B Out:")), 2, 2, 1, 1, 0, 0, 0, 0);
   s_b_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_ym, s_b_tf_o, 3, 2, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("G Out:")), 0, 3, 1, 1, 0, 0, 0, 0);
   s_g_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->green), 50);
   e_widget_table_object_append(tablef_ym, s_g_tf_o, 1, 3, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("A Out:")), 2, 3, 1, 1, 0, 0, 0, 0);
   s_a_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_ym, s_a_tf_o, 3, 3, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("R Shad:")), 0, 4, 1, 1, 0, 0, 0, 0);
   s_r_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->red), 50);
   e_widget_table_object_append(tablef_ym, s_r_tf_s, 1, 4, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("B Shad:")), 2, 4, 1, 1, 0, 0, 0, 0);
   s_b_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_ym, s_b_tf_s, 3, 4, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("G Shad:")), 0, 5, 1, 1, 0, 0, 0, 0);
   s_g_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->green), 50);
   e_widget_table_object_append(tablef_ym, s_g_tf_s, 1, 5, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_ym, e_widget_label_add(evas, D_("A Shad:")), 2, 5, 1, 1, 0, 0, 0, 0);
   s_a_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(YNC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_ym, s_a_tf_s, 3, 5, 1, 1, 0, 0, 0, 0);
   e_widget_framelist_object_append(ym, tablef_ym);
   e_widget_table_object_append(o, ym, 1, 3, 1, 1, 1, 1, 1, 1);
//////////////////////////////////////////////////////////////////////////////
   // Day Font colors
   Evas_Object *tablef_day;

   tablef_day = e_widget_table_add(evas, 0);
   day_text = e_widget_framelist_add(evas, D_("Day Font"), 1);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Red:")), 0, 0, 1, 1, 0, 0, 0, 0);
   s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->red), 50);
   e_widget_table_object_append(tablef_day, s_r_tf, 1, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Blue:")), 2, 0, 1, 1, 0, 0, 0, 0);
   s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_day, s_b_tf, 3, 0, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Green:")), 0, 1, 1, 1, 0, 0, 0, 0);
   s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->green), 50);
   e_widget_table_object_append(tablef_day, s_g_tf, 1, 1, 1, 1, 0, 0, 0, 0);
   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("Alpha:")), 2, 1, 1, 1, 0, 0, 0, 0);
   s_a_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_day, s_a_tf, 3, 1, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("R Out:")), 0, 2, 1, 1, 0, 0, 0, 0);
   s_r_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->red), 50);
   e_widget_table_object_append(tablef_day, s_r_tf_o, 1, 2, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("B Out:")), 2, 2, 1, 1, 0, 0, 0, 0);
   s_b_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_day, s_b_tf_o, 3, 2, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("G Out:")), 0, 3, 1, 1, 0, 0, 0, 0);
   s_g_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->green), 50);
   e_widget_table_object_append(tablef_day, s_g_tf_o, 1, 3, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("A Out:")), 2, 3, 1, 1, 0, 0, 0, 0);
   s_a_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_day, s_a_tf_o, 3, 3, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("R Shad:")), 0, 4, 1, 1, 0, 0, 0, 0);
   s_r_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->red), 50);
   e_widget_table_object_append(tablef_day, s_r_tf_s, 1, 4, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("B Shad:")), 2, 4, 1, 1, 0, 0, 0, 0);
   s_b_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->blue), 50);
   e_widget_table_object_append(tablef_day, s_b_tf_s, 3, 4, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("G Shad:")), 0, 5, 1, 1, 0, 0, 0, 0);
   s_g_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->green), 50);
   e_widget_table_object_append(tablef_day, s_g_tf_s, 1, 5, 1, 1, 0, 0, 0, 0);

   e_widget_table_object_append(tablef_day, e_widget_label_add(evas, D_("A Shad:")), 2, 5, 1, 1, 0, 0, 0, 0);
   s_a_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"), 0, 254, 1, 0, NULL, &(DTC_Ptr->alpha), 50);
   e_widget_table_object_append(tablef_day, s_a_tf_s, 3, 5, 1, 1, 0, 0, 0, 0);

   e_widget_framelist_object_append(day_text, tablef_day);
   e_widget_table_object_append(o, day_text, 0, 3, 1, 1, 1, 1, 1, 1);

   return o;
}
