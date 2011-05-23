#include "ephoto.h"

#ifdef HAVE_LIBEXIF
#include <libexif/exif-data.h>
#endif

/* ROTATION is disabled until we do it properly, and properly means
 * elm_photocam and elm_image rotates their images internally.
 * Rotating the scroller is not correct and was rejected by Raster and others.
 */

#define ROTATION 0
#define ZOOM_STEP 0.2

typedef struct _Ephoto_Single_Browser Ephoto_Single_Browser;
typedef struct _Ephoto_Viewer Ephoto_Viewer;

struct _Ephoto_Single_Browser
{
   Ephoto *ephoto;
   Evas_Object *layout;
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
   Evas_Object *box;
   Evas_Object *image;
   Evas_Object *photocam;
   double zoom;
   Eina_Bool fit:1;
};

static Eina_Bool
_path_is_jpeg(const char *path_stringshared)
{
   size_t len = eina_stringshare_strlen(path_stringshared);
   const char *ext;

   if (len < sizeof(".jpg")) 
     return EINA_FALSE;
   ext = path_stringshared + len - (sizeof(".jpg") - 1);
   if (strcasecmp(ext, ".jpg") == 0) 
     return EINA_TRUE;

   if (len < sizeof(".jpeg")) 
     return EINA_FALSE;
   ext = path_stringshared + len - (sizeof(".jpeg") - 1);
   if (strcasecmp(ext, ".jpeg") == 0) 
     return EINA_TRUE;

   return EINA_FALSE;
}

