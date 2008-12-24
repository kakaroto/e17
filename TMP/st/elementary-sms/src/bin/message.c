#include "common.h"

typedef struct _Message_UI Message_UI;

struct _Message_UI
{
   Evas_Object *win, *bb;
   void *handle;
};

static void
on_anchor(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   Elm_Entry_Anchorblock_Info *ei = event_info;
}

static void
on_reply(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   printf("reply to message\n");
}

static void
on_forward(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   printf("forward message\n");
}

static void
on_add(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   printf("add contact\n");
}

static void
on_filter(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   printf("filter messages to/from this contact only\n");
}

static void
on_wipe(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   printf("delete message completely\n");
   evas_object_del(mui->bb);
   data_message_del(mui->handle);
   free(mui);
}

static void
on_trash(void *data, Evas_Object *obj, void *event_info)
{
   Message_UI *mui = data;
   printf("move message to trash\n");
   evas_object_del(mui->bb);
   data_message_trash(mui->handle);
   free(mui);
}

Evas_Object *
create_message(Evas_Object *win, 
               const char *title, const char *date, const char *icon, 
               Evas_Bool is_me, Evas_Bool have_contact,
               const char *text, void *handle)
{
   Message_UI *mui;
   Evas_Object *bb, *bx, *bx2, *bt, *ab, *hv, *ph, *fr;
   
   mui = calloc(1, sizeof(Message_UI));
   mui->win = win;
   mui->handle = handle;
   
   bb = elm_bubble_add(win);
   mui->bb = bb;
   evas_object_data_set(bb, "message_data", mui);
   
   if (is_me) elm_bubble_corner_set(bb, "top_right");
   elm_bubble_label_set(bb, title);
   elm_bubble_info_set(bb, date);

   ph = elm_photo_add(win);
   elm_photo_size_set(ph, 40);
   elm_photo_file_set(ph, icon);
   elm_bubble_icon_set(bb, ph);
   evas_object_show(ph);
   
   evas_object_size_hint_weight_set(bb, 1.0, 0.0);
   evas_object_size_hint_align_set(bb, -1.0,-1.0);
   
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);

   ab = elm_anchorblock_add(win);
   elm_anchorblock_hover_style_set(ab, "popout");
   elm_anchorblock_hover_parent_set(ab, win);
   elm_anchorblock_text_set(ab, text);
   evas_object_smart_callback_add(ab, "anchor,clicked", on_anchor, mui);
   evas_object_size_hint_align_set(ab, -1.0, 0.0);
   evas_object_size_hint_align_set(ab, -1.0, -1.0);
   elm_box_pack_end(bx, ab);
   evas_object_show(ab);
   
   fr = elm_frame_add(win);
   elm_frame_style_set(fr, "outdent_bottom");
   evas_object_size_hint_weight_set(fr, 1.0, 0.0);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);
   
   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, 1);

   bt = elm_hoversel_add(win);
   elm_hoversel_hover_parent_set(bt, win);
   elm_hoversel_label_set(bt, "Options");
   elm_hoversel_item_add(bt, "Forward", NULL, ELM_ICON_NONE, on_forward, mui);
   if ((!is_me) && (!have_contact))
     elm_hoversel_item_add(bt, "Add", NULL, ELM_ICON_NONE, on_add, mui);
   elm_hoversel_item_add(bt, "Filter", NULL, ELM_ICON_NONE, on_filter, mui);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_size_hint_align_set(bt, 0.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   if (!is_me)
     {
        bt = elm_button_add(win);
        elm_button_label_set(bt, "Reply");
        evas_object_smart_callback_add(bt, "clicked", on_reply, NULL);
        evas_object_size_hint_weight_set(bt, 0.0, 0.0);
        evas_object_size_hint_align_set(bt, 0.0, -1.0);
        elm_box_pack_end(bx2, bt);
        evas_object_show(bt);
     }
   
   bt = elm_hoversel_add(win);
   elm_hoversel_hover_parent_set(bt, win);
   elm_hoversel_label_set(bt, "Delete");
   elm_hoversel_item_add(bt, "Wipe", NULL, ELM_ICON_NONE, on_wipe, mui);
   elm_hoversel_item_add(bt, "Trash", NULL, ELM_ICON_NONE, on_trash, mui);
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, 1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);

   elm_frame_content_set(fr, bx2);
   evas_object_show(bx2);

   elm_box_pack_end(bx, fr);
   evas_object_show(fr);
   
   elm_bubble_content_set(bb, bx);
   evas_object_show(bx);
   
   return bb;
}
