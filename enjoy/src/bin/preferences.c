#include "private.h"

static void
preferences_db_clear_do(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *frame = data;
   elm_naviframe_item_pop(frame);
   enjoy_db_clear();
}

static void
preferences_db_clear(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *frame = data;
   Evas_Object *box, *bt;

   box = elm_box_add(frame);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Yes, clear the database!");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   elm_naviframe_item_push(frame, "Clear Database?", NULL, NULL, box, NULL);
   elm_genlist_item_selected_set(it, EINA_FALSE);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_db_clear_do, frame);
}

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

static void
preferences_db_optimize(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *frame = data;
   Evas_Object *box, *bt;

   box = elm_box_add(frame);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Yes, cleanup and optimize the database!");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   elm_naviframe_item_push(frame, "Cleanup and Optimize Database?",
                           NULL, NULL, box, NULL);
   elm_genlist_item_selected_set(it, EINA_FALSE);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_db_optimize_do, frame);
}

struct db_folder_add_ctx
{
   Evas_Object *status, *frame, *box, *button;
   Elm_Object_Item *page;
   unsigned int errors, processed;
   char *path;
   lms_t *lms;
   Ecore_Thread *thread;
   Ecore_Animator *status_updater;
   Eina_Bool canceled;
};

static void
_lms_scan(void *data, Ecore_Thread *thread __UNUSED__)
{
   struct db_folder_add_ctx *ctx = data;

   DBG("Scanning '%s' from thread", ctx->path);

   // TODO: using single process as sqlite is giving "database is locked"
   // with 2-process lms_process(), even if the main process is not using
   // the db anymore :-(
   // TODO: move to 2 process, as crashing the parser may crash enjoy!

   if (ctx->canceled) return;
   if (lms_check_single_process(ctx->lms, ctx->path) != 0)
     ERR("couldn't check \"%s\".", ctx->path);

   if (ctx->canceled) return;
   if (lms_process_single_process(ctx->lms, ctx->path) != 0)
     ERR("couldn't process \"%s\".", ctx->path);
}

static void
_lms_progress(lms_t *lms __UNUSED__, const char *path, int path_len __UNUSED__, lms_progress_status_t status, void *data)
{
   struct db_folder_add_ctx *ctx = data;
   DBG("status=%d '%s'", status, path);

   /* lazy: no need to lock, only this thread writes, the other just reads */
   if (status == LMS_PROGRESS_STATUS_UP_TO_DATE ||
       status == LMS_PROGRESS_STATUS_PROCESSED)
     ctx->processed++;
   else if (status == LMS_PROGRESS_STATUS_KILLED ||
            status == LMS_PROGRESS_STATUS_ERROR_PARSE ||
            status == LMS_PROGRESS_STATUS_ERROR_COMM)
     ctx->errors++;
}

static void
db_folder_add_ctx_free(struct db_folder_add_ctx *ctx)
{
   free(ctx->path);
   lms_free(ctx->lms);
   free(ctx);
}

