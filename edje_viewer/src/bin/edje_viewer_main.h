#ifndef _EVM_H
#define _EVM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.h>
#include <Eet.h>
#include <Ecore_File.h>

#include <Edje.h>
#include <Elementary.h>

#define CONFIG_VERSION 1

#define FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; }} while (0);

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

extern int __log_domain;
#define DBG(...) EINA_LOG_DOM_DBG(__log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__log_domain, __VA_ARGS__)

typedef struct _Viewer Viewer;
typedef struct _Config Config;
typedef struct _Config_Group Config_Group;
typedef struct _Group Group;
typedef struct _Part Part;

struct _Viewer
{
    Config *config;
    Evas_Object *win;

    struct {
        Evas_Object *win, *ly, *tbar, *tree, *parts_list;
        Evas_Object *toggles_win, *entry;
        Evas_Object *sig_box, *sig_list, *sig_signal;
    } gui;


    Elm_Genlist_Item_Class *gc;

    const char *theme_file;

    Eina_Inlist *groups;

    Group *visible_group;
    Part *visible_part;

    Eina_List *signals;
    Eina_List *hoversel_items;

    struct {
        char *buf;

        Ecore_Timer *timer;

        Eina_Bool visible : 1;
    } typebuf;

    Eet_Data_Descriptor  *config_edd;
    Eet_Data_Descriptor  *groups_edd;
    Ecore_Timer *config_save_timer;

    Eina_Bool entry_visible : 1;
};

struct _Config
{
    int config_version;

    const char *edje_file;
    Eina_List *groups;
    Eina_Bool show_parts;
    Eina_Bool show_entry;
    Eina_Bool show_signals;
};

struct _Config_Group
{
    const char *name;

    Eina_Bool visible;
};

struct _Group
{
   EINA_INLIST;

   Viewer *v;

   Evas_Object *obj;
   Evas_Object *check;

   const char *name;

   Elm_Genlist_Item *item;
   Elm_Toolbar_Item *ti;

   Eina_Inlist *parts;

   Eina_Bool active : 1;
   Eina_Bool visible : 1;
};

struct _Part
{
   EINA_INLIST;

   Group *grp;
   const char *name;
   Elm_List_Item *item;

   Evas_Object *highlight;
};

#include "edje_viewer_gui.h"

void config_save(Viewer *v, Eina_Bool immediate);
void config_group_change(Viewer *v, const char *name, Eina_Bool visible, Eina_Bool del);
Config_Group * config_group_get(Viewer *v, const char *name);

#endif
