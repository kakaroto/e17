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
calendar_face_start(Calendar * calendar)
{
   Evas_List          *managers, *l2, *l;
   E_Menu_Item        *mi;

   calendar_config_menu_new(calendar);
   managers = e_manager_list();
   for (l = managers; l; l = l->next)
     {
        E_Manager          *man;

        man = l->data;
        for (l2 = man->containers; l2; l2 = l2->next)
          {
             E_Container        *con;
             Calendar_Face      *face;

             con = l2->data;
             face = calendar_face_new(con, calendar);
             if (face)
               {
                  calendar->con = con;
                  calendar->faces = evas_list_append(calendar->faces, face);
                  mi = e_menu_item_new(calendar->config_menu);
                  e_menu_item_label_set(mi, con->name);
                  e_menu_item_submenu_set(mi, face->menu);

               }
          }
     }

}

/***************************************************
/ Function: Start up the face
/ Purpose: 
/
*****************************************************/
Calendar_Face      *
calendar_face_new(E_Container * con, Calendar * calendar)
{
   // Setup date finding stuff
   time_t              now;
   struct tm           date;

   time(&now);
   date = *localtime(&now);
   int                 check, i;

   Calendar_Face      *face;

   face = E_NEW(Calendar_Face, 1);
   if (!face)
      return NULL;
   face->con = con;
   e_object_ref(E_OBJECT(con));
   calendar_face_menu_new(face, calendar);
   evas_event_freeze(con->bg_evas);

   // Make calendar object
   face->calendar_object = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->calendar_object,
                            PACKAGE_DATA_DIR "/calendar.edj", "default");
   evas_object_show(face->calendar_object);

   //Make prev_year object
   face->prev_year = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->prev_year, calendar->conf->arrow_path, "PY");
   evas_object_event_callback_add(face->prev_year, EVAS_CALLBACK_MOUSE_DOWN,
                                  calendar_face_prev_year, calendar);

   //Make next_year object
   face->next_year = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->next_year, calendar->conf->arrow_path, "NY");
   evas_object_event_callback_add(face->next_year, EVAS_CALLBACK_MOUSE_DOWN,
                                  calendar_face_next_year, calendar);

   //Make prev_month object
   face->prev_month = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->prev_month, calendar->conf->arrow_path, "PM");
   evas_object_event_callback_add(face->prev_month, EVAS_CALLBACK_MOUSE_DOWN,
                                  calendar_face_prev_month, calendar);

   //Make next_month object
   face->next_month = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->next_month, calendar->conf->arrow_path, "NM");
   evas_object_event_callback_add(face->next_month, EVAS_CALLBACK_MOUSE_DOWN,
                                  calendar_face_next_month, calendar);

   //swallow and show arrows on calendar object
   edje_object_part_swallow(face->calendar_object, "PM", face->prev_month);
   edje_object_part_swallow(face->calendar_object, "NM", face->next_month);
   edje_object_part_swallow(face->calendar_object, "PY", face->prev_year);
   edje_object_part_swallow(face->calendar_object, "NY", face->next_year);
   evas_object_show(face->prev_month);
   evas_object_show(face->next_month);
   evas_object_show(face->prev_year);
   evas_object_show(face->next_year);

   //create first table that will be swallowed into edje
   face->table_object = e_table_add(con->bg_evas);
   e_table_homogenous_set(face->table_object, 1);       // 1 = homo, 0 = nonhomo
   edje_object_part_swallow(face->calendar_object, "swallow",
                            face->table_object);
   evas_object_show(face->table_object);

   //Make topimage object
   face->topimage_object = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->topimage_object,
                            calendar->conf->TopImage_path, "default");
   //Make background object
   face->background_object = edje_object_add(con->bg_evas);
   check =
       edje_object_file_set(face->background_object,
                            calendar->conf->Background_path, "back");

   //Make label object
   face->label_object = edje_object_add(con->bg_evas);
   if (!calendar->conf->UserCS)
      check =
          edje_object_file_set(face->label_object, calendar->conf->label_path,
                               "default");
   else
      check =
          edje_object_file_set(face->label_object, calendar->conf->label_path,
                               "user");
   set_day_label(face->label_object, calendar->conf->DayofWeek_Start,
                 calendar->conf->view_month, calendar->conf->view_year);

   //pack table based on if top image is used or not
   if (calendar->conf->ImageYes == 0)
     {
        //Setup Top Image -- Puts topimage in top 3 rows

        e_table_pack(face->table_object, face->topimage_object, 0, 0, 1, 3);
        e_table_pack_options_set(face->topimage_object, 1, 1, 1, 1, 0.5, 0.5, 0,
                                 0, -1, -1);
        evas_object_show(face->topimage_object);
        // Puts label row in 4 row
        e_table_pack(face->table_object, face->label_object, 0, 4, 1, 1);
        e_table_pack_options_set(face->label_object, 1, 1, 1, 1, 0.5, 0.5, 0, 0,
                                 -1, -1);
        evas_object_show(face->label_object);
        // Puts background in rows 5 - 9
        e_table_pack(face->table_object, face->background_object, 0, 5, 1, 5);
        e_table_pack_options_set(face->background_object, 1, 1, 1, 1, 0.5, 0.5,
                                 0, 0, -1, -1);
        evas_object_show(face->background_object);
     }
   else
     {
        // Puts label row in 1 row
        e_table_pack(face->table_object, face->label_object, 0, 0, 1, 1);
        e_table_pack_options_set(face->label_object, 1, 1, 1, 1, 0.5, 0.5, 0, 0,
                                 -1, -1);
        evas_object_show(face->label_object);
        //Setup background -- put it in the back of the table (2nd row as no topimage)
        e_table_pack(face->table_object, face->background_object, 0, 1, 1, 5);
        e_table_pack_options_set(face->background_object, 1, 1, 1, 1, 0.5, 0.5,
                                 0, 0, -1, -1);
        evas_object_show(face->background_object);
     }

   //Make day objects
   i = 0;
   for (; i < 35; i++)
     {
        //make weekend objects
        face->weekend_object[i] = E_NEW(weekend_face, 1);
        face->weekend_object[i]->menu = face->menu;
        face->weekend_object[i]->obj = edje_object_add(con->bg_evas);
        if (!calendar->conf->UserCS)
           check = edje_object_file_set(face->weekend_object[i]->obj,
                                        calendar->conf->weekend_path,
                                        "default");
        else
           check = edje_object_file_set(face->weekend_object[i]->obj,
                                        calendar->conf->weekend_path, "user");
        evas_object_event_callback_add(face->weekend_object[i]->obj,
                                       EVAS_CALLBACK_MOUSE_DOWN,
                                       calendar_face_cb_mouse_down,
                                       face->weekend_object[i]);
        calendar_face_menu_day(face->weekend_object[i], calendar);
        //make weekday objects
        face->weekday_object[i] = E_NEW(weekday_face, 1);
        face->weekday_object[i]->menu = face->menu;
        face->weekday_object[i]->obj = edje_object_add(con->bg_evas);
        if (!calendar->conf->UserCS)
           check = edje_object_file_set(face->weekday_object[i]->obj,
                                        calendar->conf->weekday_path,
                                        "default");
        else
           check = edje_object_file_set(face->weekday_object[i]->obj,
                                        calendar->conf->weekday_path, "user");
        evas_object_event_callback_add(face->weekday_object[i]->obj,
                                       EVAS_CALLBACK_MOUSE_DOWN,
                                       calendar_face_cb_mouse_down,
                                       face->weekday_object[i]);
        calendar_face_menu_day(face->weekday_object[i], calendar);
        //make today objects
        face->today_object[i] = E_NEW(today_face, 1);
        face->today_object[i]->menu = face->menu;
        face->today_object[i]->obj = edje_object_add(con->bg_evas);
        if (!calendar->conf->UserCS)
           check = edje_object_file_set(face->today_object[i]->obj,
                                        calendar->conf->today_path, "default");
        else
           check = edje_object_file_set(face->today_object[i]->obj,
                                        calendar->conf->today_path, "user");
        evas_object_event_callback_add(face->today_object[i]->obj,
                                       EVAS_CALLBACK_MOUSE_DOWN,
                                       calendar_face_cb_mouse_down,
                                       face->today_object[i]);
        calendar_face_menu_day(face->today_object[i], calendar);
     }

   /* setup calendar object */
   face->o_calendar_table = e_table_add(con->bg_evas);
   e_table_homogenous_set(face->o_calendar_table, 1);
   edje_object_part_swallow(face->background_object, "swallow",
                            face->o_calendar_table);
   evas_object_show(face->o_calendar_table);
   //Time to do some packing....
   int                 x = 0;

   i = 0;
   int                 row = 0;

   while (x < 35)
     {
        for (; i < 7; i++)
          {
             e_table_pack(face->o_calendar_table, face->weekend_object[x]->obj,
                          i, row, 1, 1);
             e_table_pack_options_set(face->weekend_object[x]->obj, 1, 1, 1, 1,
                                      0.5, 0.5, 0, 0, -1, -1);

             e_table_pack(face->o_calendar_table, face->weekday_object[x]->obj,
                          i, row, 1, 1);
             e_table_pack_options_set(face->weekday_object[x]->obj, 1, 1, 1, 1,
                                      0.5, 0.5, 0, 0, -1, -1);

             e_table_pack(face->o_calendar_table, face->today_object[x]->obj, i,
                          row, 1, 1);
             e_table_pack_options_set(face->today_object[x]->obj, 1, 1, 1, 1,
                                      0.5, 0.5, 0, 0, -1, -1);
             x++;
          }
        i = 0;
        row++;
     }

   fill_in_caltable(calendar->conf->DayofWeek_Start, face, 0);

   /* setup gadman */
   face->gmc = e_gadman_client_new(con->gadman);
   e_gadman_client_domain_set(face->gmc, "module.calendar",
                              increment_cal_count());
   e_gadman_client_policy_set(face->gmc,
                              E_GADMAN_POLICY_ANYWHERE | E_GADMAN_POLICY_HMOVE |
                              E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE |
                              E_GADMAN_POLICY_VSIZE);
   //e_gadman_client_min_size_set(face->gmc, 14, 7);
   e_gadman_client_align_set(face->gmc, 1.0, 1.0);
   e_gadman_client_resize(face->gmc, 240, 320);
   e_gadman_client_change_func_set(face->gmc, calendar_face_cb_gmc_change,
                                   face);
   e_gadman_client_load(face->gmc);

   evas_event_thaw(con->bg_evas);

   int                 skew = calculate_skew(calendar);

   calendar_add_dates(face, calendar, skew);
   calendar_face_set_text(calendar);
   if (calendar->conf->UserCS)
     {
        calendar_update_text_fonts(calendar, face);
        update_colors(calendar, face);
     }
   return face;
}

