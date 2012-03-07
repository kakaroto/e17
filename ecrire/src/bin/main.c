#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_ECORE_X
# include <Ecore_X.h>
#endif

#include <Elementary.h>

#include "mess_header.h"
#include "cfg.h"
#include "ui/ui.h"

static Eina_Unicode plain_utf8 = EINA_TRUE;
static Ecrire_Entry *main_ec_ent;

static void print_usage(const char *bin);
static void editor_font_set(Ecrire_Entry *ent, const char *font, int font_size);

static void
_init_entry(Ecrire_Entry *ent)
{
   Elm_Entry_Change_Info *inf;
   editor_font_set(ent, _ent_cfg->font.name, _ent_cfg->font.size);

   /* Init the undo stack */
   EINA_LIST_FREE(ent->undo_stack, inf)
     {
        if (inf)
          {
             if (inf->insert)
               {
                  eina_stringshare_del(inf->change.insert.content);
               }
             else
               {
                  eina_stringshare_del(inf->change.del.content);
               }
             free(inf);
          }
     }
   ent->undo_stack = ent->undo_stack_ptr =
      eina_list_append(ent->undo_stack, NULL);
   ent->last_saved_stack_ptr = ent->undo_stack_ptr;
   ent->undo_stack_can_merge = EINA_FALSE;

   elm_object_item_disabled_set(ent->undo_item, EINA_TRUE);
   elm_object_item_disabled_set(ent->redo_item, EINA_TRUE);
}

static void
_alert_if_need_saving(void (*done)(void *data), Ecrire_Entry *ent)
{
   if (!elm_object_item_disabled_get(ent->save_item))
     {
        ui_alert_need_saving(ent->entry, done, ent);
     }
   else
     {
        done(ent);
     }
}

static void
_sel_start(void *data, Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   elm_object_item_disabled_set(ent->copy_item, EINA_FALSE);
   elm_object_item_disabled_set(ent->cut_item, EINA_FALSE);
}

static void
_sel_clear(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   elm_object_item_disabled_set(ent->copy_item, EINA_TRUE);
   elm_object_item_disabled_set(ent->cut_item, EINA_TRUE);
}

static void
_update_cur_file(const char *file, Ecrire_Entry *ent)
{
   const char *saving = (!elm_object_item_disabled_get(ent->save_item)) ?
      "*" : "";
   eina_stringshare_replace(&ent->filename, file);
     {
        char buf[1024];
        if (ent->filename)
           snprintf(buf, sizeof(buf), _("%s%s - %s"), saving, ent->filename,
                 PACKAGE_NAME);
        else
           snprintf(buf, sizeof(buf), _("%sUntitled %d - %s"), saving,
                 ent->unsaved, PACKAGE_NAME);

        elm_win_title_set(ent->win, buf);
     }
}

static void
_cur_changed(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   char buf[50];
   int line;
   int col;
   const Evas_Object *tb = elm_entry_textblock_get(obj);
   const Evas_Textblock_Cursor *mcur = evas_object_textblock_cursor_get(tb);
   Evas_Textblock_Cursor *cur = evas_object_textblock_cursor_new(tb);
   line =
      evas_textblock_cursor_line_geometry_get(mcur, NULL, NULL, NULL, NULL) + 1;
   evas_textblock_cursor_copy(mcur, cur);
   evas_textblock_cursor_line_char_first(cur);
   col = evas_textblock_cursor_pos_get(mcur) -
      evas_textblock_cursor_pos_get(cur) + 1;
   evas_textblock_cursor_free(cur);

   snprintf(buf, sizeof(buf), _("Ln %d, Col %d"), line, col);
   elm_object_text_set(data, buf);
}

static void
_cur_changed_manual(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   ent->undo_stack_can_merge = EINA_FALSE;
}

static void
_update_undo_redo_items(Ecrire_Entry *ent)
{
   elm_object_item_disabled_set(ent->undo_item,
         !eina_list_next(ent->undo_stack_ptr));
   elm_object_item_disabled_set(ent->redo_item,
         !eina_list_prev(ent->undo_stack_ptr));

   if (ent->undo_stack_ptr == ent->last_saved_stack_ptr)
     {
        elm_object_item_disabled_set(ent->save_item, EINA_TRUE);
        _update_cur_file(ent->filename, ent);
     }
   else if (elm_object_item_disabled_get(ent->save_item))
     {
        elm_object_item_disabled_set(ent->save_item, EINA_FALSE);
        _update_cur_file(ent->filename, ent);
     }

}

