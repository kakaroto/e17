#include "private.h"

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

static const char *
preferences_cover_clear_categegory_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Album Arts";
}

static const char *
preferences_cover_clear_label_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Clear";
}

static Eina_Bool
preferences_cover_clear_activate(Enjoy_Preferences_Plugin *p __UNUSED__, Evas_Object *naviframe, Evas_Object **prev_btn __UNUSED__, Evas_Object **next_btn __UNUSED__, Evas_Object **content, Eina_Bool *auto_prev_btn __UNUSED__)
{
   Evas_Object *box, *bt;

   box = elm_box_add(naviframe);

   bt = elm_button_add(box);
   elm_object_text_set(bt, "Yes, clear the album arts!");
   evas_object_size_hint_align_set(bt, -1.0, 0.5);
   evas_object_show(bt);
   elm_box_pack_end(box, bt);

   evas_object_smart_callback_add
     (bt, "clicked", preferences_cover_clear_do, naviframe);

   *content = box;
   return EINA_TRUE;
}

static Enjoy_Preferences_Plugin *preferences_cover_clear_plugin = NULL;
Eina_Bool
preferences_cover_clear_register(void)
{
   static const Enjoy_Preferences_Plugin_Api api = {
     ENJOY_PREFERENCES_PLUGIN_API_VERSION,
     preferences_cover_clear_categegory_get,
     preferences_cover_clear_label_get,
     preferences_cover_clear_activate
   };
   preferences_cover_clear_plugin = enjoy_preferences_plugin_register
     (&api, ENJOY_PLUGIN_PRIORITY_NORMAL);
   return !!preferences_cover_clear_plugin;
}

void
preferences_cover_clear_unregister(void)
{
   enjoy_preferences_plugin_unregister(preferences_cover_clear_plugin);
   preferences_cover_clear_plugin = NULL;
}