static void _lms_scan_abort(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void
preferences_db_folder_add_dismiss(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct db_folder_add_ctx *ctx = data;

   evas_object_event_callback_del_full
     (ctx->status, EVAS_CALLBACK_DEL, _lms_scan_abort, ctx);

   elm_naviframe_item_pop_to(elm_naviframe_bottom_item_get(ctx->frame));
   enjoy_db_start_usage();
   db_folder_add_ctx_free(ctx);
}

static void
preferences_db_folder_add_stop(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct db_folder_add_ctx *ctx = data;
   lms_stop_processing(ctx->lms);
}

static void
_lms_scan_finish(struct db_folder_add_ctx *ctx, Eina_Bool success)
{
   char buf[1024];

   INF("Finished scanning with %s", success ? "success" : "interruptions");

   ctx->thread = NULL;
   if (!ctx->status) goto end; /* object deleted */

   if (ctx->errors)
     snprintf(buf, sizeof(buf), "Finished! Scanned %u files, %u errors.",
              ctx->processed, ctx->errors);
   else
     snprintf(buf, sizeof(buf), "Finished! Scanned %u files without errors.",
              ctx->processed);

   elm_object_text_set(ctx->status, buf);

   elm_object_text_set(ctx->button, "Dismiss");
   evas_object_smart_callback_del
     (ctx->button, "clicked", preferences_db_folder_add_stop);
   evas_object_smart_callback_add
     (ctx->button, "clicked", preferences_db_folder_add_dismiss, ctx);

 end:
   if (ctx->status_updater)
     {
        ecore_animator_del(ctx->status_updater);
        ctx->status_updater = NULL;
     }
   if (!ctx->status) db_folder_add_ctx_free(ctx);
}

static void
_lms_scan_end(void *data, Ecore_Thread *thread __UNUSED__)
{
   struct db_folder_add_ctx *ctx = data;
   _lms_scan_finish(ctx, EINA_TRUE);
}

static void
_lms_scan_cancel(void *data, Ecore_Thread *thread __UNUSED__)
{
   struct db_folder_add_ctx *ctx = data;
   _lms_scan_finish(ctx, EINA_FALSE);
}

static void
_lms_scan_abort(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct db_folder_add_ctx *ctx = data;
   if (!ctx->thread) return;
   WRN("Canceling scanner thread!");
   ctx->canceled = EINA_TRUE;
   lms_stop_processing(ctx->lms);
   ecore_thread_cancel(ctx->thread);
   ctx->status = NULL;
}

static Eina_Bool
preferences_db_folder_add_updater(void *data)
{
   struct db_folder_add_ctx *ctx = data;
   char buf[1024];

   if (!ctx->status)
     {
        ctx->status_updater = NULL;
        return EINA_FALSE;
     }

   if (ctx->errors)
     snprintf(buf, sizeof(buf), "Scanned %u files (%u errors)...",
              ctx->processed, ctx->errors);
   else
     snprintf(buf, sizeof(buf), "Scanned %u files...", ctx->processed);

   elm_object_text_set(ctx->status, buf);

   return EINA_TRUE;
}

static void
preferences_db_folder_add_do(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *fs = data;
   Evas_Object *box = elm_object_parent_widget_get(fs);
   Evas_Object *frame = elm_object_parent_widget_get(box);
   Evas_Object *bt, *win;
   const char *path = elm_fileselector_entry_path_get(fs);
   struct stat st;
   struct db_folder_add_ctx *ctx;
   Eina_Bool prev_btn_auto_pushed;

   if ((!path) || (stat(path, &st) != 0) || (!S_ISDIR(st.st_mode)))
     {
        ERR("Invalid path: %s", path);
        // TODO: add notify alert?
        return;
     }

   ctx = calloc(1, sizeof(struct db_folder_add_ctx));
   if (!ctx)
     {
        ERR("Could not allocate ctx!");
        return;
     }

   ctx->lms = lms_new(enjoy_db_path_get());
   if (!ctx->lms)
     {
        ERR("Could not create lightmediascanner instance!");
        free(ctx);
        return;
     }

   /* conservative:
    * commit every 20 insertions, wait at most 30s for slave feedback.
    *
    * commit interval: If it crashes, then at most that amount of songs
    * will be lost.
    */
   lms_set_commit_interval(ctx->lms, 20);
   lms_set_slave_timeout(ctx->lms, 30000);
   lms_set_progress_callback(ctx->lms, _lms_progress, ctx, NULL);

   if (!enjoy_lms_parsers_add(ctx->lms))
     {
        ERR("could not find any parser.");
        goto free_lms;
     }

   enjoy_lms_charsets_add(ctx->lms);

   enjoy_db_stop_usage();
   ctx->box = box = elm_box_add(frame);
   ctx->frame = frame;
   ctx->status = elm_label_add(box);
   elm_object_text_set(ctx->status, "Starting...");
   evas_object_size_hint_align_set(ctx->status, -1.0, 0.5);
   evas_object_show(ctx->status);
   elm_box_pack_end(box, ctx->status);

   ctx->button = bt = elm_button_add(box);
   elm_object_text_set(bt, "Stop scanning");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);
   evas_object_smart_callback_add
     (bt, "clicked", preferences_db_folder_add_stop, ctx);

   prev_btn_auto_pushed = elm_naviframe_prev_btn_auto_pushed_get(frame);
   elm_naviframe_prev_btn_auto_pushed_set(frame, EINA_FALSE);
   ctx->page = elm_naviframe_item_push
     (frame, "Importing Music", NULL, NULL, box, NULL);
   elm_naviframe_prev_btn_auto_pushed_set(frame, prev_btn_auto_pushed);

   ctx->path = strdup(path);

   ctx->thread = ecore_thread_run
     (_lms_scan, _lms_scan_end, _lms_scan_cancel, ctx);
   if (!ctx->thread)
     goto free_lms;

   ctx->status_updater = ecore_animator_add
     (preferences_db_folder_add_updater, ctx);

   evas_object_event_callback_add
     (ctx->status, EVAS_CALLBACK_DEL, _lms_scan_abort, ctx);

   return;

 free_lms:
   if (ctx->page)
     elm_naviframe_item_pop(frame);
   else if (ctx->box)
     evas_object_del(ctx->box);
   lms_free(ctx->lms);
   free(ctx->path);
   free(ctx);
}

