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
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *orient_layout;
   Evas_Object *orient_edje;
   Evas_Object *viewer;
   Evas_Object *toolbar;
   struct {
      Elm_Toolbar_Item *back;
      Elm_Toolbar_Item *zoom_in;
      Elm_Toolbar_Item *zoom_out;
      Elm_Toolbar_Item *zoom_1;
      Elm_Toolbar_Item *zoom_fit;
      Elm_Toolbar_Item *go_first;
      Elm_Toolbar_Item *go_prev;
      Elm_Toolbar_Item *go_next;
      Elm_Toolbar_Item *go_last;
#ifdef ROTATION
      Elm_Toolbar_Item *rotate_counterclock;
      Elm_Toolbar_Item *rotate_clock;
      Elm_Toolbar_Item *flip_horiz;
      Elm_Toolbar_Item *flip_vert;
#endif
      Elm_Toolbar_Item *slideshow;
   } action;
   const char *pending_path;
   Ephoto_Entry *entry;
   Ephoto_Orient orient;
   Eina_List *handlers;
   Eina_List *entries;
   Eina_List *current_index;
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
        evas_object_resize(v->image, w, h);
        evas_object_show(v->image);
        elm_scroller_content_set(obj, v->image);
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
   EINA_SAFETY_ON_NULL_RETURN_VAL(sb->ephoto, NULL);

   return eina_list_nth(sb->entries, 0);
}

static Ephoto_Entry *
_last_entry_find(Ephoto_Single_Browser *sb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(sb->ephoto, NULL);

   return eina_list_data_get(eina_list_last(sb->entries));
}

static void
_ephoto_single_browser_toolbar_eval(Ephoto_Single_Browser *sb)
{
   if (!sb->entry)
     {
        elm_toolbar_item_disabled_set(sb->action.go_first, EINA_TRUE);
        elm_toolbar_item_disabled_set(sb->action.go_prev, EINA_TRUE);
        elm_toolbar_item_disabled_set(sb->action.go_next, EINA_TRUE);
        elm_toolbar_item_disabled_set(sb->action.go_last, EINA_TRUE);
        elm_toolbar_item_disabled_set(sb->action.slideshow, EINA_TRUE);
     }
   else
     {
        Eina_Bool is_first = sb->entry == _first_entry_find(sb);
        Eina_Bool is_last = sb->entry == _last_entry_find(sb);

        elm_toolbar_item_disabled_set(sb->action.go_first, is_first);
        elm_toolbar_item_disabled_set(sb->action.go_prev, is_first);
        elm_toolbar_item_disabled_set(sb->action.go_next, is_last);
        elm_toolbar_item_disabled_set(sb->action.go_last, is_last);
        elm_toolbar_item_disabled_set(sb->action.slideshow, EINA_FALSE);
     }
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
        sb->viewer = _viewer_add(sb->orient_layout, sb->entry->path);
        elm_layout_content_set
          (sb->orient_layout, "elm.swallow.content", sb->viewer);
        evas_object_show(sb->viewer);
        evas_object_event_callback_add
          (sb->viewer, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, sb);
        edje_object_part_text_set(sb->edje, "elm.text.title", bname);
        ephoto_title_set(sb->ephoto, bname);
        sb->orient = ephoto_file_orient_get(sb->entry->path);
        _orient_apply(sb);
     }

   elm_object_focus(sb->layout);

   _ephoto_single_browser_toolbar_eval(sb);
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
   elm_toolbar_item_selected_set(sb->action.zoom_in, EINA_FALSE);
   _zoom_in(sb);
}

static void
_zoom_out_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.zoom_out, EINA_FALSE);
   _zoom_out(sb);
}

static void
_zoom_1_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.zoom_1, EINA_FALSE);
   _zoom_set(sb, 1.0);
}

static void
_zoom_fit_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.zoom_fit, EINA_FALSE);
   _zoom_fit(sb);
}

static void
_next_entry(Ephoto_Single_Browser *sb)
{
   Eina_List *next;
   Ephoto_Entry *entry = NULL;
   EINA_SAFETY_ON_NULL_RETURN(sb->entry);

   next = eina_list_next(sb->current_index);
   if (!eina_list_data_get(next))
     next = eina_list_nth_list(sb->entries, 0);   
   entry = eina_list_data_get(next);

   if (entry)
     {
        DBG("next is '%s'", entry->path);
        ephoto_single_browser_entry_set(sb->layout, entry);
     }
}

