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
static Evas_Object *entry;
static const char *filename = NULL;
static int unsaved = 1;
static Elm_Object_Item *copy_item, *cut_item, *save_item, *paste_item, *undo_item, *redo_item;

/* Undo stack */
static Eina_List *undo_stack;
static Eina_List *undo_stack_ptr;
static Eina_List *last_saved_stack_ptr = NULL;
static Eina_Bool undo_stack_can_merge = EINA_FALSE;

static void print_usage(const char *bin);
static void editor_font_set(Evas_Object *ent, const char *font, int font_size);

static void
_init_entry(Evas_Object *ent)
{
   Elm_Entry_Change_Info *inf;
   editor_font_set(ent, _ent_cfg->font.name, _ent_cfg->font.size);

   /* Init the undo stack */
   EINA_LIST_FREE(undo_stack, inf)
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
   undo_stack = undo_stack_ptr = eina_list_append(undo_stack, NULL);
   last_saved_stack_ptr = undo_stack_ptr;

   elm_object_item_disabled_set(undo_item, EINA_TRUE);
   elm_object_item_disabled_set(redo_item, EINA_TRUE);
}

static void
_alert_if_need_saving(void (*done)(void *data), void *data)
{
   if (!elm_object_item_disabled_get(save_item))
     {
        ui_alert_need_saving(done, data);
     }
   else
     {
        done(data);
     }
}

static void
_sel_start(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   elm_object_item_disabled_set(copy_item, EINA_FALSE);
   elm_object_item_disabled_set(cut_item, EINA_FALSE);
}

static void
_sel_clear(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   elm_object_item_disabled_set(copy_item, EINA_TRUE);
   elm_object_item_disabled_set(cut_item, EINA_TRUE);
}

