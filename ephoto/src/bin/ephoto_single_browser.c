#include "ephoto.h"

#ifdef HAVE_LIBEXIF
#include <libexif/exif-data.h>
#endif

/* ROTATION is disabled until we do it properly, and properly means
 * elm_photocam and elm_image rotates their images internally.
 * Rotating the scroller is not correct and was rejected by Raster and others.
 */

#define ROTATION 0

typedef struct _Ephoto_Single_Browser Ephoto_Single_Browser;
typedef struct _Ephoto_Viewer Ephoto_Viewer;

struct _Ephoto_Single_Browser
{
   Ephoto *ephoto;
   Evas_Object *box;
   Evas_Object *viewer;
   const char *pending_path;
   Ephoto_Entry *entry;
   Ephoto_Orient orient;
   Eina_List *handlers;
   Eina_List *entries;
   Eina_List *current_index;
};

struct _Ephoto_Viewer
{
   Evas_Object *scroller;
   Evas_Object *image;
   double zoom;
   Eina_Bool fit:1;
};

static void
_viewer_del(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Viewer *v = data;
   free(v);
}

static Evas_Object *
_viewer_add(Evas_Object *parent, const char *path)
{
   Ephoto_Viewer *v = calloc(1, sizeof(Ephoto_Viewer));
   Evas_Object *obj;
   Evas_Coord w, h;
   int err;

   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   obj = v->scroller = elm_scroller_add(parent);
   EINA_SAFETY_ON_NULL_GOTO(obj, error);
   v->image = evas_object_image_filled_add(evas_object_evas_get(parent));
   evas_object_image_file_set(v->image, path, NULL);
   err = evas_object_image_load_error_get(v->image);
   if (err != EVAS_LOAD_ERROR_NONE) goto load_error;
   evas_object_image_size_get(v->image, &w, &h);
   evas_object_size_hint_align_set(v->image, 0.5, 0.5);
   evas_object_size_hint_weight_set(v->image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_min_set(v->image, w, h);
   evas_object_size_hint_max_set(v->image, w, h);
   evas_object_resize(v->image, w, h);
   evas_object_show(v->image);
   elm_scroller_content_set(obj, v->image);
   evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(obj, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_data_set(obj, "viewer", v);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _viewer_del, v);
   return obj;

 load_error:
   ERR("could not load image '%s': %s", path, evas_load_error_str(err));
   evas_object_del(obj);
 error:
   free(v);
   return NULL;
}

static void
_viewer_zoom_apply(Ephoto_Viewer *v, double zoom)
{
   Evas_Coord w, h;

   v->zoom = zoom;
   evas_object_image_size_get(v->image, &w, &h);
   w *= zoom;
   h *= zoom;
   evas_object_size_hint_min_set(v->image, w, h);
   evas_object_size_hint_max_set(v->image, w, h);
}

static void
_viewer_zoom_fit_apply(Ephoto_Viewer *v)
{
   Evas_Coord cw, ch, iw, ih;
   double zx, zy, zoom;

   evas_object_geometry_get(v->scroller, NULL, NULL, &cw, &ch);
   evas_object_image_size_get(v->image, &iw, &ih);

   if ((cw <= 0) || (ch <= 0)) return; /* object still not resized */
   EINA_SAFETY_ON_TRUE_RETURN(iw <= 0);
   EINA_SAFETY_ON_TRUE_RETURN(ih <= 0);

   zx = (double)cw / (double)iw;
   zy = (double)ch / (double)ih;

   zoom = (zx < zy) ? zx : zy;
   _viewer_zoom_apply(v, zoom);
}

static void
_viewer_resized(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   _viewer_zoom_fit_apply(data);
}

static void
_viewer_zoom_fit(Evas_Object *obj)
{
   Ephoto_Viewer *v = evas_object_data_get(obj, "viewer");
   EINA_SAFETY_ON_NULL_RETURN(v);

   if (v->fit) return;
   v->fit = EINA_TRUE;

   evas_object_event_callback_add
     (v->scroller, EVAS_CALLBACK_RESIZE, _viewer_resized, v);

   _viewer_zoom_fit_apply(v);
}

static void
_ephoto_single_browser_recalc(Ephoto_Single_Browser *sb)
{
   if (sb->viewer)
     {
        evas_object_del(sb->viewer);
        sb->viewer = NULL;
     }

   if (sb->entry)
     {
        const char *bname = ecore_file_file_get(sb->entry->path);
        sb->viewer = _viewer_add(sb->box, sb->entry->path);
        elm_box_pack_end(sb->box, sb->viewer);
        evas_object_show(sb->viewer);
        ephoto_title_set(sb->ephoto, bname);
        _viewer_zoom_fit(sb->viewer);       
     }

   elm_object_focus(sb->box);
}

static void
_box_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   Ecore_Event_Handler *handler;

   EINA_LIST_FREE(sb->handlers, handler)
      ecore_event_handler_del(handler);
   if (sb->pending_path)
     eina_stringshare_del(sb->pending_path);
   free(sb);
}

static Eina_Bool
_ephoto_single_populate_end(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_single_entry_create(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   Ephoto_Event_Entry_Create *ev = event;
   Ephoto_Entry *e;

   e = ev->entry;
   if (!sb->entry && sb->pending_path && e->path == sb->pending_path)
     {
        eina_stringshare_del(sb->pending_path);
        sb->pending_path = NULL;
        ephoto_single_browser_entry_set(sb->ephoto->single_browser, e);
     }

   return ECORE_CALLBACK_PASS_ON;
}

Evas_Object *
ephoto_single_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Ephoto_Single_Browser *sb;

   sb = calloc(1, sizeof(Ephoto_Single_Browser));
   EINA_SAFETY_ON_NULL_GOTO(sb, error);
   sb->ephoto = ephoto;

   sb->box = elm_box_add(parent);
   evas_object_size_hint_weight_set
     (sb->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (sb->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_event_callback_add(sb->box, EVAS_CALLBACK_DEL, _box_del, sb);
   evas_object_data_set(sb->box, "single_browser", sb);
   evas_object_show(sb->box);

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_single_populate_end, sb));

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE_THUMB, _ephoto_single_entry_create, sb));

   return sb->box;

 error:
   evas_object_del(sb->box);
   return NULL;
}

void
ephoto_single_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   EINA_SAFETY_ON_NULL_RETURN(sb);

   sb->entry = entry;

   _ephoto_single_browser_recalc(sb);
}

void
ephoto_single_browser_path_pending_set(Evas_Object *obj, const char *path)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   EINA_SAFETY_ON_NULL_RETURN(sb);

   DBG("Setting pending path '%s'", path);
   sb->pending_path = eina_stringshare_add(path);
}

