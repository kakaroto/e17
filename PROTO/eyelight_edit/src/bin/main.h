/*
 * =====================================================================================
 *
 *       Filename:  main.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/29/2009 02:12:03 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  MAIN_INC
#define  MAIN_INC

#include <Elementary.h>
#include <Eyelight_Smart.h>
#include <Eyelight.h>
#include <Eyelight_Edit.h>

/**
 * TODO
 * Catch the errors (area does'nt exists, can't load the presentation ...)
 * fix #405, bug with insert_after in an elm genlist
 * rewrite list/slide
 *
 * Classics stuff : select the layout, add an text item, image item ...
 */

/* we use 2 presentations,
 * one is displayed in the center
 * and the second is used to display the slides in the left list
 */
Evas_Object *pres;

Evas_Object *win;

#define D_(str) gettext(str)

typedef struct Evas_Object_Couple Evas_Object_Couple;
typedef struct list_item List_Item;

struct Evas_Object_Couple
{
    Evas_Object *left;
    Evas_Object *right;
};
#include "slides_list.h"
#include "presentation.h"
#include "slides_grid.h"


struct list_item {
    Eyelight_Thumb *thumb;

    //used by slides_list
    Elm_Genlist_Item *item;
    Evas_Object *icon;
    int *icon_data;

    //used by slides_grid;
    Evas_Object *grid_icon;
    int *grid_icon_data;
};

extern Eina_List *l_slides;

//edit

void utils_save(const char *file);


void utils_slide_insert(List_Item *item_prev);
void utils_slide_delete(List_Item *item);
void utils_slide_move(int id_slide, int id_after);
void utils_edit_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void utils_edit_area_up(void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_down(void *data, Evas_Object *obj, void *event_info);
void utils_obj_unselect();
void utils_slide_change_cb(Eyelight_Viewer *pres, int old_slide, int new_slide, void *data);
void utils_edit_area_add(void *data, Evas_Object *obj, void *event_info);
void utils_edit_slide_default_areas_reinit(void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_delete(void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_image_add(void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_vertical_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_vertical_homogeneous_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_vertical_flow_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_horizontal_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_horizontal_homogeneous_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_horizontal_flow_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_layout_stack_set (void *data, Evas_Object *obj, void *event_info);
void utils_edit_area_defaults_init(void *data, Evas_Object *obj, void *event_info);



void utils_edit_image_file_change(void *data, Evas_Object *obj, void *event_info);
void utils_edit_image_border_change(void *data, Evas_Object *obj, void *event_info);
void utils_edit_image_shadow_change(void *data, Evas_Object *obj, void *event_info);
char *utils_file_move_in_pres(const char *file);
void utils_edit_object_down(void *data, Evas_Object *obj, void *event_info);
void utils_edit_object_up(void *data, Evas_Object *obj, void *event_info);
void utils_edit_object_delete(void *data, Evas_Object *obj, void *event_info);
//


Evas_Object *toolbar_create();

void slides_list_slide_append_relative(List_Item *item, List_Item *item_prev);
void slides_list_slide_delete(List_Item *item);

Evas_Object *slideshow_create();

Evas_Object *tabpanel_create();

void slide_menu_show(List_Item *item, int x, int y);

Evas_Object *rightpanel_create();
void rightpanel_area_show();
void rightpanel_empty_show();
void rightpanel_image_show();
void rightpanel_image_data_set(const char* file, int border, int shadow);
void rightpanel_area_layout_set(const char *layout);

#endif   /* ----- #ifndef MAIN_INC  ----- */

