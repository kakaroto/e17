#include "ephoto.h"

#define ZOOM_MAX 512
#define ZOOM_MED 256
#define ZOOM_MIN 128
#define ZOOM_STEP 32

static Evas_Object *_ephoto_thumbnail_icon_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__);
static char *_ephoto_thumbnail_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__);
static void _ephoto_thumbnail_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__);
static void _ephoto_change_dir(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info);
static void _ephoto_zoom_in(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_zoom_out(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_show_flow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);

static const Elm_Gengrid_Item_Class _ephoto_thumbnail_class = {
     "thumb",
     {
        _ephoto_thumbnail_label_get,
        _ephoto_thumbnail_icon_get,
        NULL,
        _ephoto_thumbnail_del
     }
   };

typedef struct _Ephoto_Thumb_Browser Ephoto_Thumb_Browser;
struct _Ephoto_Thumb_Browser
{
   Evas_Object *box;
   Evas_Object *dir_entry;
   Evas_Object *grid;
   Evas_Object *toolbar;
   int thumb_size;
   struct {
      Elm_Toolbar_Item *zoom_in;
      Elm_Toolbar_Item *zoom_out;
      Elm_Toolbar_Item *view_flow;
      Elm_Toolbar_Item *slideshow;
   } action;
};

Ephoto_Thumb_Browser *etb;

Evas_Object *
ephoto_thumb_browser_add(void)
{
   etb = calloc(1, sizeof(Ephoto_Thumb_Browser));
   etb->thumb_size = ZOOM_MED;

   etb->box = elm_box_add(ephoto->win);
   elm_box_horizontal_set(etb->box, EINA_FALSE);
   elm_box_homogenous_set(etb->box, EINA_FALSE);
   evas_object_size_hint_weight_set
     (etb->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ephoto->win, etb->box);
   evas_object_show(etb->box);

   etb->toolbar = elm_toolbar_add(etb->box);
   elm_toolbar_homogenous_set(etb->toolbar, EINA_TRUE);
   elm_toolbar_mode_shrink_set(etb->toolbar, ELM_TOOLBAR_SHRINK_MENU);
   elm_toolbar_menu_parent_set(etb->toolbar, etb->box);
   evas_object_size_hint_weight_set(etb->toolbar, 0.0, 0.0);
   evas_object_size_hint_align_set(etb->toolbar, EVAS_HINT_FILL, 0.0);

   etb->action.zoom_in = elm_toolbar_item_append
     (etb->toolbar, "zoom-in", "Zoom In", _ephoto_zoom_in, NULL);
   elm_toolbar_item_priority_set(etb->action.zoom_in, 0);
   etb->action.zoom_out = elm_toolbar_item_append
     (etb->toolbar, "zoom-out", "Zoom Out", _ephoto_zoom_out, NULL);
   elm_toolbar_item_priority_set(etb->action.zoom_out, 1);
   etb->action.view_flow = elm_toolbar_item_append
     (etb->toolbar, "image", "View Flow", _ephoto_show_flow, NULL);
   elm_toolbar_item_priority_set(etb->action.view_flow, 2);
   etb->action.slideshow = elm_toolbar_item_append
     (etb->toolbar, "media-playback-start", "Slideshow", NULL, NULL);
   elm_toolbar_item_priority_set(etb->action.slideshow, 3);

   elm_toolbar_icon_size_set(etb->toolbar, 32);
   elm_box_pack_end(etb->box, etb->toolbar);
   evas_object_show(etb->toolbar);

   etb->dir_entry = elm_fileselector_entry_add(etb->box);
   elm_fileselector_entry_path_set(etb->dir_entry, ephoto->directory);
   elm_fileselector_entry_button_label_set(etb->dir_entry, "Choose");
   elm_fileselector_entry_is_save_set(etb->dir_entry, EINA_FALSE);
   elm_fileselector_entry_inwin_mode_set(etb->dir_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(etb->dir_entry, 0.0, 0.0);
   evas_object_size_hint_align_set(etb->dir_entry, EVAS_HINT_FILL, 0.0);
   evas_object_smart_callback_add
     (etb->dir_entry, "file,chosen", _ephoto_change_dir, NULL);
   elm_box_pack_end(etb->box, etb->dir_entry);
   evas_object_show(etb->dir_entry);

   etb->grid = elm_gengrid_add(etb->box);
   elm_gengrid_align_set(etb->grid, 0.5, 0.5);
   elm_gengrid_bounce_set(etb->grid, EINA_FALSE, EINA_TRUE);
   elm_gengrid_item_size_set(etb->grid, etb->thumb_size, etb->thumb_size);
   evas_object_size_hint_align_set
     (etb->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set
     (etb->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(etb->box, etb->grid);
   evas_object_show(etb->grid);

   return etb->box;
}

void
ephoto_thumb_browser_del(void)
{
   evas_object_del(etb->box);
   free(etb);
}

void
ephoto_thumb_browser_show(void)
{
   ephoto->prev_state = ephoto->state;
   ephoto->state = EPHOTO_STATE_THUMB;

   elm_pager_content_promote(ephoto->pager, ephoto->thumb_browser);
}

void 
ephoto_thumb_browser_thumb_append(const char *file)
{
   char *f;
   const Elm_Gengrid_Item_Class *egic;

   f = strdup(file);

   egic = &_ephoto_thumbnail_class;
   elm_gengrid_item_append(etb->grid, egic, f, NULL, NULL);
}

static Evas_Object *
_ephoto_thumbnail_icon_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Evas_Object *o;
   const char *file = data;

   ethumb_client_format_set(ephoto->client, ETHUMB_THUMB_FDO);
   ethumb_client_size_set(ephoto->client, etb->thumb_size, etb->thumb_size);

   o = elm_thumb_add(ephoto->win);
   elm_object_style_set(o, "noframe");
   elm_thumb_file_set(o, file, NULL);
   evas_object_show(o);

   return o;
}

static char *
_ephoto_thumbnail_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   const char *file = data, *f;

   f = ecore_file_file_get(file);

   return strdup(f);
}

static void
_ephoto_thumbnail_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{

}

static void 
_ephoto_change_dir(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   const char *path = event_info;

   if (!path) return;

   elm_gengrid_clear(etb->grid);
   ephoto->images = eina_list_free(ephoto->images);
   ephoto->current_index = NULL;
   eina_stringshare_del(ephoto->directory);
   ephoto->directory = eina_stringshare_add(path);
   ephoto_populate();
}

static void 
_ephoto_zoom_in(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set(etb->action.zoom_in, EINA_FALSE);
   etb->thumb_size += ZOOM_STEP;
   if (etb->thumb_size >= ZOOM_MAX)
     etb->thumb_size = ZOOM_MAX;
   elm_gengrid_item_size_set(etb->grid, etb->thumb_size, etb->thumb_size);
   elm_toolbar_item_disabled_set(etb->action.zoom_in, etb->thumb_size == ZOOM_MAX);
   elm_toolbar_item_disabled_set(etb->action.zoom_out, etb->thumb_size == ZOOM_MIN);
}

static void 
_ephoto_zoom_out(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set(etb->action.zoom_out, EINA_FALSE);
   etb->thumb_size -= ZOOM_STEP;
   if (etb->thumb_size <= ZOOM_MIN)
     etb->thumb_size = ZOOM_MIN;
   elm_gengrid_item_size_set(etb->grid, etb->thumb_size, etb->thumb_size);
   elm_toolbar_item_disabled_set(etb->action.zoom_out, etb->thumb_size == ZOOM_MIN);
   elm_toolbar_item_disabled_set(etb->action.zoom_in, etb->thumb_size == ZOOM_MAX);
}

static void
_ephoto_show_flow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set
     (elm_toolbar_selected_item_get(etb->toolbar), EINA_FALSE);

   ephoto_flow_browser_show(); 
}
