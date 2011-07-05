#include <Elementary.h>
#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#ifndef ELM_LIB_QUICKLAUNCH

void
_title_clicked(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("title clicked!\n");
}

void
_page2(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *btn, *btn2, *nf = data;
   Elm_Naviframe_Item *it;

   btn = elm_button_add(nf);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(btn, "naviframe test2!");

   btn2 = elm_button_add(nf);
   evas_object_size_hint_align_set(btn2, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(btn2, "Next");

   it = elm_naviframe_item_push(nf,
                                "Page 2",
                                NULL,
                                btn2,
                                btn,
                                NULL);
   elm_naviframe_item_subtitle_label_set(it, "Page Temp!");
}

void
test_naviframe(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *win, *bg, *nf, *btn;
   Elm_Naviframe_Item *it;

   win = elm_win_add(NULL, "naviframe", ELM_WIN_BASIC);
   elm_win_title_set(win, "Naviframe");
   elm_win_focus_highlight_enabled_set(win, EINA_TRUE);
   elm_win_autodel_set(win, 1);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   nf = elm_naviframe_add(win);
   elm_win_resize_object_add(win, nf);
   evas_object_show(nf);
   evas_object_smart_callback_add(nf, "title,clicked", _title_clicked, 0);

   btn = elm_button_add(nf);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(btn, "clicked", _page2, nf);
   elm_object_text_set(btn, "naviframe test!");

   it = elm_naviframe_item_push(nf,
                                "Page 1",
                                NULL,
                                NULL,
                                btn,
                                "no_transit");

   evas_object_resize(win, 400, 600);
   evas_object_show(win);
}
#endif
