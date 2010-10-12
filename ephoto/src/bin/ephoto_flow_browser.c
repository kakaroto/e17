#include "ephoto.h"

#ifdef HAVE_LIBEXIF
#include <libexif/exif-data.h>
#endif

#define ZOOM_MIN 0.1
#define ZOOM_MAX 10.0
#define ZOOM_STEP 0.2

typedef struct _Ephoto_Flow_Browser Ephoto_Flow_Browser;
typedef struct _Ephoto_Viewer Ephoto_Viewer;

struct _Ephoto_Flow_Browser
{
   Ephoto *ephoto;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *viewer;
   const char *path;
   Ephoto_Entry *entry;
   Ephoto_Orient orient;
   double zoom;
};

struct _Ephoto_Viewer
{
   Evas_Object *photocam;
   Evas_Object *scroller;
   Evas_Object *image;
};

static void _zoom_set(Ephoto_Flow_Browser *fb, double zoom);

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
_viewer_zoom_set(Evas_Object *obj, float zoom)
{
   Ephoto_Viewer *v = evas_object_data_get(obj, "viewer");
   EINA_SAFETY_ON_NULL_RETURN(v);

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
_orient_apply(Ephoto_Flow_Browser *fb)
{
   const char *sig;
   switch (fb->orient)
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
      default:
         return;
     }
   edje_object_signal_emit(fb->edje, sig, "ephoto");
}

static void
_rotate_counterclock(Ephoto_Flow_Browser *fb)
{
   switch (fb->orient)
     {
      case EPHOTO_ORIENT_0:
         fb->orient = EPHOTO_ORIENT_270;
         break;
      case EPHOTO_ORIENT_90:
         fb->orient = EPHOTO_ORIENT_0;
         break;
      case EPHOTO_ORIENT_180:
         fb->orient = EPHOTO_ORIENT_90;
         break;
      case EPHOTO_ORIENT_270:
         fb->orient = EPHOTO_ORIENT_180;
         break;
     }
   _orient_apply(fb);
}

static void
_rotate_clock(Ephoto_Flow_Browser *fb)
{
   switch (fb->orient)
     {
      case EPHOTO_ORIENT_0:
         fb->orient = EPHOTO_ORIENT_90;
         break;
      case EPHOTO_ORIENT_90:
         fb->orient = EPHOTO_ORIENT_180;
         break;
      case EPHOTO_ORIENT_180:
         fb->orient = EPHOTO_ORIENT_270;
         break;
      case EPHOTO_ORIENT_270:
         fb->orient = EPHOTO_ORIENT_0;
         break;
     }
   _orient_apply(fb);
}

static void
_mouse_wheel(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Flow_Browser *fb = data;
   Evas_Event_Mouse_Wheel *ev = event_info;
   if (!evas_key_modifier_is_set(ev->modifiers, "Control")) return;

   if (ev->z > 0) _zoom_set(fb, fb->zoom + ZOOM_STEP);
   else _zoom_set(fb, fb->zoom - ZOOM_STEP);
}

static Ephoto_Entry *
_first_entry_find(Ephoto_Flow_Browser *fb)
{
   const Eina_List *l;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN_VAL(fb->ephoto, NULL);

   EINA_LIST_FOREACH(fb->ephoto->entries, l, entry)
     if (!entry->is_dir) return entry;
   return NULL;
}

static Ephoto_Entry *
_last_entry_find(Ephoto_Flow_Browser *fb)
{
   const Eina_List *l;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN_VAL(fb->ephoto, NULL);

   EINA_LIST_REVERSE_FOREACH(fb->ephoto->entries, l, entry)
     if (!entry->is_dir) return entry;
   return NULL;
}

