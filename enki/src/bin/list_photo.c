#include "main.h"
#include "evas_object/photo_object.h"
#include "slideshow.h"

#define DEFAULT_W 192
#define DEFAULT_H 128

static PL_Header_Item_Class itc_album;
static PL_Child_Item_Class itc_photo;

static Evas_Object *_album_icon_get(const void *data, Evas_Object *obj);
static Evas_Object *_photo_icon_get(const void *data, Evas_Object *obj);
static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);
static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info);
static void _collection_cb(void *data, Evas_Object *obj, void *event_info);
static void _tag_cb(void *data, Evas_Object *obj, void *event_info);
static void _open(void *data, Evas_Object *obj, void *event_info);
static void _right_click(void *data, Evas_Object *obj, void *event_info);
static void _sorts_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_unselect_all_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_album_select_all_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_album_unselect_all_cb(void *data, Evas_Object *obj, void *event_info);
static void _tg_multiselect_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void _album_sync_flickr_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _photo_sync_flickr_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

List_Photo *list_photo_new(Evas_Object *edje)
{
   Evas_Object *bx, *sl, *bx2, *lbl, *bt, *tg;
   List_Photo *enlil_photo = calloc(1, sizeof(List_Photo));

   enlil_photo->photo_w = DEFAULT_W;
   enlil_photo->photo_h = DEFAULT_H;

   bx = elm_box_add(edje);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   evas_object_show(bx);
   enlil_photo->bx = bx;

   enlil_photo->o_list = photos_list_object_add(edje);
   photos_list_object_sub_items_size_set(enlil_photo->o_list, DEFAULT_W, DEFAULT_H);
   evas_object_smart_callback_add(enlil_photo->o_list, "open", _open, enlil_photo);
   evas_object_smart_callback_add(enlil_photo->o_list, "clicked,right", _right_click, enlil_photo);
   evas_object_smart_callback_add(enlil_photo->o_list, "clicked,menu", _right_click, enlil_photo);
   evas_object_size_hint_weight_set(enlil_photo->o_list, 1.0, 1.0);
   evas_object_size_hint_align_set(enlil_photo->o_list, -1.0, -1.0);
   evas_object_show(enlil_photo->o_list);
   elm_box_pack_end(bx, enlil_photo->o_list);

   itc_album.func.icon_get = _album_icon_get;
   itc_photo.func.icon_get = _photo_icon_get;

   bx2 = elm_box_add(edje);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, -1.0);
   evas_object_size_hint_align_set(bx2, -1.0, 1.0);
   elm_box_pack_end(bx, bx2);
   evas_object_show(bx2);

   lbl = elm_label_add(edje);
   enlil_photo->lbl_nb_albums_photos = lbl;
   evas_object_show(lbl);
   evas_object_size_hint_weight_set(lbl, 1.0, 0.0);
   evas_object_size_hint_align_set(lbl, 0.0, 0.5);
   elm_box_pack_end(bx2, lbl);

   tg = elm_toggle_add(edje);
   enlil_photo->multiselection = tg;
   evas_object_size_hint_weight_set(tg, -1.0, -1.0);
   evas_object_size_hint_align_set(tg, 1.0, -1.0);
   elm_toggle_label_set(tg, "Multi selection");
   elm_toggle_state_set(tg, 1);
   elm_toggle_states_labels_set(tg, "No", "Yes");
   evas_object_smart_callback_add(tg, "changed", _tg_multiselect_changed_cb, enlil_photo);
   elm_box_pack_end(bx2, tg);
   evas_object_show(tg);

   bt = elm_button_add(edje);
   evas_object_size_hint_weight_set(bt, -1.0, -1.0);
   evas_object_size_hint_align_set(bt, 1.0, -1.0);
   elm_button_label_set(bt, D_("Unselect all"));
   evas_object_smart_callback_add(bt, "clicked", _bt_unselect_all_cb, enlil_photo);
   evas_object_show(bt);

   elm_box_pack_end(bx2, bt);
   sl = elm_slider_add(edje);
   elm_slider_label_set(sl, "Zoom");
   elm_slider_indicator_format_set(sl, "%3.0f");
   elm_slider_min_max_set(sl, 1, 100);
   elm_slider_value_set(sl, 50);
   elm_slider_unit_format_set(sl, "%4.0f");
   evas_object_size_hint_weight_set(sl, -1.0, -1.0);
   evas_object_size_hint_align_set(sl, 1.0, -1.0);
   evas_object_smart_callback_add(sl, "delay,changed", _slider_zoom_cb, enlil_photo);
   evas_object_show(sl);
   elm_box_pack_end(bx2, sl);
   enlil_photo->sl = sl;

   return enlil_photo;
}

