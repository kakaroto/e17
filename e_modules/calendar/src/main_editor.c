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

   Calendar *calendar;
};

static Evas_Object 	  *_create_widgets(E_Config_Dialog *cfd, Evas *evas, Config *cfdata);
static void 	    	 *_create_data(E_Config_Dialog *cfd);
static void 	    	  _free_data(E_Config_Dialog *cfd, void *data);
static Evas_Object  	 *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, void *data);
static int 	     	_basic_apply_data(E_Config_Dialog *cfd, void *data);
static Evas_Object   *_color_edit_advanced_create_widgets(E_Config_Dialog *cfd,Evas *evas,void *data);
static int            _color_edit_advanced_apply_data(E_Config_Dialog *cfd, void *data);
#define IFDUP(src, dst) if(src) dst = strdup(src); else dst = NULL

/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
/*****************************************************/
void
e_int_config_calendar(void *con, void *calendar)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View v;
   /* methods */
   v.create_cfdata           = _create_data;
   v.free_cfdata             = _free_data;
   v.basic.apply_cfdata      = _basic_apply_data;
   v.basic.create_widgets    = _basic_create_widgets;
   v.advanced.apply_cfdata   = _color_edit_advanced_apply_data;
   v.advanced.create_widgets = _color_edit_advanced_create_widgets;

   /* create config diaolg for NULL object/data */
   cfd = e_config_dialog_new(con, _("Calendar Settings"), NULL, 0, &v, calendar);
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
/*****************************************************/
static void *
_create_data(E_Config_Dialog *cfd)
{
   CFData *cfdata;
   cfdata = E_NEW(CFData, 1);
   cfdata->calendar = cfd->data;
   cfdata->temp_ImageYes = cfdata->calendar->conf->ImageYes;
   cfdata->temp_UserCS = cfdata->calendar->conf->UserCS;

   CalFonts *YTC_Ptr = cfdata->calendar->conf->YM_text_class->data;     
   CalFonts *DTCs_Ptr = cfdata->calendar->conf->Day_text_class->data;      
   CalFonts *TC_Ptr  = cfdata->calendar->conf->text_class->data;      

   c_array *TBC_Ptr = cfdata->calendar->conf->Today_s_back_colors->data;
   c_array *TTC_Ptr = cfdata->calendar->conf->Today_s_text_colors->data;
   c_array *WeTC_Ptr = cfdata->calendar->conf->Weekend_text_colors->data; 
   c_array *WdTC_Ptr = cfdata->calendar->conf->WeekDay_text_colors->data; 
   c_array *WeBC_Ptr = cfdata->calendar->conf->Weekend_back_colors->data; 
   c_array *WdBC_Ptr = cfdata->calendar->conf->WeekDay_back_colors->data; 
   c_array *YNC_Ptr = cfdata->calendar->conf->YearMon_numb_colors->data;  
   c_array *DTC_Ptr = cfdata->calendar->conf->DayWeek_text_colors->data;  

   char temp[50];
   sprintf(temp,"%d",YTC_Ptr->size);
   IFDUP(temp,cfdata->size);
   sprintf(temp,"%d",DTCs_Ptr->size);
   IFDUP(temp,cfdata->size1);
   sprintf(temp,"%d",TC_Ptr->size);
   IFDUP(temp,cfdata->size2);


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
/*****************************************************/
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
/*****************************************************/
static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, void *data)
{
   /* generate the core widget layout for a basic dialog */
   Evas_Object *o, *of, *ob, *of2, *of3, *fs, *of1,*check;
   E_Radio_Group *rg;
   E_Manager *man;
   E_Container *con;

   CFData *cfdata;
   cfdata = data;

   o = e_widget_table_add(evas, 0);

   of1 = e_widget_framelist_add(evas, _("Color/Text Scheme"), 0);
   check = e_widget_check_add(evas, _("User"), &(cfdata->calendar->conf->UserCS));
   e_widget_framelist_object_append(of1,check);
   e_widget_table_object_append(o, of1, 0, 0,1,1,1,1,1,1);

   of2 = e_widget_framelist_add(evas, _("Backgroud Image"), 0);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->ImageYes));
   ob = e_widget_radio_add(evas, _("With Top Image"), 0, rg);
   e_widget_framelist_object_append(of2, ob);
   ob = e_widget_radio_add(evas, _("Without Top Image"), 1, rg);
   e_widget_framelist_object_append(of2, ob);
   e_widget_table_object_append(o, of2, 0, 1,1,1,1,1,1,1);

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
   e_widget_table_object_append(o, of, 0, 2,1,1,1,1,1,1);

   return o;
}
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
/*****************************************************/
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

   if ((calendar->conf->ImageYes != cfdata->temp_ImageYes) || (calendar->conf->UserCS !=
							                   cfdata->temp_UserCS))
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
   redraw_calendar(calendar,SwitchImage);
   e_border_button_bindings_grab_all();

   return 1; /* Apply was OK */
}
/***************************************************
/ Function: 
/ Purpose:  create widgets for advanced dialog
/
/
/*****************************************************/
/*static Evas_Object *
_color_edit_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, void *data)
{
   /* generate the core widget layout for a basic dialog */
