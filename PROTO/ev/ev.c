/*
 * Copyright 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Elementary.h>
#include <Ecore_X.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

#define DBG(...)            EINA_LOG_DOM_DBG(_ev_log_dom, __VA_ARGS__)
#define INF(...)            EINA_LOG_DOM_INFO(_ev_log_dom, __VA_ARGS__)
#define WRN(...)            EINA_LOG_DOM_WARN(_ev_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(_ev_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(_ev_log_dom, __VA_ARGS__)

static int _ev_log_dom = -1;
static Elm_Genlist_Item_Class itc;;
static Evas_Object *img, *list = NULL;
static int root_x = -1, root_y = -1;

static void
_close(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   ecore_main_loop_quit();
}

static void
_key(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, Evas_Event_Key_Down *key)
{
   Elm_Genlist_Item *it;
   DBG("%p: %s", obj, key->keyname);
   if (!strcmp(key->keyname, "space"))
     {
        it = elm_genlist_selected_item_get(list);
        DBG("current: %p", it);
        it = elm_genlist_item_next_get(it);
        if (!it) it = elm_genlist_first_item_get(list);
        DBG("next: %p", it);
        elm_image_file_set(img, elm_genlist_item_data_get(it), NULL);
        elm_genlist_item_selected_set(it, EINA_TRUE);
        elm_genlist_item_bring_in(it);
     }
   else if ((!strcmp(key->keyname, "Return")) || (!strcmp(key->keyname, "KP_Enter")))
     {
        if (obj == elm_object_parent_widget_get(img)) return;
        it = elm_genlist_selected_item_get(list);
        if (!it) return;
        elm_image_file_set(img, elm_genlist_item_data_get(it), NULL);
        elm_genlist_item_bring_in(it);
     }
   else if (key->keyname[0] == 'q')
     ecore_main_loop_quit();
}

static void
_del(void *data, Evas_Object *obj __UNUSED__)
{
   eina_stringshare_del(data);
}

static char *
_label(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return strdup(data);
}

static void
_pick(void *data __UNUSED__, Evas_Object *obj __UNUSED__, Elm_Genlist_Item *ev)
{
   const char *file, *f, *p;
   Evas_Object *win;
   int x, y;

   DBG("pick");
   elm_image_file_get(img, &f, &p);
   file = elm_genlist_item_data_get(ev);
   if (f && (!strcmp(file, f))) return;

   elm_image_file_set(img, file, NULL);
   elm_image_object_size_get(img, &x, &y);
   if ((x >= root_x) || (y >= root_y))
     {
        double dx, dy;

        dx = (double)root_x / (double) x;
        dy = (double)root_y / (double) y;
        if (dx > dy)
          {
             evas_object_resize(img, (int)(dy * x), root_y);
             evas_object_resize(elm_object_parent_widget_get(img), (int)(dy * x), root_y);
          }
        else
          {
             evas_object_resize(img, root_x, (int)(dx * y));
             evas_object_resize(elm_object_parent_widget_get(img), root_x, (int)(dx * y));
          }
     }
   else
     {
        evas_object_resize(img, x, y);
        evas_object_resize(elm_object_parent_widget_get(img), x, y);
     }
   win = elm_object_parent_widget_get(img);
   elm_win_title_set(win, file);
   INF("x=%i, y=%i", x, y);
}

static void
_show(void *data, Evas_Object *obj __UNUSED__, void *ev __UNUSED__)
{
   DBG("show");
   if (evas_object_visible_get(data))
     evas_object_hide(data);
   else
     evas_object_show(data);
}

static void
add_single(Evas_Object *list, const char *file)
{
   elm_genlist_item_append(list, &itc, file, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL /*data*/);
}

static void
add_dir(Evas_Object *list, const char *dir)
{
   Eina_Iterator *it;
   const char *str;

   it = eina_file_ls(dir);
   EINA_ITERATOR_FOREACH(it, str)
     add_single(list, str);
   eina_iterator_free(it);
}

static void
load_stuff(int argc, char **argv, Evas_Object *list)
{
   struct stat st;
   int x;

   for (x = 1; x < argc; x++)
     {
        stat(argv[x], &st);
        if (S_ISREG(st.st_mode))
          add_single(list, eina_stringshare_add(argv[x]));
        else if (S_ISDIR(st.st_mode))
          add_dir(list, argv[x]);
        else
          ERR("wtf is this %s", argv[x]);
     }
   elm_genlist_item_selected_set(elm_genlist_first_item_get(list), EINA_TRUE);
}

int
main(int argc, char *argv[])
{
   Evas_Object *listwin, *win, *bg, *box;
   int num_wins, nw;
   Ecore_X_Window zw, zwr, *roots;

   itc.item_style     = "default";
   itc.func.label_get = _label;
   itc.func.icon_get  = NULL;
   itc.func.state_get = NULL;
   itc.func.del       = _del;

   elm_init(argc, argv);
   ecore_app_args_set(argc, (const char**)argv);

   _ev_log_dom = eina_log_domain_register("ev", EINA_COLOR_HIGH EINA_COLOR_CYAN);
   if (argc == 1)
     {
        printf("Usage: %s filename filename directory fileglob*\n", argv[0]);
        exit(1);
     }
   //eina_log_domain_level_set("ev", EINA_LOG_LEVEL_DBG);

   win = elm_win_add(NULL, NULL, ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   img = elm_image_add(win);
   evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(img);
   evas_object_show(win);

   zw = elm_win_xwindow_get(win);
   zwr = ecore_x_window_root_get(zw);
   roots = ecore_x_window_root_list(&num_wins);
   if ((!roots) || (num_wins <= 0))
     ERR("Could not determine screen size; autoscaling disabled");
   else
     {
        for (nw = 0; nw < num_wins; nw++)
          {
             if (roots[nw] != zwr) continue;

             ecore_x_window_size_get(zwr, &root_x, &root_y);
             break;
          }
        free(roots);
     }

   listwin = elm_win_add(NULL, "ev", ELM_WIN_BASIC);
   elm_win_autodel_set(listwin, EINA_TRUE);

   bg = elm_bg_add(listwin);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(listwin, bg);
   evas_object_show(bg);

   box = elm_box_add(listwin);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(listwin, box);
   evas_object_show(box);

   list = elm_genlist_add(listwin);
   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, list);
   evas_object_show(list);
   evas_object_smart_callback_add(list, "clicked,double", (Evas_Smart_Cb)_pick, NULL);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)_key, NULL);
   evas_object_event_callback_add(list, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)_key, NULL);
   evas_object_smart_callback_add(img, "clicked", (Evas_Smart_Cb)_show, listwin);
   evas_object_smart_callback_add(win, "delete,request", _close, NULL);

   load_stuff(argc, argv, list);

   _pick(NULL, NULL, elm_genlist_selected_item_get(list));

   evas_object_resize(listwin, 450, 350);

   elm_run();
   elm_shutdown();

   return 0;
}
