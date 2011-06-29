#include "ephoto.h"

#ifdef HAVE_LIBEXIF
#include <libexif/exif-data.h>
#endif

/* ROTATION is disabled until we do it properly, and properly means
 * elm_photocam and elm_image rotates their images internally.
 * Rotating the scroller is not correct and was rejected by Raster and others.
 */
//#define ROTATION

#define ZOOM_STEP 0.2

typedef struct _Ephoto_Single_Browser Ephoto_Single_Browser;
typedef struct _Ephoto_Viewer Ephoto_Viewer;

struct _Ephoto_Single_Browser
{
   Ephoto *ephoto;
   Evas_Object *main;
   Evas_Object *bar;
   Evas_Object *sentry;
   Evas_Object *viewer;
   const char *pending_path;
   Ephoto_Entry *entry;
   Ephoto_Orient orient;
   Eina_List *handlers;
};

struct _Ephoto_Viewer
{
   Evas_Object *photocam;
   Evas_Object *scroller;
   Evas_Object *image;
   double zoom;
   Eina_Bool fit:1;
};

static void _zoom_set(Ephoto_Single_Browser *sb, double zoom);
static void _zoom_in(Ephoto_Single_Browser *sb);
static void _zoom_out(Ephoto_Single_Browser *sb);

static Eina_Bool
_path_is_jpeg(const char *path_stringshared)
{
   size_t len = eina_stringshare_strlen(path_stringshared);
   const char *ext;

   if (len < sizeof(".jpg")) return EINA_FALSE;
   ext = path_stringshared + len - (sizeof(".jpg") - 1);
   if (strcasecmp(ext, ".jpg") == 0) return EINA_TRUE;

   if (len < sizeof(".jpeg")) return EINA_FALSE;
   ext = path_stringshared + len - (sizeof(".jpeg") - 1);
   if (strcasecmp(ext, ".jpeg") == 0) return EINA_TRUE;

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
        if (err != EVAS_LOAD_ERROR_NONE) goto load_error;
        elm_photocam_paused_set(obj, EINA_TRUE);
        evas_object_smart_callback_add
          (obj, "loaded", _viewer_photocam_loaded, v);
     }
   else
     {
        Evas_Coord w, h;
        obj = v->scroller = elm_scroller_add(parent);
        EINA_SAFETY_ON_NULL_GOTO(obj, error);
        v->image = evas_object_image_filled_add(evas_object_evas_get(parent));
        evas_object_image_file_set(v->image, path, NULL);
        err = evas_object_image_load_error_get(v->image);
        if (err != EVAS_LOAD_ERROR_NONE) goto load_error;
        evas_object_image_size_get(v->image, &w, &h);
        evas_object_size_hint_align_set(v->image, 0.5, 0.5);
        evas_object_size_hint_min_set(v->image, w, h);
        evas_object_size_hint_max_set(v->image, w, h);
        elm_scroller_content_set(v->scroller, v->image);
        evas_object_show(v->scroller);
        evas_object_show(v->image);
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
   if (v->photocam) elm_photocam_zoom_set(v->photocam, 1.0 / zoom);
   else
     {
        Evas_Coord w, h;
        evas_object_image_size_get(v->image, &w, &h);
        w *= zoom;
        h *= zoom;
        evas_object_size_hint_min_set(v->image, w, h);
        evas_object_size_hint_max_set(v->image, w, h);
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
_orient_apply(Ephoto_Single_Browser *sb)
{
#ifdef ROTATION
   const char *sig;
   switch (sb->orient)
     {
      case EPHOTO_ORIENT_0:
         sig = "state,rotate,0";
         break;
      case EPHOTO_ORIENT_90:
         sig = "state,rotate,90";
         break;
      case EPHOTO_ORIENT_180:
         sig = "state,rotate,180";
         break;
      case EPHOTO_ORIENT_270:
         sig = "state,rotate,270";
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ:
         sig = "state,flip,horiz";
         break;
      case EPHOTO_ORIENT_FLIP_VERT:
         sig = "state,flip,vert";
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ_90:
         sig = "state,flip,horiz,90";
         break;
      case EPHOTO_ORIENT_FLIP_VERT_90:
         sig = "state,flip,vert,90";
         break;
      default:
         return;
     }
   DBG("orient: %d, signal '%s'", sb->orient, sig);
   edje_object_signal_emit(sb->orient_edje, sig, "ephoto");
#else
   (void)sb;
#endif
}

#ifdef ROTATION
static void
_rotate_counterclock(Ephoto_Single_Browser *sb)
{
   switch (sb->orient)
     {
      case EPHOTO_ORIENT_0:
         sb->orient = EPHOTO_ORIENT_270;
         break;
      case EPHOTO_ORIENT_90:
         sb->orient = EPHOTO_ORIENT_0;
         break;
      case EPHOTO_ORIENT_180:
         sb->orient = EPHOTO_ORIENT_90;
         break;
      case EPHOTO_ORIENT_270:
         sb->orient = EPHOTO_ORIENT_180;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ_90;
         break;
      case EPHOTO_ORIENT_FLIP_VERT:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT_90;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ_90:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT;
         break;
      case EPHOTO_ORIENT_FLIP_VERT_90:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ;
         break;
     }
   _orient_apply(sb);
}

static void
_rotate_clock(Ephoto_Single_Browser *sb)
{
   switch (sb->orient)
     {
      case EPHOTO_ORIENT_0:
         sb->orient = EPHOTO_ORIENT_90;
         break;
      case EPHOTO_ORIENT_90:
         sb->orient = EPHOTO_ORIENT_180;
         break;
      case EPHOTO_ORIENT_180:
         sb->orient = EPHOTO_ORIENT_270;
         break;
      case EPHOTO_ORIENT_270:
         sb->orient = EPHOTO_ORIENT_0;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT_90;
         break;
      case EPHOTO_ORIENT_FLIP_VERT:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ_90;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ_90:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ;
         break;
      case EPHOTO_ORIENT_FLIP_VERT_90:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT;
         break;
     }
   _orient_apply(sb);
}

static void
_flip_horiz(Ephoto_Single_Browser *sb)
{
   switch (sb->orient)
     {
      case EPHOTO_ORIENT_0:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ;
         break;
      case EPHOTO_ORIENT_90:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ_90;
         break;
      case EPHOTO_ORIENT_180:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT;
         break;
      case EPHOTO_ORIENT_270:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT_90;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ:
         sb->orient = EPHOTO_ORIENT_0;
         break;
      case EPHOTO_ORIENT_FLIP_VERT:
         sb->orient = EPHOTO_ORIENT_180;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ_90:
         sb->orient = EPHOTO_ORIENT_90;
         break;
      case EPHOTO_ORIENT_FLIP_VERT_90:
         sb->orient = EPHOTO_ORIENT_270;
         break;
     }
   _orient_apply(sb);
}

static void
_flip_vert(Ephoto_Single_Browser *sb)
{
   switch (sb->orient)
     {
      case EPHOTO_ORIENT_0:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT;
         break;
      case EPHOTO_ORIENT_90:
         sb->orient = EPHOTO_ORIENT_FLIP_VERT_90;
         break;
      case EPHOTO_ORIENT_180:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ;
         break;
      case EPHOTO_ORIENT_270:
         sb->orient = EPHOTO_ORIENT_FLIP_HORIZ_90;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ:
         sb->orient = EPHOTO_ORIENT_180;
         break;
      case EPHOTO_ORIENT_FLIP_VERT:
         sb->orient = EPHOTO_ORIENT_0;
         break;
      case EPHOTO_ORIENT_FLIP_HORIZ_90:
         sb->orient = EPHOTO_ORIENT_270;
         break;
      case EPHOTO_ORIENT_FLIP_VERT_90:
         sb->orient = EPHOTO_ORIENT_90;
         break;
     }
   _orient_apply(sb);
}
#endif

static void
_mouse_wheel(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Single_Browser *sb = data;
   Evas_Event_Mouse_Wheel *ev = event_info;
   if (!evas_key_modifier_is_set(ev->modifiers, "Control")) return;

   if (ev->z > 0) _zoom_in(sb);
   else _zoom_out(sb);
}

static Ephoto_Entry *
_first_entry_find(Ephoto_Single_Browser *sb)
{
   const Eina_List *l;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN_VAL(sb->ephoto, NULL);

   EINA_LIST_FOREACH(sb->ephoto->entries, l, entry)
     if (!entry->is_dir) return entry;
   return NULL;
}

static Ephoto_Entry *
_last_entry_find(Ephoto_Single_Browser *sb)
{
   const Eina_List *l;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN_VAL(sb->ephoto, NULL);

   EINA_LIST_REVERSE_FOREACH(sb->ephoto->entries, l, entry)
     if (!entry->is_dir) return entry;
   return NULL;
}

Ephoto_Orient
ephoto_file_orient_get(const char *path)
{
#ifndef HAVE_LIBEXIF
   return EPHOTO_ORIENT_0;
#else
   Ephoto_Orient orient = EPHOTO_ORIENT_0;
   ExifData *exif;
   ExifEntry *entry;
   ExifByteOrder bo;

   if (!_path_is_jpeg(path)) return orient;

   exif = exif_data_new_from_file(path);
   if (!exif) goto end;
   bo = exif_data_get_byte_order(exif);
   entry = exif_data_get_entry(exif, EXIF_TAG_ORIENTATION);
   if (!entry) goto end_entry;

   orient = exif_get_short(entry->data, bo);
   DBG("orient=%d", orient);
   if ((orient < 1) || (orient > 8))
     {
        ERR("exif orient not supported: %d", orient);
        orient = EPHOTO_ORIENT_0;
     }

 end_entry:
   exif_data_free(exif);
 end:
   return orient;
#endif
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
        sb->viewer = _viewer_add(sb->main, sb->entry->path);
        elm_box_pack_end(sb->main, sb->viewer);
        evas_object_show(sb->viewer);
        evas_object_event_callback_add
          (sb->viewer, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, sb);
        elm_scrolled_entry_entry_set(sb->sentry, bname);
        ephoto_title_set(sb->ephoto, bname);
        sb->orient = ephoto_file_orient_get(sb->entry->path);
        _orient_apply(sb);
     }

   elm_object_focus(sb->main);
}

static void
_zoom_set(Ephoto_Single_Browser *sb, double zoom)
{
   DBG("zoom %f", zoom);
   if (zoom <= 0.0) return;
   _viewer_zoom_set(sb->viewer, zoom);
}

static void
_zoom_fit(Ephoto_Single_Browser *sb)
{
   if (sb->viewer) _viewer_zoom_fit(sb->viewer);
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

static void
_zoom_in_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _zoom_in(sb);
}

static void
_zoom_out_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _zoom_out(sb);
}