static void
_ephoto_flow_browser_toolbar_eval(Ephoto_Flow_Browser *fb)
{
   if (!fb->entry)
     {
        edje_object_signal_emit(fb->edje, "slideshow,disable", "ephoto");
        edje_object_signal_emit(fb->edje, "prev,disable", "ephoto");
        edje_object_signal_emit(fb->edje, "next,disable", "ephoto");
     }
   else
     {
        edje_object_signal_emit(fb->edje, "slideshow,enable", "ephoto");

        if (fb->entry == _first_entry_find(fb))
          edje_object_signal_emit(fb->edje, "prev,disable", "ephoto");
        else
          edje_object_signal_emit(fb->edje, "prev,enable", "ephoto");

        if (fb->entry == _last_entry_find(fb))
          edje_object_signal_emit(fb->edje, "next,disable", "ephoto");
        else
          edje_object_signal_emit(fb->edje, "next,enable", "ephoto");
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
   int exif_orient;

   if (!_path_is_jpeg(path)) return orient;

   exif = exif_data_new_from_file(path);
   EINA_SAFETY_ON_NULL_GOTO(exif, end);
   bo = exif_data_get_byte_order(exif);
   entry = exif_data_get_entry(exif, EXIF_TAG_ORIENTATION);
   EINA_SAFETY_ON_NULL_GOTO(entry, end_entry);

   exif_orient = exif_get_short(entry->data, bo);
   DBG("exif_orient=%d", exif_orient);
   switch (exif_orient)
     {
      case 1:
         orient = EPHOTO_ORIENT_0;
         break;
      case 3:
         orient = EPHOTO_ORIENT_180;
         break;
      case 6:
         orient = EPHOTO_ORIENT_90;
         break;
      case 8:
         orient = EPHOTO_ORIENT_270;
         break;
      default:
         ERR("exif orient not supported: %d", exif_orient);
     }

 end_entry:
   exif_data_free(exif);
 end:
   return orient;
#endif
}

static void
_ephoto_flow_browser_recalc(Ephoto_Flow_Browser *fb)
{
   if (fb->viewer)
     {
        evas_object_del(fb->viewer);
        fb->viewer = NULL;
     }

   if (fb->path)
     {
        const char *bname = ecore_file_file_get(fb->path);
        fb->viewer = _viewer_add(fb->layout, fb->path);
        elm_layout_content_set(fb->layout, "ephoto.swallow.flow", fb->viewer);
        evas_object_event_callback_add
          (fb->viewer, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, fb);
        edje_object_part_text_set(fb->edje, "ephoto.text.title", bname);
        ephoto_title_set(fb->ephoto, bname);
        fb->orient = ephoto_file_orient_get(fb->path);
        _orient_apply(fb);
     }

   _ephoto_flow_browser_toolbar_eval(fb);
}

static void
_zoom_set(Ephoto_Flow_Browser *fb, double zoom)
{
   if (zoom > ZOOM_MAX) zoom = ZOOM_MAX;
   else if (zoom < ZOOM_MIN) zoom = ZOOM_MIN;

   DBG("zoom %f", zoom);
   _viewer_zoom_set(fb->viewer, zoom);
   fb->zoom = zoom;

   if (zoom <= ZOOM_MIN)
     edje_object_signal_emit(fb->edje, "zoom_out,disable", "ephoto");
   else
     edje_object_signal_emit(fb->edje, "zoom_out,enable", "ephoto");

   if (zoom >= ZOOM_MAX)
     edje_object_signal_emit(fb->edje, "zoom_in,disable", "ephoto");
   else
     edje_object_signal_emit(fb->edje, "zoom_in,enable", "ephoto");
}

static void
_zoom_in(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto_Flow_Browser *fb = data;
   _zoom_set(fb, fb->zoom + ZOOM_STEP);
}

static void
_zoom_out(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto_Flow_Browser *fb = data;
   _zoom_set(fb, fb->zoom - ZOOM_STEP);
}

static void
_next_entry(Ephoto_Flow_Browser *fb)
{
   Elm_Gengrid_Item *it;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN(fb->entry);
   EINA_SAFETY_ON_NULL_RETURN(fb->entry->item);

   it = fb->entry->item;
   while ((it = elm_gengrid_item_next_get(it)))
     {
        entry = elm_gengrid_item_data_get(it);
        if (!entry->is_dir) break;
     }
   if (!it) return;
   DBG("next is '%s'", entry->path);
   ephoto_flow_browser_entry_set(fb->layout, entry);
}

static void
_prev_entry(Ephoto_Flow_Browser *fb)
{
   Elm_Gengrid_Item *it;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN(fb->entry);
   EINA_SAFETY_ON_NULL_RETURN(fb->entry->item);

   it = fb->entry->item;
   while ((it = elm_gengrid_item_prev_get(it)))
     {
        entry = elm_gengrid_item_data_get(it);
        if (!entry->is_dir) break;
     }
   if (!it) return;
   DBG("prev is '%s'", entry->path);
   ephoto_flow_browser_entry_set(fb->layout, entry);
}

static void
_first_entry(Ephoto_Flow_Browser *fb)
{
   Ephoto_Entry *entry = _first_entry_find(fb);
   if (!entry) return;
   DBG("first is '%s'", entry->path);
   ephoto_flow_browser_entry_set(fb->layout, entry);
}

static void
_last_entry(Ephoto_Flow_Browser *fb)
{
   Ephoto_Entry *entry = _last_entry_find(fb);
   if (!entry) return;
   DBG("last is '%s'", entry->path);
   ephoto_flow_browser_entry_set(fb->layout, entry);
}

static void
_next_button(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto_Flow_Browser *fb = data;
   _next_entry(fb);
}

static void
_prev_button(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto_Flow_Browser *fb = data;
   _prev_entry(fb);
}

static void
_back(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto_Flow_Browser *fb = data;
   evas_object_smart_callback_call(fb->layout, "back", fb->entry);
}

static void
_key_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Flow_Browser *fb = data;
   Evas_Event_Key_Down *ev = event_info;
   Eina_Bool ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
   const char *k = ev->keyname;

   if (ctrl)
     {
        if ((!strcmp(k, "plus")) || (!strcmp(k, "equal")))
          _zoom_set(fb, fb->zoom + ZOOM_STEP);
        else if (!strcmp(k, "minus"))
          _zoom_set(fb, fb->zoom - ZOOM_STEP);
        else if (!strcmp(k, "0"))
          _zoom_set(fb, 1.0);

        return;
     }

   if (!strcmp(k, "Escape"))
     evas_object_smart_callback_call(fb->layout, "back", fb->entry);
   else if (!strcmp(k, "Left"))
     _prev_entry(fb);
   else if (!strcmp(k, "Right"))
     _next_entry(fb);
   else if (!strcmp(k, "Home"))
     _first_entry(fb);
   else if (!strcmp(k, "End"))
     _last_entry(fb);
   else if (!strcmp(k, "bracketleft"))
     _rotate_counterclock(fb);
   else if (!strcmp(k, "bracketright"))
     _rotate_clock(fb);
   else if (!strcmp(k, "F5"))
     {
        if (fb->entry)
          evas_object_smart_callback_call(fb->layout, "slideshow", fb->entry);
     }
}

