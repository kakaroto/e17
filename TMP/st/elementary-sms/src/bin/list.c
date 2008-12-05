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
   Eina_List *l;
   time_t tnow;
   struct tm *tmnow;

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

   tzset();
   tnow = time(NULL);
   tmnow = localtime(&tnow);
   for (l = (Eina_List *)data_message_all_list(); l; l = l->next)
     {
        Data_Message *msg = l->data;
        Data_Contact *ctc;
        Evas_Object *msgui;
        const char *title;
        const char *when;
        char *icon;
        time_t t;
        struct tm *tm;
        char tbuf[256], fbuf[512], tobuf[512];
        
        title = "???";
        ctc = data_contact_by_tel_find(msg->from_to);
        if (ctc)
          {
             if (ctc->name.nicks)
               title = ctc->name.nicks->data;
             else if (ctc->name.display)
               title = ctc->name.display;
             else if ((ctc->name.lasts) && (ctc->name.firsts))
               {
                  snprintf(fbuf, sizeof(fbuf), "%s %s", ctc->name.firsts, ctc->name.lasts);
                  title = fbuf;
               }
             else if (ctc->name.lasts)
               title = ctc->name.lasts->data;
             else
               title = msg->from_to;
          }
        else
          title = msg->from_to;
        if (msg->flags & DATA_MESSAGE_SENT)
          {
             snprintf(tobuf, sizeof(tobuf), "To: %s", title);
             title = tobuf;
          }
        when = "???";
        t = msg->timestamp;
        tm = localtime(&t);
        if (tm)
          {
             if (t > tnow)
               {
                  snprintf(tbuf, sizeof(tbuf), "Future");
                  when = tbuf;
               }
             else if ((tnow - t) < (24 * 60 * 60))
               {
                  strftime(tbuf, sizeof(tbuf) - 1, "%H:%M:%S", tm);
                  when = tbuf;
               }
             else if ((tnow - t) < (6 * 24 * 60 * 60))
               {
                  strftime(tbuf, sizeof(tbuf) - 1, "%a %H:%M", tm);
                  when = tbuf;
               }
             else
               {
                  strftime(tbuf, sizeof(tbuf) - 1, "%d/%m/%y %H:%M", tm);
                  when = tbuf;
               }
          }
        icon = NULL;
        if (ctc) icon = data_contact_photo_file_get(ctc);
        msgui = create_message
          (win, title, when, icon,
           (msg->flags & DATA_MESSAGE_SENT) ? 1 : 0,
           msg->body, 
           msg);
        if (icon) free(icon);
        elm_box_pack_end(bx2, msgui);
        evas_object_show(msgui);
     }
   
   evas_object_show(bx);
   
   evas_object_resize(win, 240, 280);
   
   evas_object_show(win);
}