static void
_zoom_1_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _zoom_set(sb, 1.0);
}

static void
_zoom_fit_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _zoom_fit(sb);
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

static void
_next_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = NULL;
   Eina_List *node;
   EINA_SAFETY_ON_NULL_RETURN(sb->entry);

   node = eina_list_search_sorted_list(sb->ephoto->entries, _entry_cmp, sb->entry->path);
   if (!node) return;
   while ((node = node->next))
     {
        entry = node->data;
        if (!entry->is_dir)
          break;
     }
   if (!entry)
     entry = _first_entry_find(sb);
   if (entry)
     {
        DBG("next is '%s'", entry->path);
        ephoto_single_browser_entry_set(sb->main, entry);
     }
}

static void
_prev_entry(Ephoto_Single_Browser *sb)
{
   Eina_List *node;
   Ephoto_Entry *entry = NULL;
   EINA_SAFETY_ON_NULL_RETURN(sb->entry);

   node = eina_list_search_sorted_list(sb->ephoto->entries, _entry_cmp, sb->entry->path);
   if (!node) return;
   while ((node = node->prev))
     {
        entry = node->data;
        if (!entry->is_dir)
          break;
     }
   if (!entry)
     entry = _last_entry_find(sb);
   if (entry)
     {
        DBG("prev is '%s'", entry->path);
        ephoto_single_browser_entry_set(sb->main, entry);
     }
}

