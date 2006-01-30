#include <time.h>
#include "e_mod_main.h"
#include <string.h>
#include <stdio.h>
#include <libintl.h>

/* module private routines */ 
static Calendar *_calendar_new();

static void _calendar_shutdown(Calendar * calendar);

static int _date_cb_check(void *calendar);

static void _clear_dates(Calendar_Face * face);

static int _calendar_count;

static E_Config_DD *conf_edd;

static E_Config_DD *conf_font_edd;

static E_Config_DD *conf_color_edd;



EAPI E_Module_Api e_modapi = 
{
   
E_MODULE_API_VERSION, 
"Calendar" 
};

/**  Public function ***/ 
   int
increment_cal_count() 
{
   
_calendar_count++;
   
return _calendar_count;

}

/************ public module routines. all modules must have these ***************/ 
/***************************************************
/ Function: 
/ Purpose:  
/ Takes: 
/
/
******************************************************/ 
EAPI void *
e_modapi_init(E_Module *module) 
{
   
Calendar * calendar;
   
      /* check module api version */ 
      if (module->api->version < E_MODULE_API_VERSION)
      
     {
        
e_error_dialog_show 
           ("Module API Error", 
"Error initializing Module: calendar\n" 
            "It requires a minimum module API version of: %i.\n" 
            "The module API advertized by Enlightenment is: %i.\n" 
            "Aborting module.", 
E_MODULE_API_VERSION, module->api->version);
        
return NULL;
     
}
   
      /* actually init buttons */ 
      calendar = _calendar_new();
   
module->config_menu = calendar->config_menu;
   
return calendar;

}

/***************************************************
/ Function: 
/ Purpose:  
/
******************************************************/ 
/*    * int e_modapi_shutdown(E_Module*) - Cleanup */ 
EAPI int 
e_modapi_shutdown(E_Module *module) 
{
   
Calendar * calendar;
   

if (module->config_menu)
      
     {
        
e_menu_deactivate(module->config_menu);
        
e_object_del(E_OBJECT(module->config_menu));
        
module->config_menu = NULL;
     
}
   

calendar = module->data;
   
if (calendar)
      
_calendar_shutdown(calendar);
   

return 1;

}

/***************************************************
/ Function: 
/ Purpose:  
/
******************************************************/ 
/*    * int e_modapi_save(E_Module*) - Save persistent data */ 
/*    ie: the calendar->conf stuff                          */ 
EAPI int 
e_modapi_save(E_Module *module) 
{
   
Calendar * calendar;
   

calendar = module->data;
   
e_config_domain_save("module.calendar", conf_edd, calendar->conf);
   
return 1;

}

/***************************************************
/ Function: 
/ Purpose:  
/
******************************************************/ 
/* * int e_modapi_info(E_Module*) - Setup module specific infomation */ 
EAPI int 
e_modapi_info(E_Module *module) 
{
   
module->icon_file = strdup(PACKAGE_DATA_DIR "/module_icon.png");
   
return 1;

}

/***************************************************
/ Function: 
/ Purpose:  
/
/
******************************************************/ 
/* * int e_modapi_about(E_Module*). - Called when Modules' About Menu is invoked. */ 
EAPI int 
e_modapi_about(E_Module *module) 
{
   
e_error_dialog_show(_("Enlightenment calendar Module"),
                        
_("A simple module to give E17 a desktop calendar "));
   
return 1;

}

/***************************************************
/ Function: 
/ Purpose:  
/
/
******************************************************/ 
   int
e_modapi_config(E_Module *m) 
{
   
Calendar * calendar;
   

calendar = m->data;
   
if (calendar)
      e_int_config_calendar(calendar->con, calendar);
   
return 1;

}


/************************ End of the required routines ******************************************/ 
   