static void
_viewer_photocam_loaded(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   elm_photocam_paused_set(obj, EINA_FALSE);
}

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
   int err;

   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   if (_path_is_jpeg(path))
     {
        obj = v->photocam = elm_photocam_add(parent);
        EINA_SAFETY_ON_NULL_GOTO(obj, error);
        err = elm_photocam_file_set(obj, path);
        if (err != EVAS_LOAD_ERROR_NONE) 
          goto load_error;
        elm_photocam_paused_set(obj, EINA_TRUE);
        evas_object_smart_callback_add
          (obj, "loaded", _viewer_photocam_loaded, v);
     }
   else
     {
        Evas_Coord w, h;
        obj = v->scroller = elm_scroller_add(parent);
        EINA_SAFETY_ON_NULL_GOTO(obj, error);

        v->box = elm_box_add(obj);
        evas_object_size_hint_weight_set(v->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(v->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(v->box);

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
        elm_box_pack_end(v->box, v->image);
        elm_scroller_content_set(obj, v->box);
     }
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
   v->zoom = zoom;
   if (v->photocam)
     elm_photocam_zoom_set(v->photocam, 1.0 / zoom);
   else
     {
        Evas_Coord w, h;
        evas_object_image_size_get(v->image, &w, &h);
        w *= zoom;
        h *= zoom; 
        evas_object_size_hint_min_set(v->image, w, h);
        evas_object_size_hint_max_set(v->image, w, h);
        elm_box_unpack(v->box, v->image);
        elm_scroller_content_unset(v->scroller);
        elm_box_pack_end(v->box, v->image);
        elm_scroller_content_set(v->scroller, v->box);
     }
}

static void
_viewer_zoom_fit_apply(Ephoto_Viewer *v)
{
   Evas_Coord cw, ch, iw, ih;
   double zx, zy, zoom;

   if (v->photocam)
     {
        evas_object_geometry_get(v->photocam, NULL, NULL, &cw, &ch);
        elm_photocam_image_size_get(v->photocam, &iw, &ih);
     }
   else
     {
        evas_object_geometry_get(v->scroller, NULL, NULL, &cw, &ch);
        evas_object_image_size_get(v->image, &iw, &ih);
     }

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
_viewer_zoom_set(Evas_Object *obj, double zoom)
{
   Ephoto_Viewer *v = evas_object_data_get(obj, "viewer");
   EINA_SAFETY_ON_NULL_RETURN(v);
   _viewer_zoom_apply(v, zoom);

   if (v->fit)
     {
        if (v->photocam)
          evas_object_event_callback_del_full
            (v->photocam, EVAS_CALLBACK_RESIZE, _viewer_resized, v);
        else
          evas_object_event_callback_del_full
            (v->scroller, EVAS_CALLBACK_RESIZE, _viewer_resized, v);
        v->fit = EINA_FALSE;
     }
}

static double
_viewer_zoom_get(Evas_Object *obj)
{
   Ephoto_Viewer *v = evas_object_data_get(obj, "viewer");
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, 0.0);
   return v->zoom;
}

static void
_viewer_zoom_fit(Evas_Object *obj)
{
   Ephoto_Viewer *v = evas_object_data_get(obj, "viewer");
   EINA_SAFETY_ON_NULL_RETURN(v);
   

   if (v->fit) return;
   v->fit = EINA_TRUE;
   if (v->photocam)
     evas_object_event_callback_add
       (v->photocam, EVAS_CALLBACK_RESIZE, _viewer_resized, v);
   else
     evas_object_event_callback_add
       (v->scroller, EVAS_CALLBACK_RESIZE, _viewer_resized, v);

   _viewer_zoom_fit_apply(v);
}

static void
_zoom_set(Ephoto_Single_Browser *sb, double zoom)
{
   if (zoom <= 0.0) return;
     _viewer_zoom_set(sb->viewer, zoom);
}

static void
_zoom_fit(Ephoto_Single_Browser *sb)
{
   if (sb->viewer)
     _viewer_zoom_fit(sb->viewer);
}

static void
_zoom_in(Ephoto_Single_Browser *sb)
{
   double change = (1.0 + ZOOM_STEP);
   _viewer_zoom_set(sb->viewer, _viewer_zoom_get(sb->viewer) * change);
}

static void
_zoom_out(Ephoto_Single_Browser *sb)
{
   double change = (1.0 - ZOOM_STEP);
   _viewer_zoom_set(sb->viewer, _viewer_zoom_get(sb->viewer) * change);
}

static int
_entry_cmp(const void *pa, const void *pb)
{
   const Ephoto_Entry *a = pa;
   const char *path = pb;
   
   if (path == a->path)
     return 0;
   else
     return strcoll(a->path, path);
}

static Ephoto_Entry *
_first_entry_find(Ephoto_Single_Browser *sb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(sb->ephoto, NULL);
   return eina_list_nth(sb->ephoto->entries, 0);
}

static Ephoto_Entry *
_last_entry_find(Ephoto_Single_Browser *sb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(sb->ephoto, NULL);
   return eina_list_data_get(eina_list_last(sb->ephoto->entries));
}

static void
_next_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = NULL;
   Eina_List *node;
   EINA_SAFETY_ON_NULL_RETURN(sb->entry);

   node = eina_list_search_sorted_list(sb->ephoto->entries, _entry_cmp, sb->entry->path);
   if (!node)
     return;
   if ((node = node->next))
     {
        entry = node->data;
     }
   if (!entry)
     entry = _first_entry_find(sb);
   if (entry)
     ephoto_single_browser_entry_set(sb->layout, entry);
}

static void
_prev_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = NULL;
   Eina_List *node;
   EINA_SAFETY_ON_NULL_RETURN(sb->entry);

   node = eina_list_search_sorted_list(sb->ephoto->entries, _entry_cmp, sb->entry->path);
   if (!node)
     return;
   if ((node = node->prev))
     {
        entry = node->data;
     }
   if (!entry)
     entry = _last_entry_find(sb);
   if (entry)
     ephoto_single_browser_entry_set(sb->layout, entry);
}

static void
_first_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = _first_entry_find(sb);

   if (!entry)
     return;
   ephoto_single_browser_entry_set(sb->layout, entry);
}

static void
_last_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = _last_entry_find(sb);

   if (!entry)
     return;
   ephoto_single_browser_entry_set(sb->layout, entry);
}

static void
_viewer_scrolled(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   Ephoto_Viewer *v = evas_object_data_get(sb->viewer, "viewer");
   int sx, sw, cx;
   if (!v->fit)
     return;
   evas_object_geometry_get(sb->viewer, &sx, 0, &sw, 0);
   if (v->photocam)
     {
        Evas_Object *i = elm_photocam_internal_image_get(v->photocam);
        evas_object_geometry_get(i, &cx, 0, 0, 0);
     }
   else
     evas_object_geometry_get(v->image, &cx, 0, 0, 0);
   if (cx < (sx-(sw/2)))
     _next_entry(sb);
   else if (cx > (sw/2))
     _prev_entry(sb);
}

