#include <Elementary.h>
#include <Efreet.h>

#include "exec.h"
#include "file.h"
#include "enna.h"

void _mime_selection_window_show(Enna_File *file)
{
   const char *mime;
   Eina_List *apps, *l;
   Efreet_Desktop *desk = NULL;

   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *fr;
   Evas_Object *list;
   Evas_Object *en;
   Evas_Object *bx;
   Evas_Object *btn_bx;
   Evas_Object *btn_ok;
   Evas_Object *btn_cancel;


   win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, _("Open File"));
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_data_set(win, "file", file);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   evas_object_size_hint_min_set(bg, 400, 300);

   fr = elm_frame_add(win);
   elm_object_style_set(fr, "pad_medium");
   evas_object_show(fr);

   elm_win_resize_object_add(win, fr);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(bx);
   elm_box_homogeneous_set(bx, EINA_FALSE);
   elm_frame_content_set(fr, bx);
   elm_box_padding_set(bx, 4, 4);

   fr = elm_frame_add(win);
   elm_object_text_set(fr, "Choose a program to open the file");
   evas_object_show(fr);
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);

   list = elm_list_add(win);
   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(list);
   elm_list_mode_set(list, ELM_LIST_LIMIT);


   mime = efreet_mime_type_get(file->uri);
   apps = efreet_util_desktop_mime_list(mime);

   EINA_LIST_FOREACH(apps, l, desk)
     {
        Eina_List *files = NULL;
        Evas_Object *ic;

        files = eina_list_append(files, file->mrl);
        printf("desk->name : %s %s\n", desk->name, desk->icon);
        
        ic = elm_icon_add(win);
        evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        evas_object_size_hint_min_set(ic, 24, 24);
        elm_icon_standard_set(ic, desk->icon);
        elm_list_item_append(list, desk->name, ic, NULL, NULL, NULL);
        /* efreet_desktop_command_get(desk, files, */
        /*                            _app_exec_cb, NULL); */

     }
   
   elm_frame_content_set(fr, list);
   elm_box_pack_end(bx, fr);


   fr = elm_frame_add(win);
   elm_object_text_set(fr, "Specific program");
   evas_object_show(fr);
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, EINA_TRUE);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND,0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(en, file->label);
   evas_object_show(en);
   evas_object_data_set(win, "entry", en);

   elm_frame_content_set(fr, en);
   elm_box_pack_end(bx, fr);

   btn_bx = elm_box_add(win);
   elm_box_horizontal_set(btn_bx, EINA_TRUE);
   evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 1.0);
   evas_object_show(btn_bx);
   elm_box_padding_set(btn_bx, 8, 2);

   btn_ok = elm_button_add(win);
   elm_object_text_set(btn_ok, _("Open it"));
   evas_object_show(btn_ok);
   evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(btn_ok, 0.0, EVAS_HINT_FILL);
   //evas_object_smart_callback_add(btn_ok, "clicked",
   //                               _dialog_rename_ok_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_ok);

   btn_cancel = elm_button_add(win);
   elm_object_text_set(btn_cancel, _("Cancel"));
   evas_object_show(btn_cancel);
   evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_cancel, 1.0, EVAS_HINT_FILL);
   //evas_object_smart_callback_add(btn_cancel, "clicked",
   //                               _dialog_cancel_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_cancel);

   elm_box_pack_end(bx, btn_bx);

   evas_object_show(win);
}

void enna_exec(Enna_File *file)
{
   if (!file || ENNA_FILE_IS_BROWSABLE(file))
     return;

   if (!file->mime)
     file->mime = eina_stringshare_add(efreet_mime_type_get(file->mrl));

   /* Do we know preferred executable for this mime ? */
   //if (_mime_is_known(file->mime))
   //  _exec(file);

   /* No : display the application selector window */
   _mime_selection_window_show(file);

}