/*   Evas_Object *o, *of, *ob, *of2, *of3, *fs, *of1,*check, *today, *weekend, *weekday, *of_y, *o2;
   Evas_Object *t_color, *tf_color, *t_font;
   Evas_Object *wd_color, *wdf_color, *wd_font;
   Evas_Object *we_color, *wef_color, *we_font;
   Evas_Object *YM_color, *YM_font, *other_c, *Day_colors;
   Evas_Object *fonts, *Day_font, *Date_font;
   Evas_Object *load_edje, *back, *arrow, *label, *t_edje, *we_edje, *wd_edje, *top;
   Evas_Object *font_title;
   Evas_Object *slider_red, *slider_green, *slider_alpha, *slider_blue;
   Evas_Object *s_r_tf, *s_g_tf, *s_a_tf, *s_b_tf, *s_r_tf_o, *s_b_tf_o, *s_g_tf_o, *s_a_tf_o, *s_r_tf_s, *s_b_tf_s, *s_g_tf_s, *s_a_tf_s, *today_f, *weekend_f, *weekday_f ;
   E_Radio_Group *rg;
   E_Manager *man;
   E_Container *con;
   Config *conf;

   CFData *cfdata;
   cfdata = data;
   conf = cfdata->calendar->conf;

   man = e_manager_current_get();
   if (!man) return;
   con = e_container_current_get(man);
   if (!con) return;
   if (con == NULL) return;

   c_array *TBC_Ptr = cfdata->calendar->conf->Today_s_back_colors->data;
   c_array *TTC_Ptr = cfdata->calendar->conf->Today_s_text_colors->data;
   c_array *WeTC_Ptr = cfdata->calendar->conf->Weekend_text_colors->data; 
   c_array *WdTC_Ptr = cfdata->calendar->conf->WeekDay_text_colors->data; 
   c_array *WeBC_Ptr = cfdata->calendar->conf->Weekend_back_colors->data; 
   c_array *WdBC_Ptr = cfdata->calendar->conf->WeekDay_back_colors->data; 
   c_array *YNC_Ptr = cfdata->calendar->conf->YearMon_numb_colors->data;  
   c_array *DTC_Ptr = cfdata->calendar->conf->DayWeek_text_colors->data;  



   o = e_widget_table_add(evas, 0);

   of2 = e_widget_framelist_add(evas, _("Backgroud Image"), 0);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->ImageYes));
   ob = e_widget_radio_add(evas, _("With Top Image"), 0, rg);
   e_widget_framelist_object_append(of2, ob);
   ob = e_widget_radio_add(evas, _("Without Top Image"), 1, rg);
   e_widget_framelist_object_append(of2, ob);
   e_widget_table_object_append(o, of2, 0, 0,1,1,0,0,0,0);

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
   e_widget_table_object_append(o, of, 0, 1,1,1,0,0,0,0);

   if (cfdata->calendar->conf->UserCS)
   {
     //Today colors box
    today = e_widget_framelist_add(evas, _("Today Background Colors"),0);
    label = e_widget_label_add(evas, _("Red:"));
    e_widget_framelist_object_append(today, label);
    slider_red = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TBC_Ptr->red), 200);
    e_widget_framelist_object_append(today, slider_red);
    label = e_widget_label_add(evas, _("Blue:"));
    e_widget_framelist_object_append(today, label);
    slider_blue = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TBC_Ptr->blue), 200);
    e_widget_framelist_object_append(today, slider_blue);
    label = e_widget_label_add(evas, _("Green:"));
    e_widget_framelist_object_append(today, label);
    slider_green = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(TBC_Ptr->green), 200);
    e_widget_framelist_object_append(today, slider_green);
    label = e_widget_label_add(evas, _("Alpha:"));
    e_widget_framelist_object_append(today, label);
    slider_alpha = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(TBC_Ptr->alpha), 200);
    e_widget_framelist_object_append(today, slider_alpha);
    e_widget_table_object_append(o, today, 1,0,1,1,0,0,0,0);

    // Today Font colors
    today_f = e_widget_framelist_add(evas, _("Today Font Colors"),0);
    label = e_widget_label_add(evas, _("Red:"));
    e_widget_framelist_object_append(today_f, label);
    s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TTC_Ptr->red), 200);
    e_widget_framelist_object_append(today_f, s_r_tf);
    label = e_widget_label_add(evas, _("Blue:"));
    e_widget_framelist_object_append(today_f, label);
    s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TTC_Ptr->blue), 200);
    e_widget_framelist_object_append(today_f, s_b_tf);
    label = e_widget_label_add(evas, _("Green:"));
    e_widget_framelist_object_append(today_f, label);
    s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(TTC_Ptr->green), 200);
    e_widget_framelist_object_append(today_f, s_g_tf);
    label = e_widget_label_add(evas, _("Alpha:"));
    e_widget_framelist_object_append(today_f, label);
    s_a_tf = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(TTC_Ptr->alpha), 200);
    e_widget_framelist_object_append(today_f, s_a_tf);
    label = e_widget_label_add(evas, _("Red Outline:"));
    e_widget_framelist_object_append(today_f, label);
    s_r_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TTC_Ptr->red), 200);
    e_widget_framelist_object_append(today_f, s_r_tf_o);
    label = e_widget_label_add(evas, _("Blue Outline:"));
    e_widget_framelist_object_append(today_f, label);
    s_b_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TTC_Ptr->blue), 200);
    e_widget_framelist_object_append(today_f, s_b_tf_o);
    label = e_widget_label_add(evas, _("Green Outline:"));
    e_widget_framelist_object_append(today_f, label);
    s_g_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(TTC_Ptr->green), 200);
    e_widget_framelist_object_append(today_f, s_g_tf_o);
    label = e_widget_label_add(evas, _("Alpha Outline:"));
    e_widget_framelist_object_append(today_f, label);
    s_a_tf_o = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(TTC_Ptr->alpha), 200);
    e_widget_framelist_object_append(today_f, s_a_tf_o);
    label = e_widget_label_add(evas, _("Red Shadow:"));
    e_widget_framelist_object_append(today_f, label);
    s_r_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TTC_Ptr->red), 200);
    e_widget_framelist_object_append(today_f, s_r_tf_s);
    label = e_widget_label_add(evas, _("Blue Shadow:"));
    e_widget_framelist_object_append(today_f, label);
    s_b_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(TTC_Ptr->blue), 200);
    e_widget_framelist_object_append(today_f, s_b_tf_s);
    label = e_widget_label_add(evas, _("Green Shadow:"));
    e_widget_framelist_object_append(today_f, label);
    s_g_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(TTC_Ptr->green), 200);
    e_widget_framelist_object_append(today_f, s_g_tf_s);
    label = e_widget_label_add(evas, _("Alpha Shadow:"));
    e_widget_framelist_object_append(today_f, label);
    s_a_tf_s = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(TTC_Ptr->alpha), 200);
    e_widget_framelist_object_append(today_f, s_a_tf_s);
    e_widget_table_object_append(o, today_f, 1,1,1,1,0,0,0,0);
///////////////////////////////////////////////////////////////////////////////////
     //weekend colors box
    weekend = e_widget_framelist_add(evas, _("Weekend Background Colors"),0);
    label = e_widget_label_add(evas, _("Red:"));
    e_widget_framelist_object_append(weekend, label);
    slider_red = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeBC_Ptr->red), 200);
    e_widget_framelist_object_append(weekend, slider_red);
    label = e_widget_label_add(evas, _("Blue:"));
    e_widget_framelist_object_append(weekend, label);
    slider_blue = e_widget_slider_add(evas,1,0, _("%1.0f"),0,254,1,0,NULL, &(WeBC_Ptr->blue), 200);
    e_widget_framelist_object_append(weekend, slider_blue);
    label = e_widget_label_add(evas, _("Green:"));
    e_widget_framelist_object_append(weekend, label);
    slider_green = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeBC_Ptr->green), 200);
    e_widget_framelist_object_append(weekend, slider_green);
    label = e_widget_label_add(evas, _("Alpha:"));
    e_widget_framelist_object_append(weekend, label);
    slider_alpha = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeBC_Ptr->alpha), 200);
    e_widget_framelist_object_append(weekend, slider_alpha);
    e_widget_table_object_append(o, weekend, 2,0,1,1,0,0,0,0);

// weekdend Font colors
    weekend_f = e_widget_framelist_add(evas, _("Weekend Font Colors"),0);
    label = e_widget_label_add(evas, _("Red:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
    e_widget_framelist_object_append(weekend_f, s_r_tf);
    label = e_widget_label_add(evas, _("Blue:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
    e_widget_framelist_object_append(weekend_f, s_b_tf);
    label = e_widget_label_add(evas, _("Green:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
    e_widget_framelist_object_append(weekend_f, s_g_tf);
    label = e_widget_label_add(evas, _("Alpha:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_a_tf = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
    e_widget_framelist_object_append(weekend_f, s_a_tf);
    label = e_widget_label_add(evas, _("Red Outline:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_r_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
    e_widget_framelist_object_append(weekend_f, s_r_tf_o);
    label = e_widget_label_add(evas, _("Blue Outline:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_b_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
    e_widget_framelist_object_append(weekend_f, s_b_tf_o);
    label = e_widget_label_add(evas, _("Green Outline:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_g_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
    e_widget_framelist_object_append(weekend_f, s_g_tf_o);
    label = e_widget_label_add(evas, _("Alpha Outline:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_a_tf_o = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
    e_widget_framelist_object_append(weekend_f, s_a_tf_o);
    label = e_widget_label_add(evas, _("Red Shadow:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_r_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
    e_widget_framelist_object_append(weekend_f, s_r_tf_s);
    label = e_widget_label_add(evas, _("Blue Shadow:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_b_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
    e_widget_framelist_object_append(weekend_f, s_b_tf_s);
    label = e_widget_label_add(evas, _("Green Shadow:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_g_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
    e_widget_framelist_object_append(weekend_f, s_g_tf_s);
    label = e_widget_label_add(evas, _("Alpha Shadow:"));
    e_widget_framelist_object_append(weekend_f, label);
    s_a_tf_s = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
    e_widget_framelist_object_append(weekend_f, s_a_tf_s);
    e_widget_table_object_append(o, weekend_f, 2,1,1,1,0,0,0,0);

///////////////////////////////////////////////////////////////////////////////////
     //weekday colors box
    weekday = e_widget_framelist_add(evas, _("Weekend Background Colors"),0);
    label = e_widget_label_add(evas, _("Red:"));
    e_widget_framelist_object_append(weekday, label);
    slider_red = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeBC_Ptr->red), 200);
    e_widget_framelist_object_append(weekday, slider_red);
    label = e_widget_label_add(evas, _("Blue:"));
    e_widget_framelist_object_append(weekday, label);
    slider_blue = e_widget_slider_add(evas,1,0, _("%1.0f"),0,254,1,0,NULL, &(WeBC_Ptr->blue), 200);
    e_widget_framelist_object_append(weekday, slider_blue);
    label = e_widget_label_add(evas, _("Green:"));
    e_widget_framelist_object_append(weekday, label);
    slider_green = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeBC_Ptr->green), 200);
    e_widget_framelist_object_append(weekday, slider_green);
    label = e_widget_label_add(evas, _("Alpha:"));
    e_widget_framelist_object_append(weekday, label);
    slider_alpha = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeBC_Ptr->alpha), 200);
    e_widget_framelist_object_append(weekday, slider_alpha);
    e_widget_table_object_append(o, weekday, 3,0,1,1,0,0,0,0);

// weekdend Font colors
   weekday_f = e_widget_framelist_add(evas, _("Weekend Font Colors"),0);

    label = e_widget_label_add(evas, _("Red:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
   e_widget_framelist_object_append(weekday_f, s_r_tf);

    label = e_widget_label_add(evas, _("Blue:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
   e_widget_framelist_object_append(weekday_f, s_b_tf);

    label = e_widget_label_add(evas, _("Green:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
   e_widget_framelist_object_append(weekday_f, s_g_tf);

    label = e_widget_label_add(evas, _("Alpha:"));
  e_widget_framelist_object_append(weekday_f, label);
   s_a_tf = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
   e_widget_framelist_object_append(weekday_f, s_a_tf);

    label = e_widget_label_add(evas, _("Red Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_r_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
   e_widget_framelist_object_append(weekday_f, s_r_tf_o);

    label = e_widget_label_add(evas, _("Blue Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_b_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
   e_widget_framelist_object_append(weekday_f, s_b_tf_o);

    label = e_widget_label_add(evas, _("Green Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_g_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
   e_widget_framelist_object_append(weekday_f, s_g_tf_o);

    label = e_widget_label_add(evas, _("Alpha Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_a_tf_o = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
   e_widget_framelist_object_append(weekday_f, s_a_tf_o);

    label = e_widget_label_add(evas, _("Red Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_r_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
   e_widget_framelist_object_append(weekday_f, s_r_tf_s);

    label = e_widget_label_add(evas, _("Blue Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_b_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
   e_widget_framelist_object_append(weekday_f, s_b_tf_s);

    label = e_widget_label_add(evas, _("Green Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_g_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
   e_widget_framelist_object_append(weekday_f, s_g_tf_s);

    label = e_widget_label_add(evas, _("Alpha Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_a_tf_s = e_widget_slider_add(evas,1,0,_("%1.0f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
   e_widget_framelist_object_append(weekday_f, s_a_tf_s);
   e_widget_table_object_append(o, weekday_f, 3,1,1,1,0,0,0,0);

/////////////////////////////////////////////////////////////
// weekdend Font colors
/*   weekday_f = e_widget_framelist_add(evas, _("Weekend Font Colors"),0);

    label = e_widget_label_add(evas, _("Red:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_r_tf = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
   e_widget_framelist_object_append(weekday_f, s_r_tf);

    label = e_widget_label_add(evas, _("Blue:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_b_tf = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
   e_widget_framelist_object_append(weekday_f, s_b_tf);

    label = e_widget_label_add(evas, _("Green:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_g_tf = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
   e_widget_framelist_object_append(weekday_f, s_g_tf);

    label = e_widget_label_add(evas, _("Alpha:"));
  e_widget_framelist_object_append(weekday_f, label);
   s_a_tf = e_widget_slider_add(evas,1,0,_("%1.2f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
   e_widget_framelist_object_append(weekday_f, s_a_tf);

    label = e_widget_label_add(evas, _("Red Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_r_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
   e_widget_framelist_object_append(weekday_f, s_r_tf_o);

    label = e_widget_label_add(evas, _("Blue Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_b_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
   e_widget_framelist_object_append(weekday_f, s_b_tf_o);

    label = e_widget_label_add(evas, _("Green Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_g_tf_o = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
   e_widget_framelist_object_append(weekday_f, s_g_tf_o);

    label = e_widget_label_add(evas, _("Alpha Outline:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_a_tf_o = e_widget_slider_add(evas,1,0,_("%1.2f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
   e_widget_framelist_object_append(weekday_f, s_a_tf_o);

    label = e_widget_label_add(evas, _("Red Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_r_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL, &(WeTC_Ptr->red), 200);
   e_widget_framelist_object_append(weekday_f, s_r_tf_s);

    label = e_widget_label_add(evas, _("Blue Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_b_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL, &(WeTC_Ptr->blue), 200);
   e_widget_framelist_object_append(weekday_f, s_b_tf_s);

    label = e_widget_label_add(evas, _("Green Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_g_tf_s = e_widget_slider_add(evas, 1, 0, _("%1.2f"),0,254,1,0,NULL,&(WeTC_Ptr->green), 200);
   e_widget_framelist_object_append(weekday_f, s_g_tf_s);

    label = e_widget_label_add(evas, _("Alpha Shadow:"));
   e_widget_framelist_object_append(weekday_f, label);
   s_a_tf_s = e_widget_slider_add(evas,1,0,_("%1.2f"),0,254,1,0,NULL,&(WeTC_Ptr->alpha), 200);
   e_widget_framelist_object_append(weekday_f, s_a_tf_s);
   e_widget_table_object_append(o, weekday_f, 3,1,1,1,0,0,0,0);
*/
/*
      other_c = e_widget_framelist_add(evas, _("Other Colors"),0);
      YM_color = e_widget_button_add(evas,"Year/Month Color",NULL,color_edit_show,con,YNC_Ptr);
      Day_colors = e_widget_button_add(evas,"Day Color",NULL,color_edit_show,con,DTC_Ptr);
      e_widget_framelist_object_append(other_c, YM_color);
      e_widget_framelist_object_append(other_c, Day_colors);
      e_widget_table_object_append(o, other_c, 2,1,1,1,0,0,0,0);
*/
//   }

