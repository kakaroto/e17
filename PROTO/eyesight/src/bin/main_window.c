#include <stdio.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Evas.h>
#include <Edje.h>

#include "config.h"
#include "eyesight_private.h"

#include "conf.h"
#include "error.h"
#include "main_window.h"
#include "plugin.h"

void
main_window_resize_cb(Ecore_Evas *ee)
{
   int w, h;
   Main_Window *main_window = (Main_Window*)ecore_evas_data_get(ee, "main_window");
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(main_window->main_window, w, h);
   evas_object_resize(main_window->controls, w, h);
}

void
main_window_delete_request_cb(Ecore_Evas *ee)
{
   /* Free */
   Main_Window *main_window = (Main_Window *)ecore_evas_data_get(ee, "main_window");
   Args *args = (Args *)ecore_evas_data_get(ee, "args");
   free(args->theme_path);
   if (args->files)
      ecore_list_destroy(args->files);
   evas_object_del(main_window->main_window);
   evas_free(main_window->evas);
   free(main_window);
   ecore_evas_free(ee);
   ecore_main_loop_quit();
}

Main_Window *
main_window_create(Args *args, Evas_List **startup_errors)
{
   Main_Window *main_window = malloc(sizeof(Main_Window));
   Ecore_Evas *ee;
   char *theme;
   Ecore_List *resize_callbacks;

   if (!(args->engine) || !strcmp(args->engine, "software_x11"))
   {
      ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
      if (!ee)
      {
         printf(_("Can't create software_X11 window. Is there an X server running?\n"));
         return 0;
      }
   }
   else if (!strcmp(args->engine, "software_ddraw"))
   {
      ee = ecore_evas_software_ddraw_new(NULL, 0, 0, 0, 0);
      if (!ee)
      {
         printf(_("Can't create software_ddraw window.0\n"));
         return 0;
      }
   }
   else if (!strcmp(args->engine, "gl_x11"))
   {
      ee = ecore_evas_gl_x11_new(0, 0, 0, 0, 0, 0);
      if (!ee)
      {
         printf(_("Can't create gl_X11 window. Is there an X server running?\n"));
         return 0;
      }
   }
   else if (!strcmp(args->engine, "xrender_x11"))
   {
      ee = ecore_evas_xrender_x11_new(0, 0, 0, 0, 0, 0);
      if (!ee)
      {
         printf(_("Can't create xrender_X11 window. Is there an X server running?\n"));
         return 0;
      }
   }
   else if (!strcmp(args->engine, "fb"))
   {
      ee = ecore_evas_fb_new(0, 0, 0, 0);
      if (!ee)
      {
         printf(_("Can't create framebuffer.\n"));
         return 0;
      }
   }
   else
   {
      printf(_("%s is an unknown engine. Valid engines are: software_x11 (default), software_ddraw, gl_x11, xrender_x11 and fb."), args->engine);
      return 0;
   }

   if (!ee)
      return 0;

   main_window->evas = ecore_evas_get(ee);
   main_window->main_window = edje_object_add(main_window->evas);

   ecore_evas_data_set(ee, "main_window", (void *)main_window);
   ecore_evas_data_set(ee, "args", (void *)args);

   // Try to open theme supplied in arguments
   char theme_loaded = 0;
   if (args->theme_path)
   {
      if (!edje_object_file_set(main_window->main_window, args->theme_path,
                                "eyesight/main_window/background"))
      {
         append_startup_error(startup_errors, ERROR_THEME, args->theme_path);
      }
      else
         theme_loaded = 1;
   }
   if (!theme_loaded)
      edje_object_file_set(main_window->main_window,
                           PACKAGE_DATA_DIR"/themes/docker/docker.edj",
                           "eyesight/main_window/background");
   evas_object_layer_set(main_window->main_window, 0);

   // Setting up foreground parts
   main_window->controls = edje_object_add(main_window->evas);
   edje_object_file_get(main_window->main_window, (const char **)&theme, NULL);
   edje_object_file_set(main_window->controls, theme, "eyesight/main_window/controls");
   evas_object_layer_set(main_window->controls, 9999);
   evas_object_name_set(main_window->controls, "controls");

   // Each resize callback will be called on resize
   resize_callbacks = ecore_list_new();
   evas_object_data_set(main_window->controls, "resize_callbacks", resize_callbacks);
   evas_object_event_callback_add(main_window->controls, EVAS_CALLBACK_RESIZE,
                                  controls_resize_cb, resize_callbacks);

   int w, h;
   evas_object_name_set(main_window->main_window, "main_window");
   edje_object_size_min_get(main_window->main_window, &w, &h);
   evas_object_move(main_window->main_window, 0, 0);
   evas_object_resize(main_window->main_window, w, h);
   evas_object_resize(main_window->controls, w, h);
   ecore_evas_resize(ee, w, h);
   ecore_evas_size_min_set(ee, w, h);

   /* VM stuff */
   ecore_evas_title_set(ee, WM_TITLE);
   ecore_evas_name_class_set(ee, WM_NAME, WM_CLASS);

   /* Callbacks */
   ecore_evas_callback_resize_set(ee, main_window_resize_cb);
   ecore_evas_callback_delete_request_set(ee, main_window_delete_request_cb);
   edje_object_signal_callback_add(main_window->controls, "load", "*",
                                   main_window_load_cb, startup_errors);
   edje_object_signal_callback_add(main_window->controls, "clicked", "exit_button",
                                   exit_clicked_cb, NULL);

   ecore_animator_frametime_set(FRAMETIME);
   evas_object_show(main_window->main_window);
   evas_object_show(main_window->controls);
   ecore_evas_show(ee);

   display_startup_error_dialog(ee, *startup_errors);
   return main_window;
}

void
main_window_load_cb(void *data, Evas_Object *o, const char *emission,
                    const char *source)
{
   if (!evas_list_data(data))
      return; // don't do anything if there's no errors
   edje_object_signal_emit(o, "error_show", "eyesight");
}

void controls_resize_cb(void *_data, Evas *e, Evas_Object *obj, void *event_info)
{
   Ecore_List *data = _data;
   Controls_Resize_Cbdata *cbdata;

   cbdata = ecore_list_first_goto(data); // Reset list
   do
   {
      if (!cbdata)
         return;
      cbdata->func(cbdata->data, e, obj, event_info);
   }
   while ((cbdata = ecore_list_next(data)));
}

void
exit_clicked_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
   main_window_delete_request_cb(ecore_evas_ecore_evas_get(evas_object_evas_get(o)));
}