static void
preferences_db_folder_add(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *frame = data;
   Evas_Object *box, *fs, *bt;
   char path[PATH_MAX];

   box = elm_box_add(frame);

   fs = elm_fileselector_entry_add(box);
   /* TODO: efreet should use xdg-user-dirs */
   snprintf(path, sizeof(path), "%s/Music", getenv("HOME"));
   if (access(path, F_OK|X_OK) == 0)
     elm_fileselector_entry_path_set(fs, path);
   else
     elm_fileselector_entry_path_set(fs, getenv("HOME"));
   elm_fileselector_entry_folder_only_set(fs, EINA_TRUE);
   elm_object_text_set(fs, "Choose...");
   evas_object_size_hint_align_set(fs, -1.0, 0.5);
   evas_object_show(fs);
   elm_box_pack_end(box, fs);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Import music from folder");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   elm_naviframe_item_push(frame, "Import folder", NULL, NULL, box, NULL);
   elm_genlist_item_selected_set(it, EINA_FALSE);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_db_folder_add_do, fs);
}

struct cover_clear_ctx
{
   Evas_Object *status, *frame, *box, *button;
   Elm_Object_Item *page;
   char *path;
   Ecore_Thread *thread;
   Eina_Bool canceled;
};

static void
_cover_clean(void *data, Ecore_Thread *thread __UNUSED__)
{
   struct cover_clear_ctx *ctx = data;

   DBG("Cleaning '%s' from thread", ctx->path);
   ecore_file_recursive_rm(ctx->path);
   DBG("Cleaned '%s' from thread", ctx->path);
}

static void
cover_clear_ctx_free(struct cover_clear_ctx *ctx)
{
   free(ctx->path);
   free(ctx);
}

