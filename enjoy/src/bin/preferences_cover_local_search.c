#include "private.h"

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

static const char *
preferences_cover_local_search_categegory_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Album Arts";
}

static const char *
preferences_cover_local_search_label_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Search locally";
}

static Eina_Bool
preferences_cover_local_search_activate(Enjoy_Preferences_Plugin *p __UNUSED__, Evas_Object *naviframe, Evas_Object **prev_btn __UNUSED__, Evas_Object **next_btn __UNUSED__, Evas_Object **content, Eina_Bool *auto_prev_btn __UNUSED__)
{
   Evas_Object *box, *bt, *lb;

   box = elm_box_add(naviframe);

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

   evas_object_smart_callback_add
     (bt, "clicked", preferences_cover_local_search_do, naviframe);

   *content = box;
   return EINA_TRUE;
}

static Enjoy_Preferences_Plugin *preferences_cover_local_search_plugin = NULL;
Eina_Bool
preferences_cover_local_search_register(void)
{
   static const Enjoy_Preferences_Plugin_Api api = {
     ENJOY_PREFERENCES_PLUGIN_API_VERSION,
     preferences_cover_local_search_categegory_get,
     preferences_cover_local_search_label_get,
     preferences_cover_local_search_activate
   };
   preferences_cover_local_search_plugin = enjoy_preferences_plugin_register
     (&api, ENJOY_PLUGIN_PRIORITY_NORMAL);
   return !!preferences_cover_local_search_plugin;
}

void
preferences_cover_local_search_unregister(void)
{
   enjoy_preferences_plugin_unregister(preferences_cover_local_search_plugin);
   preferences_cover_local_search_plugin = NULL;
}
