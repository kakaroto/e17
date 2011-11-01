/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include <Edje.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Elementary.h>
#include <Eio.h>

#include "enna.h"
#include "enna_config.h"
#include "utils.h"
#include "module.h"
#include "explorer.h"
#include "activity.h"
#include "localfiles.h"
#include "udisks.h"
#include "exec.h"

#define EDJE_GROUP_MAIN_LAYOUT "enna/main/layout"
#define EDJE_PART_MAINMENU_SWALLOW "enna.mainmenu.swallow"

static const Ecore_Getopt options = {
  PACKAGE_NAME,
  "%prog [options] [directory]",
  PACKAGE_VERSION "Revision:" ENNA_STRINGIFY(VREV),
  "(C) 2011 aguirre.nicolas@gmail.com",
  "GPLv2",
  "Enna Explorer",
  EINA_TRUE,
  {
    ECORE_GETOPT_STORE_STR('t', "theme", "Specify the theme to be used"),
    ECORE_GETOPT_STORE_STR('g', "geometry", "Specify window geometry (--geometry 1280x720)"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

/* Global Variable Enna *enna*/
Enna *enna;

static char *app_theme = "tablet";
static char *app_geometry = NULL;
static Eina_Bool app_exit = EINA_FALSE;
static unsigned int app_w = 1024;
static unsigned int app_h = 600;
static unsigned int app_x_off = 0;
static unsigned int app_y_off = 0;
static const char *app_path = NULL;

/* Functions */
static int _create_gui(void);

/* Callbacks */

static void _window_delete_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   ecore_main_loop_quit();
}

static void
_window_resize_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Coord w, h;

   evas_object_geometry_get(enna->win, NULL, NULL, &w, &h);
   evas_object_resize(enna->layout, w - 2 * app_x_off, h - 2 * app_y_off);
   evas_object_move(enna->layout, app_x_off, app_y_off);
}

/* Functions */

static int _enna_init(int argc, char **argv)
{
   Eina_List *l;
   Enna_Class_Activity *a;

   enna_config_init();

   if (app_theme)
     {
        ENNA_FREE(enna_config->theme);
        enna_config->theme = strdup(app_theme);
     }
   enna_config_load_theme();


   elm_init(argc, argv);
   elm_need_efreet();
   elm_need_ethumb();
   efreet_mime_init();
   eio_init();
   enna_udisks_init();
   enna_exec_init();

   if (!_create_gui())
     return 0;


   enna->start_path = app_path;
   // create explorer
   enna_localfiles_init();
   enna_explorer_init();
   /* Dinamically init activities */
   EINA_LIST_FOREACH(enna_activities_get(), l, a)
     enna_activity_init(a->name);

   return 1;
}

static int _create_gui(void)
{
   // set custom elementary theme
   elm_theme_extension_add(enna_config->eth, enna_config_theme_get());

   // main window
   enna->win = elm_win_add(NULL, "enna", ELM_WIN_BASIC);
   if (!enna->win)
     return 0;

   elm_object_theme_set(enna->win, enna_config->eth);
   elm_win_title_set(enna->win, "Enna Explorer");
   evas_object_smart_callback_add(enna->win, "delete,request",
                                  _window_delete_cb, NULL);
   evas_object_event_callback_add(enna->win, EVAS_CALLBACK_RESIZE,
                                  _window_resize_cb, NULL);

   enna->evas = evas_object_evas_get(enna->win);

   /* Enable evas cache (~4 backgrounds in the cache at a time) : 1 background =  1280x720*4 = 3,7MB */
   /* ==> Set cache to 16MB */
   evas_image_cache_set(enna->evas, 4 * 4 * 1024 * 1024);

   // main layout widget
   enna->layout = elm_layout_add(enna->win);
   if (!elm_layout_file_set(enna->layout, enna_config_theme_get(), EDJE_GROUP_MAIN_LAYOUT))
     {
        return 0;
     }
   evas_object_size_hint_weight_set(enna->layout, 1.0, 1.0);
   evas_object_show(enna->layout);


   // show all
   evas_object_resize(enna->win, app_w, app_h);
   evas_object_resize(enna->layout, app_w - 2 * app_x_off, app_h - 2 * app_y_off);
   evas_object_move(enna->layout, app_x_off, app_y_off);

   evas_object_show(enna->win);

   return 1;
}

static void _enna_shutdown(void)
{
   enna_activity_del_all();

   evas_object_del(enna->o_background);
   evas_object_del(enna->o_content);

   elm_shutdown();
   enna_util_shutdown();
   enna_exec_shutdown();
   ENNA_FREE(enna);
}

static void _opt_geometry_parse(const char *optarg,
                                unsigned int *pw, unsigned int *ph, unsigned int *px, unsigned int *py)
{
   int w = 0, h = 0;
   int x = 0, y = 0;
   int ret;

   ret = sscanf(optarg, "%dx%d:%d:%d", &w, &h, &x, &y);

   if ( ret != 2 && ret != 4 )
     return;

   if (pw) *pw = w;
   if (ph) *ph = h;
   if (px) *px = x;
   if (py) *py = y;
}


static Eina_Bool
exit_signal(void *data __UNUSED__, int type __UNUSED__, void *e)
{
   Ecore_Event_Signal_Exit *event = e;

   fprintf(stderr,
           "Enna got exit signal [interrupt=%u, quit=%u, terminate=%u]\n",
           event->interrupt, event->quit, event->terminate);

   ecore_main_loop_quit();
   return 1;
}

int main(int argc, char **argv)
{
   int res = EXIT_FAILURE;
   int args;
   char path[PATH_MAX];
   Eina_List *l;
   Eina_List *p;
   const char *pname;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(app_theme),
     ECORE_GETOPT_VALUE_STR(app_geometry),
     ECORE_GETOPT_VALUE_BOOL(app_exit),
     ECORE_GETOPT_VALUE_NONE
   };

   init_locale();
   eina_init();


   args = ecore_getopt_parse(&options, values, argc, argv);

   if (args < 0)
     return -1;

   if (app_exit)
       return 0;


   if (app_geometry)
       _opt_geometry_parse(app_geometry, &app_w, &app_h, &app_x_off, &app_y_off);

   if (argc < args + 1)
     {
        const char *tmp;
        tmp = getcwd(path, PATH_MAX);
        app_path = eina_stringshare_add(path);
     }
   else
     {
        app_path = eina_stringshare_add(argv[args]);
     }

   enna = calloc(1, sizeof(Enna));

   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_signal, enna);

   enna_util_init();

   if (!_enna_init(argc, argv))
     goto out;


   pname = elm_profile_current_get();
   printf("Current profile : %s\n", pname);
   enna->profile = eina_stringshare_add(pname);
   p = elm_profile_list_get();
   printf("Profiles : \n");
   EINA_LIST_FOREACH(p, l, pname)
     {
        printf("%s\n", pname);
     }

   ecore_main_loop_begin();

   _enna_shutdown();
   res = EXIT_SUCCESS;

 out:
   return res;
}

