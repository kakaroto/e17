/* EXCHANGE - a library to interact with exchange.enlightenment.org
 * Copyright (C) 2008 Massimiliano Calamelli
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Ecore.h>
#include <Exchange.h>

#define INFO(...) EINA_LOG_DOM_INFO(__exchange_smart_log_domain, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(__exchange_smart_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(__exchange_smart_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__exchange_smart_log_domain, __VA_ARGS__)

static int __exchange_smart_log_domain = -1;

static Evas_Object *bg, *cont, *exsm;

static void
on_destroy(Ecore_Evas *ee)
{
    ecore_main_loop_quit();
}

static void
on_resize(Ecore_Evas * ee)
{
   int w, h;

   evas_output_size_get(ecore_evas_get(ee), &w, &h);
   evas_object_resize(bg, w , h);
   evas_object_resize(cont, w - 20, h - 20);
}

static void
on_scroll(void *data, Evas_Object *o, const char *em, const char *src)
{
   double dx, dy;
   int box_w, box_h;

   edje_object_part_drag_value_get(o, src, &dx, &dy);
   //edje_object_part_drag_size_set(o, src, 0, 0.8);
   //printf("SCROLL %f %f %s\n", dx, dy, em);

   evas_object_size_hint_min_get(exsm, &box_w, &box_h);
   //printf("HINTS__: %d %d\n", box_w, box_h);

   exchange_smart_object_offset_set(exsm, 0, box_h * dy);
}

static void
on_wheel_up(void *data, Evas_Object *o, const char *em, const char *src)
{
   Evas_Coord x, y;

   exchange_smart_object_offset_get(exsm, &x, &y);
   y -= 30;
   if (y < 0) y = 0;
   exchange_smart_object_offset_set(exsm, x, y);
}

static void
on_wheel_down(void *data, Evas_Object *o, const char *em, const char *src)
{
   Evas_Coord x, y, w, h;

   exchange_smart_object_offset_get(exsm, &x, &y);
   evas_object_size_hint_min_get(exsm, &w, &h);
   y += 30;
   if (y > h) y = h;
   exchange_smart_object_offset_set(exsm, x, y);
}

static void
on_button(void *data, Evas_Object *o, const char *em, const char *src)
{
   DBG("THEME SELECTED: %s [%s]\n", em, src);
   if (!strcmp(src, "btn_themes")) exchange_smart_object_remote_group_set(exsm, "Border");
   if (!strcmp(src, "btn_wallp")) exchange_smart_object_remote_group_set(exsm, "Wallpaper");
   if (!strcmp(src, "btn_apps")) exchange_smart_object_remote_group_set(exsm, "Applications");
   if (!strcmp(src, "btn_mods")) exchange_smart_object_remote_group_set(exsm, "Modules");

   exchange_smart_object_run(exsm);
}

static void
on_apply(const char *path, void *data)
{
   WRN("THEME SELECTED: %s [%p]\n", path, data);
}

int
main(int argc, char **argv)
{
   Ecore_Evas  *ee;
   Evas *evas;
   char buf[4096];

   /* Init Stuff */
   if (!eina_init())
      return 0;

   __exchange_smart_log_domain = eina_log_domain_register("Exchange", EINA_COLOR_BLUE);
   if (!__exchange_smart_log_domain)
   {
      EINA_LOG_ERR("Could not register log domain: Exchange");
      goto shutdown_eina;
   }

   if (!ecore_init())
      goto shutdown_eina;
   evas_init();
   if (!ecore_evas_init())
      goto shutdown_ecore;
   if (!ecore_file_init())
      goto shutdown_evas;
   edje_init();
   if (!exchange_init())
      goto shutdown_ecore_file;

   /* Create Ecore Evas Window */;
   ee = ecore_evas_new(NULL, 0, 0, 0, 0, NULL);
   INFO("Using Evas Engine: %s", ecore_evas_engine_name_get(ee));
   evas = ecore_evas_get(ee);
   ecore_evas_title_set(ee, "Exchange Smart Test   (downloads goes in /tmp)");
   ecore_evas_callback_resize_set(ee, on_resize);
   ecore_evas_callback_destroy_set(ee, on_destroy);
   ecore_evas_size_min_set(ee, 450, 200);
   

   /* White Background */
   bg = evas_object_rectangle_add(evas);
   evas_object_color_set(bg, 255, 255, 255, 255);
   evas_object_move(bg, 0, 0);
   evas_object_show(bg);

   /* Create the Edje Container */
   snprintf(buf, sizeof(buf), "%s/exchange_smart.edj", PACKAGE_DATA_DIR);
   if (!ecore_file_exists(buf))
   {
      ERR("Can't find smart theme file '%s'\n", buf);
      return 1;
   }
   cont = edje_object_add(evas);
   edje_object_file_set(cont, buf, "exchange/smart/test");
   evas_object_move(cont, 10, 10);
   evas_object_show(cont);
   edje_object_signal_callback_add(cont, "drag", "scroller", on_scroll, NULL);
   edje_object_signal_callback_add(cont, "mouse,wheel,0,-1", "shadow", on_wheel_up, NULL);
   edje_object_signal_callback_add(cont, "mouse,wheel,0,1", "shadow", on_wheel_down, NULL);
   edje_object_signal_callback_add(cont, "mouse,wheel,0,-1", "scroller.confine", on_wheel_up, NULL);
   edje_object_signal_callback_add(cont, "mouse,wheel,0,1", "scroller.confine", on_wheel_down, NULL);
   edje_object_signal_callback_add(cont, "mouse,down,1", "btn_*", on_button, NULL);

   /* The Exchange Smart Object*/
   exsm = exchange_smart_object_add(evas);
   exchange_smart_object_local_path_set(exsm, "/tmp");
   exchange_smart_object_apply_cb_set(exsm, on_apply, NULL);

   /* Swallow the smart object inside the edje test interface */
   edje_object_part_swallow(cont, "swallow.content", exsm);

   /* Show the window */
   ecore_evas_resize(ee, 450, 400);
   ecore_evas_show(ee);

   /* Enter the mail loop*/
   ecore_main_loop_begin();


   /* Delete the evas objects */
   evas_object_del(exsm);
   evas_object_del(cont);

   /* Shutdown everything else */
   exchange_shutdown();
   edje_shutdown();
   shutdown_ecore_file:
      ecore_file_shutdown();
   shutdown_evas:
      evas_shutdown();
   shutdown_ecore:
      ecore_shutdown();
   shutdown_eina:
      eina_shutdown();

   return 0;
}