static void
_first_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = _first_entry_find(sb);
   if (!entry) return;
   DBG("first is '%s'", entry->path);
   ephoto_single_browser_entry_set(sb->main, entry);
}

static void
_last_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = _last_entry_find(sb);
   if (!entry) return;
   DBG("last is '%s'", entry->path);
   ephoto_single_browser_entry_set(sb->main, entry);
}

static void
_go_first(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _first_entry(sb);
}

static void
_go_prev(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _prev_entry(sb);
}

static void
_go_next(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _next_entry(sb);
}

static void
_go_last(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _last_entry(sb);
}

#ifdef ROTATION
static void
_go_rotate_counterclock(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _rotate_counterclock(sb);
}

static void
_go_rotate_clock(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _rotate_clock(sb);
}

static void
_go_flip_horiz(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _flip_horiz(sb);
}

static void
_go_flip_vert(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   _flip_vert(sb);
}
#endif

static void
_slideshow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   if (sb->entry)
     evas_object_smart_callback_call(sb->main, "slideshow", sb->entry);
}

static void
_back(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   evas_object_smart_callback_call(sb->main, "back", sb->entry);
}

static void
_changed_file_text(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
}

static void
_key_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Single_Browser *sb = data;
   Evas_Event_Key_Down *ev = event_info;
   Eina_Bool ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
   Eina_Bool shift = evas_key_modifier_is_set(ev->modifiers, "Shift");
   const char *k = ev->keyname;

   DBG("key pressed '%s'", k);
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

   if (!strcmp(k, "Escape"))
     evas_object_smart_callback_call(sb->main, "back", sb->entry);
   else if (!strcmp(k, "Left") || !strcmp(k, "BackSpace"))
     _prev_entry(sb);
   else if (!strcmp(k, "Right") || !strcmp(k, "space"))
     _next_entry(sb);
   else if (!strcmp(k, "Home"))
     _first_entry(sb);
   else if (!strcmp(k, "End"))
     _last_entry(sb);