static void
_undo_stack_add(Ecrire_Entry *ent, Elm_Entry_Change_Info *_info)
{
   Elm_Entry_Change_Info *inf;

   ent->undo_stack = eina_list_split_list(ent->undo_stack, eina_list_prev(ent->undo_stack_ptr),
         &ent->undo_stack_ptr);

   EINA_LIST_FREE(ent->undo_stack, inf)
     {
        if (inf->insert)
          {
             eina_stringshare_del(inf->change.insert.content);
          }
        else
          {
             eina_stringshare_del(inf->change.del.content);
          }
        free(inf);
     }

   /* FIXME: Do a smarter merge, actually merge the structures, not just
    * mark them to be merged. */
#if 0
   inf = (Elm_Entry_Change_Info *) eina_list_data_get(undo_stack_ptr);
   /* If true, we should merge with the current top */
   if (undo_stack_can_merge && (_info->insert == inf->insert))
     {
     }
   else
#endif
     {
        Elm_Entry_Change_Info *head_inf = eina_list_data_get(ent->undo_stack_ptr);

        inf = calloc(1, sizeof(*inf));
        memcpy(inf, _info, sizeof(*inf));
        if (inf->insert)
          {
             eina_stringshare_ref(inf->change.insert.content);
          }
        else
          {
             eina_stringshare_ref(inf->change.del.content);
          }

        if (ent->undo_stack_can_merge && (inf->insert == head_inf->insert))
           inf->merge = EINA_TRUE;

        ent->undo_stack_ptr = eina_list_prepend(ent->undo_stack_ptr, inf);
     }

   ent->undo_stack = ent->undo_stack_ptr;

   ent->undo_stack_can_merge = EINA_TRUE;

   _update_undo_redo_items(ent);
}

static void
_undo_redo_do(Ecrire_Entry *ent, Elm_Entry_Change_Info *inf, Eina_Bool undo)
{
   EINA_LOG_DBG("%s: %s", (undo) ? "Undo" : "Redo",
         inf->change.insert.content);

   if ((inf->insert && undo) || (!inf->insert && !undo))
     {
        const Evas_Object *tb = elm_entry_textblock_get(ent->entry);
        Evas_Textblock_Cursor *mcur, *end;
        mcur = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get(tb);
        end = evas_object_textblock_cursor_new(tb);

        if (inf->insert)
          {
             elm_entry_cursor_pos_set(ent->entry, inf->change.insert.pos);
             evas_textblock_cursor_pos_set(end, inf->change.insert.pos +
                   inf->change.insert.plain_length);
          }
        else
          {
             elm_entry_cursor_pos_set(ent->entry, inf->change.del.start);
             evas_textblock_cursor_pos_set(end, inf->change.del.end);
          }

        evas_textblock_cursor_range_delete(mcur, end);
        evas_textblock_cursor_free(end);
        elm_entry_calc_force(ent->entry);
     }
   else
     {
        if (inf->insert)
          {
             elm_entry_cursor_pos_set(ent->entry, inf->change.insert.pos);
             elm_entry_entry_insert(ent->entry, inf->change.insert.content);
          }
        else
          {
             size_t start;
             start = (inf->change.del.start < inf->change.del.end) ?
                inf->change.del.start : inf->change.del.end;

             elm_entry_cursor_pos_set(ent->entry, start);
             elm_entry_entry_insert(ent->entry, inf->change.insert.content);
             elm_entry_cursor_pos_set(ent->entry, inf->change.del.end);
          }
     }

   /* No matter what, once we did an undo/redo we don't want to merge,
    * even if we got backt to the top of the stack. */
   ent->undo_stack_can_merge = EINA_FALSE;
}

static void
_undo(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   /* In undo we care about the current item */
   Ecrire_Entry *ent = data;
   Elm_Entry_Change_Info *inf = NULL;
   if (!eina_list_next(ent->undo_stack_ptr))
      return;

   do
     {
        inf = eina_list_data_get(ent->undo_stack_ptr);

        _undo_redo_do(ent, inf, EINA_TRUE);

        if (eina_list_next(ent->undo_stack_ptr))
          {
             ent->undo_stack_ptr = eina_list_next(ent->undo_stack_ptr);
          }
        else
          {
             break;
          }
     }
   while (inf && inf->merge);

   _update_undo_redo_items(ent);
}

