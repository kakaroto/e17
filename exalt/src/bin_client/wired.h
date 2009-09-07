/*
 * =====================================================================================
 *
 *       Filename:  wired.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/01/2009 06:59:40 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  WIRED_INC
#define  WIRED_INC

typedef struct _wired Wired;

#include "main.h"
#include "iface_list.h"


struct _wired
{
    Iface_List_Elt* iface;

    int dhcp;

    Evas_Object *frame;

    Evas_Object *icon;

    Evas_Object *btn_activate;
    Evas_Object *btn_deactivate;

    Evas_Object *radio_dhcp;
    Evas_Object *radio_static;

    Evas_Object* entry_ip;
    Evas_Object* entry_netmask;
    Evas_Object* entry_gateway;

    Evas_Object* entry_cmd;

    Evas_Object *btn_back;
    Evas_Object *btn_close;
    Evas_Object *btn_apply;
    char* cmd;
};

void wired_set(Wired *wired, Iface_List_Elt* iface);
Wired* wired_new(Evas_Object *win);
void wired_response(Wired *wired,Exalt_DBus_Response *response);

void wired_icon_update(Wired *wired);
void wired_disabled_update(Wired *wired);


void wired_cb_entry(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);

void wired_cb_dhcp(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wired_cb_static(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);


// callbacks buttons
void wired_cb_back(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wired_cb_close(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wired_cb_apply(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);

void wired_cb_activate(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wired_cb_deactivate(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);



#endif   /* ----- #ifndef WIRED_INC  ----- */