static void
_layout_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Flow_Browser *fb = data;
   eina_stringshare_del(fb->path);
   free(fb);
}

Evas_Object *
ephoto_flow_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *layout = elm_layout_add(parent);
   Ephoto_Flow_Browser *fb;

   EINA_SAFETY_ON_NULL_RETURN_VAL(layout, NULL);

   fb = calloc(1, sizeof(Ephoto_Flow_Browser));
   EINA_SAFETY_ON_NULL_GOTO(fb, error);
   fb->ephoto = ephoto;
   fb->layout = layout;
   fb->edje = elm_layout_edje_get(layout);
   fb->zoom = 1.0;
   evas_object_event_callback_add(layout, EVAS_CALLBACK_DEL, _layout_del, fb);
   evas_object_event_callback_add
     (layout, EVAS_CALLBACK_KEY_DOWN, _key_down, fb);
   elm_object_focus_allow_set(layout, EINA_TRUE);
   evas_object_data_set(layout, "flow_browser", fb);

   edje_object_signal_callback_add
     (fb->edje, "zoom_out,clicked", "ephoto", _zoom_out, fb);
   edje_object_signal_callback_add
     (fb->edje, "zoom_in,clicked", "ephoto", _zoom_in, fb);
   edje_object_signal_callback_add
     (fb->edje, "prev,clicked", "ephoto", _prev_button, fb);
   edje_object_signal_callback_add
     (fb->edje, "next,clicked", "ephoto", _next_button, fb);
   edje_object_signal_callback_add
     (fb->edje, "back", "ephoto", _back, fb);

   if (!elm_layout_file_set(layout, THEME_FILE, "ephoto/flow/layout"))
     {
        evas_object_del(layout);
        return NULL;
     }

   _ephoto_flow_browser_toolbar_eval(fb);

   return layout;

 error:
   evas_object_del(layout);
   return NULL;
}

void
ephoto_flow_browser_path_set(Evas_Object *obj, const char *path)
{
   Ephoto_Flow_Browser *fb = evas_object_data_get(obj, "flow_browser");
   EINA_SAFETY_ON_NULL_RETURN(fb);

   DBG("path '%s', was '%s'", path ? path : "", fb->path ? fb->path : "");
   if (!eina_stringshare_replace(&fb->path, path)) return;
   fb->entry = NULL;
   fb->zoom = 1.0;
   _ephoto_flow_browser_recalc(fb);
}

void
ephoto_flow_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_Flow_Browser *fb = evas_object_data_get(obj, "flow_browser");
   EINA_SAFETY_ON_NULL_RETURN(fb);

   DBG("entry %p, was %p", entry, fb->entry);
   fb->entry = entry;
   if (!eina_stringshare_replace(&fb->path, entry->path))
     _ephoto_flow_browser_toolbar_eval(fb);
   else
     {
        fb->zoom = 1.0;
        _ephoto_flow_browser_recalc(fb);
     }
}