static void
_redo(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   Elm_Entry_Change_Info *inf = NULL;
   if (!eina_list_prev(ent->undo_stack_ptr))
      return;

   do
     {
        if (eina_list_prev(ent->undo_stack_ptr))
          {
             ent->undo_stack_ptr = eina_list_prev(ent->undo_stack_ptr);
             /* In redo we care about the NEW item */
             inf = eina_list_data_get(ent->undo_stack_ptr);
             _undo_redo_do(ent, inf, EINA_FALSE);
          }
        else
          {
             break;
          }

        /* Update inf to next for the condition. */
        if (eina_list_prev(ent->undo_stack_ptr))
          {
             inf = eina_list_data_get(eina_list_prev(ent->undo_stack_ptr));
          }
     }
   while (inf && inf->merge);

   _update_undo_redo_items(ent);
}

static void
_ent_changed(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ecrire_Entry *ent = data;
   elm_object_item_disabled_set(ent->save_item, EINA_FALSE);
   _update_cur_file(ent->filename, ent);

   /* Undo/redo */
   _undo_stack_add(ent, event_info);
}

static void
_load_to_entry(Ecrire_Entry *ent, const char *file)
{
   if (file)
     {
        char *buf;

        if (plain_utf8)
           buf = _load_plain(file);
        else
           buf = _load_file(file);

        elm_object_text_set(ent->entry, "");
        _init_entry(ent);
        elm_entry_entry_append(ent->entry, buf);
        elm_object_item_disabled_set(ent->save_item, EINA_TRUE);
        free(buf);
     }
   else
     {
        _init_entry(ent);
     }

   _update_cur_file(file, ent);
}

static void
_fs_open_done(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info)
{
   const char *selected = event_info;
   if (selected)
      _load_to_entry(main_ec_ent, selected);
}

static void
_save_do(const char *file, Ecrire_Entry *ent)
{
   if (plain_utf8)
      _save_plain_utf8(file, elm_object_text_get(ent->entry));
   else
      _save_markup_utf8(file, elm_object_text_get(ent->entry));

   elm_object_item_disabled_set(ent->save_item, EINA_TRUE);
   ent->last_saved_stack_ptr = ent->undo_stack_ptr;
   _update_cur_file(file, ent);
}

static void
_fs_save_done(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info)
{
   const char *selected = event_info;

   if (selected)
     {
        _save_do(selected, main_ec_ent);
     }
}

static void
_open_do(void *data)
{
   Ecrire_Entry *ent = data;
   ui_file_open_save_dialog_open(ent->win, _fs_open_done, EINA_FALSE);
}

static void
_open(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   _alert_if_need_saving(_open_do, ent);
}

void
editor_save(Ecrire_Entry *ent)
{
   if (ent->filename)
     {
        _save_do(ent->filename, ent);
     }
   else
     {
        ui_file_open_save_dialog_open(ent->win, _fs_save_done, EINA_TRUE);
     }
}

static void
_save(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   editor_save(ent);
}

static void
_save_as(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   ui_file_open_save_dialog_open(ent->win, _fs_save_done, EINA_TRUE);
}

static void
_new_do(void *data)
{
   Ecrire_Entry *ent = data;
   elm_object_text_set(ent->entry, "");
   _init_entry(ent);
   elm_object_item_disabled_set(ent->save_item, EINA_TRUE);
   _update_cur_file(NULL, ent);
}

static void
_new(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   _alert_if_need_saving(_new_do, ent);
}

static void
_cut(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   elm_entry_selection_cut(ent->entry);
}

static void
_copy(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   elm_entry_selection_copy(ent->entry);
}

static void
_paste(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   elm_entry_selection_paste(ent->entry);
}

static void
_find(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   ui_find_dialog_open(ent->win, ent);
}

static void
_font_settings(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ecrire_Entry *ent = data;
   ui_font_dialog_open(elm_object_top_widget_get(data), ent, _ent_cfg->font.name, _ent_cfg->font.size);
}


static void
_win_del_do(void *data __UNUSED__)
{
   elm_exit();
}

static void
my_win_del(void *data, Evas_Object *obj, void *event_info)
{
   Ecrire_Entry *ent = data;
   (void) data;
   (void) obj;
   (void) event_info;
   _alert_if_need_saving(_win_del_do, ent);
}

