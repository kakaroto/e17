#include "ui.h"

static void
_ui_animate_done_cb(UI_WIN *ui, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if ((!ui->settings_box) || (!elm_flip_front_visible_get(ui->flip))) return;
   evas_object_del(ui->settings_box);
   ui->settings_box = NULL;
}

void
ui_key_grab_set(UI_WIN *ui, const char *key, Eina_Bool enable)
{
   Evas *e;
   Evas_Modifier_Mask ctrl, shift, alt;

   e = evas_object_evas_get(ui->win);
   ctrl = evas_key_modifier_mask_get(e, "Control");
   shift = evas_key_modifier_mask_get(e, "Shift");
   alt = evas_key_modifier_mask_get(e, "Alt");
   if (enable)
     1 | evas_object_key_grab(ui->win, key, 0, ctrl | shift | alt, 1); /* worst warn_unused ever. */
   else
     evas_object_key_ungrab(ui->win, key, 0, ctrl | shift | alt);
}

void
ui_win_init(UI_WIN *ui)
{
   Evas_Object *obj, *fr, *box, *win, *ic;
   Evas *e;
   Evas_Modifier_Mask ctrl, shift, alt;

   settings_finagle(ui);
   win = ui->win = elm_win_util_standard_add("shotgun", "Shotgun");

   ic = evas_object_image_add(evas_object_evas_get(win));
   evas_object_image_file_set(ic, DATA_DIR "/icons/hicolor/48x48/apps/shotgun.png", NULL);
   if (evas_object_image_load_error_get(ic))
     evas_object_image_file_set(ic, PACKAGE_SRC_DIR "/data/shotgun.png", NULL);
   if (evas_object_image_load_error_get(ic))
     evas_object_del(ic);
   else
     elm_win_icon_object_set(win, ic);
   elm_win_icon_name_set(win, "shotgun");

   elm_win_autodel_set(win, 1);
   elm_win_screen_constrain_set(win, 1);
   e = evas_object_evas_get(win);
   ctrl = evas_key_modifier_mask_get(e, "Control");
   shift = evas_key_modifier_mask_get(e, "Shift");
   alt = evas_key_modifier_mask_get(e, "Alt");
   1 | evas_object_key_grab(win, "Escape", 0, ctrl | shift | alt, 1); /* worst warn_unused ever. */
   1 | evas_object_key_grab(win, "q", ctrl, shift | alt, 1); /* worst warn_unused ever. */

   obj = elm_bg_add(win);
   EXPAND(obj);
   FILL(obj);
   elm_win_resize_object_add(win, obj);
   evas_object_show(obj);

   if (ui->settings->enable_illume)
     {
        ui->illume_box = box = elm_box_add(win);
        elm_box_homogeneous_set(box, EINA_FALSE);
        elm_box_horizontal_set(box, EINA_TRUE);
        EXPAND(box);
        FILL(box);
        elm_win_resize_object_add(win, box);
        evas_object_show(box);

        ui->illume_frame = fr = elm_frame_add(win);
        EXPAND(fr);
        FILL(fr);
        if (ui->type)
          elm_object_text_set(fr, "Login");
        else
          elm_object_text_set(fr, "Contacts");
        elm_box_pack_end(box, fr);
     }

   ui->flip = elm_flip_add(win);
   EXPAND(ui->flip);
   FILL(ui->flip);
   evas_object_smart_callback_add(ui->flip, "animate,done", (Evas_Smart_Cb)_ui_animate_done_cb, ui);

   IF_ILLUME(ui)
     {
        elm_object_content_set(fr, ui->flip);
        evas_object_show(fr);
     }
   else
     elm_win_resize_object_add(win, ui->flip);

   ui->box = box = elm_box_add(win);
   elm_box_homogeneous_set(box, EINA_FALSE);
   IF_ILLUME(ui)
     WEIGHT(box, 0, EVAS_HINT_EXPAND);
   else
     EXPAND(box);
   evas_object_show(box);

   elm_object_part_content_set(ui->flip, "front", box);

   evas_object_show(ui->flip);
}