void list_photo_data_set(List_Photo *list_photo, Enlil_Data *enlil_data)
{
   list_photo->enlil_data = enlil_data;
   enlil_data->list_photo = list_photo;
}

void list_photo_album_add(List_Photo *list_photo, Enlil_Album *album)
{
   Enlil_Album_Data *enlil_album_data = enlil_album_user_data_get(album);

   enlil_album_data->list_photo_item = photos_list_object_item_header_append(list_photo->o_list, &itc_album,
	 album);
}

void list_photo_album_append_relative(List_Photo *list_photo, Enlil_Album *album, PL_Header_Item *relative)
{
   Enlil_Album_Data *enlil_album_data = enlil_album_user_data_get(album);

   enlil_album_data->list_photo_item = photos_list_object_item_header_append_relative(
	 list_photo->o_list, &itc_album,
	 album, relative);
}

void list_photo_photo_add(List_Photo *list_photo, Enlil_Album *album, Enlil_Photo *photo)
{
   char buf[PATH_MAX];
   const char *s, *_s;
   Eina_List *l, *l_next;
   Enlil_Album_Data *enlil_album_data = enlil_album_user_data_get(album);
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);

   enlil_photo_data->list_photo_item = photos_list_object_item_append(list_photo->o_list, &itc_photo,
	 enlil_album_data->list_photo_item, photo);

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo), enlil_photo_file_name_get(photo));
   s = eina_stringshare_add(buf);

   EINA_LIST_FOREACH_SAFE(enlil_photo_data->enlil_data->auto_open, l, l_next, _s)
     {
	if(_s == s)
	  {
	     enlil_photo_data->enlil_data->auto_open = eina_list_remove(
		   enlil_photo_data->enlil_data->auto_open, _s);
	     _open(enlil_photo_data, list_photo->o_list, photo);
	     eina_stringshare_del(_s);
	  }
     }
   eina_stringshare_del(s);
}

void list_photo_photo_append_relative(List_Photo *list_photo, Enlil_Album *album, Enlil_Photo *photo, PL_Child_Item *relative)
{
   char buf[PATH_MAX];
   const char *s, *_s;
   Eina_List *l, *l_next;
   Enlil_Album_Data *enlil_album_data = enlil_album_user_data_get(album);
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);

   enlil_photo_data->list_photo_item = photos_list_object_item_append_relative(list_photo->o_list, &itc_photo,
	 enlil_album_data->list_photo_item, photo, relative);

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo), enlil_photo_file_name_get(photo));
   s = eina_stringshare_add(buf);

   EINA_LIST_FOREACH_SAFE(enlil_photo_data->enlil_data->auto_open, l, l_next, _s)
     {
	if(_s == s)
	  {
	     enlil_photo_data->enlil_data->auto_open = eina_list_remove(
		   enlil_photo_data->enlil_data->auto_open, _s);
	     _open(enlil_photo_data, list_photo->o_list, photo);
	     eina_stringshare_del(_s);
	  }
     }
   eina_stringshare_del(s);
}