static void
editor_font_set(Ecrire_Entry *ent, const char *font, int font_size)
{
   const Evas_Object *tb = elm_entry_textblock_get(ent->entry);
   Eina_Strbuf *sbuf;

   eina_stringshare_replace(&_ent_cfg->font.name, font);
   _ent_cfg->font.size = font_size;

   sbuf = eina_strbuf_new();

   if (_ent_cfg->font.name)
     {
        eina_strbuf_append_printf(sbuf, "font=\\'%s\\'", _ent_cfg->font.name);
     }

   if (_ent_cfg->font.size > 0)
     {
        eina_strbuf_append_printf(sbuf, " font_size=\\'%d\\'",
              _ent_cfg->font.size);
     }

   if (eina_strbuf_length_get(sbuf) > 0)
     {
        Evas_Textblock_Style *ts = evas_textblock_style_new();

        eina_strbuf_prepend(sbuf, "DEFAULT='");
        eina_strbuf_append(sbuf, "'");
        evas_textblock_style_set(ts, eina_strbuf_string_get(sbuf));

        evas_object_textblock_style_user_push((Evas_Object *) tb, ts);
     }
   else
     {
        evas_object_textblock_style_user_pop((Evas_Object *) tb);
     }

   elm_entry_calc_force(ent->entry);

   eina_strbuf_free(sbuf);
}

void
editor_font_choose(Ecrire_Entry *ent, const char *font, int size)
{
   editor_font_set(ent, font, size);

   /* Save the font for future runs */
   ecrire_cfg_save();
}

#ifdef HAVE_ECORE_X
static Eina_Bool
_selection_notify(void *data, int type __UNUSED__, void *_event)
{
   Ecrire_Entry *ent = data;
   Ecore_X_Event_Fixes_Selection_Notify *event =
      (Ecore_X_Event_Fixes_Selection_Notify *) _event;

   if (!event)
      return ECORE_CALLBACK_PASS_ON;

   if (event->selection == ECORE_X_SELECTION_CLIPBOARD)
     {
        elm_object_item_disabled_set(ent->paste_item,
              (event->reason != ECORE_X_OWNER_CHANGE_REASON_NEW_OWNER));
     }

   return ECORE_CALLBACK_PASS_ON;
}
#endif

