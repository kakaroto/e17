#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Elementary.h>

#include "../mess_header.h"
Evas_Object *font_win, *list, *fsize;

void
my_win_del(void *data, Evas_Object *obj, void *event_info)
{
   (void) data;
   (void) obj;
   (void) event_info;
   font_win = NULL;
}

static Eina_List *
_font_list_get(const Evas *e)
{
   Eina_List *flist = evas_font_available_list(e);
   Eina_List *itr, *nitr;
   const char *font, *prev_font = NULL;
   flist = eina_list_sort(flist, eina_list_count(flist),
         (Eina_Compare_Cb) strcasecmp);
   EINA_LIST_FOREACH_SAFE(flist, itr, nitr, font)
     {
        Elm_Font_Properties *efp;

        efp = elm_font_properties_get(font);
        /* Remove dups */
        if (prev_font && !strcmp(efp->name, prev_font))
          {
             flist = eina_list_remove_list(flist, itr);
          }
        else
          {
             eina_stringshare_replace(&font, efp->name);
             prev_font = font;
             eina_list_data_set(itr, font);
          }

        elm_font_properties_free(efp);
     }

   return flist;
}

static void
_set_clicked(void *data,
      Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   const char *selected = NULL;
   Elm_Object_Item *list_it = elm_list_selected_item_get(list);
   if (list_it)
     selected = elm_object_item_text_get(list_it);
   editor_font_choose(data, selected, elm_spinner_value_get(fsize));
}


Evas_Object *
ui_font_dialog_open(Evas_Object *parent, Evas_Object *entry)
{
   Evas_Object *win, *bg, *bx, *btn, *hbx, *lbl;

   if (font_win)
     {
        evas_object_show(font_win);
        return font_win;
     }

   font_win = win = elm_win_add(parent, "font-select", ELM_WIN_TOOLBAR);
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, _("Select Font"));
   evas_object_smart_callback_add(win, "delete,request", my_win_del, entry);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   list = elm_list_add(win);
   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(bx, list);
   evas_object_show(list);

   hbx = elm_box_add(bx);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   elm_box_pack_end(bx, hbx);
   evas_object_show(hbx);

   lbl = elm_label_add(hbx);
   elm_object_text_set(lbl, _("Font size:"));
   elm_box_pack_end(hbx, lbl);
   evas_object_show(lbl);

   fsize = elm_spinner_add(hbx);
   elm_spinner_label_format_set(fsize, _("%.0f pts"));
   elm_spinner_step_set(fsize, 1);
   elm_spinner_wrap_set(fsize, EINA_FALSE);
   elm_object_style_set (fsize, "vertical");
   elm_spinner_min_max_set(fsize, 0, 72);
   evas_object_size_hint_align_set(fsize, 0.0, 0.5);
   elm_box_pack_end(hbx, fsize);
   evas_object_show(fsize);

   btn = elm_button_add(win);
   elm_object_text_set(btn, _("Set"));
   evas_object_size_hint_align_set(btn, 1.0, 0.0);
   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(btn);
   elm_box_pack_end(hbx, btn);
   evas_object_smart_callback_add(btn, "clicked", _set_clicked, entry);

   /* Populate list */
     {
        const char *font;
        Eina_List *flist, *itr;

        flist = _font_list_get(evas_object_evas_get(list));
        EINA_LIST_FOREACH(flist, itr, font)
          {
             elm_list_item_append(list, font, NULL, NULL, NULL, NULL);
          }

        EINA_LIST_FREE(flist, font)
           eina_stringshare_del(font);
     }


   /* Forcing it to be the min height. */
   evas_object_resize(win, 300, 500);
   evas_object_show(win);

   return win;
}