#if ROTATION
   else if (!strcmp(k, "bracketleft"))
     {
        if (!shift) _rotate_counterclock(sb);
        else        _flip_horiz(sb);
     }
   else if (!strcmp(k, "bracketright"))
     {
        if (!shift) _rotate_clock(sb);
        else        _flip_vert(sb);
     }
#endif
   else if (!strcmp(k, "F5"))
     {
        if (sb->entry)
          evas_object_smart_callback_call(sb->main, "slideshow", sb->entry);
     }
}

static void
_entry_free(void *data, const Ephoto_Entry *entry __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   sb->entry = NULL;
}

static void
_main_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
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
        DBG("Adding entry %p for path %s", e, sb->pending_path);

        eina_stringshare_del(sb->pending_path);
        sb->pending_path = NULL;
        ephoto_single_browser_entry_set(sb->ephoto->single_browser, e);
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Evas_Object *
_button_add(Evas_Object *box, const char *image)
{
   Evas_Object *but, *ic;

   but = elm_button_add(box);

   ic = elm_icon_add(but);
   elm_icon_file_set(ic, image, NULL);
   evas_object_size_hint_min_set(ic, 16, 16);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_BOTH, 1, 1);

   elm_button_icon_set(but, ic);
   evas_object_size_hint_align_set(but, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(box, but);
   evas_object_show(but);

   return but;
}

