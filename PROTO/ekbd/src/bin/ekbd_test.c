#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include "Ekbd.h"

#define REMOVE_ME_MAGIC_VALUE 130

static void _test_quit(void *data, Evas_Object *obj, void *event);
static int elm_main(int argc, char **argv);

static Evas_Object *_win;
static Evas_Object *_kbd;

static void
_test_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   elm_exit();
}

static Ekbd_Layout *
_keyboard_default_get(Eina_Bool vertical)
{
   const Eina_List *lkl, *l;
   Ekbd_Layout *kl;
   char buf[1024];
   if (vertical)
     snprintf(buf, sizeof(buf), "Original_v.kbd");
   else
     snprintf(buf, sizeof(buf), "Original.kbd");
   /* TODO catch type in first */
   lkl = ekbd_object_layout_get(_kbd);
   EINA_LIST_FOREACH(lkl, l, kl)
     {
        if ((!strcmp(ecore_file_file_get(ekbd_layout_path_get(kl)), buf)))
          return kl;
     }
   return NULL;
}


static void
_keyboard_layout_update(Eina_Bool vertical)
{
   Ekbd_Layout *kl = NULL;
   const Eina_List *lkl;
   kl = _keyboard_default_get(vertical);
   if (!kl)
     {
        lkl = ekbd_object_layout_get(_kbd);
        if (lkl)
          kl = eina_list_data_get(lkl);
     }
   if (kl)
     ekbd_object_layout_select(_kbd, kl);
}

static void
_ekbd_cb_key_pressed(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ekbd_Event_Key_Pressed *ev;
   ev = event_info;
   if (!ev) return;
#ifdef HAVE_X11
   ekbd_send_x_press(ev->key, ev->mod);
#endif
}

static void
_keyboard_add()
{
   char *p;
   char buf[PATH_MAX];
   size_t len;
   Eina_File_Direct_Info *info;
   Eina_Iterator *ls;

   _kbd = ekbd_object_add(evas_object_evas_get(_win));
   ekbd_object_theme_set(_kbd,  PACKAGE_DATA_DIR"/themes/default.edj");

   ekbd_object_aspect_fixed_set(_kbd, EINA_TRUE);
   len = eina_str_join_len(buf, sizeof(buf), '/', PACKAGE_DATA_DIR,
                           sizeof(PACKAGE_DATA_DIR) - 1,
                           "keyboards", 9);
   if (len + 2 >= sizeof(buf)) return;

   ls = eina_file_direct_ls(buf);
   EINA_ITERATOR_FOREACH(ls, info)
     {
        p = strrchr(info->path + info->name_start, '.');
        if ((p) && (!strcmp(p, ".kbd")))
          {
             ekbd_object_layout_add(_kbd, info->path);
          }
     }
   eina_iterator_free(ls);
   evas_object_smart_callback_add(_kbd, "key,pressed",
                                  _ekbd_cb_key_pressed, NULL);

   _keyboard_layout_update(EINA_FALSE);
}

static void
_keyboard_resize(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   int x, w;
   Eina_Bool vertical;
   evas_object_geometry_get(obj, &x, NULL, &w, NULL);
   vertical = !((w - x) > 1024);
   _keyboard_layout_update(vertical);
}

static int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   Evas_Object *bg, *box, *o;
   printf("Hello, welcome to test virtual keyboard\n");

#ifdef HAVE_X11
   printf("have x11\n");
#else
   printf("no x11\n");
#endif
   ekbd_init();
   _win = elm_win_add(NULL, "Virtual_Keyboard", ELM_WIN_UTILITY);
   elm_win_title_set(_win, "Keyboard");
   elm_win_alpha_set(_win, EINA_TRUE);
   elm_win_shaped_set(_win, EINA_TRUE);
   elm_win_prop_focus_skip_set(_win, EINA_TRUE);
   elm_win_keyboard_win_set(_win, EINA_TRUE);
//   elm_win_borderless_set(_win, EINA_TRUE);
   evas_object_smart_callback_add(_win, "delete,request", _test_quit, NULL);

   box = elm_box_add(_win);
   elm_box_horizontal_set(box, EINA_FALSE);
   evas_object_size_hint_weight_set(box ,EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(_win, box);
   evas_object_show(box);
   o = evas_object_rectangle_add(evas_object_evas_get(box));
   evas_object_pass_events_set(o, EINA_TRUE);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_min_set(o, 30, REMOVE_ME_MAGIC_VALUE);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   bg = elm_bg_add(_win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, bg);
   evas_object_show(bg);

   box = elm_box_add(_win);
   elm_box_horizontal_set(box, EINA_FALSE);
   evas_object_size_hint_weight_set(box ,EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(_win, box);
   evas_object_show(box);
   o = evas_object_rectangle_add(evas_object_evas_get(box));
   evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_pass_events_set(o, EINA_TRUE);
   //evas_object_color_set(o, 0, 0, 0, 128);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_size_hint_min_set(o, 30, REMOVE_ME_MAGIC_VALUE);
   elm_box_pack_end(box, o);
   evas_object_show(o);

   _keyboard_add();
   evas_object_size_hint_align_set(_kbd, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(_kbd, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(box, _kbd);
   evas_object_show(_kbd);
   evas_object_event_callback_add(_kbd, EVAS_CALLBACK_RESIZE,
                                  _keyboard_resize, NULL);

   evas_object_resize(_win, 640, 480 + (REMOVE_ME_MAGIC_VALUE / 2));

   evas_object_show(_win);
   elm_run();
   ekbd_shutdown();
   elm_shutdown();

   return 0;
}

ELM_MAIN()