/****************    Functions dealing with mouse ********************************/
/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
void
calendar_face_cb_mouse_down(void *data, Evas * e, Evas_Object * obj,
                            void *event_info)
{
   day_face           *face;
   Evas_Event_Mouse_Down *ev;
   E_Manager          *man;
   E_Container        *con;

   face = data;
   ev = event_info;

   man = e_manager_current_get();
   if (!man)
      return;
   con = e_container_current_get(man);
   if (!con)
      return;
   if (con == NULL)
      return;

   if (ev->button == 3)
     {
        e_menu_activate_mouse(face->menu, e_zone_current_get(con),
                              ev->output.x, ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        e_util_container_fake_mouse_up_all_later(con);
     }

}
/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
void
calendar_face_prev_month(void *data, Evas * e, Evas_Object * obj,
                         void *event_info)
{
   Calendar           *calendar;

   calendar = data;
   calendar->conf->view_month--;
   if (calendar->conf->view_month < 0)
     {
        calendar->conf->view_month = 11;
        calendar->conf->view_year--;
     }
   redraw_calendar(calendar, 0);
}
/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
void
calendar_face_prev_year(void *data, Evas * e, Evas_Object * obj,
                        void *event_info)
{
   Calendar           *calendar;

   calendar = data;
   calendar->conf->view_year--;
   redraw_calendar(calendar, 0);
}
/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
void
calendar_face_next_year(void *data, Evas * e, Evas_Object * obj,
                        void *event_info)
{
   Calendar           *calendar;

   calendar = data;
   calendar->conf->view_year++;
   redraw_calendar(calendar, 0);
}
/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
void
calendar_face_next_month(void *data, Evas * e, Evas_Object * obj,
                         void *event_info)
{
   Calendar           *calendar;

   calendar = data;
   calendar->conf->view_month++;
   if (calendar->conf->view_month > 11)
     {
        calendar->conf->view_month = 0;
        calendar->conf->view_year++;
     }
   redraw_calendar(calendar, 0);
}

/**********************  gadman functions ******************/

/***************************************************
/ Function: 
/ Purpose:  
/
/
*****************************************************/
void
calendar_face_cb_menu_edit(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Calendar_Face      *face;

   face = data;
   e_gadman_mode_set(face->gmc->gadman, E_GADMAN_MODE_EDIT);
}
/***************************************************
/ Function: _calendar_face_cb_gmc_change(void *data, E_Gadman_client *gmc, E_Gadman_Change change)
/ Purpose:  
/ Returns: nothing
/
*****************************************************/
void
calendar_face_cb_gmc_change(void *data, E_Gadman_Client * gmc,
                            E_Gadman_Change change)
{
   Calendar_Face      *face;
   Evas_Coord          x, y, w, h;

   face = data;
   switch (change)
     {
       case E_GADMAN_CHANGE_MOVE_RESIZE:
          e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);
          evas_object_move(face->calendar_object, x, y);
          evas_object_resize(face->calendar_object, w, h);
          break;
       case E_GADMAN_CHANGE_RAISE:
          evas_object_raise(face->calendar_object);
          evas_object_raise(face->topimage_object);
          break;
       case E_GADMAN_CHANGE_EDGE:
       case E_GADMAN_CHANGE_ZONE:
          /* FIXME
           * Must we do something here?
           */
          break;
     }
}