static void
_prev_entry(Ephoto_Single_Browser *sb)
{
   Eina_List *prev;
   Ephoto_Entry *entry = NULL;
   EINA_SAFETY_ON_NULL_RETURN(sb->entry);

   prev = eina_list_prev(sb->current_index);
   if (!eina_list_data_get(prev))
     prev = eina_list_last(sb->entries);
   entry = eina_list_data_get(prev);

   if (entry)
     {
        DBG("prev is '%s'", entry->path);
        ephoto_single_browser_entry_set(sb->layout, entry);
     }
}

static void
_first_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = _first_entry_find(sb);
   if (!entry) return;
   DBG("first is '%s'", entry->path);
   ephoto_single_browser_entry_set(sb->layout, entry);
}

static void
_last_entry(Ephoto_Single_Browser *sb)
{
   Ephoto_Entry *entry = _last_entry_find(sb);
   if (!entry) return;
   DBG("last is '%s'", entry->path);
   ephoto_single_browser_entry_set(sb->layout, entry);
}

static void
_go_first(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.go_first, EINA_FALSE);
   _first_entry(sb);
}

static void
_go_prev(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.go_prev, EINA_FALSE);
   _prev_entry(sb);
}

static void
_go_next(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.go_next, EINA_FALSE);
   _next_entry(sb);
}

static void
_go_last(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.go_last, EINA_FALSE);
   _last_entry(sb);
}

#ifdef ROTATION
static void
_go_rotate_counterclock(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.rotate_counterclock, EINA_FALSE);
   _rotate_counterclock(sb);
}

static void
_go_rotate_clock(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.rotate_clock, EINA_FALSE);
   _rotate_clock(sb);
}

static void
_go_flip_horiz(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.flip_horiz, EINA_FALSE);
   _flip_horiz(sb);
}

static void
_go_flip_vert(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.flip_vert, EINA_FALSE);
   _flip_vert(sb);
}
#endif

static void
_slideshow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.slideshow, EINA_FALSE);
   if (sb->entry)
     evas_object_smart_callback_call(sb->layout, "slideshow", sb->entry);
}

static void
_back(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;
   elm_toolbar_item_selected_set(sb->action.back, EINA_FALSE);
   evas_object_smart_callback_call(sb->layout, "back", sb->entry);
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
     evas_object_smart_callback_call(sb->layout, "back", sb->entry);
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
          evas_object_smart_callback_call(sb->layout, "slideshow", sb->entry);
     }
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

static Elm_Toolbar_Item *
_toolbar_item_add(Ephoto_Single_Browser *sb, const char *icon, const char *label, int priority, Evas_Smart_Cb cb)
{
   Elm_Toolbar_Item *item = elm_toolbar_item_append(sb->toolbar, icon, label,
                                                    cb, sb);
   elm_toolbar_item_priority_set(item, priority);
   return item;
}

static Elm_Toolbar_Item *
_toolbar_item_separator_add(Ephoto_Single_Browser *sb)
{
   Elm_Toolbar_Item *it = elm_toolbar_item_append
     (sb->toolbar, NULL, NULL, NULL, NULL);
   elm_toolbar_item_separator_set(it, EINA_TRUE);
   return it;
}

