#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H
   
#include <e.h>
#include <Ecore.h>
#include "add_event_dialog.h"
#include "add_todo_dialog.h"
   


#define CALENDAR_CONFIG_VERSION 1000
   
typedef struct _Config Config;


typedef struct _calendar Calendar;


typedef struct _calendar_Face Calendar_Face;


typedef struct _calfonts CalFonts;


typedef struct _ColorArray c_array;


typedef struct _day_face weekday_face;


typedef struct _day_face weekend_face;


typedef struct _day_face today_face;


typedef struct _day_face day_face;



struct _calfonts 
{
   

int size;
   

char *font;
   

Calendar *calendar;


};


struct _ColorArray 
{
   

Calendar *calendar;
   

int red, red_o, red_s;
   

int green, green_o, green_s;
   

int blue, blue_o, blue_s;
   

int alpha, alpha_o, alpha_s;


};




/* color list -> red, green, blue, alpha, outline red, green, blue, alpha, shadow red, green, blue, alpha */ 

struct _Config 
{
   

Evas_List *faces;
   

int ImageYes;
   

int Today;
   

int config_version;
   

Evas_List *Today_s_text_colors;
   

Evas_List *Today_s_back_colors;
   

Evas_List *Weekend_text_colors;
   

Evas_List *Weekend_back_colors;
   

Evas_List *WeekDay_text_colors;
   

Evas_List *WeekDay_back_colors;
   

Evas_List *YearMon_numb_colors;
   

Evas_List *DayWeek_text_colors;
   

Evas_List *YM_text_class;
   

Evas_List *Day_text_class;
   

Evas_List *text_class;
   

int DayofWeek_Start;
   

char *TopImage_path;
   

char *Background_path;
   

char *ConfigFile_path;
   

char *weekend_path;
   

char *today_path;
   

char *weekday_path;
   

char *label_path;
   

char *arrow_path;
   

int UserCS;
   

int view_month;
   

int view_year;


};



struct _calendar 
{
   

Evas_List *faces;
   

E_Menu *config_menu;
   

Ecore_Timer *date_check_timer;
   

Config *conf;
   

E_Container *con;


};



struct _calendar_Face 
{
   

E_Container *con;
   

E_Menu *menu;
   

E_Menu *menu1;             //for todo list & personnal events
   E_Menu *sub;
   

E_Menu *sub2;
   

E_Menu *sub3;
   

Evas_Object *calendar_object;
   

E_Gadman_Client *gmc;
   

Evas_Object *topimage_object;
   

Evas_Object *background_object;
   

Evas_Object *table_object;
   

Evas_Object *o_calendar_table;
   

weekday_face *weekday_object[35];
   

weekend_face *weekend_object[35];
   

today_face *today_object[35];
   

Evas_Object *label_object;
   

Evas_Object *next_month;
   

Evas_Object *prev_month;
   

Evas_Object *next_year;
   

Evas_Object *prev_year;


};



struct _day_face 
{
   

Evas_Object *obj;
   

E_Menu *menu;
   

E_Menu *menu1;
   

char events[50][256];
   

char todo[50][256];
   

char holiday[10][256];
   

int eventtype;
   

int holidaytype;


};




EAPI extern E_Module_Api e_modapi;



EAPI void *e_modapi_init(E_Module *m);


EAPI int e_modapi_shutdown(E_Module *m);


EAPI int e_modapi_save(E_Module *m);


EAPI int e_modapi_about(E_Module *m);



void redraw_calendar(Calendar *calendar, int SwitchImage);


void read_conf_files(void *data, E_Menu *m, E_Menu_Item *mi);


void calendar_config_menu_new(Calendar *calendar);


int increment_cal_count();





#include "calendar_color_func.h"
#include "calendar_text_fun.h"
#include "dialog_func.h"
#include "calendar_func.h"
#include "cal_face_func.h"
#include "today_editor.h"
#include "weekday_editor.h"
#include "weekend_editor.h"
#include "otherfonts_editor.h"
#include "main_editor.h"
   
#endif  /* 
 */