/* module private routines */ 
/***************************************************
/ Function: _calendar_new()
/ Purpose:  create new calendar
/
/
******************************************************/ 
static Calendar *
_calendar_new() 
{
   
Calendar * calendar;
   
time_t now;
   
struct tm date;

   
time(&now);
   
date = *localtime(&now);
   

_calendar_count = 0;
   
calendar = E_NEW(Calendar, 1);
   
if (!calendar)
      return NULL;
   

conf_font_edd = E_CONFIG_DD_NEW("CalFonts", CalFonts);
   
#undef T
#undef D
#define T CalFonts
#define D conf_font_edd
      E_CONFIG_VAL(D, T, font, STR);
   
E_CONFIG_VAL(D, T, size, INT);
   

conf_color_edd = E_CONFIG_DD_NEW("c_array", c_array);
   
#undef T
#undef D
#define T c_array
#define D conf_color_edd
      E_CONFIG_VAL(D, T, red, INT);
   
E_CONFIG_VAL(D, T, green, INT);
   
E_CONFIG_VAL(D, T, blue, INT);
   
E_CONFIG_VAL(D, T, alpha, INT);
   
E_CONFIG_VAL(D, T, red_o, INT);
   
E_CONFIG_VAL(D, T, green_o, INT);
   
E_CONFIG_VAL(D, T, blue_o, INT);
   
E_CONFIG_VAL(D, T, alpha_o, INT);
   
E_CONFIG_VAL(D, T, red_s, INT);
   
E_CONFIG_VAL(D, T, green_s, INT);
   
E_CONFIG_VAL(D, T, blue_s, INT);
   
E_CONFIG_VAL(D, T, alpha_s, INT);
   

conf_edd = E_CONFIG_DD_NEW("calendar_Config", Config);
   
#undef T
#undef D
#define T Config
#define D conf_edd
      E_CONFIG_VAL(D, T, ImageYes, INT);
   
E_CONFIG_VAL(D, T, config_version, INT);
   
E_CONFIG_VAL(D, T, DayofWeek_Start, INT);
   
E_CONFIG_VAL(D, T, TopImage_path, STR);
   
E_CONFIG_VAL(D, T, Background_path, STR);
   
E_CONFIG_VAL(D, T, ConfigFile_path, STR);
   
E_CONFIG_VAL(D, T, label_path, STR);
   
E_CONFIG_VAL(D, T, today_path, STR);
   
E_CONFIG_VAL(D, T, weekend_path, STR);
   
E_CONFIG_VAL(D, T, weekday_path, STR);
   
E_CONFIG_VAL(D, T, arrow_path, STR);
   
E_CONFIG_VAL(D, T, UserCS, INT);
   
//Store colors
      E_CONFIG_LIST(D, T, Today_s_text_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, Today_s_back_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, Weekend_text_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, WeekDay_text_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, Weekend_back_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, WeekDay_back_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, YearMon_numb_colors, conf_color_edd);
   
E_CONFIG_LIST(D, T, DayWeek_text_colors, conf_color_edd);
   
//Store fonts
      E_CONFIG_LIST(D, T, YM_text_class, conf_font_edd);
   
E_CONFIG_LIST(D, T, Day_text_class, conf_font_edd);
   
E_CONFIG_LIST(D, T, text_class, conf_font_edd);
   

calendar->conf = e_config_domain_load("module.calendar", conf_edd);
   
if (!calendar->conf)
      
     {
        
c_array * Temp_Ptr, *Temp_Ptr1, *Temp_Ptr2, *Temp_Ptr3, *Temp_Ptr4;
        
c_array * Temp_Ptr5, *Temp_Ptr6, *Temp_Ptr7;
        
Temp_Ptr = E_NEW(c_array, 1);
        
Temp_Ptr->red = -1;
        
Temp_Ptr1 = E_NEW(c_array, 1);
        
Temp_Ptr1->red = -1;
        
Temp_Ptr2 = E_NEW(c_array, 1);
        
Temp_Ptr2->red = -1;
        
Temp_Ptr3 = E_NEW(c_array, 1);
        
Temp_Ptr3->red = -1;
        
Temp_Ptr4 = E_NEW(c_array, 1);
        
Temp_Ptr4->red = -1;
        
Temp_Ptr5 = E_NEW(c_array, 1);
        
Temp_Ptr5->red = -1;
        
Temp_Ptr6 = E_NEW(c_array, 1);
        
Temp_Ptr6->red = -1;
        
Temp_Ptr7 = E_NEW(c_array, 1);
        
Temp_Ptr7->red = -1;
        

CalFonts * Temp_ClassPtr;
        
Temp_ClassPtr = E_NEW(CalFonts, 1);
        
Temp_ClassPtr->font = "";
        

CalFonts * Temp_ClassPtr1;
        
Temp_ClassPtr1 = E_NEW(CalFonts, 1);
        
Temp_ClassPtr1->font = "";
        

CalFonts * Temp_ClassPtr2;
        
Temp_ClassPtr2 = E_NEW(CalFonts, 1);
        
Temp_ClassPtr2->font = "";
        


calendar->conf = E_NEW(Config, 1);

        
calendar->conf->ImageYes = 0;
        
calendar->conf->TopImage_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/topimage.edj");
        
calendar->conf->Background_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/backimage.edj");
        
calendar->conf->today_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/today.edj");
        
calendar->conf->weekend_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/weekend.edj");
        
calendar->conf->weekday_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/weekday.edj");
        
calendar->conf->label_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/label.edj");
        
calendar->conf->arrow_path =
           (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/arrow.edj");
        
calendar->conf->ConfigFile_path = "";
        

calendar->conf->Today_s_text_colors =
           evas_list_append(calendar->conf->Today_s_text_colors, Temp_Ptr);
        
calendar->conf->Today_s_back_colors =
           evas_list_append(calendar->conf->Today_s_back_colors, Temp_Ptr1);
        
calendar->conf->Weekend_text_colors =
           evas_list_append(calendar->conf->Weekend_text_colors, Temp_Ptr2);
        
calendar->conf->Weekend_back_colors =
           evas_list_append(calendar->conf->Weekend_back_colors, Temp_Ptr3);
        
calendar->conf->WeekDay_text_colors =
           evas_list_append(calendar->conf->WeekDay_text_colors, Temp_Ptr4);
        
calendar->conf->WeekDay_back_colors =
           evas_list_append(calendar->conf->WeekDay_back_colors, Temp_Ptr5);
        
calendar->conf->YearMon_numb_colors =
           evas_list_append(calendar->conf->YearMon_numb_colors, Temp_Ptr6);
        
calendar->conf->DayWeek_text_colors =
           evas_list_append(calendar->conf->DayWeek_text_colors, Temp_Ptr7);
        

calendar->conf->YM_text_class =
           evas_list_append(calendar->conf->YM_text_class, Temp_ClassPtr);
        
calendar->conf->Day_text_class =
           evas_list_append(calendar->conf->Day_text_class, Temp_ClassPtr1);
        
calendar->conf->text_class =
           evas_list_append(calendar->conf->text_class, Temp_ClassPtr2);
        

calendar->conf->DayofWeek_Start = 0;
        
calendar->conf->UserCS = FALSE;
        

calendar_face_set_colors(calendar);
        
calendar_face_set_text(calendar);
     
}
   
   else
      
     {
        
if (strncmp(calendar->conf->TopImage_path, " ", 2) == 0)
           
          {
             
calendar->conf->TopImage_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/topimage.edj");
             
calendar->conf->Background_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/backimage.edj");
             
calendar->conf->today_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/today.edj");
             
calendar->conf->weekend_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/weekend.edj");
             
calendar->conf->weekday_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/weekday.edj");
             
calendar->conf->label_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/label.edj");
             
calendar->conf->arrow_path =
                (char *)evas_stringshare_add(PACKAGE_DATA_DIR "/arrow.edj");
          
}
     

} 
calendar->conf->view_month = date.tm_mon;
   
calendar->conf->view_year = date.tm_year + 1900;
   
calendar->conf->Today = date.tm_mday;
   
//Start up Face
      calendar_face_start(calendar);
   
//Add Timer
      calendar->date_check_timer =
      ecore_timer_add(60, _date_cb_check, calendar);
   
return calendar;

}

/***************************************************
/ Function: _calendar_shutdown(Calendar *calendar)
/ Purpose:  shutdown calendar module
/
/
******************************************************/ 
static void 
_calendar_shutdown(Calendar * calendar) 
{
   
Evas_List *list;

   

E_CONFIG_DD_FREE(conf_edd);
   
E_CONFIG_DD_FREE(conf_font_edd);
   
E_CONFIG_DD_FREE(conf_color_edd);
   
ecore_timer_del(calendar->date_check_timer);
   

for (list = calendar->faces; list; list = list->next)
      
     {
        
calendar_face_free(list->data);
        
_calendar_count--;
     
}
   
evas_list_free(calendar->faces);
   
e_object_del(E_OBJECT(calendar->config_menu));
   

free_Calfonts(calendar);
   

      // need to free color list
      evas_list_free(calendar->conf->Today_s_text_colors);
   
evas_list_free(calendar->conf->Today_s_back_colors);
   
evas_list_free(calendar->conf->Weekend_text_colors);
   
evas_list_free(calendar->conf->Weekend_back_colors);
   
evas_list_free(calendar->conf->WeekDay_text_colors);
   
evas_list_free(calendar->conf->WeekDay_back_colors);
   
evas_list_free(calendar->conf->YearMon_numb_colors);
   
evas_list_free(calendar->conf->DayWeek_text_colors);
   


evas_stringshare_del(calendar->conf->TopImage_path);
   
evas_stringshare_del(calendar->conf->Background_path);
   
evas_stringshare_del(calendar->conf->today_path);
   
evas_stringshare_del(calendar->conf->weekend_path);
   
evas_stringshare_del(calendar->conf->weekday_path);
   
evas_stringshare_del(calendar->conf->label_path);
   



free(calendar->conf);
   
free(calendar);

}

/***************************************************
/ Function: _calendar_config_menu_new(Calendar *calendar)
/ Purpose:  create new calender menu
/
/
******************************************************/ 
   void
calendar_config_menu_new(Calendar * calendar) 
{
   
calendar->config_menu = e_menu_new();

} 
/***************************************************
/ Function: 
/ Purpose:  
/
/
******************************************************/ 
static int 
_date_cb_check(void *data) 
{
   
time_t now;
   
Calendar * calendar;
   
struct tm date;

   

time(&now);
   
date = *localtime(&now);
   

calendar = data;
   

if ((calendar->conf->Today == date.tm_mday)
         && (calendar->conf->view_month == date.tm_mon)
         && 
(calendar->conf->view_year == date.tm_year))
      
return 1;
   
   else
      
     {
        
Evas_List *l;

        
for (l = calendar->faces; l; l = l->next)
           
          {
             
Calendar_Face * face;
             
face = l->data;
             
fill_in_caltable(calendar->conf->DayofWeek_Start, face, 1);
             
_clear_dates(face);
             
if (calendar->conf->UserCS)
                
               {
                  
update_colors(calendar, face);
               
}
             
int skew = calculate_skew(calendar);

             
calendar_add_dates(face, calendar, skew);
             
set_day_label(face->label_object, calendar->conf->DayofWeek_Start,
                            date.tm_mon, 
date.tm_year);
     
} 
} 
return 1;

}


/***************************************************
/ Function: 
/ Purpose:  
/
/
******************************************************/ 
static void 
_clear_dates(Calendar_Face * face) 
{
   
int x;

   

for (x = 0; x < 35; x++)
      
edje_object_part_text_set(face->weekday_object[x]->obj, "date-text",
                                 " ");
   
for (x = 0; x < 35; x++)
      
edje_object_part_text_set(face->weekend_object[x]->obj, "date-text",
                                 " ");
   
for (x = 0; x < 35; x++)
      
edje_object_part_text_set(face->today_object[x]->obj, "date-text", " ");

}


/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
/
/
******************************************************/ 
   void
read_conf_files(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
Calendar * calendar;
   
calendar = data;
   
calendar_face_set_colors(calendar);
   
calendar_face_set_text(calendar);
   
redraw_calendar(calendar, 0);

} 
/***************************************************
/ Function: 
/ Purpose:  
/
/
******************************************************/ 
void 
redraw_calendar(Calendar * calendar, int SwitchImage) 
{
   
Evas_List *l;

   
Calendar_Face * face;
   
int skew;

   
time_t now;
   
struct tm date;

   
time(&now);
   
date = *localtime(&now);
   

if (SwitchImage == 1)      //Ok, so I need to redraw the whole calendar
     {
        
for (l = calendar->faces; l; l = l->next)
           
          {
             
face = l->data;
             
calendar_face_free(face);
             
calendar->faces = evas_list_remove(l, face);
          
}
        
calendar_face_start(calendar);
     
}
   
   else
      
     {
        
for (l = calendar->faces; l; l = l->next)
           
          {
             
face = l->data;
             
fill_in_caltable(calendar->conf->DayofWeek_Start, face, 1);
             
_clear_dates(face);
             
skew = calculate_skew(calendar);
             
calendar_add_dates(face, calendar, skew);
             
set_day_label(face->label_object, calendar->conf->DayofWeek_Start,
                            calendar->conf->view_month,
                            calendar->conf->view_year);
             
if (calendar->conf->UserCS)
                
               {
                  
calendar_update_text_fonts(calendar, face);
                  
update_colors(calendar, face);
               
}
          
}
     
}

}


