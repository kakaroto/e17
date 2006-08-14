#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   /*- BASIC -*/ 
   char event[50][256];
   char todo[50][256];
   char holiday[10][256];
   
   /*- common -*/ 
   day_face *DayToFix;
};

/* local subsystem functions */ 
static void _add_event_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static void *_add_event_create_data(E_Config_Dialog *cfd);
static void _add_event_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static int _add_event_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static Evas_Object *_add_event_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data);


/* externally accessible functions */ 
/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
*****************************************************/ 
void 
add_event_show(void *con, void *DayToFix) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   v = E_NEW(E_Config_Dialog_View, 1);

   if (v)
     {
        /* methods */ 
        v->create_cfdata = _add_event_create_data;
        v->free_cfdata = _add_event_free_data;
        v->basic.apply_cfdata = _add_event_basic_apply_data;
        v->basic.create_widgets = _add_event_basic_create_widgets;

        /* create config diaolg */ 
        cfd = e_config_dialog_new(con, D_("Font Editor"), "Calendar", "_e_modules_calendar_add_event_dialog", NULL, 0, v, DayToFix);
     }
}

/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
****************************************************/ 
static void *
_add_event_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   cfdata->DayToFix = E_NEW(day_face, 1);

   if (!cfdata)
      return NULL;
   cfdata->DayToFix = cfd->data;

   // _add_event_fill_data(cfdata);
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
_add_event_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data) 
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
static int 
_add_event_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data) 
{
   e_config_save_queue();
   
   //redraw_calendar(f_cfdata->FontsToModify_L->calendar,0);
   return 1;
}

/***************************************************
/ Function: 
/ Purpose:  
/ Returns: nothing
/ Takes: 
/
*****************************************************/ 
static Evas_Object *
_add_event_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data) 
{
   /* generate the core widget layout for a basic dialog */ 
   Evas_Object *o, *of, *ob, *of1;
   E_Radio_Group *rg;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Event Type"), 0);
   rg = e_widget_radio_group_new(&(data->DayToFix->eventtype));
   ob = e_widget_radio_add(evas, D_("Birthday"), 1, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Anniversary"), 2, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Other"), 3, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   of1 = e_widget_framelist_add(evas, D_("Event"), 0);
   Evas_Object *entry;

   entry = e_widget_entry_add(evas, &data->event[0]);
   e_widget_min_size_set(entry, 100, 1);
   e_widget_framelist_object_append(of1, entry);
   e_widget_list_object_append(o, of1, 1, 1, 0.5);
   return o;
}