static Evas_Object *_album_icon_get(const void *data, Evas_Object *obj)
{
   Enlil_Album *album = (Enlil_Album *) data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   Evas_Object *ly = elm_layout_add(obj);
   elm_layout_file_set(ly, THEME, "album_header");
   Evas_Object *o = elm_layout_edje_get(ly);
   edje_object_part_text_set(o, "object.text.album_name", enlil_album_name_get(album));
   evas_object_show(ly);

   Evas_Object *bx = elm_box_add(obj);
   elm_box_horizontal_set(bx, 1);
   evas_object_size_hint_weight_set(bx, 1.0, -1.0);
   evas_object_size_hint_align_set(bx, 0.0, 0.5);
   elm_layout_content_set(ly, "object.swallow", bx);

   Evas_Object *bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Slideshow"));
   evas_object_smart_callback_add(bt, "clicked", _slideshow_cb, album);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Options"));
   evas_object_smart_callback_add(bt, "clicked", _collection_cb, album);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Tags"));
   evas_object_smart_callback_add(bt, "clicked", _tag_cb, album);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Sorts"));
   evas_object_smart_callback_add(bt, "clicked", _sorts_cb, album);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Select all"));
   evas_object_size_hint_weight_set(bt, 0.0, -1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   evas_object_smart_callback_add(bt, "clicked", _bt_album_select_all_cb, album);
   evas_object_show(bt);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Unselect all"));
   evas_object_size_hint_weight_set(bt, 0.0, -1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   evas_object_smart_callback_add(bt, "clicked", _bt_album_unselect_all_cb, album);
   evas_object_show(bt);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(obj);
   elm_object_style_set(bt, "anchor");
   elm_button_label_set(bt, D_("Empty"));
   evas_object_size_hint_weight_set(bt, 1.0, -1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.5);
   elm_box_pack_end(bx, bt);


   Evas_Object *ic = edje_object_add(evas_object_evas_get(obj));
   album_data->netsync.icon = ic;
   evas_object_show(ic);
   edje_object_file_set(ic, THEME, "flickr/sync");
   evas_object_size_hint_weight_set(ic, 1.0, 1.0);
   evas_object_size_hint_align_set(ic, 1.0, 0.0);
   evas_object_event_callback_add(ic, EVAS_CALLBACK_MOUSE_UP, _album_sync_flickr_cb, album);
   edje_object_signal_emit(ic,
	 album_flickr_edje_signal_get(album), "");

   elm_layout_content_set(ly, "object.swallow.sync", ic);

   return ly;
}

static Evas_Object *_photo_icon_get(const void *data, Evas_Object *obj)
{
   const char *s = NULL;
   Enlil_Photo *photo = (Enlil_Photo *) data;
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);
   Enlil_Data *enlil_data = enlil_photo_data->enlil_data;
   List_Photo *enlil_photo = enlil_data->list_photo;

   Evas_Object *o = photo_object_add(obj);
   photo_object_theme_file_set(o, THEME, "photo");

   if(enlil_photo_data->cant_create_thumb == 1)
     return o;

   if(enlil_photo->photo_w <= 128 && enlil_photo->photo_h<=128)
     s = enlil_thumb_photo_get(photo, Enlil_THUMB_FDO_NORMAL, thumb_done_cb, thumb_error_cb, NULL);
   else
     s = enlil_thumb_photo_get(photo, Enlil_THUMB_FDO_LARGE, thumb_done_cb, thumb_error_cb, NULL);

   evas_image_cache_flush (evas_object_evas_get(obj));

   if(s)
     photo_object_file_set(o, s , NULL);
   else
     photo_object_progressbar_set(o, EINA_TRUE);

   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO)
     photo_object_camera_set(o, EINA_TRUE);

   Evas_Object *flickr =
      photo_object_flickr_state_set(o, photo_flickr_edje_signal_get(enlil_photo_data->netsync.state));
   evas_object_event_callback_add(flickr, EVAS_CALLBACK_MOUSE_UP, _photo_sync_flickr_cb, photo);

   photo_object_text_set(o, enlil_photo_name_get(photo));

   evas_object_show(o);
   return o;
}

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *enlil_photo = (List_Photo*) data;

   double val = elm_slider_value_get(enlil_photo->sl);

   enlil_photo->photo_w = DEFAULT_W*2*val/100;
   enlil_photo->photo_h = DEFAULT_H*2*val/100;
   photos_list_object_sub_items_size_set(enlil_photo->o_list, enlil_photo->photo_w, enlil_photo->photo_h);
}

static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   PL_Child_Item *item =  NULL;

   Eina_List *items = photos_list_object_selected_get(enlil_data->list_photo->o_list);

   if(items)
     item = eina_list_data_get(items);

   Enlil_Photo *photo = photos_list_object_item_data_get(item);

   if(!item || album != enlil_photo_album_get(photo))
     photo = NULL;

   slideshow_clear();
   slideshow_album_add(album, photo);

   slideshow_show();
}

