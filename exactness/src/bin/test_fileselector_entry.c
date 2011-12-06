#include <Elementary.h>
#include "tsuite.h"

static void
_file_chosen(void            *data,
             Evas_Object *obj __UNUSED__,
             void            *event_info)
{
   Evas_Object *entry = data;
   char *file = event_info;
   elm_entry_entry_set(entry, file);
   printf("File chosen: %s\n", file);
}

static void
_inwin_mode_toggle(void            *data,
                   Evas_Object *obj __UNUSED__,
                   void *event_info __UNUSED__)
{
   Evas_Object *fs_en = data;
   Eina_Bool value = elm_fileselector_entry_inwin_mode_get(fs_en);
   elm_fileselector_entry_inwin_mode_set(fs_en, !value);
   printf("Inwin mode set to: %s\n", value ? "false" : "true");
}

static void
_folder_only_toggle(void            *data,
                    Evas_Object *obj __UNUSED__,
                    void *event_info __UNUSED__)
{
   Evas_Object *fs_en = data;
   Evas_Object *ic = elm_fileselector_entry_button_icon_get(fs_en);
   Eina_Bool value = elm_fileselector_entry_folder_only_get(fs_en);
   elm_fileselector_entry_folder_only_set(fs_en, !value);
   printf("Folder only flag set to: %s\n", value ? "false" : "true");
   if (!value)
     {
        elm_icon_standard_set(ic, "folder");
        elm_object_text_set(fs_en, "Select a folder");
     }
   else
     {
        elm_icon_standard_set(ic, "file");
        elm_object_text_set(fs_en, "Select a file");
     }
}

static void
_expandable_toggle(void            *data,
                   Evas_Object *obj __UNUSED__,
                   void *event_info __UNUSED__)
{
   Evas_Object *fs_en = data;
   Eina_Bool value = elm_fileselector_entry_expandable_get(fs_en);
   elm_fileselector_entry_expandable_set(fs_en, !value);
   printf("Expandable flag set to: %s\n", value ? "false" : "true");
}

static void
_disabled_toggle(void            *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Evas_Object *fs_en = data;
   Eina_Bool value = elm_object_disabled_get(fs_en);
   elm_object_disabled_set(fs_en, !value);
   printf("Disabled flag set to: %s\n", value ? "false" : "true");
}

enum _api_state
{
   ICON_UNSET,
   WINDOW_TITLE_SET,
   API_STATE_LAST
};
typedef enum _api_state api_state;

static void
set_api_state(api_data *api)
{
   const Eina_List *items = elm_box_children_get(api->data);
   if(!eina_list_count(items))
     return;

   /* Get first item of list of vbox children */
   Evas_Object *fs_bt = eina_list_nth(items, 0);

   /* use elm_box_children_get() to get list of children */
   switch(api->state)
     { /* Put all api-changes under switch */
      case ICON_UNSET:
         elm_fileselector_entry_button_icon_unset(fs_bt);
         break;

      case WINDOW_TITLE_SET:
         elm_fileselector_entry_window_title_set(fs_bt, "Custom title from API");
         break;

      case API_STATE_LAST:

         break;
      default:
         return;
     }
}

static void
_api_bt_clicked(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{  /* Will add here a SWITCH command containing code to modify test-object */
   /* in accordance a->state value. */
   api_data *a = data;
   char str[128];

   printf("clicked event on API Button: api_state=<%d>\n", a->state);
   set_api_state(a);
   a->state++;
   sprintf(str, "Next API function (%u)", a->state);
   elm_object_text_set(a->bt, str);
   elm_object_disabled_set(a->bt, a->state == API_STATE_LAST);
}

static void
create_dir_struct(void)
{
   FILE *fp;
   mkdir("/tmp/test_fs_bt", S_IRWXU);
   fp = fopen("/tmp/test_fs_bt/a_file.txt", "w");
   if (fp) fclose(fp);
   fp = fopen("/tmp/test_fs_bt/k_file.txt", "w");
   if (fp) fclose(fp);
   fp = fopen("/tmp/test_fs_bt/m_file.txt", "w");
   if (fp) fclose(fp);

   mkdir("/tmp/test_fs_bt/a_subdir", S_IRWXU);
   fp = fopen("/tmp/test_fs_bt/a_subdir/d_sub_file.txt", "w");
   if (fp) fclose(fp);
   fp = fopen("/tmp/test_fs_bt/a_subdir/j_sub_file.txt", "w");
   if (fp) fclose(fp);
}

TEST_START(test_fileselector_entry)
{
   Evas_Object *bg, *vbox, *hbox, *bxx, *ic, *bt, *fs_en, *en, *lb;

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   bxx = elm_box_add(win);
   elm_win_resize_object_add(win, bxx);
   evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bxx);

   vbox = elm_box_add(win);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   api->data = vbox;
   evas_object_show(vbox);

   api->bt = elm_button_add(win);
   elm_object_text_set(api->bt, "Next API function");
   evas_object_smart_callback_add(api->bt, "clicked", _api_bt_clicked, (void *) api);
   elm_box_pack_end(bxx, api->bt);
   elm_object_disabled_set(api->bt, api->state == API_STATE_LAST);
   evas_object_show(api->bt);

   elm_box_pack_end(bxx, vbox);

   create_dir_struct(); /* Create a dir struct in /tmp */

   /* file selector entry */
   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "file");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   fs_en = elm_fileselector_entry_add(win);
   elm_fileselector_entry_path_set(fs_en, "/tmp/test_fs_bt");
   elm_object_text_set(fs_en, "Select a file");
   elm_fileselector_entry_button_icon_set(fs_en, ic);
   evas_object_size_hint_weight_set(fs_en, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(fs_en, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_box_pack_end(vbox, fs_en);
   evas_object_show(fs_en);
   evas_object_show(ic);

   /* attribute setting buttons */
   hbox = elm_box_add(win);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   elm_box_pack_end(vbox, hbox);
   evas_object_show(hbox);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Toggle inwin mode");
   evas_object_smart_callback_add(bt, "clicked", _inwin_mode_toggle, fs_en);
   elm_box_pack_end(hbox, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Toggle folder only mode");
   evas_object_smart_callback_add(bt, "clicked", _folder_only_toggle, fs_en);
   elm_box_pack_end(hbox, bt);
   evas_object_show(bt);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Toggle expandable mode");
   evas_object_smart_callback_add(bt, "clicked", _expandable_toggle, fs_en);
   elm_box_pack_end(hbox, bt);
   evas_object_show(bt);

   lb = elm_label_add(win);
   elm_object_text_set(lb, "Last selection:");
   elm_box_pack_end(vbox, lb);
   evas_object_show(lb);

   hbox = elm_box_add(win);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   elm_box_pack_end(vbox, hbox);
   evas_object_show(hbox);

   bt = elm_button_add(win);
   elm_object_text_set(bt, "Toggle disabled");
   evas_object_smart_callback_add(bt, "clicked", _disabled_toggle, fs_en);
   elm_box_pack_end(hbox, bt);
   evas_object_show(bt);

   en = elm_entry_add(win);
   elm_entry_line_wrap_set(en, ELM_WRAP_NONE);
   elm_entry_editable_set(en, EINA_FALSE);
   evas_object_smart_callback_add(fs_en, "file,chosen", _file_chosen, en);
   elm_box_pack_end(vbox, en);
   evas_object_show(en);

   evas_object_resize(win, 400, 500);
   evas_object_show(win);
}
TEST_END
