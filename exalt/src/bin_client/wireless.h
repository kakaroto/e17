/*
 * =====================================================================================
 *
 *       Filename:  wireless.h
 *
 *    Description:
 *
 *
 *        Version:  1.0
 *        Created:  04/05/2009 12:52:41 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  WIRELESS_INC
#define  WIRELESS_INC

typedef struct _wireless Wireless;

#include "main.h"
#include "iface_list.h"


typedef struct _hoversel_data
{
    int choice;
    Wireless *wireless;
    char* desc;
}Hoversel_Data;

struct _wireless
{
    Iface_List_Elt *network;

    Evas_Object *frame;

    Evas_Object *lbl_essid;
    Evas_Object *lbl_security;

    //list of wpa mode supported by the network
    Evas_Object *hoversel_wpa;
    Eina_List *l_data_hoversel;
    Hoversel_Data* data_hoversel_current;
    Eina_List *l_item_hoversel;

    //radio used in the wep mode
    Evas_Object *radio_hexa;
    Evas_Object *radio_plain;


    Evas_Object  *entry_password;

    Evas_Object *btn_back;
    Evas_Object *btn_close;
    Evas_Object *btn_apply;
};


Wireless* wireless_new(Evas_Object *win);
void wireless_set(Wireless *wireless, Iface_List_Elt* network);
void wireless_hoversel_wpa_clean(Wireless *wireless);


void wireless_disabled_update(Wireless *wireless);
void wireless_cb_plain(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wireless_cb_hexa(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);

void wireless_hoversel_wpa_select(void *data,
        Evas_Object *obj __UNUSED__,
        void* event_info __UNUSED__);

void wireless_cb_entry(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);

void wireless_cb_back(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wireless_cb_close(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);
void wireless_cb_apply(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__);

#endif   /* ----- #ifndef WIRELESS_INC  ----- */