/***************  Face menus ************************/

/***************************************************
/ Function: _calendar_face_menu_new(Calendar_Face *face,Calendar *calendar)
/ Purpose:  
/ Returns: nothing
/
*****************************************************/
void
calendar_face_menu_new(Calendar_Face * face, Calendar * calendar)
{
   E_Menu_Item        *mi;

   /* Setup Menus */
   face->menu = e_menu_new();
   /* Main Menu Items */
//Add Edit Mode option
   mi = e_menu_item_new(face->menu);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, calendar_face_cb_menu_edit, face);

   mi = e_menu_item_new(face->menu);
   e_menu_item_label_set(mi, _("Config Dialog"));
   e_menu_item_callback_set(mi, config_dialog, calendar);

}
/***************************************************
/ Function: calendar_face_menu_new(Calendar_Face *face,Calendar *calendar)
/ Purpose:  
/ Returns: nothing
/
*****************************************************/
void
calendar_face_menu_day(day_face * face, Calendar * calendar)
{
   E_Menu_Item        *mi;

   face->menu1 = e_menu_new();
   /* Main Menu Items */
//View Events
   mi = e_menu_item_new(face->menu1);
   e_menu_item_label_set(mi, _("View Events"));
   e_menu_item_callback_set(mi, view_event_dialog, face);
//View Todo
   mi = e_menu_item_new(face->menu1);
   e_menu_item_label_set(mi, _("View TODO"));
   e_menu_item_callback_set(mi, view_todo_dialog, face);
//Add Event option
   mi = e_menu_item_new(face->menu1);
   e_menu_item_label_set(mi, _("Add Event"));
   e_menu_item_callback_set(mi, add_event_dialog, face);
//Add Todo Option
   mi = e_menu_item_new(face->menu1);
   e_menu_item_label_set(mi, _("Add TODO"));
   e_menu_item_callback_set(mi, add_todo_dialog, face);
//Remove Event
   mi = e_menu_item_new(face->menu1);
   e_menu_item_label_set(mi, _("Remove Event"));
   e_menu_item_callback_set(mi, remove_event_dialog, face);
//Remove todo
   mi = e_menu_item_new(face->menu1);
   e_menu_item_label_set(mi, _("Remove TODO"));
   e_menu_item_callback_set(mi, remove_todo_dialog, face);
}
/****************  Close it down ********************/
/***************************************************
/ Function: calendar_face_free(Calendar_Face *face)
/ Purpose:  
/
*****************************************************/
void
calendar_face_free(Calendar_Face * face)
{
   int                 i = 0;

   for (; i < 35; i++)
     {
        evas_object_del(face->weekend_object[i]->obj);
        evas_object_del(face->weekday_object[i]->obj);
        evas_object_del(face->today_object[i]->obj);
        e_object_del(E_OBJECT(face->weekend_object[i]->menu));
        e_object_del(E_OBJECT(face->weekday_object[i]->menu));
        e_object_del(E_OBJECT(face->today_object[i]->menu));
     }
   if (face->con)
      e_object_unref(E_OBJECT(face->con));
   e_object_del(E_OBJECT(face->gmc));
   evas_object_del(face->topimage_object);
   evas_object_del(face->calendar_object);
   evas_object_del(face->label_object);
   evas_object_del(face->background_object);
   evas_object_del(face->o_calendar_table);
   evas_object_del(face->table_object);
   e_object_del(E_OBJECT(face->menu));
   evas_object_del(face->prev_year);
   evas_object_del(face->prev_month);
   evas_object_del(face->next_year);
   evas_object_del(face->next_month);
   if (face->sub)
      e_object_del(E_OBJECT(face->sub));
   if (face->sub2)
      e_object_del(E_OBJECT(face->sub2));
   if (face->sub3)
      e_object_del(E_OBJECT(face->sub3));
   free(face);
}