static void _cover_clean_abort(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void
preferences_cover_clear_dismiss(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct cover_clear_ctx *ctx = data;

   evas_object_event_callback_del_full
     (ctx->status, EVAS_CALLBACK_DEL, _cover_clean_abort, ctx);

   elm_naviframe_item_pop_to(elm_naviframe_bottom_item_get(ctx->frame));
   cover_clear_ctx_free(ctx);
}

static void
_cover_clean_finish(struct cover_clear_ctx *ctx, Eina_Bool success)
{
   INF("Finished scanning with %s", success ? "success" : "interruptions");

   ctx->thread = NULL;
   if (!ctx->status) goto end; /* object deleted */

   elm_object_text_set(ctx->status, "Finished clearing album arts.");

   elm_object_disabled_set(ctx->button, EINA_FALSE);

 end:
   if (!ctx->status) cover_clear_ctx_free(ctx);
}

static void
_cover_clean_end(void *data, Ecore_Thread *thread __UNUSED__)
{
   struct cover_clear_ctx *ctx = data;
   _cover_clean_finish(ctx, EINA_TRUE);
}

static void
_cover_clean_cancel(void *data, Ecore_Thread *thread __UNUSED__)
{
   struct cover_clear_ctx *ctx = data;
   _cover_clean_finish(ctx, EINA_FALSE);
}

static void
_cover_clean_abort(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct cover_clear_ctx *ctx = data;
   if (!ctx->thread) return;
   WRN("Canceling cleanup thread!");
   ctx->canceled = EINA_TRUE;
   ecore_thread_cancel(ctx->thread);
   ctx->status = NULL;
}

static void
preferences_cover_clear_do(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *frame = data;
   Evas_Object *box, *bt;
   struct cover_clear_ctx *ctx;
   Eina_Bool prev_btn_auto_pushed;

   ctx = calloc(1, sizeof(struct cover_clear_ctx));
   if (!ctx)
     {
        ERR("Could not allocate ctx!");
        return;
     }
   if (asprintf(&ctx->path, "%s/covers/", enjoy_cache_dir_get()) < 1)
     {
        ERR("Could not allocate cover's path!");
        free(ctx);
        return;
     }

   enjoy_db_clear_covers();
   ctx->box = box = elm_box_add(frame);
   ctx->frame = frame;
   ctx->status = elm_label_add(box);
   elm_object_text_set(ctx->status, "Cleaning, wait...");
   evas_object_size_hint_align_set(ctx->status, -1.0, 0.5);
   evas_object_show(ctx->status);
   elm_box_pack_end(box, ctx->status);

   ctx->button = bt = elm_button_add(box);
   elm_object_text_set(bt, "Dismiss");
   elm_object_disabled_set(bt, EINA_TRUE);
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);
   evas_object_smart_callback_add
     (bt, "clicked", preferences_cover_clear_dismiss, ctx);

   prev_btn_auto_pushed = elm_naviframe_prev_btn_auto_pushed_get(frame);
   elm_naviframe_prev_btn_auto_pushed_set(frame, EINA_FALSE);
   ctx->page = elm_naviframe_item_push
     (frame, "Cleaning Album Arts", NULL, NULL, box, NULL);
   elm_naviframe_prev_btn_auto_pushed_set(frame, prev_btn_auto_pushed);

   ctx->thread = ecore_thread_run
     (_cover_clean, _cover_clean_end, _cover_clean_cancel, ctx);
   if (!ctx->thread)
     goto free_ctx;

   evas_object_event_callback_add
     (ctx->status, EVAS_CALLBACK_DEL, _cover_clean_abort, ctx);

   return;

 free_ctx:
   if (ctx->page)
     elm_naviframe_item_pop(frame);
   else if (ctx->box)
     evas_object_del(ctx->box);
   free(ctx);
}

static void
preferences_cover_clear(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *frame = data;
   Evas_Object *box, *bt;

   box = elm_box_add(frame);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Yes, clear the album arts!");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   elm_naviframe_item_push(frame, "Clear Album Arts?", NULL, NULL, box, NULL);
   elm_genlist_item_selected_set(it, EINA_FALSE);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_cover_clear_do, frame);
}

struct cover_local_search_ctx
{
   Evas_Object *status, *frame, *box, *button;
   Elm_Object_Item *page;
   unsigned int found, processed;
   Ecore_Idler *idler;
   Evas *evas;
   DB *db;
   Eina_Iterator *itr;
};

static void
cover_local_search_ctx_free(struct cover_local_search_ctx *ctx)
{
   eina_iterator_free(ctx->itr);
   free(ctx);
}

