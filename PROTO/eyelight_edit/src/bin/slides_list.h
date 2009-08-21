/*
 * =====================================================================================
 *
 *       Filename:  slides_list.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/29/2009 02:28:00 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  SLIDES_LIST_INC
#define  SLIDES_LIST_INC


#include "main.h"



//list of List_Item
//the same list is used in slides_grid.c
extern Eina_List* l_slides;
void item_delete_cb(void *data, Evas_Object *o, const char *emission, const char *source);

Evas_Object *slides_list_new();
void slides_list_update();
void slides_list_item_move(int id, int id_after);
void slides_list_slide_append_relative(List_Item *item, List_Item *item_prev);


#endif   /* ----- #ifndef SLIDES_LIST_INC  ----- */