static void
_mouse_wheel(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Single_Browser *sb = data;
   Ephoto_Viewer *v = evas_object_data_get(sb->viewer, "viewer");
   Evas_Event_Mouse_Wheel *ev = event_info;
   if (!evas_key_modifier_is_set(ev->modifiers, "Control")) return;

   if (ev->z > 0) _zoom_set(sb, v->zoom + ZOOM_STEP);
   else _zoom_set(sb, v->zoom - ZOOM_STEP);
}

static void
_key_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Single_Browser *sb = data;
   Evas_Event_Key_Down *ev = event_info;
   Eina_Bool ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
   Eina_Bool shift = evas_key_modifier_is_set(ev->modifiers, "Shift");
   const char *k = ev->keyname;

   if (ctrl)
     {
        if ((!strcmp(k, "plus")) || (!strcmp(k, "equal")))
          _zoom_in(sb);
        else if (!strcmp(k, "minus"))
          _zoom_out(sb);
        else if (!strcmp(k, "0"))
          {
             if (shift) _zoom_fit(sb);
             else _zoom_set(sb, 1.0);
          }

        return;
     }
   if (!strcmp(k, "Left") || !strcmp(k, "BackSpace"))
     _prev_entry(sb);
   else if (!strcmp(k, "Right") || !strcmp(k, "space"))
     _next_entry(sb);
   else if (!strcmp(k, "Home"))
     _first_entry(sb);
   else if (!strcmp(k, "End"))
     _last_entry(sb);
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
        sb->viewer = _viewer_add(sb->layout, sb->entry->path);
        elm_layout_content_set(sb->layout, "ephoto.swallow.content", sb->viewer);
        evas_object_event_callback_add
          (sb->viewer, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, sb);
        evas_object_smart_callback_add
          (sb->viewer, "scroll", _viewer_scrolled, sb);
        evas_object_show(sb->viewer);
        ephoto_title_set(sb->ephoto, bname);       
     }

   elm_object_focus(sb->layout);
}

static void
_entry_free(void *data, const Ephoto_Entry *entry __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   sb->entry = NULL;
}

static void
_layout_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   Ecore_Event_Handler *handler;

   EINA_LIST_FREE(sb->handlers, handler)
      ecore_event_handler_del(handler);
   if (sb->entry)
     ephoto_entry_free_listener_del(sb->entry, _entry_free, sb);
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

   sb->layout = elm_layout_add(parent);
   if (!elm_layout_file_set(sb->layout, THEME_FILE, "ephoto/layout/single/browser"))
     {
        ERR("Could not load style 'ephoto/layout/single/browser' from theme");
        goto error;
     }
   evas_object_size_hint_weight_set
     (sb->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (sb->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_event_callback_add(sb->layout, EVAS_CALLBACK_KEY_DOWN, _key_down, sb);
   evas_object_event_callback_add(sb->layout, EVAS_CALLBACK_DEL, _layout_del, sb);
   evas_object_data_set(sb->layout, "single_browser", sb);
   evas_object_show(sb->layout);

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_single_populate_end, sb));

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE_THUMB, _ephoto_single_entry_create, sb));

   return sb->layout;

 error:
   evas_object_del(sb->layout);
   return NULL;
}

void
ephoto_single_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   Eina_Bool same_file = EINA_FALSE;
   EINA_SAFETY_ON_NULL_RETURN(sb);

   if (sb->entry)
     {
        ephoto_entry_free_listener_del(sb->entry, _entry_free, sb);
        if (entry && entry->path == sb->entry->path)
          same_file = EINA_TRUE;
     }

   sb->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, sb);

   if (sb->entry || !same_file)
     {
        _ephoto_single_browser_recalc(sb);
        _zoom_fit(sb);
     }
}

void
ephoto_single_browser_path_pending_set(Evas_Object *obj, const char *path)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   EINA_SAFETY_ON_NULL_RETURN(sb);

   DBG("Setting pending path '%s'", path);
   sb->pending_path = eina_stringshare_add(path);
}

