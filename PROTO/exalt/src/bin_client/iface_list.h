/*
 * =====================================================================================
 *
 *       Filename:  iface_list.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/02/09 22:48:10 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  IFACE_LIST_INC
#define  IFACE_LIST_INC

#include "main.h"

typedef struct _iface_list
{
    Evas_Object* list;
} Iface_List;

Iface_List iface_list;

typedef struct _iface_elt
{
    char* iface;
    Iface_Type type;
    Evas_Object *icon;
    Elm_List_Item *item;
} Iface_Elt;

void iface_list_create();
void iface_list_add(char* iface, Iface_Type iface_type);
void iface_list_sel(void *data, Evas_Object* o, void* event_info);
void iface_list_icon_update(char* iface, Exalt_DBus_Response* response);
void iface_list_response(Exalt_DBus_Response* response);
Iface_Type iface_list_iface_type_get(char* iface);



#endif   /* ----- #ifndef IFACE_LIST_INC  ----- */