/*   load_edje = e_widget_framelist_add(evas, _("Load EDJEs"),0);
   back = e_widget_button_add(evas,"Background",NULL,file_select_bk_dialog,con,conf);
   top = e_widget_button_add(evas,"Top Image",NULL,file_select_ti_dialog,con,conf);
   //arrow = e_widget_button_add(evas,"Arrows",NULL,file_select_bk_dialog,con,NULL);
   //label = e_widget_button_add(evas,"Label",NULL,file_select_bk_dialog,con,NULL);
   //t_edje = e_widget_button_add(evas,"Today EDJE",NULL,file_select_bk_dialog,con,NULL);
   //we_edje = e_widget_button_add(evas,"Weekend EDJE",NULL,file_select_bk_dialog,con,NULL);
   //wd_edje = e_widget_button_add(evas,"Weekday EDJE",NULL,file_select_bk_dialog,con,NULL);
   e_widget_framelist_object_append(load_edje, back);
   e_widget_framelist_object_append(load_edje, top);
   //e_widget_framelist_object_append(load_edje, arrow);
   //e_widget_framelist_object_append(load_edje, label);
   //e_widget_framelist_object_append(load_edje, t_edje);
   //e_widget_framelist_object_append(load_edje, we_edje);
   //e_widget_framelist_object_append(load_edje, wd_edje);
   e_widget_table_object_append(o, load_edje, 2,2,1,1,0,0,0,0);
/*

//Add option to read config file
   	mi = e_menu_item_new(face->menu);
   	e_menu_item_label_set(mi, _("Read Conf Files"));
   	e_menu_item_callback_set(mi, read_conf_files, calendar); 


   return o;
}*/
/***************************************************
/ Function: 
/ Purpose:  Move data from dialog to config object 
/           after "advanced" apply button presesed.
/
/*****************************************************/
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
   int AllGood;
   CalFonts *YTC_Ptr = cfdata->calendar->conf->YM_text_class->data;     
   CalFonts *DTCs_Ptr = cfdata->calendar->conf->Day_text_class->data;      
   CalFonts *TC_Ptr  = cfdata->calendar->conf->text_class->data;      

   if ((calendar->conf->ImageYes != cfdata->temp_ImageYes) || (calendar->conf->UserCS !=
							                   cfdata->temp_UserCS))
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
 
   AllGood = sscanf(cfdata->size,"%d",&(YTC_Ptr->size));
   if (AllGood == 0) {return -1;}
   AllGood = sscanf(cfdata->size,"%d",&(DTCs_Ptr->size));
   if (AllGood == 0) {return -1;}
   AllGood = sscanf(cfdata->size,"%d",&(TC_Ptr->size));
   if (AllGood == 0) {return -1;}

   e_config_save_queue();
   redraw_calendar(calendar,SwitchImage);
   e_border_button_bindings_grab_all();

   return 1; /* Apply was OK */
}
/***************************************************
/ Function: 
/ Purpose:  create widgets for advanced dialog
/
/
/*****************************************************/
static Evas_Object *
_color_edit_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, void *data)
{
   /* generate the core widget layout for a basic dialog */
   Evas_Object *o, *of, *ob, *of2, *of3, *fs, *of1,*check, *today, *weekend, *weekday, *of_y, *o2;
   Evas_Object *fonts, *Day_font, *Date_font;
   Evas_Object *font_title;
   Evas_Object *slider_red, *slider_green, *slider_alpha, *slider_blue;
   Evas_Object *s_r_tf, *s_g_tf, *s_a_tf, *s_b_tf, *s_r_tf_o, *s_b_tf_o, *s_g_tf_o, *s_a_tf_o, *s_r_tf_s, *s_b_tf_s, *s_g_tf_s, *s_a_tf_s, *today_f, *weekend_f, *weekday_f ;
   Evas_Object *ym, *day_text, *start_table;
   Evas_Object *start_tableB, *ofB;
   Evas_Object *entry;
   Evas_Object *entry2;
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
   CalFonts *TC_Ptr  = cfdata->calendar->conf->text_class->data;      


   man = e_manager_current_get();
   if (!man) return;
   con = e_container_current_get(man);
   if (!con) return;
   if (con == NULL) return;

   o = e_widget_table_add(evas, 0);

   of2 = e_widget_framelist_add(evas, _("Top Image"), 1);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->ImageYes));
   ob = e_widget_radio_add(evas, _("With"), 0, rg);
   e_widget_framelist_object_append(of2, ob);
   ob = e_widget_radio_add(evas, _("Without"), 1, rg);
   e_widget_framelist_object_append(of2, ob);
   e_widget_table_object_append(o, of2, 0, 0,1,1,1,1,1,1);

   start_table = e_widget_table_add(evas, 0);
   of = e_widget_framelist_add(evas, _("First Day of Week"), 0);
   rg = e_widget_radio_group_new(&(cfdata->calendar->conf->DayofWeek_Start));
   ob = e_widget_radio_add(evas, _("Sunday"), 0, rg);
   e_widget_table_object_append(start_table, ob, 0, 0,1,1,0,0,0,0);
   ob = e_widget_radio_add(evas, _("Monday"), 1, rg);
   e_widget_table_object_append(start_table, ob, 0, 1,1,1,0,0,0,0);
   ob = e_widget_radio_add(evas, _("Tuesday"), 2, rg);
   e_widget_table_object_append(start_table, ob, 0, 2,1,1,0,0,0,0);
   ob = e_widget_radio_add(evas, _("Wednesday"), 3, rg);
   e_widget_table_object_append(start_table, ob, 0, 3,1,1,0,0,0,0);
   ob = e_widget_radio_add(evas, _("Thursday"), 4, rg);
   e_widget_table_object_append(start_table, ob, 1, 0,1,1,0,0,0,0);
   ob = e_widget_radio_add(evas, _("Friday"), 5, rg);
   e_widget_table_object_append(start_table, ob, 1, 1,1,1,0,0,0,0);
   ob = e_widget_radio_add(evas, _("Saturday"), 6, rg);
   e_widget_table_object_append(start_table, ob, 1,2,1,1,0,0,0,0);
   e_widget_framelist_object_append(of, start_table);
   e_widget_table_object_append(o, of, 0, 1,1,1,1,1,1,1);


   if (cfdata->calendar->conf->UserCS)
   {

     /*************  Other Fonts  *************/
    font_title = e_widget_framelist_add(evas, _("Fonts"),0);
      of_y = e_widget_table_add(evas, 1);
      //label on top
      e_widget_table_object_append(of_y, e_widget_label_add(evas, "Font"),
                                     1, 0, 1, 1, 1, 1, 1, 1);
      e_widget_table_object_append(of_y, e_widget_label_add(evas, "Size"),
                                     2, 0, 1, 1,1, 1, 1, 1);
     //Font row
      e_widget_table_object_append(of_y, e_widget_label_add(evas, "Year/Mon"),
                                     0, 1, 1, 1, 1, 1, 1, 1);
     {
        entry = e_widget_entry_add(evas, &YTC_Ptr->font);
        e_widget_min_size_set(entry, 100, 1);
        e_widget_table_object_append(of_y, entry, 1,1,1,1,1,1,1,1);
     } 
     e_widget_table_object_append(of_y, e_widget_slider_add(evas, 1, 0, _("%1.2f"),4,48,1,0,NULL, &(YTC_Ptr->size), 60), 2, 1, 1, 1, 1, 1, 1, 1);

      //label for 2nd set
      e_widget_table_object_append(of_y, e_widget_label_add(evas, "Day"),
                                     0, 2, 1, 1, 1, 1, 1, 1);
     //Font row
     e_widget_table_object_append(of_y, e_widget_entry_add(evas, &DTCs_Ptr->font),
                                     1, 2, 1, 1, 1, 1, 1, 1);

     e_widget_table_object_append(of_y, e_widget_slider_add(evas, 1, 0, _("%1.2f"),4,48,1,0,NULL, &(DTCs_Ptr->size), 60), 2, 2, 1, 1, 1, 1, 1, 1);
      //label for 3rd set
      e_widget_table_object_append(of_y, e_widget_label_add(evas, "Date"),
                                     0, 3, 1, 1, 1, 1, 1, 1);
      //Font row
      e_widget_table_object_append(of_y, e_widget_entry_add(evas, &TC_Ptr->font),
                                     1, 3, 1, 1, 1, 1, 1, 1);
      e_widget_table_object_append(of_y, e_widget_slider_add(evas, 1, 0, _("%1.2f"),4,48,1,0,NULL, &(TC_Ptr->size), 60), 2, 3, 1, 1, 1, 1, 1, 1);

      e_widget_framelist_object_append(font_title, of_y);
      e_widget_table_object_append(o, font_title, 1,0,1,1,0,0,0,0);


   	start_tableB = e_widget_table_add(evas, 0);
   	ofB = e_widget_framelist_add(evas, _("Configure Colors"), 0);
	e_widget_table_object_append(start_tableB,e_widget_button_add(evas,"Today",NULL,e_int_config_calendar_today,con,calendar),0,0,1,1,0,0,0,0);
	e_widget_table_object_append(start_tableB,e_widget_button_add(evas,"Other",NULL,e_int_config_calendar_other,con,calendar),0,1,1,1,0,0,0,0);
    	e_widget_table_object_append(start_tableB,e_widget_button_add(evas,"Weekend",NULL,e_int_config_calendar_weekend,con,calendar),1,0,1,1,0,0,0,0);
    	e_widget_table_object_append(start_tableB,e_widget_button_add(evas,"Weekday",NULL,e_int_config_calendar_weekday,con,calendar),1,1,1,1,0,0,0,0);
   	e_widget_framelist_object_append(ofB, start_tableB);
   	e_widget_table_object_append(o, ofB, 1, 1,1,1,1,1,1,1);
   }

//Add option to read config file
/*  	mi = e_menu_item_new(face->menu);
   	e_menu_item_label_set(mi, _("Read Conf Files"));
   	e_menu_item_callback_set(mi, read_conf_files, calendar); 

*/
   return o;
}