#include <string.h>
#include <stdio.h>
#include "e.h"
#include "e_mod_main.h"

/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
file_select_bk_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
Config *config;

   
config = data;
   
E_File_Dialog * dia;
   

dia = e_file_dialog_new(m->zone->container);
   
if (!dia)
      return;
   
e_file_dialog_title_set(dia, "Select Background .edj File");
   
e_file_dialog_select_callback_add(dia, e_file_bk_select_cb, config);
   
e_file_dialog_show(dia);


}

/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
e_file_bk_select_cb(E_File_Dialog * dia, char *file, void *data) 
{
   
char *ext;

   
Config *config;
   
Evas_Object *o;

   

ext = strrchr(file, '.');
   
if (!ext)
      return;
   
if (strcasecmp(ext, ".edj") != 0)
      
     {
        
e_error_dialog_show 
 ("error!", "This isn't an .edj file!");
        
return;
     
}
   

int check = edje_object_file_set(o, PACKAGE_LIB_DIR 
                                      "/calendar/calendar.edj",
                                      
"back");

   
if (check == 0)
      
     {
        
e_error_dialog_show 
           ("error", "This .edj doesn't meet the requirements \
            for use with calendar -- please see the README \
	     for more info.");
        
return;
     
}
   

config->Background_path = (char *)evas_stringshare_add(file);
   
e_object_del(E_OBJECT(dia));

} 

/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
file_select_ti_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
Config *config;

   
config = data;
   
E_File_Dialog * dia;
   

dia = e_file_dialog_new(m->zone->container);
   
if (!dia)
      return;
   
e_file_dialog_title_set(dia, "Select TopImage .edj File");
   
e_file_dialog_select_callback_add(dia, e_file_ti_select_cb, config);
   
e_file_dialog_show(dia);


}

/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
e_file_ti_select_cb(E_File_Dialog * dia, char *file, void *data) 
{
   
char *ext;

   
Config *config;
   
Evas_Object *o;

   

ext = strrchr(file, '.');
   
if (!ext)
      return;
   
if (strcasecmp(ext, ".edj") != 0)
      
     {
        
e_error_dialog_show 
 ("error!", "This isn't an .edj file!");
        
return;
     
}
   

int check = edje_object_file_set(o, PACKAGE_LIB_DIR 
                                      "/calendar/calendar.edj",
                                      
"Jan");

   
if (check == 0)
      
     {
        
e_error_dialog_show 
           ("error!", "This .edj doesn't meet the requirements \
            for use with calendar -- please see the README \
	     for more info.");
        
return;
     
}
   

config->TopImage_path = (char *)evas_stringshare_add(file);
   
e_object_del(E_OBJECT(dia));

} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
file_select_cf_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
Config *config;

   
config = data;
   
E_File_Dialog * dia;
   

dia = e_file_dialog_new(m->zone->container);
   
if (!dia)
      return;
   
e_file_dialog_title_set(dia, "Select Config File");
   
e_file_dialog_select_callback_add(dia, e_file_cf_select_cb, config);
   
e_file_dialog_show(dia);

}

/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
e_file_cf_select_cb(E_File_Dialog * dia, char *file, void *data) 
{
   
Config *config;

   

config->ConfigFile_path = (char *)evas_stringshare_add(file);
   
e_object_del(E_OBJECT(dia));

} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
config_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
Calendar * calendar;
   
calendar = data;
   
e_int_config_calendar(m->zone->container, calendar);

} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
add_event_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
day_face * DayToFix;
   
DayToFix = data;
   
add_event_show(m->zone->container, DayToFix);

} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
add_todo_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
day_face * DayToFix;
   
DayToFix = data;
   
      //add_todo_show(m->zone->container,DayToFix); 
} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
remove_event_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
day_face * DayToFix;
   
DayToFix = data;
   
      //font_edit_show(m->zone->container,FontToFix); 
} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
remove_todo_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
day_face * DayToFix;
   
DayToFix = data;
   
      //font_edit_show(m->zone->container,FontToFix); 
} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
view_event_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
day_face * DayToFix;
   
DayToFix = data;
   
      //font_edit_show(m->zone->container,FontToFix); 
} 
/***************************************************
/ Function: 
/ Purpose:  
/
*****************************************************/ 
   void
view_todo_dialog(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   
day_face * DayToFix;
   
DayToFix = data;
   
      //font_edit_show(m->zone->container,FontToFix); 
} 