int
main(int argc, char *argv[])
{
   Evas_Object *bg, *tbar, *bx, *cur_info;
   Evas_Coord w = 600, h = 600;
   int c;

   opterr = 0;

   while ((c = getopt (argc, argv, "")) != -1)
     {
        switch (c)
          {
           case '?':
              print_usage(argv[0]);
              if (isprint (optopt))
                {
                   EINA_LOG_ERR("Unknown option or requires an argument `-%c'.",
                         optopt);
                }
              else
                {
                   EINA_LOG_ERR("Unknown option character `\\x%x'.", optopt);
                }
              return 1;
              break;
           default:
              abort();
          }
     }

   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALE_DIR);
   textdomain(PACKAGE);

   elm_init(argc, argv);

   ecrire_cfg_init(PACKAGE_NAME);
   ecrire_cfg_load();

   main_ec_ent = calloc(1, sizeof(*main_ec_ent));
   main_ec_ent->unsaved = 1;
   main_ec_ent->filename = NULL;
   main_ec_ent->last_saved_stack_ptr = NULL;
   main_ec_ent->undo_stack_can_merge = EINA_FALSE;

   if (optind < argc)
     {
        main_ec_ent->filename = eina_stringshare_add(argv[optind]);
     }

   EINA_LOG_DBG("Opening filename: '%s'", main_ec_ent->filename);

   main_ec_ent->win = elm_win_add(NULL, "editor", ELM_WIN_BASIC);
   elm_win_autodel_set(main_ec_ent->win, EINA_FALSE);

   bg = elm_bg_add(main_ec_ent->win);
   elm_win_resize_object_add(main_ec_ent->win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(main_ec_ent->win);
   elm_win_resize_object_add(main_ec_ent->win, bx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   tbar = elm_toolbar_add(main_ec_ent->win);
   elm_toolbar_homogeneous_set(tbar, 0);
   elm_toolbar_shrink_mode_set(tbar, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_no_select_mode_set(tbar, EINA_TRUE);
   elm_toolbar_align_set(tbar, 0.0);
   evas_object_size_hint_weight_set(tbar, 0.0, 0.0);
   evas_object_size_hint_align_set(tbar, EVAS_HINT_FILL, 0.0);

   elm_box_pack_end(bx, tbar);
   evas_object_show(tbar);

   main_ec_ent->entry = elm_entry_add(main_ec_ent->win);
   elm_entry_scrollable_set(main_ec_ent->entry, EINA_TRUE);
   elm_entry_line_wrap_set(main_ec_ent->entry, _ent_cfg->wrap_type);
   elm_entry_cnp_mode_set(main_ec_ent->entry, ELM_CNP_MODE_PLAINTEXT);
   evas_object_size_hint_align_set(main_ec_ent->entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(main_ec_ent->entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, main_ec_ent->entry);
   evas_object_show(main_ec_ent->entry);

   cur_info = elm_label_add(main_ec_ent->win);
   _cur_changed(cur_info, main_ec_ent->entry, NULL);
   evas_object_size_hint_align_set(cur_info, 1.0, 0.5);
   evas_object_size_hint_weight_set(cur_info, EVAS_HINT_EXPAND, 0.0);
   elm_box_pack_end(bx, cur_info);
   evas_object_show(cur_info);

   evas_object_smart_callback_add(main_ec_ent->entry, "cursor,changed",
         _cur_changed, cur_info);
   evas_object_smart_callback_add(main_ec_ent->entry, "cursor,changed,manual",
         _cur_changed_manual, main_ec_ent);
   evas_object_smart_callback_add(main_ec_ent->entry, "changed,user", _ent_changed, main_ec_ent);
   evas_object_smart_callback_add(main_ec_ent->entry, "undo,request", _undo, main_ec_ent);
   evas_object_smart_callback_add(main_ec_ent->entry, "redo,request", _redo, main_ec_ent);
   evas_object_smart_callback_add(main_ec_ent->entry, "selection,start", _sel_start, main_ec_ent);
   evas_object_smart_callback_add(main_ec_ent->entry, "selection,cleared", _sel_clear, main_ec_ent);

   elm_toolbar_item_append(tbar, "document-new", _("New"), _new, main_ec_ent);
   elm_toolbar_item_append(tbar, "document-open", _("Open"), _open, main_ec_ent);
   main_ec_ent->save_item =
      elm_toolbar_item_append(tbar, "document-save", _("Save"), _save, main_ec_ent);
   elm_toolbar_item_append(tbar, "document-save-as", _("Save As"), _save_as,
            main_ec_ent);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   main_ec_ent->undo_item =
      elm_toolbar_item_append(tbar, "edit-undo", _("Undo"), _undo, main_ec_ent);
   main_ec_ent->redo_item =
      elm_toolbar_item_append(tbar, "edit-redo", _("Redo"), _redo, main_ec_ent);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   main_ec_ent->cut_item = elm_toolbar_item_append(tbar, "edit-cut", _("Cut"), _cut, main_ec_ent);
   main_ec_ent->copy_item =
      elm_toolbar_item_append(tbar, "edit-copy", _("Copy"), _copy, main_ec_ent);
   main_ec_ent->paste_item =
      elm_toolbar_item_append(tbar, "edit-paste", _("Paste"), _paste, main_ec_ent);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   elm_toolbar_item_append(tbar, "edit-find-replace", _("Find & Replace"),
         _find, main_ec_ent);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   elm_toolbar_item_append(tbar, "preferences-system", _("Settings"),
         _font_settings, main_ec_ent);

#ifdef HAVE_ECORE_X
   if (!ecore_x_selection_owner_get(ECORE_X_ATOM_SELECTION_CLIPBOARD))
     {
        elm_object_item_disabled_set(main_ec_ent->paste_item, EINA_TRUE);
     }

   ecore_x_fixes_selection_notification_request(ECORE_X_ATOM_SELECTION_CLIPBOARD);
   ecore_event_handler_add(ECORE_X_EVENT_FIXES_SELECTION_NOTIFY,
         _selection_notify, main_ec_ent);
#endif

   /* We don't have a selection when we start, make the items disabled */
   elm_object_item_disabled_set(main_ec_ent->copy_item, EINA_TRUE);
   elm_object_item_disabled_set(main_ec_ent->cut_item, EINA_TRUE);
   elm_object_item_disabled_set(main_ec_ent->save_item, EINA_TRUE);

   evas_object_resize(main_ec_ent->win, w, h);

   evas_object_smart_callback_add(main_ec_ent->win, "delete,request", my_win_del, main_ec_ent);
   evas_object_show(main_ec_ent->win);

   _load_to_entry(main_ec_ent, main_ec_ent->filename);

   elm_object_focus_set(main_ec_ent->win, EINA_TRUE);

   elm_run();

   ecrire_cfg_shutdown();

   return 0;
}

static void
print_usage(const char *bin)
{
   fprintf(stderr,
         "Usage: %s [filename]\n", bin);
}
