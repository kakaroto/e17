/*
 * =====================================================================================
 *
 *       Filename:  iface_list.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  24/03/09 21:38:24 CET
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


typedef enum _Iface_List_Enum Iface_List_Enum;
typedef struct _Iface_List_Elt Iface_List_Elt;

#include "main.h"
#include <Elementary.h>
#include <Ecore.h>

enum _Iface_List_Enum
{
    ITEM_IFACE,
    ITEM_NETWORK
};

struct _Iface_List_Elt
{
    Iface_List_Enum type;

    char* iface;
    Iface_Type iface_type;

    int nb_use;

    Elm_Genlist_Item * item;

    char *ip;
    int is_link;
    int is_up;

    Evas_Object *box;
    Evas_Object *icon;
    Evas_Object *lbl_name;
    Evas_Object *lbl_ip;


    //wireless network
    Exalt_Wireless_Network *wn;
    int find;

    Ecore_Timer* scan_timer;
};

void iface_list_elt_free(Iface_List_Elt *elt);

Evas_Object* iface_list_new();

void iface_list_add(Evas_Object *l, const char* iface, Iface_Type type);
void iface_list_response(Evas_Object *l, Exalt_DBus_Response* response);
Elm_Genlist_Item* iface_list_get_elt_from_name(Evas_Object *list,const char* iface);


void network_list_interval_get(
        Evas_Object* list, const char* iface,
        Elm_Genlist_Item** first, Elm_Genlist_Item** last);
void network_list_notify_scan(char* iface, Eina_List* networks, void* user_data );
int network_scan_timer_cb(void *data);
void iface_list_promote(Evas_Object *list);

#endif   /* ----- #ifndef IFACE_LIST_INC  ----- */