static Eina_Bool
_ephoto_single_populate_end(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Single_Browser *sb = data;

   _ephoto_single_browser_toolbar_eval(sb);

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

Evas_Object *
ephoto_single_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *layout = elm_layout_add(parent), *button;
   Ephoto_Single_Browser *sb;

   EINA_SAFETY_ON_NULL_RETURN_VAL(layout, NULL);

   sb = calloc(1, sizeof(Ephoto_Single_Browser));
   EINA_SAFETY_ON_NULL_GOTO(sb, error);
   sb->ephoto = ephoto;
   sb->layout = layout;
   sb->edje = elm_layout_edje_get(layout);
   evas_object_event_callback_add(layout, EVAS_CALLBACK_DEL, _layout_del, sb);
   evas_object_event_callback_add
     (layout, EVAS_CALLBACK_KEY_DOWN, _key_down, sb);
   evas_object_data_set(layout, "single_browser", sb);

   if (!elm_layout_theme_set
       (layout, "layout", "application", "toolbar-content-back"))
     {
        ERR("could not load style 'toolbar-content-back' from theme");
        goto error;
     }

   /*TODO This is hack. Better Idea?*/
   button = edje_object_part_external_object_get(sb->edje, "back");
   evas_object_del(button);
   
   sb->toolbar = edje_object_part_external_object_get
     (sb->edje, "elm.external.toolbar");
   if (!sb->toolbar)
     {
        ERR("no toolbar in layout!");
        goto error;
     }
   elm_toolbar_homogenous_set(sb->toolbar, EINA_FALSE);
   elm_toolbar_mode_shrink_set(sb->toolbar, ELM_TOOLBAR_SHRINK_MENU);
   elm_toolbar_menu_parent_set(sb->toolbar, parent);

   sb->action.back = _toolbar_item_add
     (sb, "edit-undo", "Back", 200, _back);

    _toolbar_item_separator_add(sb);

   sb->action.slideshow = _toolbar_item_add
     (sb, "media-playback-start", "Slideshow", 150, _slideshow);

   sb->action.zoom_in = _toolbar_item_add
     (sb, "zoom-in", "Zoom In", 100, _zoom_in_cb);
   sb->action.zoom_out = _toolbar_item_add
     (sb, "zoom-out", "Zoom Out", 80, _zoom_out_cb);
   sb->action.zoom_1 = _toolbar_item_add
     (sb, "zoom-original", "Zoom 1:1", 50, _zoom_1_cb);
   sb->action.zoom_fit = _toolbar_item_add
     (sb, "zoom-fit-best", "Zoom Fit", 40, _zoom_fit_cb);

   _toolbar_item_separator_add(sb);

   sb->action.go_first = _toolbar_item_add(sb, "go-first", "First", 50, _go_first);
   sb->action.go_prev = _toolbar_item_add
     (sb, "go-previous", "Previous", 100, _go_prev);
   sb->action.go_next = _toolbar_item_add(sb, "go-next", "Next", 50, _go_next);
   sb->action.go_last = _toolbar_item_add(sb, "go-last", "Last", 50, _go_last);

   _toolbar_item_separator_add(sb);

#ifdef ROTATION
   sb->action.rotate_counterclock = _toolbar_item_add
     (sb, "object-rotate-left", "Rotate Left", 50, _go_rotate_counterclock);
   sb->action.rotate_clock = _toolbar_item_add
     (sb, "object-rotate-right", "Rotate Right", 30, _go_rotate_clock);
   sb->action.flip_horiz = _toolbar_item_add
     (sb, "object-flip-horizontal", "Flip Horiz.", 30, _go_flip_horiz);
   sb->action.flip_vert = _toolbar_item_add
     (sb, "object-flip-vertical", "Flip Vert.", 30, _go_flip_vert);

   elm_toolbar_item_tooltip_text_set
     (sb->action.rotate_counterclock,
      "Rotate object to the left (counter-clockwise)");
   elm_toolbar_item_tooltip_text_set
     (sb->action.rotate_clock, "Rotate object to the right (clockwise)");

   elm_toolbar_item_tooltip_text_set
     (sb->action.flip_horiz, "Flip object horizontally");
   elm_toolbar_item_tooltip_text_set
     (sb->action.flip_vert, "Flip object vertically");
#endif

   sb->orient_layout = elm_layout_add(layout);
   if (!elm_layout_theme_set
       (sb->orient_layout, "layout", "ephoto", "orient"))
     {
        ERR("could not load style 'ephoto/orient' from theme");
        goto error;
     }
   sb->orient_edje = elm_layout_edje_get(sb->orient_layout);
   elm_layout_content_set(sb->layout, "elm.swallow.content", sb->orient_layout);
   elm_object_focus_custom_chain_append(sb->layout, sb->orient_layout, NULL);

   _ephoto_single_browser_toolbar_eval(sb);

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_single_populate_end, sb));

   sb->handlers = eina_list_append
      (sb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE, _ephoto_single_entry_create, sb));

   return layout;

 error:
   evas_object_del(layout);
   return NULL;
}

void
ephoto_single_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Single_Browser *sb = evas_object_data_get(obj, "single_browser");
   Eina_List *l;
   Ephoto_Entry *itr; 
   Eina_Bool same_file = EINA_FALSE;
   EINA_SAFETY_ON_NULL_RETURN(sb);

   DBG("entry %p, was %p", entry, sb->entry);

   if (sb->entry)
     {
        ephoto_entry_free_listener_del(sb->entry, _entry_free, sb);
        if (entry && entry->path == sb->entry->path)
          same_file = EINA_TRUE;
     }

   sb->entry = entry;

   if (entry)
     ephoto_entry_free_listener_add(entry, _entry_free, sb);

   if (!sb->entry || same_file)
     _ephoto_single_browser_toolbar_eval(sb);
   else
     {
        _ephoto_single_browser_recalc(sb);
        _zoom_fit(sb);
     }
   eina_list_free(sb->entries);
   sb->entries = NULL;
   EINA_LIST_FOREACH(sb->ephoto->entries, l, itr)
     {
        sb->entries = eina_list_append(sb->entries, itr);
        if (itr == entry) sb->current_index = eina_list_last(sb->entries);
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