static void
_update_cur_file(const char *file, Evas_Object *win)
{
   const char *saving = (!elm_object_item_disabled_get(save_item)) ? "*" : "";
   eina_stringshare_replace(&filename, file);
     {
        char buf[1024];
        if (filename)
           snprintf(buf, sizeof(buf), _("%s%s - %s"), saving, filename, PACKAGE_NAME);
        else
           snprintf(buf, sizeof(buf), _("%sUntitled %d - %s"), saving, unsaved,
                 PACKAGE_NAME);

        elm_win_title_set(win, buf);
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
_cur_changed_manual(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   undo_stack_can_merge = EINA_FALSE;
}

static void
_update_undo_redo_items(Evas_Object *ent)
{
   elm_object_item_disabled_set(undo_item, !eina_list_next(undo_stack_ptr));
   elm_object_item_disabled_set(redo_item, !eina_list_prev(undo_stack_ptr));

   if (undo_stack_ptr == last_saved_stack_ptr)
     {
        elm_object_item_disabled_set(save_item, EINA_TRUE);
        _update_cur_file(filename, elm_object_top_widget_get(ent));
     }
   else if (elm_object_item_disabled_get(save_item))
     {
        elm_object_item_disabled_set(save_item, EINA_FALSE);
        _update_cur_file(filename, elm_object_top_widget_get(ent));
     }

}

static void
_undo_stack_add(Elm_Entry_Change_Info *_info)
{
   Elm_Entry_Change_Info *inf;

   undo_stack = eina_list_split_list(undo_stack, eina_list_prev(undo_stack_ptr),
         &undo_stack_ptr);

   EINA_LIST_FREE(undo_stack, inf)
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
        Elm_Entry_Change_Info *head_inf = eina_list_data_get(undo_stack_ptr);

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

        if (undo_stack_can_merge && (inf->insert == head_inf->insert))
           inf->merge = EINA_TRUE;

        undo_stack_ptr = eina_list_prepend(undo_stack_ptr, inf);
     }

   undo_stack = undo_stack_ptr;

   undo_stack_can_merge = EINA_TRUE;

   _update_undo_redo_items(entry);
}

static void
_undo_redo_do(Evas_Object *ent, Elm_Entry_Change_Info *inf, Eina_Bool undo)
{
   EINA_LOG_DBG("%s: %s", (undo) ? "Undo" : "Redo",
         inf->change.insert.content);

   if ((inf->insert && undo) || (!inf->insert && !undo))
     {
        const Evas_Object *tb = elm_entry_textblock_get(ent);
        Evas_Textblock_Cursor *mcur, *end;
        mcur = (Evas_Textblock_Cursor *) evas_object_textblock_cursor_get(tb);
        end = evas_object_textblock_cursor_new(tb);

        if (inf->insert)
          {
             elm_entry_cursor_pos_set(ent, inf->change.insert.pos);
             evas_textblock_cursor_pos_set(end, inf->change.insert.pos +
                   inf->change.insert.plain_length);
          }
        else
          {
             elm_entry_cursor_pos_set(ent, inf->change.del.start);
             evas_textblock_cursor_pos_set(end, inf->change.del.end);
          }

        evas_textblock_cursor_range_delete(mcur, end);
        evas_textblock_cursor_free(end);
        elm_entry_calc_force(ent);
     }
   else
     {
        if (inf->insert)
          {
             elm_entry_cursor_pos_set(ent, inf->change.insert.pos);
             elm_entry_entry_insert(ent, inf->change.insert.content);
          }
        else
          {
             size_t start;
             start = (inf->change.del.start < inf->change.del.end) ?
                inf->change.del.start : inf->change.del.end;

             elm_entry_cursor_pos_set(ent, start);
             elm_entry_entry_insert(ent, inf->change.insert.content);
             elm_entry_cursor_pos_set(ent, inf->change.del.end);
          }
     }

   /* No matter what, once we did an undo/redo we don't want to merge,
    * even if we got backt to the top of the stack. */
   undo_stack_can_merge = EINA_FALSE;
}

static void
_undo(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   /* In undo we care about the current item */
   Elm_Entry_Change_Info *inf = NULL;
   if (!eina_list_next(undo_stack_ptr))
      return;

   do
     {
        inf = eina_list_data_get(undo_stack_ptr);

        _undo_redo_do(data, inf, EINA_TRUE);

        if (eina_list_next(undo_stack_ptr))
          {
             undo_stack_ptr = eina_list_next(undo_stack_ptr);
          }
        else
          {
             break;
          }
     }
   while (inf && inf->merge);

   _update_undo_redo_items(data);
}

static void
_redo(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elm_Entry_Change_Info *inf = NULL;
   if (!eina_list_prev(undo_stack_ptr))
      return;

   do
     {
        if (eina_list_prev(undo_stack_ptr))
          {
             undo_stack_ptr = eina_list_prev(undo_stack_ptr);
             /* In redo we care about the NEW item */
             inf = eina_list_data_get(undo_stack_ptr);
             _undo_redo_do(data, inf, EINA_FALSE);
          }
        else
          {
             break;
          }

        /* Update inf to next for the condition. */
        if (eina_list_prev(undo_stack_ptr))
          {
             inf = eina_list_data_get(eina_list_prev(undo_stack_ptr));
          }
     }
   while (inf && inf->merge);

   _update_undo_redo_items(data);
}

static void
_ent_changed(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   elm_object_item_disabled_set(save_item, EINA_FALSE);
   _update_cur_file(filename, data);

   /* Undo/redo */
   _undo_stack_add(event_info);
}

static void
_load_to_entry(Evas_Object *ent, const char *file)
{
   if (file)
     {
        char *buf;

        if (plain_utf8)
           buf = _load_plain(file);
        else
           buf = _load_file(file);

        elm_object_text_set(ent, "");
        _init_entry(ent);
        elm_entry_entry_append(ent, buf);
        elm_object_item_disabled_set(save_item, EINA_TRUE);
        free(buf);
     }
   else
     {
        _init_entry(ent);
     }

   _update_cur_file(file, elm_object_top_widget_get(ent));
}

static void
_fs_open_done(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info)
{
   const char *selected = event_info;
   _load_to_entry(entry, selected);
}

static void
_save_do(const char *file, Evas_Object *ent)
{
   if (plain_utf8)
      _save_plain_utf8(file, elm_object_text_get(ent));
   else
      _save_markup_utf8(file, elm_object_text_get(ent));

   elm_object_item_disabled_set(save_item, EINA_TRUE);
   last_saved_stack_ptr = undo_stack_ptr;
   _update_cur_file(file, elm_object_top_widget_get(ent));
}

static void
_fs_save_done(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
      void *event_info)
{
   const char *selected = event_info;

   if (selected)
     {
        _save_do(selected, entry);
     }
}

static void
_open_do(void *data)
{
   ui_file_open_save_dialog_open(data, _fs_open_done, EINA_FALSE);
}

static void
_open(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   _alert_if_need_saving(_open_do, data);
}

static void
_save(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (filename)
     {
        _save_do(filename, entry);
     }
   else
     {
        ui_file_open_save_dialog_open(data, _fs_save_done, EINA_TRUE);
     }
}

static void
_new_do(void *data)
{
   elm_object_text_set(data, "");
   _init_entry(data);
   elm_object_item_disabled_set(save_item, EINA_TRUE);
   _update_cur_file(NULL, elm_object_top_widget_get(data));
}

static void
_new(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   _alert_if_need_saving(_new_do, data);
}

static void
_cut(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_entry_selection_cut(data);
}

static void
_copy(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_entry_selection_copy(data);
}

static void
_paste(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_entry_selection_paste(data);
}

static void
_find(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   ui_find_dialog_open(elm_object_top_widget_get(data), data);
}

static void
_font_settings(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   ui_font_dialog_open(elm_object_top_widget_get(data), data);
}


static void
_win_del_do(void *data __UNUSED__)
{
   elm_exit();
}

static void
my_win_del(void *data, Evas_Object *obj, void *event_info)
{
   (void) data;
   (void) obj;
   (void) event_info;
   _alert_if_need_saving(_win_del_do, data);
}

static void
editor_font_set(Evas_Object *ent, const char *font, int font_size)
{
#define _ENT_TAG_PREFIX "ecrire_format"
#define _ENT_FORMAT_PREFIX "+ " _ENT_TAG_PREFIX
   const Evas_Object *tb = elm_entry_textblock_get(ent);
   Evas_Textblock_Cursor *cur = evas_object_textblock_cursor_new(tb);
   evas_textblock_cursor_paragraph_first(cur);

   eina_stringshare_replace(&_ent_cfg->font.name, font);
   _ent_cfg->font.size = font_size;
   /* Remove first format node */
     {
        const Evas_Object_Textblock_Node_Format *fnode;
        const char *ftext;
        fnode = evas_textblock_node_format_first_get(tb);
        ftext = evas_textblock_node_format_text_get(fnode);
        if (ftext && !strncmp(ftext, _ENT_FORMAT_PREFIX,
                 strlen(_ENT_FORMAT_PREFIX)))
          {
             evas_textblock_node_format_remove_pair((Evas_Object *) tb,
                   (Evas_Object_Textblock_Node_Format *) fnode);
          }
     }

     {
        /* Make sure the temp buffers total len is less than 1024 */
        char buf[1024] = "<" _ENT_TAG_PREFIX;
        if (_ent_cfg->font.name)
          {
             char tfont[100];
             snprintf(tfont, sizeof(tfont), " font='%s'", _ent_cfg->font.name);
             strcat(buf, tfont);
          }

        if (_ent_cfg->font.size > 0)
          {
             char tfont[100];
             snprintf(tfont, sizeof(tfont), " font_size='%d'", _ent_cfg->font.size);
             strcat(buf, tfont);
          }
        strcat(buf, ">");

        evas_textblock_cursor_format_prepend(cur, buf);
        elm_entry_calc_force(ent);
     }

   evas_textblock_cursor_free(cur);
#undef _ENT_FORMAT_PREFIX
#undef _ENT_TAG_PREFIX
}

void
editor_font_choose(Evas_Object *ent, const char *font, int size)
{
   editor_font_set(ent, font, size);

   /* Save the font for future runs */
   ecrire_cfg_save();
}

#ifdef HAVE_ECORE_X
static Eina_Bool
_selection_notify(void *data __UNUSED__, int type __UNUSED__, void *_event)
{
   Ecore_X_Event_Fixes_Selection_Notify *event =
      (Ecore_X_Event_Fixes_Selection_Notify *) _event;

   if (!event)
      return ECORE_CALLBACK_PASS_ON;

   if (event->selection == ECORE_X_SELECTION_CLIPBOARD)
     {
        elm_object_item_disabled_set(paste_item,
              (event->reason != ECORE_X_OWNER_CHANGE_REASON_NEW_OWNER));
     }

   return ECORE_CALLBACK_PASS_ON;
}
#endif

int
main(int argc, char *argv[])
{
   Evas_Object *win, *bg, *tbar, *bx, *cur_info;
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

   if (optind < argc)
     {
        filename = eina_stringshare_add(argv[optind]);
     }

   EINA_LOG_DBG("Opening filename: '%s'", filename);

   win = elm_win_add(NULL, "editor", ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_FALSE);

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bx = elm_box_add(win);
   elm_win_resize_object_add(win, bx);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx);

   tbar = elm_toolbar_add(win);
   elm_toolbar_homogeneous_set(tbar, 0);
   elm_toolbar_shrink_mode_set(tbar, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_no_select_mode_set(tbar, EINA_TRUE);
   elm_toolbar_align_set(tbar, 0.0);
   evas_object_size_hint_weight_set(tbar, 0.0, 0.0);
   evas_object_size_hint_align_set(tbar, EVAS_HINT_FILL, 0.0);

   elm_box_pack_end(bx, tbar);
   evas_object_show(tbar);

   entry = elm_entry_add(win);
   elm_entry_scrollable_set(entry, EINA_TRUE);
   elm_entry_line_wrap_set(entry, _ent_cfg->wrap_type);
   elm_entry_cnp_textonly_set(entry, EINA_TRUE);
   evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(bx, entry);
   evas_object_show(entry);

   cur_info = elm_label_add(win);
   _cur_changed(cur_info, entry, NULL);
   evas_object_size_hint_align_set(cur_info, 1.0, 0.5);
   evas_object_size_hint_weight_set(cur_info, EVAS_HINT_EXPAND, 0.0);
   elm_box_pack_end(bx, cur_info);
   evas_object_show(cur_info);

   evas_object_smart_callback_add(entry, "cursor,changed", _cur_changed,
         cur_info);
   evas_object_smart_callback_add(entry, "cursor,changed,manual",
         _cur_changed_manual,
         cur_info);
   evas_object_smart_callback_add(entry, "changed,user", _ent_changed, win);
   evas_object_smart_callback_add(entry, "undo,request", _undo, entry);
   evas_object_smart_callback_add(entry, "redo,request", _redo, entry);
   evas_object_smart_callback_add(entry, "selection,start", _sel_start, win);
   evas_object_smart_callback_add(entry, "selection,cleared", _sel_clear, win);

   elm_toolbar_item_append(tbar, "document-new", _("New"), _new, entry);
   elm_toolbar_item_append(tbar, "document-open", _("Open"), _open, win);
   save_item =
      elm_toolbar_item_append(tbar, "document-save", _("Save"), _save, win);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   undo_item =
      elm_toolbar_item_append(tbar, "edit-undo", _("Undo"), _undo, entry);
   redo_item =
      elm_toolbar_item_append(tbar, "edit-redo", _("Redo"), _redo, entry);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   cut_item = elm_toolbar_item_append(tbar, "edit-cut", _("Cut"), _cut, entry);
   copy_item =
      elm_toolbar_item_append(tbar, "edit-copy", _("Copy"), _copy, entry);
   paste_item =
      elm_toolbar_item_append(tbar, "edit-paste", _("Paste"), _paste, entry);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   elm_toolbar_item_append(tbar, "edit-find-replace", _("Find & Replace"),
         _find, entry);
   elm_toolbar_item_separator_set(
         elm_toolbar_item_append(tbar, "", "", NULL, NULL), EINA_TRUE);
   elm_toolbar_item_append(tbar, "preferences-system", _("Settings"),
         _font_settings, entry);

#ifdef HAVE_ECORE_X
   if (!ecore_x_selection_owner_get(ECORE_X_ATOM_SELECTION_CLIPBOARD))
     {
        elm_object_item_disabled_set(paste_item, EINA_TRUE);
     }

   ecore_x_fixes_selection_notification_request(ECORE_X_ATOM_SELECTION_CLIPBOARD);
   ecore_event_handler_add(ECORE_X_EVENT_FIXES_SELECTION_NOTIFY,
         _selection_notify, NULL);
#endif

   /* We don't have a selection when we start, make the items disabled */
   elm_object_item_disabled_set(copy_item, EINA_TRUE);
   elm_object_item_disabled_set(cut_item, EINA_TRUE);
   elm_object_item_disabled_set(save_item, EINA_TRUE);

   evas_object_resize(win, w, h);

   evas_object_smart_callback_add(win, "delete,request", my_win_del, entry);
   evas_object_show(win);

   _load_to_entry(entry, filename);

   elm_object_focus_set(win, EINA_TRUE);

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
