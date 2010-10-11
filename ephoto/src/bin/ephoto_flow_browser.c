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
_viewer_del(void *data, Evas *e, Evas_Object *obj, void *event_info)
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
_mouse_wheel(void *data, Evas *e, Evas_Object *o, void *event_info)
{
   Ephoto_Flow_Browser *fb = data;
   Evas_Event_Mouse_Wheel *ev = event_info;
   if (!evas_key_modifier_is_set(ev->modifiers, "Control")) return;

   if (ev->z > 0) _zoom_set(fb, fb->zoom + ZOOM_STEP);
   else _zoom_set(fb, fb->zoom - ZOOM_STEP);
}

static Ephoto_Entry *
_find_first_entry(Ephoto_Flow_Browser *fb)
{
   const Eina_List *l;
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN_VAL(fb->ephoto, NULL);

   EINA_LIST_FOREACH(fb->ephoto->entries, l, entry)
     if (!entry->is_dir) return entry;
   return NULL;
}

static Ephoto_Entry *
_find_last_entry(Ephoto_Flow_Browser *fb)
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

        if (fb->entry == _find_first_entry(fb))
          edje_object_signal_emit(fb->edje, "prev,disable", "ephoto");
        else
          edje_object_signal_emit(fb->edje, "prev,enable", "ephoto");

        if (fb->entry == _find_last_entry(fb))
          edje_object_signal_emit(fb->edje, "next,disable", "ephoto");
        else
          edje_object_signal_emit(fb->edje, "next,enable", "ephoto");
     }
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
_zoom_in(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Ephoto_Flow_Browser *fb = data;
   _zoom_set(fb, fb->zoom + ZOOM_STEP);
}

static void
_zoom_out(void *data, Evas_Object *o, const char *emission, const char *source)
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
   Ephoto_Entry *entry = _find_first_entry(fb);
   if (!entry) return;
   DBG("first is '%s'", entry->path);
   ephoto_flow_browser_entry_set(fb->layout, entry);
}

static void
_last_entry(Ephoto_Flow_Browser *fb)
{
   Ephoto_Entry *entry = _find_last_entry(fb);
   if (!entry) return;
   DBG("last is '%s'", entry->path);
   ephoto_flow_browser_entry_set(fb->layout, entry);
}

static void
_next_button(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Ephoto_Flow_Browser *fb = data;
   _next_entry(fb);
}

static void
_prev_button(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Ephoto_Flow_Browser *fb = data;
   _prev_entry(fb);
}

static void
_back(void *data, Evas_Object *o, const char *emission, const char *source)
{
   Ephoto_Flow_Browser *fb = data;
   evas_object_smart_callback_call(fb->layout, "back", fb->entry);
}

static void
_key_down(void *data, Evas *e, Evas_Object *o, void *event_info)
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
}

static void
_layout_del(void *data, Evas *e, Evas_Object *o, void *event_info)
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


#if 0

