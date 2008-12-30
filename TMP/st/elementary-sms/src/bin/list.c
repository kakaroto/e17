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
      (ctc ? 1 : 0),
      msg->body, 
      msg);
   if (icon) free(icon);
   return msgui;
}

static Evas_Object *window, *box, *content;

static Evas_Object *inwin;

static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
on_select_ok(void *data, Evas_Object *obj, void *event_info)
{
   printf("FIXME: take number in entry\n");
   evas_object_del(inwin);
   inwin = NULL;
}

static void
on_to_select(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *win, *sc, *bx, *en, *bx2, *bt;
   
   win = window;
   printf("FIXME: show contact list to select from or phone number entry\n");
   inwin = elm_win_inwin_add(win);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   
   sc = elm_scroller_add(win);
   elm_scroller_content_min_limit(sc, 0, 1);
   evas_object_size_hint_weight_set(sc, 1.0, 0.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_box_pack_end(bx, sc);
   
   en = elm_entry_add(win);
   elm_entry_single_line_set(en, 1);
   elm_entry_entry_set(en, "Select or Enter");
   evas_object_size_hint_weight_set(en, 1.0, 0.0);
   evas_object_size_hint_align_set(en, -1.0, 0.0);
   elm_entry_select_all(en);
   elm_scroller_content_set(sc, en);
   evas_object_show(en);
   evas_object_show(sc);
   
   sc = elm_scroller_add(win);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc,  -1.0, -1.0);
   elm_box_pack_end(bx, sc);
   evas_object_show(sc);
   
   bx2 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_scroller_content_set(sc, bx2);
   evas_object_show(bx2);

   bt = elm_button_add(window);
   elm_button_label_set(bt, "OK");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_smart_callback_add(bt, "clicked", on_select_ok, NULL);
   evas_object_show(bt);   
   
   elm_win_inwin_content_set(inwin, bx);
   evas_object_show(bx);
   
   elm_win_inwin_activate(inwin);
}

static void
on_send(void *data, Evas_Object *obj, void *event_info)
{
   printf("FIXME: send sms\n");
}

static void
on_write(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *np, *bx, *bx2, *ph, *lb, *bt;
   
   /* FIXME: content -> editor + to who display + select "who" */
   if (content) evas_object_del(content);
   
   bx = elm_box_add(window);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   
   content = bx;
   
   bx2 = elm_box_add(window);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   
   ph = elm_photo_add(window);
   evas_object_smart_callback_add(ph, "clicked", on_to_select, NULL);
   evas_object_size_hint_weight_set(ph, 0.0, 1.0);
   evas_object_size_hint_align_set(ph, -1.0, -1.0);
   elm_box_pack_end(bx2, ph);
   evas_object_show(ph);
   
   lb = elm_label_add(window);
   elm_label_label_set(lb, "Select...");
   evas_object_size_hint_weight_set(lb, 1.0, 1.0);
   evas_object_size_hint_align_set(lb, -1.0, 0.5);
   elm_box_pack_end(bx2, lb);
   evas_object_show(lb);
   
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);
   
   np = elm_notepad_add(window);
   evas_object_size_hint_weight_set(np, 1.0, 1.0);
   evas_object_size_hint_align_set(np, -1.0, -1.0);
   elm_box_pack_end(bx, np);
   evas_object_show(np);
   
   bt = elm_button_add(window);
   elm_button_label_set(bt, "Send");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_smart_callback_add(bt, "clicked", on_send, NULL);
   evas_object_show(bt);   

   elm_box_pack_end(box, bx);
   evas_object_show(bx);
}

static int
_sort_msg_newest(const void *data1, const void *data2)
{
   const Data_Message *msg1 = data1;
   const Data_Message *msg2 = data2;
   if (msg1->timestamp > msg2->timestamp) return -1;
   else if (msg1->timestamp < msg2->timestamp) return 1;
   return 0;
}

