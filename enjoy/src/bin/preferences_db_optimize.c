#include "private.h"

static void
preferences_db_optimize_do(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *frame = data;
   DB *db = enjoy_db_get();
   elm_naviframe_item_pop(frame);

   db_files_cleanup(db);
   db_album_covers_cleanup(db);
   db_vacuum(db);
}

static const char *
preferences_db_optimize_categegory_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Database";
}

static const char *
preferences_db_optimize_label_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Cleanup and optimize";
}

static Eina_Bool
preferences_db_optimize_activate(Enjoy_Preferences_Plugin *p __UNUSED__, Evas_Object *naviframe, Evas_Object **prev_btn __UNUSED__, Evas_Object **next_btn __UNUSED__, Evas_Object **content, Eina_Bool *auto_prev_btn __UNUSED__)
{
   Evas_Object *box, *bt;

   box = elm_box_add(naviframe);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Yes, cleanup and optimize the database!");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_db_optimize_do, naviframe);

   *content = box;
   return EINA_TRUE;
}

static Enjoy_Preferences_Plugin *preferences_db_optimize_plugin = NULL;
Eina_Bool
preferences_db_optimize_register(void)
{
   static const Enjoy_Preferences_Plugin_Api api = {
     ENJOY_PREFERENCES_PLUGIN_API_VERSION,
     preferences_db_optimize_categegory_get,
     preferences_db_optimize_label_get,
     preferences_db_optimize_activate
   };
   preferences_db_optimize_plugin = enjoy_preferences_plugin_register
     (&api, ENJOY_PLUGIN_PRIORITY_NORMAL);
   return !!preferences_db_optimize_plugin;
}

void
preferences_db_optimize_unregister(void)
{
   enjoy_preferences_plugin_unregister(preferences_db_optimize_plugin);
   preferences_db_optimize_plugin = NULL;
}