/*Callbacks*/
static void _ephoto_go_back(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_first(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_last(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_next(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_previous(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_slideshow(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_rotate_counterclockwise(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_rotate_clockwise(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_editor(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_flow_browser_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_flow_browser_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_zoom_in(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_zoom_out(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_zoom_regular_size(void *data, Evas_Object *obj, void *event_info);

typedef struct _Ephoto_Flow_Browser Ephoto_Flow_Browser;

struct _Ephoto_Flow_Browser
{
        Evas_Object *flow_browser;
        Eina_List *iter;
        Evas_Object *image;
        Evas_Object *image2;
        Evas_Object *toolbar;
        const char *cur_image;

        Ephoto_Orient orient;
};

static const char *toolbar_items[] = {
	"First",
	"Previous",
	"Next",
	"Last",
	"Slideshow",
	"Rotate CW",
	"Rotate CCW",
	"Zoom In",
	"Zoom Out"
};

static void
_ephoto_set_title(const char *file)
{
	char *buffer;
	int length;

	length = strlen(file) + strlen("Ephoto - ") + 1;
	buffer = alloca(length);
	snprintf(buffer, length, "Ephoto - %s", file);
	elm_win_title_set(em->win, buffer);
}

static void
_photocal_loaded_cb(void *data, Evas_Object *obj, void *event)
{
  Ephoto_Flow_Browser *ef = data;
  printf("Load\n");
  elm_photocam_paused_set(ef->image, EINA_FALSE);
}

static void
_ephoto_go_update(Ephoto_Flow_Browser *ef)
{
	const char *file_type;
        int success = 0;

	elm_layout_content_unset(ef->flow_browser, "ephoto.flow.swallow");

	evas_object_hide(ef->image);
        evas_object_hide(ef->image2);

	file_type = efreet_mime_type_get(ef->cur_image);
	if (file_type && !strcmp(file_type, "image/jpeg"))
	{
		success = elm_photocam_file_set(ef->image, ef->cur_image) == EVAS_LOAD_ERROR_NONE;
		elm_photocam_zoom_mode_set(ef->image, ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);     
		evas_object_smart_callback_add(ef->image, "loaded", _photocal_loaded_cb, ef);
	    elm_photocam_paused_set(ef->image, EINA_TRUE);
		elm_layout_content_set(ef->flow_browser, "ephoto.flow.swallow", ef->image);
		
		evas_object_show(ef->image);
		evas_object_hide(ef->image2);

#ifdef HAVE_LIBEXIF
                int orientation = 0;

                ExifData  *exif = exif_data_new_from_file(ef->cur_image);
                ExifEntry *entry = NULL;
                ExifByteOrder bo;
               
                if (exif)
                {
                        entry = exif_data_get_entry(exif, EXIF_TAG_ORIENTATION);
                        if (entry)
                        {
                                bo = exif_data_get_byte_order(exif);
                                orientation = exif_get_short(entry->data, bo);
                        }
                        exif_data_free(exif);
                }

                if (orientation > 1 && orientation < 9)
                {
                        Evas_Object *o = elm_layout_edje_get(ef->flow_browser);

                        switch (orientation)
                        {
                                case 2:		/* Horizontal flip */
                                        break;
                                case 3:		/* Rotate 180 clockwise */
                                        ef->orient = EPHOTO_ORIENT_180;
                                        edje_object_signal_emit(o, "ef,state,rotate,180", "ef");
                                        break;
                                case 4:		/* Vertical flip */
                                        break;
                                case 5:		/* Transpose */
                                        break;
                                case 6:		/* Rotate 90 clockwise */
                                        ef->orient = EPHOTO_ORIENT_90;
                                        edje_object_signal_emit(o, "ef,state,rotate,90", "ef");
                                        break;
                                case 7:		/* Transverse */
                                        break;
                                case 8:		/* Rotate 90 counter-clockwise */
                                        ef->orient = EPHOTO_ORIENT_270;
                                        edje_object_signal_emit(o, "ef,state,rotate,270", "ef");
                                        break;
                        }
                }
#endif
	} else {
  	        success = elm_image_file_set(ef->image2, ef->cur_image, NULL);
		elm_layout_content_set(ef->flow_browser, "ephoto.flow.swallow", ef->image2);
		evas_object_show(ef->image2);
		evas_object_hide(ef->image);
	}

	if (success)
                _ephoto_set_title(ef->cur_image);

        elm_toolbar_item_unselect_all(ef->toolbar);
}

/*Create the flow browser*/
Evas_Object *
ephoto_create_flow_browser(Evas_Object *parent)
{
	Evas_Object *o;
	Ephoto_Flow_Browser *ef;

	ef = calloc(1, sizeof(Ephoto_Flow_Browser));
	
	ef->flow_browser = elm_layout_add(parent);
	elm_layout_file_set(ef->flow_browser,
			    PACKAGE_DATA_DIR "/themes/default/ephoto.edj", 
			    "ephoto/flow/layout");
	//elm_win_resize_object_add(em->win, em->flow_browser);
	evas_object_size_hint_weight_set(ef->flow_browser, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ef->flow_browser, EVAS_HINT_FILL, EVAS_HINT_FILL);

	ef->image = elm_photocam_add(ef->flow_browser);
        evas_object_size_hint_weight_set(ef->image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(ef->image, EVAS_HINT_FILL, EVAS_HINT_FILL);

	ef->image2 = elm_image_add(ef->flow_browser);
	elm_image_smooth_set(ef->image2, EINA_TRUE);
        evas_object_size_hint_weight_set(ef->image2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(ef->image2, EVAS_HINT_FILL, EVAS_HINT_FILL);

	ef->toolbar = elm_toolbar_add(ef->flow_browser);
        elm_toolbar_icon_size_set(ef->toolbar, 24);
        elm_toolbar_homogenous_set(ef->toolbar, EINA_TRUE);
        elm_toolbar_scrollable_set(ef->toolbar, EINA_FALSE);
        evas_object_size_hint_weight_set(ef->toolbar, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(ef->toolbar, EVAS_HINT_FILL, 0.5);
	elm_layout_content_set(ef->flow_browser, "ephoto.toolbar.swallow", ef->toolbar);
	evas_object_show(ef->toolbar);

	o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/go_back.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Go Back", _ephoto_go_back, ef);

        o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/first.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "First", _ephoto_go_first, ef);

	o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/previous.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Previous", _ephoto_go_previous, ef);

	o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/next.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Next", _ephoto_go_next, ef);

        o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/last.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Last", _ephoto_go_last, ef);

	o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Slideshow", _ephoto_go_slideshow, ef);

        o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "RotateCW", _ephoto_go_rotate_clockwise, ef);

        o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "RotateCCW", _ephoto_go_rotate_counterclockwise, ef);

        o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Zoom In", _ephoto_zoom_in, ef);

        o = elm_icon_add(ef->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(ef->toolbar, o, "Zoom Out", _ephoto_zoom_out, ef);

	evas_object_event_callback_add(ef->flow_browser, EVAS_CALLBACK_SHOW,
						_ephoto_flow_browser_show_cb, ef);
	evas_object_event_callback_add(ef->flow_browser, EVAS_CALLBACK_DEL,
						_ephoto_flow_browser_del_cb, ef);
	evas_object_data_set(ef->flow_browser, "flow_browser", ef);

	return ef->flow_browser;
}

/*Show the flow browser*/

void
ephoto_flow_browser_image_set(Evas_Object *obj, const char *current_image)
{	
        Ephoto_Flow_Browser *ef;
	Elm_Toolbar_Item *o;
	unsigned int i;

        ef = evas_object_data_get(obj, "flow_browser");

	if (current_image)
        {
		ef->cur_image = current_image;

		evas_object_event_callback_add(ef->flow_browser, EVAS_CALLBACK_KEY_DOWN,
						_ephoto_key_pressed, ef);
		evas_object_event_callback_add(ef->flow_browser, EVAS_CALLBACK_MOUSE_WHEEL,
						_ephoto_mouse_wheel, ef);

		// TODO: ef->iter = eina_list_data_find_list(em->images, current_image);
		for (i = 0; i < (sizeof (toolbar_items) / sizeof (char*)); ++i)
		{
			o = elm_toolbar_item_find_by_label(ef->toolbar, toolbar_items[i]);
			elm_toolbar_item_disabled_set(o, !ef->iter ? EINA_TRUE : EINA_FALSE);
		}

                DBG("iter: %p", ef->iter);

		_ephoto_go_update(ef);
	}
	else
	{
		for (i = 0; i < (sizeof (toolbar_items) / sizeof (char*)); ++i)
                {
                        o = elm_toolbar_item_find_by_label(ef->toolbar, toolbar_items[i]);
                        elm_toolbar_item_disabled_set(o, EINA_TRUE);
                }
	}
	evas_object_show(ef->toolbar);
	evas_object_show(ef->flow_browser);
	evas_object_focus_set(ef->flow_browser, 1);
}

static void
_ephoto_flow_browser_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{

}

/*Delete the flow browser*/
static void 
_ephoto_flow_browser_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{

}

/*A key has been pressed*/
static const struct
{
	const char *name;
	const char *modifiers;
	void (*func)(void *data, Evas_Object *obj, void *event_info);
} keys[] = {
	{ "Left", NULL, _ephoto_go_previous },
	{ "Right", NULL, _ephoto_go_next },
	{ "space", NULL, _ephoto_go_next },
	{ "Escape", NULL, _ephoto_go_back },
	{ "bracketleft", NULL, _ephoto_go_rotate_counterclockwise },
	{ "bracketright", NULL, _ephoto_go_rotate_clockwise },
	{ "Home", NULL, _ephoto_go_first },
	{ "End", NULL, _ephoto_go_last },
	{ "F5", NULL, _ephoto_go_slideshow},
	{ "plus", "Control", _ephoto_zoom_in},
	{ "minus", "Control", _ephoto_zoom_out},
	{ "0", "Control", _ephoto_zoom_regular_size},
        { "e", NULL, _ephoto_go_editor },
	{ NULL, NULL, NULL }
};

static void
_ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Key_Down *eku;
	int i;

	eku = (Evas_Event_Key_Down *)event_data;
	DBG("Key name: %s", eku->key);
	for (i = 0; keys[i].name; ++i)
		if ((!strcmp(eku->key, keys[i].name)) &&
		    ((keys[i].modifiers == NULL) || (evas_key_modifier_is_set(eku->modifiers, keys[i].modifiers))))
			keys[i].func(data, NULL, NULL);
}

static void
_ephoto_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Mouse_Wheel *emw = (Evas_Event_Mouse_Wheel *) event_data;
	if (evas_key_modifier_is_set(emw->modifiers, "Control"))
	{
		if (emw->z < 0)
			_ephoto_zoom_in(data, NULL, NULL);
		else
			_ephoto_zoom_out(data, NULL, NULL);
	}
}

/*Go back to the thumbnail viewer*/
static void 
_ephoto_go_back(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Flow_Browser *ef = data;
    
	evas_object_smart_callback_call(ef->flow_browser, "delete,request", NULL);
	/* elm_toolbar_item_unselect_all(ef->toolbar); */
	/* em->thumb_browser = ephoto_create_thumb_browser(em->layout); */
	/* elm_layout_content_set(em->layout, "ephoto.content.swallow", em->thumb_browser); */
    
}

/*Go to the very first image in the list*/
static void 
_ephoto_go_first(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Flow_Browser *ef = data;

	// TODO: ef->iter = em->images;

	ef->cur_image = eina_list_data_get(ef->iter);
	_ephoto_go_update(ef);
}

/*Go to the very last image in the list*/
static void 
_ephoto_go_last(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Flow_Browser *ef = data;

	// TODO: ef->iter = eina_list_last(em->images);
	ef->cur_image = eina_list_data_get(ef->iter);

	_ephoto_go_update(ef);
}

/*Go to the next image in the list*/
static void 
_ephoto_go_next(void *data, Evas_Object *obj, void *event_info)
{
	Ephoto_Flow_Browser *ef = data;

	ef->iter = eina_list_next(ef->iter);
	// TODO: if (!ef->iter) ef->iter = em->images;

	ef->cur_image = eina_list_data_get(ef->iter);

	_ephoto_go_update(ef);
}

/*Go to the previous image in the list*/
static void 
_ephoto_go_previous(void *data, Evas_Object *obj, void *event_info)
{
	Ephoto_Flow_Browser *ef = data;

	ef->iter = eina_list_prev(ef->iter);
        /* TODO:
	if (!ef->iter)
		ef->iter = eina_list_last(em->images);
        */

	ef->cur_image = eina_list_data_get(ef->iter);

	_ephoto_go_update(ef);
}

/*Go to the slideshow*/
static void
_ephoto_go_slideshow(void *data, Evas_Object *obj, void *event_info)
{   
        Ephoto_Flow_Browser *ef = data;
        // FIXME
	//ephoto_hide_flow_browser();
	// TODO ephoto_show_slideshow(1, ef->cur_image);
	elm_toolbar_item_unselect_all(ef->toolbar);
}

static void
_ephoto_go_rotate_counterclockwise(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Flow_Browser *ef = data;
        Evas_Object *o = elm_layout_edje_get(ef->flow_browser);

        switch(ef->orient)
        {
                case EPHOTO_ORIENT_0:
                        ef->orient = EPHOTO_ORIENT_270;
                        edje_object_signal_emit(o, "ef,state,rotate,270", "ef");
                        break;
                case EPHOTO_ORIENT_90:
                        ef->orient = EPHOTO_ORIENT_0;
                        edje_object_signal_emit(o, "ef,state,rotate,0", "ef");
                        break;
                case EPHOTO_ORIENT_180:
                        ef->orient = EPHOTO_ORIENT_90;
                        edje_object_signal_emit(o, "ef,state,rotate,90", "ef");
                        break;
                case EPHOTO_ORIENT_270:
                        ef->orient = EPHOTO_ORIENT_180;
                        edje_object_signal_emit(o, "ef,state,rotate,180", "ef");
                        break;
        }
	elm_toolbar_item_unselect_all(ef->toolbar);
}

static void
_ephoto_go_rotate_clockwise(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Flow_Browser *ef = data;
        Evas_Object *o = elm_layout_edje_get(ef->flow_browser);

        switch(ef->orient)
        {
                case EPHOTO_ORIENT_0:
                        ef->orient = EPHOTO_ORIENT_90;
                        edje_object_signal_emit(o, "ef,state,rotate,90", "ef");
                        break;
                case EPHOTO_ORIENT_90:
                        ef->orient = EPHOTO_ORIENT_180;
                        edje_object_signal_emit(o, "ef,state,rotate,180", "ef");
                        break;
                case EPHOTO_ORIENT_180:
                        ef->orient = EPHOTO_ORIENT_270;
                        edje_object_signal_emit(o, "ef,state,rotate,270", "ef");
                        break;
                case EPHOTO_ORIENT_270:
                        ef->orient = EPHOTO_ORIENT_0;
                        edje_object_signal_emit(o, "ef,state,rotate,0", "ef");
                        break;
        }
	elm_toolbar_item_unselect_all(ef->toolbar);
}

static void
_ephoto_go_editor(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Flow_Browser *ef = data;
        Ecore_Exe *exe;
        char buf[PATH_MAX];

        snprintf(buf, sizeof(buf), em->config->editor, (char *) ef->cur_image);
        DBG("Editor command: %s", buf);
        exe = ecore_exe_run(buf, NULL);
        ecore_exe_free(exe);
}

/* Zoom in in image */
static void
_ephoto_zoom_in(void *data, Evas_Object *obj, void *event)
{
	Ephoto_Flow_Browser *ef = data;
	double zoom;

	elm_photocam_zoom_mode_set(ef->image, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
	zoom = elm_photocam_zoom_get(ef->image);
	zoom -= 0.4;
	if (zoom < 0.1)
		zoom = 0.1;
	elm_photocam_zoom_set(ef->image, zoom);
	elm_toolbar_item_unselect_all(ef->toolbar);
}

/* Zoom out in image */
static void
_ephoto_zoom_out(void *data, Evas_Object *obj, void *event)
{
	Ephoto_Flow_Browser *ef = data;
	double zoom;

	elm_photocam_zoom_mode_set(ef->image, ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
	zoom = elm_photocam_zoom_get(ef->image);
	zoom += 0.4;
	elm_photocam_zoom_set(ef->image, zoom);
	elm_toolbar_item_unselect_all(ef->toolbar);
}

/* Zoom regular size in image */
static void
_ephoto_zoom_regular_size(void *data, Evas_Object *obj, void *event)
{
	Ephoto_Flow_Browser *ef = data;
	elm_photocam_zoom_mode_set(ef->image, ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
}

#endif