static void
_cover_local_search_abort(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct cover_local_search_ctx *ctx = data;
   if (!ctx->idler) return;
   WRN("Canceling search idler!");
   ecore_idler_del(ctx->idler);
   ctx->idler = NULL;
   ctx->status = NULL;
}

static void _cover_local_search_abort(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void
preferences_cover_local_search_dismiss(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct cover_local_search_ctx *ctx = data;

   evas_object_event_callback_del_full
     (ctx->status, EVAS_CALLBACK_DEL, _cover_local_search_abort, ctx);

   elm_naviframe_item_pop_to(elm_naviframe_bottom_item_get(ctx->frame));
   enjoy_db_start_usage();
   cover_local_search_ctx_free(ctx);
}

static void _cover_local_search_finish(struct cover_local_search_ctx *ctx);
static void
preferences_cover_local_search_stop(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   struct cover_local_search_ctx *ctx = data;
   ecore_idler_del(ctx->idler);
   ctx->idler = NULL;
   _cover_local_search_finish(ctx);
}

static void
_cover_local_search_finish(struct cover_local_search_ctx *ctx)
{
   char buf[1024];

   INF("Finished searching local album arts.");

   if (ctx->idler)
     {
        ecore_idler_del(ctx->idler);
        ctx->idler = NULL;
     }
   if (!ctx->status) goto end; /* object deleted */

   if (ctx->found)
     snprintf(buf, sizeof(buf), "Finished! Found %u files.", ctx->found);
   else
     snprintf(buf, sizeof(buf), "Finished! Tried %u, did not found album arts.",
              ctx->processed);

   elm_object_text_set(ctx->status, buf);

   elm_object_text_set(ctx->button, "Dismiss");
   evas_object_smart_callback_del
     (ctx->button, "clicked", preferences_cover_local_search_stop);
   evas_object_smart_callback_add
     (ctx->button, "clicked", preferences_cover_local_search_dismiss, ctx);

 end:
   if (!ctx->status) cover_local_search_ctx_free(ctx);
}

static Eina_Bool
_cover_local_search_idler(void *data)
{
   struct cover_local_search_ctx *ctx = data;
   Album *album, *album_itr = NULL;
   char buf[1024];

   if (!eina_iterator_next(ctx->itr, (void **)&album_itr))
     {
        _cover_local_search_finish(ctx);
        return EINA_FALSE;
     }

   ctx->processed++;

   album = db_album_copy(album_itr);
   if (!album)
     return EINA_TRUE;

   if (!db_album_covers_fetch(ctx->db, album))
     DBG("No album art fetched from DB, album_id=%"PRIi64" (%s by %s)",
         album->id, album->name, album->artist);

   if (!cover_album_local_find(ctx->evas, ctx->db, album))
     {
        WRN("Could find local covers, album_id=%"PRIi64" (%s by %s)",
            album->id, album->name, album->artist);
        goto end;
     }

   ctx->found++;

 end:
   snprintf(buf, sizeof(buf), "Found %u album arts, %u processed.",
            ctx->found, ctx->processed);
   elm_object_text_set(ctx->status, buf);

   db_album_free(album);
   return EINA_TRUE;
}

static void
preferences_cover_local_search_do(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *frame = data;
   Evas_Object *box, *bt;
   struct cover_local_search_ctx *ctx;
   Eina_Bool prev_btn_auto_pushed;
   DB *db = enjoy_db_get();

   if (!db)
     {
        ERR("Could not get db instance!");
        return;
     }

   ctx = calloc(1, sizeof(struct cover_local_search_ctx));
   if (!ctx)
     {
        ERR("Could not allocate ctx!");
        return;
     }

   ctx->itr = db_albums_get(db);
   ctx->db = db;
   ctx->evas = evas_object_evas_get(frame);

   ctx->box = box = elm_box_add(frame);
   ctx->frame = frame;
   ctx->status = elm_label_add(box);
   elm_object_text_set(ctx->status, "Searching, wait...");
   evas_object_size_hint_align_set(ctx->status, -1.0, 0.5);
   evas_object_show(ctx->status);
   elm_box_pack_end(box, ctx->status);

   ctx->button = bt = elm_button_add(box);
   elm_object_text_set(bt, "Stop searching");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);
   evas_object_smart_callback_add
     (bt, "clicked", preferences_cover_local_search_stop, ctx);

   prev_btn_auto_pushed = elm_naviframe_prev_btn_auto_pushed_get(frame);
   elm_naviframe_prev_btn_auto_pushed_set(frame, EINA_FALSE);
   ctx->page = elm_naviframe_item_push
     (frame, "Searching Album Arts", NULL, NULL, box, NULL);
   elm_naviframe_prev_btn_auto_pushed_set(frame, prev_btn_auto_pushed);

   ctx->idler = ecore_idler_add(_cover_local_search_idler, ctx);
   if (!ctx->idler)
     goto free_ctx;

   evas_object_event_callback_add
     (ctx->status, EVAS_CALLBACK_DEL, _cover_local_search_abort, ctx);

   return;

 free_ctx:
   if (ctx->page)
     elm_naviframe_item_pop(frame);
   else if (ctx->box)
     evas_object_del(ctx->box);
   eina_iterator_free(ctx->itr);
   free(ctx);
}

