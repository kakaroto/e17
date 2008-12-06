#include "common.h"

static Evas_Object *
_create_message(Evas_Object *win, Data_Message *msg)
{
   Data_Contact *ctc;
   Evas_Object *msgui;
   const char *title;
   const char *when;
   char *icon;
   time_t t, tnow;
   struct tm *tm, tmnow;
   char tbuf[256], fbuf[512], tobuf[512];
   
   tnow = time(NULL);
   tm = localtime(&tnow);
   memcpy(&tmnow, tm, sizeof(struct tm));
   
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
   if (t > tnow)
     {
        snprintf(tbuf, sizeof(tbuf), "Future");
        when = tbuf;
     }
   else if ((tnow - t) < (24 * 60 * 60))
     {
        if (tmnow.tm_yday != tm->tm_yday)
          {
             strftime(tbuf, sizeof(tbuf) - 1, "Yesterday %H:%M", tm);
             when = tbuf;
          }
        else
          {
             strftime(tbuf, sizeof(tbuf) - 1, "%H:%M:%S", tm);
             when = tbuf;
          }
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
   icon = NULL;
   if (ctc) icon = data_contact_photo_file_get(ctc);
   msgui = create_message
     (win, title, when, icon,
      (msg->flags & DATA_MESSAGE_SENT) ? 1 : 0,
      msg->body, 
      msg);
   if (icon) free(icon);
   return msgui;
}

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

static int
_sort_msg_newset(const void *data1, const void *data2)
{
   const Data_Message *msg1 = data1;
   const Data_Message *msg2 = data2;
   if (msg1->timestamp > msg2->timestamp) return -1;
   else if (msg1->timestamp < msg2->timestamp) return 1;
   return 0;
}

void
create_main_win(void)
{
   Evas_Object *win, *bg, *bx, *bt, *sc, *bx2;
   Eina_List *l, *mlist = NULL;

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

   for (l = (Eina_List *)data_message_all_list(); l; l = l->next)
     mlist = eina_list_append(mlist, l->data);

   // sort newest first
   mlist = eina_list_sort(mlist, eina_list_count(mlist), _sort_msg_newset);
   
   for (l = mlist; l; l = l->next)
     {
        Data_Message *msg = l->data;
        Evas_Object *msgui = _create_message(win, msg);
        elm_box_pack_end(bx2, msgui);
        evas_object_show(msgui);
     }
   
   eina_list_free(mlist);
   
   evas_object_show(bx);
   
   evas_object_resize(win, 240, 280);
   
   evas_object_show(win);
}
