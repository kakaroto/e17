#include "common.h"

typedef struct _Message_UI Message_UI;

struct _Message_UI
{
   Evas_Object *win;
   struct {
      Evas_Object *options_hv, *delete_hv, *bx, *bt;
   } hover;
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

Evas_Object *
create_message(Evas_Object *win, 
               const char *title, const char *date, const char *icon, 
               Evas_Bool is_me, const char *text, void *handle)
{
   Message_UI *mui;
   Evas_Object *bb, *bx, *bx2, *bt, *ab, *hv;
   
   mui = calloc(1, sizeof(Message_UI));
   mui->win = win;
   mui->handle = handle;
   
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
   evas_object_smart_callback_add(ab, "anchor,clicked", on_anchor, mui);
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
   evas_object_smart_callback_add(bt, "clicked", on_reply, NULL);
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
