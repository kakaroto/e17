#include "common.h"

static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
on_write(void *data, Evas_Object *obj, void *event_info)
{
}

static void
on_recent(void *data, Evas_Object *obj, void *event_info)
{
}

static void
on_chats(void *data, Evas_Object *obj, void *event_info)
{
}

void
create_main_win(void)
{
   Evas_Object *win, *bg, *bx, *bt, *sc, *bx2;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Messages");
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);
   
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   elm_win_resize_object_add(win, bx);

   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Write");
   evas_object_smart_callback_add(bt, "clicked", on_write, NULL);
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Recent");
   evas_object_smart_callback_add(bt, "clicked", on_recent, NULL);
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Chats");
   evas_object_smart_callback_add(bt, "clicked", on_chats, NULL);
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);
   
   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_box_pack_end(bx, sc);
   evas_object_show(sc);

   bx2 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_scroller_content_set(sc, bx2);
   evas_object_show(bx2);

     {
        Evas_Object *msg;
        
        msg = create_message
          (win, "Me", "Yesterday", find_contact_icon(NULL), 1,
           "Hello world. This is my first SMS.", 
           NULL);
        elm_box_pack_end(bx2, msg);
        evas_object_show(msg);
        
        msg = create_message
          (win, "Friend", "today", find_contact_icon("1234"), 0,
           "Great! You have a phone that works. This is fantastic.<br>"
           "Now We can send messages all day!<br>"
           "This is cool!", 
           NULL);
        elm_box_pack_end(bx2, msg);
        evas_object_show(msg);
        
        msg = create_message
          (win, "Another ", "10 minutes ago", find_contact_icon("321"), 0,
           "I just love to send you long SMS's so this one will be no "
           "exception. I'll fill it with all sorts of junk to see if your "
           "Messages application on your phone works well and can handle "
           "long messages. I hope it can, because this one is long.<br>"
           "<br>"
           "Now as to me actually saying something useful - forget it. "
           "I won't be doing any such silliness. This will just waste "
           "lots of space and time and fill up part of your phone, so "
           "don't bother trying to extrať any usefulness from this message "
           "as it won't be useful at all. I hope that helps you skip this "
           "message quickly and get on with something useful. Actually I "
           "don't hope that - that would be contrary to the point of this "
           "message. I hope it wastes lots of your time!",
           NULL);
        elm_box_pack_end(bx2, msg);
        evas_object_show(msg);
     }
   
   evas_object_show(bx);
   
   evas_object_resize(win, 240, 280);
   
   evas_object_show(win);
}