static void _open(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   //List_Photo *enlil_photo = data;
   Enlil_Photo *photo = event_info;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if(photo_data->panel_image) return ;
   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO)
     {
	snprintf(buf, PATH_MAX, "%s \"%s/%s\"", media_player,
	      enlil_photo_path_get(photo), enlil_photo_file_name_get(photo));
	ecore_exe_run(buf, NULL);
     }
   else
	panel_image_new(obj, photo);
}

static void _right_click(void *data, Evas_Object *obj, void *event_info)
{
   int x, y;
   Enlil_Photo *photo = event_info;
   PL_Child_Item *child;
   Eina_List *l, *photos = NULL;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   EINA_LIST_FOREACH(photos_list_object_selected_get(enlil_data->list_photo->o_list), l, child)
	photos = eina_list_append(photos, photos_list_object_item_data_get(child));

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   Photo_Menu *photo_menu = photo_menu_new(photo_data->enlil_data->win->win, photo, photos);
   elm_menu_move(photo_menu->menu, x, y);

   Evas_Object *menu = edje_object_part_external_object_get(global_object, "object.photo.menu");
   printf("%p\n", menu);
   evas_object_show(menu);
}


static void _collection_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   int x,y;

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   Album_Menu *album_menu = album_menu_new(album_data->enlil_data->win->win, album);
   elm_menu_move(album_menu->menu, x, y);
}

static void _tag_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   int x,y;

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   Album_Menu *album_menu = album_tag_menu_new(album_data->enlil_data->win->win, album);
   elm_menu_move(album_menu->menu, x, y);
}

static void _sorts_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   int x,y;

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   Album_Menu *album_menu = album_sorts_menu_new(album_data->enlil_data->win->win, album);
   elm_menu_move(album_menu->menu, x, y);
}

static void _bt_unselect_all_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *list_photo = data;
   Eina_List *l, *l_next;
   PL_Child_Item *item;

   EINA_LIST_FOREACH_SAFE(photos_list_object_selected_get(list_photo->o_list), l, l_next, item)
     {
	photos_list_object_item_selected_set(item, EINA_FALSE);
     }
}

static void _bt_album_select_all_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Photo *photo;
   Enlil_Photo_Data *photo_data;
   Eina_List *l;
   Eina_Bool multiselect = photos_list_object_multiselect_get(enlil_data->list_photo->o_list);

   photos_list_object_multiselect_set(enlil_data->list_photo->o_list, EINA_TRUE);
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
     {
	photo_data = enlil_photo_user_data_get(photo);
	photos_list_object_item_selected_set(photo_data->list_photo_item, EINA_TRUE);
     }
   photos_list_object_multiselect_set(enlil_data->list_photo->o_list, multiselect);
}

static void _bt_album_unselect_all_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   Enlil_Photo *photo;
   Enlil_Photo_Data *photo_data;
   Eina_List *l;

   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
     {
	photo_data = enlil_photo_user_data_get(photo);
	photos_list_object_item_selected_set(photo_data->list_photo_item, EINA_FALSE);
     }
}

static void _tg_multiselect_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   List_Photo *list_photo = data;

   photos_list_object_multiselect_set(list_photo->o_list,
	 !elm_toggle_state_get(list_photo->multiselection));
   if(elm_toggle_state_get(list_photo->multiselection))
     _bt_unselect_all_cb(data, NULL, NULL);
}

static void _album_sync_flickr_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Enlil_Album *album = data;
   //Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   //Evas_Object *sync =
   flickr_sync_new(enlil_data->win->win, album);
}

static void _photo_sync_flickr_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Enlil_Photo *photo = data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if(photo_data->netsync.state == PHOTO_FLICKR_FLICKRNOTUPTODATE)
     {
	upload_add(enlil_data->ul, photo);
     }
   else if(photo_data->netsync.state == PHOTO_FLICKR_NOTUPTODATE)
     {
	//get the list of sizes of the photo
	//Enlil_Flickr_Job *job =
	//   enlil_flickr_job_get_photo_sizes_prepend(enlil_photo_netsync_id_get(photo),
	//	 flickr_photos_notinlocal_sizes_get_cb, photo);
	//if(!eina_list_data_find(photo_data->netsync.jobs, job))
	  //photo_data->netsync.jobs = eina_list_append(photo_data->netsync.jobs, job);
     }
   else if(photo_data->netsync.state == PHOTO_FLICKR_NOTINFLICKR)
     {
	upload_add(enlil_data->ul, photo);
     }
}