static void
on_recent(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *sc, *bx2;
   Eina_List *l, *mlist = NULL;
   int from, num, i;
   
   if (content) evas_object_del(content);
   
   sc = elm_scroller_add(window);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_box_pack_end(box, sc);
   evas_object_show(sc);
   
   content = sc;

   bx2 = elm_box_add(window);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_scroller_content_set(sc, bx2);
   evas_object_show(bx2);

   tzset();

   for (l = (Eina_List *)data_message_all_list(); l; l = l->next)
     {
        // FIXME: use filter
        mlist = eina_list_append(mlist, l->data);
     }

   // sort newest first
   mlist = eina_list_sort(mlist, eina_list_count(mlist), _sort_msg_newest);
   // FIXME: from and num are inputs
   from = 0; // from message # 0
   num = 50; // 50 messages max;
   
   for (l = mlist, i = 0; l; l = l->next, i++)
     {
        Data_Message *msg = l->data;
        if ((msg->flags & DATA_MESSAGE_TRASH))
          continue;
        if (i >= from)
          {
             if (i == from)
               {
                  if (l->prev)
                    {
                       // FIXME: add a "newer" button
                    }
               }
             Evas_Object *msgui = _create_message(window, msg);
             elm_box_pack_end(bx2, msgui);
             evas_object_show(msgui);
          }
        if (i >= (from + num - 1))
          {
             if (l->next)
               {
                  // FIXME: add a "older" button
               }
             break;
          }
     }
   
   eina_list_free(mlist);
}

static void
on_chats(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *sc, *bx2;
   
   if (content) evas_object_del(content);
   
   sc = elm_scroller_add(window);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_box_pack_end(box, sc);
   evas_object_show(sc);
   
   content = sc;

   bx2 = elm_box_add(window);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_scroller_content_set(sc, bx2);
   evas_object_show(bx2);

   /* FIXME: delete content and add a scrolled list of everyone who has sent
    * you and sms or that you have sent one to - in order of most recent */
}

static void
on_trash(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *sc, *bx2;
   Eina_List *l, *mlist = NULL;
   int from, num, i;
   
   if (content) evas_object_del(content);
   
   sc = elm_scroller_add(window);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_box_pack_end(box, sc);
   evas_object_show(sc);
   
   content = sc;

   bx2 = elm_box_add(window);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   elm_scroller_content_set(sc, bx2);
   evas_object_show(bx2);

   tzset();

   for (l = (Eina_List *)data_message_all_list(); l; l = l->next)
     {
        // FIXME: use filter
        mlist = eina_list_append(mlist, l->data);
     }

   // sort newest first
   mlist = eina_list_sort(mlist, eina_list_count(mlist), _sort_msg_newest);
   // FIXME: from and num are inputs
   from = 0; // from message # 0
   num = 50; // 50 messages max;
   
   for (l = mlist, i = 0; l; l = l->next, i++)
     {
        Data_Message *msg = l->data;
        if (!(msg->flags & DATA_MESSAGE_TRASH))
          continue;
        if (i >= from)
          {
             if (i == from)
               {
                  if (l->prev)
                    {
                       // FIXME: add a "newer" button
                    }
               }
             Evas_Object *msgui = _create_message(window, msg);
             elm_box_pack_end(bx2, msgui);
             evas_object_show(msgui);
          }
        if (i >= (from + num - 1))
          {
             if (l->next)
               {
                  // FIXME: add a "older" button
               }
             break;
          }
     }
   
   eina_list_free(mlist);
}

void
create_main_win(void)
{
   Evas_Object *win, *bg, *bx, *sc, *tb, *ic;
   Elm_Toolbar_Item *tbi;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Messages");
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);
   
   window = win;
   
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   elm_win_resize_object_add(win, bx);
   
   box = bx;

   tb = elm_toolbar_add(win);
   evas_object_size_hint_weight_set(tb, 1.0, 0.0);
   evas_object_size_hint_align_set(tb, -1.0, -1.0);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "edit");
   elm_toolbar_item_add(tb, ic, "Write", on_write, NULL);
   evas_object_show(ic);
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "clock");
   tbi = elm_toolbar_item_add(tb, ic, "Recent", on_recent, NULL);
   evas_object_show(ic);
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "chat");
   elm_toolbar_item_add(tb, ic, "Chats", on_chats, NULL);
   evas_object_show(ic);
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "delete");
   elm_toolbar_item_add(tb, ic, "Trash", on_trash, NULL);
   evas_object_show(ic);
   
   elm_box_pack_end(bx, tb);
   evas_object_show(tb);

   elm_toolbar_item_select(tbi);
   
   evas_object_show(bx);
   
   evas_object_resize(win, 240, 280);
   
   evas_object_show(win);
}
