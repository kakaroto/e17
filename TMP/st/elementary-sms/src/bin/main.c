#include <Elementary.h>

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

typedef struct _Message_UI Message_UI;

struct _Message_UI
{
   Evas_Object *win;
   struct {
      Evas_Object *options_hv, *delete_hv, *bx, *bt;
   } hover;
};

static const char *
find_contact_icon(const char *contact)
{
   // FIXME: find contact with address 'contact'. NULL == owner
   return NULL;
}

static void
on_hover_down(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   evas_object_del(mui->hover.bt);
   mui->hover.bt = NULL;
   evas_object_del(mui->hover.bx);
   mui->hover.bx = NULL;
}

static void
on_options(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   Evas_Object *bx, *bt, *win, *hv;

   win = mui->win;
   hv = mui->hover.options_hv;

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Options");
   elm_hover_content_set(hv, "middle", bt);
   mui->hover.bt = bt;
   evas_object_show(bt);
   
   bx = elm_box_add(win);
   mui->hover.bx = bx;
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, "Add");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, "Ignore");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Filter");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   
   elm_hover_content_set
     (hv, 
      elm_hover_best_content_location_get(hv, ELM_HOVER_AXIS_VERTICAL), 
      bx);
   evas_object_show(bx);
   
   evas_object_show(hv);
}

static void
on_delete(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   Evas_Object *bx, *bt, *win, *hv;

   win = mui->win;
   hv = mui->hover.delete_hv;

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Delete");
   elm_hover_content_set(hv, "middle", bt);
   mui->hover.bt = bt;
   evas_object_show(bt);
   
   bx = elm_box_add(win);
   mui->hover.bx = bx;
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, "Wipe");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, "Trash");
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   elm_hover_content_set
     (hv, 
      elm_hover_best_content_location_get(hv, ELM_HOVER_AXIS_VERTICAL), 
      bx);
   evas_object_show(bx);
   
   evas_object_show(hv);
}

static Evas_Object *
create_message(Evas_Object *win, 
               const char *title, const char *date, const char *icon, 
               Evas_Bool is_me, const char *text, void *handle)
{
   Message_UI *mui;
   Evas_Object *bb, *bx, *bx2, *bt, *ab, *hv;
   
   mui = calloc(1, sizeof(Message_UI));
   mui->win = win;
   
   bb = elm_bubble_add(win);
   elm_bubble_label_set(bb, title);
   elm_bubble_info_set(bb, date);

   evas_object_data_set(bb, "message_data", mui);
   
   if (icon)
     {
        // FIXME: need a photoframe widget
//        elm_bubble_icon_set(bb, ic);
//        evas_object_show(ic);
     }
   evas_object_size_hint_weight_set(bb, 1.0, 0.0);
   evas_object_size_hint_align_set(bb, -1.0,-1.0);
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);

   ab = elm_anchorblock_add(win);
   elm_anchorblock_hover_style_set(ab, "popout");
   elm_anchorblock_hover_parent_set(ab, win);
   elm_anchorblock_text_set(ab, text);
//   evas_object_smart_callback_add(ab, "anchor,clicked", my_anchorblock_anchor, av);
   evas_object_size_hint_align_set(ab, -1.0, 0.0);
   evas_object_size_hint_align_set(ab, -1.0, -1.0);
   elm_box_pack_end(bx, ab);
   evas_object_show(ab);
   
   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 0.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Reply");
//   evas_object_smart_callback_add(bt, "clicked", on_new_message, NULL);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_size_hint_align_set(bt, 0.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   bt = elm_button_add(win);
   elm_button_label_set(bt, "Options");
   evas_object_smart_callback_add(bt, "clicked", on_options, mui);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_size_hint_align_set(bt, 0.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   
   hv = elm_hover_add(win);
   mui->hover.options_hv = hv;
   evas_object_smart_callback_add(hv, "clicked", on_hover_down, mui);
   elm_hover_style_set(hv, "popout");
   elm_hover_parent_set(hv, win);
   elm_hover_target_set(hv, bt);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Delete");
   evas_object_smart_callback_add(bt, "clicked", on_delete, mui);
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, 1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   hv = elm_hover_add(win);
   mui->hover.delete_hv = hv;
   evas_object_smart_callback_add(hv, "clicked", on_hover_down, mui);
   elm_hover_style_set(hv, "popout");
   elm_hover_parent_set(hv, win);
   elm_hover_target_set(hv, bt);

   
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);
   
   elm_bubble_content_set(bb, bx);
   evas_object_show(bx);
   
   return bb;
}

// sms main window - setup
static void
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
   
   
   evas_object_show(win);
}

int
main(int argc, char **argv)
{
   int i;
   
   elm_init(argc, argv);
   for (i = 1; i < argc; i++)
     {
	// parse args
     }
   create_main_win();
   elm_run();
   elm_shutdown();
   return 0; 
}