static void
preferences_cover_local_search(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *frame = data;
   Evas_Object *box, *bt, *lb;

   box = elm_box_add(frame);

   lb = elm_label_add(box);
   elm_object_text_set(lb, "Search happens at directory containing the files.");
   evas_object_size_hint_align_set(lb, -1.0, 0.5);
   evas_object_show(lb);
   elm_box_pack_end(box, lb);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Start searching!");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   elm_naviframe_item_push(frame, "Search Album Arts?", NULL, NULL, box, NULL);
   elm_genlist_item_selected_set(it, EINA_FALSE);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_cover_local_search_do, frame);
}

static char *
prefs_itc_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return strdup(data);
}

static const Elm_Genlist_Item_Class prefs_itc = {
  "default",
  {
    prefs_itc_text_get,
    NULL,
    NULL,
    NULL
  }
};

static const Elm_Genlist_Item_Class prefs_itc_group = {
  "group_index",
  {
    prefs_itc_text_get,
    NULL,
    NULL,
    NULL
  }
};

static Evas_Object *
preferences_root_add(Evas_Object *parent)
{
   Evas_Object *lst = elm_genlist_add(parent);
   Elm_Genlist_Item *grp;

   grp = elm_genlist_item_append(lst, &prefs_itc_group, "Database",
                                 NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);

   elm_genlist_item_append
     (lst, &prefs_itc, "Clear", grp, ELM_GENLIST_ITEM_NONE,
      preferences_db_clear, parent);
   elm_genlist_item_append
     (lst, &prefs_itc, "Cleanup & Optimize", grp, ELM_GENLIST_ITEM_NONE,
      preferences_db_optimize, parent);
   elm_genlist_item_append
     (lst, &prefs_itc, "Import folder", grp, ELM_GENLIST_ITEM_NONE,
      preferences_db_folder_add, parent);


   grp = elm_genlist_item_append(lst, &prefs_itc_group, "Album Arts",
                                 NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);

   elm_genlist_item_append
     (lst, &prefs_itc, "Clear", grp, ELM_GENLIST_ITEM_NONE,
      preferences_cover_clear, parent);

   elm_genlist_item_append
     (lst, &prefs_itc, "Search locally", grp, ELM_GENLIST_ITEM_NONE,
      preferences_cover_local_search, parent);

   return lst;
}

Evas_Object *
preferences_add(Evas_Object *parent)
{
   Evas_Object *frame = elm_naviframe_add(parent);
   Evas_Object *root = preferences_root_add(frame);

   elm_naviframe_item_push(frame, "Preferences", NULL, NULL, root, NULL);

   return frame;
}
