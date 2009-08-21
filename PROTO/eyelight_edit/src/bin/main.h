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

    //used by slides_grid;
    Evas_Object *grid_icon;
};

extern Eina_List *l_slides;

void utils_slide_insert(List_Item *item_prev);
void utils_slide_delete(List_Item *item);
void utils_slide_move(int id_slide, int id_after);

Evas_Object *toolbar_create();

void slides_list_slide_append_relative(List_Item *item, List_Item *item_prev);
void slides_list_slide_delete(List_Item *item);

Evas_Object *slideshow_create();

Evas_Object *tabpanel_create();

void slide_menu_show(List_Item *item, int x, int y);
#endif   /* ----- #ifndef MAIN_INC  ----- */