Evas_Object *
ephoto_single_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *box = elm_box_add(parent);
   Evas_Object *but, *sep;
   Ephoto_Single_Browser *sb;

   EINA_SAFETY_ON_NULL_RETURN_VAL(box, NULL);

   sb = calloc(1, sizeof(Ephoto_Single_Browser));
   EINA_SAFETY_ON_NULL_GOTO(sb, error);
   sb->ephoto = ephoto;
   sb->main = box;
   elm_box_horizontal_set(sb->main, EINA_FALSE);
   elm_box_homogeneous_set(sb->main, EINA_FALSE);
   evas_object_event_callback_add(sb->main, EVAS_CALLBACK_DEL, _main_del, sb);
   evas_object_event_callback_add
     (sb->main, EVAS_CALLBACK_KEY_DOWN, _key_down, sb);
   evas_object_data_set(sb->main, "single_browser", sb);

   sb->bar = elm_box_add(sb->main);
   elm_box_horizontal_set(sb->bar, EINA_TRUE);
   elm_box_homogeneous_set(sb->bar, EINA_FALSE);
   evas_object_size_hint_weight_set(sb->bar, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sb->bar, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(sb->bar);
   elm_box_pack_end(sb->main, sb->bar);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/back_grid.png");
   evas_object_smart_callback_add(but, "clicked", _back, sb);

   sep = elm_separator_add(sb->bar);
   elm_box_pack_end(sb->bar, sep);
   evas_object_show(sep);
   
   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/zoom-in.png");
   evas_object_smart_callback_add(but, "clicked", _zoom_in_cb, sb);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/zoom-out.png");
   evas_object_smart_callback_add(but, "clicked", _zoom_out_cb, sb);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/zoom-fit.png");
   evas_object_smart_callback_add(but, "clicked", _zoom_fit_cb, sb);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/zoom-original.png");
   evas_object_smart_callback_add(but, "clicked", _zoom_1_cb, sb);

   sep = elm_separator_add(sb->bar);
   elm_box_pack_end(sb->bar, sep);
   evas_object_show(sep);

   sb->sentry = elm_scrolled_entry_add(sb->bar);
   evas_object_size_hint_weight_set(sb->sentry, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(sb->sentry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_scrolled_entry_single_line_set(sb->sentry, EINA_TRUE);
   elm_scrolled_entry_scrollbar_policy_set(sb->sentry, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_object_disabled_set(sb->sentry, EINA_TRUE);
   evas_object_smart_callback_add
     (sb->sentry, "activated", _changed_file_text, sb);
   elm_box_pack_end(sb->bar, sb->sentry);
   evas_object_show(sb->sentry);

   sep = elm_separator_add(sb->bar);
   elm_box_pack_end(sb->bar, sep);
   evas_object_show(sep);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/first.png");
   evas_object_smart_callback_add(but, "clicked", _go_first, sb);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/back.png");
   evas_object_smart_callback_add(but, "clicked", _go_prev, sb);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/forward.png");
   evas_object_smart_callback_add(but, "clicked", _go_next, sb);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/last.png");
   evas_object_smart_callback_add(but, "clicked", _go_last, sb);

   sep = elm_separator_add(sb->bar);
   elm_box_pack_end(sb->bar, sep);
   evas_object_show(sep);

   but = _button_add(sb->bar, PACKAGE_DATA_DIR "/images/slideshow.png");
   evas_object_smart_callback_add(but, "clicked", _slideshow, sb);

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_single_populate_end, sb));

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE, _ephoto_single_entry_create, sb));

   return sb->main;

 error:
   evas_object_del(sb->main);
   return NULL;
}

void
ephoto_single_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   EINA_SAFETY_ON_NULL_RETURN(sb);

   DBG("entry %p, was %p", entry, sb->entry);

   if (sb->entry)
     ephoto_entry_free_listener_del(sb->entry, _entry_free, sb);
  
   sb->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, sb);
   _ephoto_single_browser_recalc(sb);
   _zoom_fit(sb);
}

void
ephoto_single_browser_path_pending_set(Evas_Object *obj, const char *path)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   EINA_SAFETY_ON_NULL_RETURN(sb);

   DBG("Setting pending path '%s'", path);
   sb->pending_path = eina_stringshare_add(path);
}
