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

#define _GNU_SOURCE
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include <Edje.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Elementary.h>

#include "enna.h"
#include "enna_config.h"
#include "utils.h"
#include "module.h"
#include "volumes.h"
#include "explorer.h"
#include "activity.h"

#define EDJE_GROUP_MAIN_LAYOUT "enna/main/layout"
#define EDJE_PART_MAINMENU_SWALLOW "enna.mainmenu.swallow"


/* Global Variable Enna *enna*/
Enna *enna;

static const char *app_theme = "phone";
static unsigned int app_w = 480;
static unsigned int app_h = 800;
static unsigned int app_x_off = 0;
static unsigned int app_y_off = 0;

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

   enna_module_init();

   if (app_theme)
     {
        ENNA_FREE(enna_config->theme);
        enna_config->theme = strdup(app_theme);
     }
   enna_config_load_theme();


   /* Create ecore events (we should put here ALL the event_type_new) */
   ENNA_EVENT_BROWSER_CHANGED = ecore_event_type_new();

   elm_init(argc, argv);
   elm_need_efreet();
   elm_need_ethumb();

   /* Load available modules */
   enna_module_load_all();

   if (!_create_gui())
     return 0;

   // create explorer
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
   enna_module_shutdown();

   evas_object_del(enna->o_background);
   evas_object_del(enna->o_content);

   elm_shutdown();
   enna_util_shutdown();
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


static void usage(char *binname)
{
   printf("Enna Explorer\n");
   printf(" Usage: %s [options ...]\n", binname);
   printf(" Available options:\n");
   printf("  -c, (--config):  Specify configuration file to be used.\n");
   printf("  -h, (--help):    Display this help.\n");
   printf("  -t, (--theme):   Specify theme name to be used.\n");
   printf("  -g, (--geometry):Specify window geometry. (geometry=1280x720)\n");
   printf("  -g, (--geometry):Specify window geometry and offset. (geometry=1280x720:10:20)\n");
   printf("\n");
   printf("  -V, (--version): Display Enna version number.\n");
   exit(EXIT_SUCCESS);
}

static void version(void)
{
   printf(PACKAGE_STRING"\n");
   exit(EXIT_SUCCESS);
}

static int parse_command_line(int argc, char **argv)
{
   int c, index;
   char short_options[] = "Vhfc:t:b:g:p:";
   struct option long_options [] =
     {
       { "help",          no_argument,       0, 'h' },
       { "version",       no_argument,       0, 'V' },
       { "fs",            no_argument,       0, 'f' },
       { "config",        required_argument, 0, 'c' },
       { "theme",         required_argument, 0, 't' },
       { "geometry",      required_argument, 0, 'g' },
       { 0,               0,                 0,  0  }
     };

   /* command line argument processing */
   while (1)
     {
        c = getopt_long(argc, argv, short_options, long_options, &index);

        if (c == EOF)
          break;

        switch (c)
          {
           case 0:
              /* opt = long_options[index].name; */
              break;

           case '?':
           case 'h':
              usage(argv[0]);
              return -1;

           case 'V':
              version();
              break;

           case 't':
              app_theme = strdup(optarg);
              break;

           case 'g':
              _opt_geometry_parse(optarg, &app_w, &app_h, &app_x_off, &app_y_off);
              break;

           default:
              usage(argv[0]);
              return -1;
          }
     }

   return 0;
}

int main(int argc, char **argv)
{
   int res = EXIT_FAILURE;

   init_locale();

   if (parse_command_line(argc, argv) < 0)
     return EXIT_SUCCESS;

   eina_init();
   enna_util_init();

   enna = calloc(1, sizeof(Enna));

   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_signal, enna);

   if (!_enna_init(argc, argv))
     goto out;

   ecore_main_loop_begin();

   _enna_shutdown();
   res = EXIT_SUCCESS;

 out:
   return res;
}

