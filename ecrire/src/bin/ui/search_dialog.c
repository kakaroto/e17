#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Elementary.h>

#include "../mess_header.h"

static Evas_Object *search_win, *sent, *rent;
static Eina_Bool forward = EINA_TRUE;
static Evas_Textblock_Cursor *cur_find;

static Eina_Bool
_find_in_entry(Evas_Object *entry, const char *text, Eina_Bool jump_next)
{
   Eina_Bool try_next = EINA_FALSE;
   const char *found;
   char *utf8;
   const Evas_Object *tb = elm_entry_textblock_get(entry);
   Evas_Textblock_Cursor *end, *start, *mcur;
   size_t initial_pos;

   if (!text || !*text)
      return EINA_FALSE;

   mcur = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get(tb);
   if (!cur_find)
     {
        cur_find = evas_object_textblock_cursor_new(tb);
     }
   else if (!evas_textblock_cursor_compare(cur_find, mcur))
     {
        try_next = EINA_TRUE;
     }

   if (forward)
     {
        evas_textblock_cursor_paragraph_last(cur_find);
        start = mcur;
        end = cur_find;
     }
   else
     {
        /* Not correct, more adjustments needed. */
        evas_textblock_cursor_paragraph_first(cur_find);
        start = cur_find;
        end = mcur;
     }

   initial_pos = evas_textblock_cursor_pos_get(start);

   utf8 = evas_textblock_cursor_range_text_get(start, end,
         EVAS_TEXTBLOCK_TEXT_PLAIN);

   if (!utf8)
      return EINA_FALSE;

   if (try_next && jump_next)
     {
        found = strstr(utf8 + 1, text);
        if (!found)
          {
             found = utf8;
          }
     }
   else
     {
        found = strstr(utf8, text);
     }

   if (found)
     {
        size_t pos = 0;
        int idx = 0;
        while ((utf8 + idx) < found)
          {
             pos++;
             eina_unicode_utf8_get_next(utf8, &idx);
          }

        elm_entry_select_none(entry);
        evas_textblock_cursor_pos_set(mcur, pos + initial_pos + strlen(text));
        elm_entry_cursor_selection_begin(entry);
        elm_entry_cursor_pos_set(entry, pos + initial_pos);
        elm_entry_cursor_selection_end(entry);
        evas_textblock_cursor_copy(mcur, cur_find);
     }

   free(utf8);

   return !!found;
}

static void
_find_clicked(void *data,
      Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   _find_in_entry(data, elm_object_text_get(sent), EINA_TRUE);
}

static void
_replace_clicked(void *data,
      Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (_find_in_entry(data, elm_object_text_get(sent), EINA_FALSE))
     {
        elm_entry_entry_insert(data, elm_object_text_get(rent));
        if (cur_find)
          {
             evas_textblock_cursor_free(cur_find);
             cur_find = NULL;
          }
     }
}

static void
my_win_del(void *data __UNUSED__, Evas_Object *obj, void *event_info)
{
   (void) obj;
   (void) event_info;
   /* Reset the stuff that need reseting */
   if (cur_find)
     {
        evas_textblock_cursor_free(cur_find);
        cur_find = NULL;
     }
   search_win = NULL;
}

Evas_Object *
ui_find_dialog_open(Evas_Object *parent, Evas_Object *entry)
{
   Evas_Object *win, *bg, *bx, *lbl, *hbx, *btn;

   if (search_win)
     {
        evas_object_show(search_win);
        return search_win;
     }

   search_win = win = elm_win_add(parent, "find-replace", ELM_WIN_TOOLBAR);
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, _("Find & Replace"));
   evas_object_smart_callback_add(win, "delete,request", my_win_del, entry);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   hbx = elm_box_add(win);
   elm_box_padding_set(hbx, 15, 0);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   evas_object_size_hint_align_set(hbx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, 0.0);
   evas_object_show(hbx);
   elm_box_pack_end(bx, hbx);

   lbl = elm_label_add(win);
   elm_object_text_set(lbl, _("Search for:"));
   evas_object_size_hint_align_set(lbl, EVAS_HINT_FILL, 0.5);
   evas_object_size_hint_weight_set(lbl, 0.0, 0.0);
   elm_box_pack_end(hbx, lbl);
   evas_object_show(lbl);

   sent = elm_entry_add(win);
   elm_entry_scrollable_set(sent, EINA_TRUE);
   elm_entry_single_line_set(sent, EINA_TRUE);
   evas_object_size_hint_align_set(sent, EVAS_HINT_FILL, 0.0);
   evas_object_size_hint_weight_set(sent, EVAS_HINT_EXPAND, 0.0);
   elm_box_pack_end(hbx, sent);
   evas_object_show(sent);

   hbx = elm_box_add(win);
   elm_box_homogeneous_set(hbx, EINA_FALSE);
   elm_box_padding_set(hbx, 15, 0);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   evas_object_size_hint_align_set(hbx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, 0.0);
   evas_object_show(hbx);
   elm_box_pack_end(bx, hbx);

   lbl = elm_label_add(win);
   elm_object_text_set(lbl, _("Replace with:"));
   evas_object_size_hint_align_set(lbl, EVAS_HINT_FILL, 0.5);
   evas_object_size_hint_weight_set(lbl, 0.0, 0.0);
   elm_box_pack_end(hbx, lbl);
   evas_object_show(lbl);

   rent = elm_entry_add(win);
   elm_entry_scrollable_set(rent, EINA_TRUE);
   elm_entry_single_line_set(rent, EINA_TRUE);
   evas_object_size_hint_align_set(rent, EVAS_HINT_FILL, 0.0);
   evas_object_size_hint_weight_set(rent, EVAS_HINT_EXPAND, 0.0);
   elm_box_pack_end(hbx, rent);
   evas_object_show(rent);

   hbx = elm_box_add(win);
   elm_box_homogeneous_set(hbx, EINA_FALSE);
   elm_box_padding_set(hbx, 15, 0);
   elm_box_horizontal_set(hbx, EINA_TRUE);
   evas_object_size_hint_align_set(hbx, 1.0, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(hbx, 0.0, 0.0);
   evas_object_show(hbx);
   elm_box_pack_end(bx, hbx);

   btn = elm_button_add(win);
   elm_object_text_set(btn, _("Find"));
   evas_object_size_hint_align_set(btn, 1.0, 0.0);
   evas_object_size_hint_weight_set(btn, 0.0, 0.0);
   evas_object_show(btn);
   elm_box_pack_end(hbx, btn);
   evas_object_smart_callback_add(btn, "clicked", _find_clicked, entry);

   btn = elm_button_add(win);
   elm_object_text_set(btn, _("Replace"));
   evas_object_size_hint_align_set(btn, 1.0, 0.0);
   evas_object_size_hint_weight_set(btn, 0.0, 0.0);
   evas_object_show(btn);
   elm_box_pack_end(hbx, btn);
   evas_object_smart_callback_add(btn, "clicked", _replace_clicked, entry);

   /* Forcing it to be the min height. */
   evas_object_resize(win, 300, 1);
   evas_object_show(win);

   cur_find = NULL;
   return win;
}
