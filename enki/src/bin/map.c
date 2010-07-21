// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "main.h"
#include "evas_object/photo_object.h"

Elm_Map_Marker_Class *itc;
Elm_Map_Group_Class *itc_group;

Elm_Map_Marker_Class *itc_gp;
Elm_Map_Group_Class *itc_gp_group;

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);

static void _mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Evas_Object *_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);
static Evas_Object *_gp_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);
static Evas_Object *_gp_marker_icon_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);
static Evas_Object *_gp_group_icon_get(Evas_Object *obj, void *data);

static void _open(void *data, Evas_Object *obj, void *event_info);
static void _photo_select_cb(void *data, Evas_Object *obj, void *event_info);
static void _menu(void *data, Evas_Object *obj, void *event_info);
static void _tg_geocaching_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void _tg_photos_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void _bt_geocaching_cb(void *data, Evas_Object *obj, void *event_info);

Map *map_new(Evas_Object *win)
{
   Evas_Object  *sl, *rect, *tg, *bx;
   Map *map = calloc(1, sizeof(Map));

   //
   map->bx = elm_box_add(win);
   evas_object_size_hint_weight_set(map->bx, 1.0, 1.0);
   evas_object_size_hint_align_set(map->bx, -1.0, -1.0);
   evas_object_show(map->bx);
   //

   //
   map->map = elm_map_add(win);
   evas_object_size_hint_weight_set(map->map, 1.0, 1.0);
   evas_object_size_hint_align_set(map->map, -1.0, -1.0);
   evas_object_show(map->map);
   elm_box_pack_end(map->bx, map->map);
   //

   itc_group = elm_map_group_class_new(map->map);

   itc = elm_map_marker_class_new(map->map);
   elm_map_marker_class_get_cb_set(itc, _marker_get);

   
   itc_gp_group = elm_map_group_class_new(map->map);
   elm_map_group_class_icon_cb_set(itc_gp_group, _gp_group_icon_get);
   elm_map_group_class_style_set(itc_gp_group, "empty");
   elm_map_group_class_zoom_grouped_set(itc_gp_group, 10);

   itc_gp = elm_map_marker_class_new(map->map);
   elm_map_marker_class_get_cb_set(itc_gp, _gp_marker_get);
   elm_map_marker_class_icon_cb_set(itc_gp, _gp_marker_icon_get);
   elm_map_marker_class_style_set(itc_gp, "empty");

   //
   bx = elm_box_add(win);
   elm_box_horizontal_set(bx, EINA_TRUE);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   elm_box_pack_end(map->bx, bx);
   evas_object_show(bx);
   //

   //
   tg = elm_toggle_add(win);
   map->display_photos = tg;
   evas_object_size_hint_weight_set(tg, -1.0, -1.0);
   evas_object_size_hint_align_set(tg, 0.0, -1.0);
   elm_toggle_label_set(tg, "Display Photos");
   elm_toggle_state_set(tg, 1);
   elm_toggle_states_labels_set(tg, "Yes", "No");
   evas_object_smart_callback_add(tg, "changed", _tg_photos_changed_cb, map);
   elm_box_pack_end(bx, tg);
   evas_object_show(tg);
   //

   //
   tg = elm_toggle_add(win);
   map->display_geocaching = tg;
   evas_object_size_hint_weight_set(tg, -1.0, -1.0);
   evas_object_size_hint_align_set(tg, 0.0, -1.0);
   elm_toggle_label_set(tg, "Display Geocaching");
   elm_toggle_state_set(tg, 0);
   elm_toggle_states_labels_set(tg, "Yes", "No");
   evas_object_smart_callback_add(tg, "changed", _tg_geocaching_changed_cb, map);
   elm_box_pack_end(bx, tg);
   evas_object_show(tg);
   //

   //
   sl = elm_slider_add(win);
   evas_object_size_hint_weight_set(sl, 1.0, 0.0);
   evas_object_size_hint_align_set(sl, 1.0, -1.0);
   elm_slider_label_set(sl, "Zoom");
   elm_slider_indicator_format_set(sl, "%3.0f");
   elm_slider_min_max_set(sl, 0, 18);
   elm_slider_value_set(sl, 0);
   elm_slider_unit_format_set(sl, "%4.0f");
   evas_object_smart_callback_add(sl, "delay,changed", _slider_zoom_cb, map);
   evas_object_show(sl);
   elm_box_pack_end(bx, sl);
   map->sl = sl;
   //

   //
   rect = evas_object_rectangle_add(evas_object_evas_get(win));
   map->rect = rect;
   evas_object_color_set(rect, 0, 0, 0, 0);
   evas_object_repeat_events_set(rect,1);
   evas_object_show(rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel_cb, map);
   evas_object_raise(rect);
   evas_object_smart_member_add(map->rect, map->map);

   evas_object_event_callback_add(map->map ,EVAS_CALLBACK_RESIZE, _move_resize_cb, map);
   evas_object_event_callback_add(map->map, EVAS_CALLBACK_MOVE, _move_resize_cb, map);
   //

   return map;
}


void map_free(Map *map)
{
   evas_object_del(map->bx);
   evas_object_del(map->rect);
   map->selected = NULL;
   free(map);
}


void map_photo_add(Map *map, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if(enlil_photo_longitude_get(photo) != 360 && enlil_photo_latitude_get(photo)!= 360)
     {
	photo_data->marker = elm_map_marker_add(map->map,
	      enlil_photo_longitude_get(photo), enlil_photo_latitude_get(photo),
	      itc, itc_group, photo);
     }
}

void map_photo_update(Map *map, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if(photo_data->marker &&
	 (enlil_photo_longitude_get(photo) == 360 || enlil_photo_latitude_get(photo)== 360))
     {
	elm_map_marker_remove(photo_data->marker);
	photo_data->marker = NULL;
	if(photo == map->selected)
	  map->selected = NULL;
     }
   else if(photo_data->marker)
     {
	if(photo == map->selected)
	  map->selected = NULL;
	elm_map_marker_update(photo_data->marker);
     }
   else if (enlil_photo_longitude_get(photo) != 360 && enlil_photo_latitude_get(photo) != 360)
     map_photo_add(map, photo);
}

void map_photo_remove(Map *map, Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if(map->selected == photo)
     map->selected = NULL;
   elm_map_marker_remove(photo_data->marker);
   photo_data->marker = NULL;
}

void map_geocaching_add(Map *map, Enlil_Geocaching *gp)
{
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   if(!elm_toggle_state_get(map->display_geocaching))
     return ;

   if(enlil_geocaching_longitude_get(gp) != 360 && enlil_geocaching_latitude_get(gp)!= 360)
     {
	gp_data->marker = elm_map_marker_add(map->map,
	      enlil_geocaching_longitude_get(gp), enlil_geocaching_latitude_get(gp),
	      itc_gp, itc_gp_group, gp);
     }
}

void map_geocaching_update(Map *map, Enlil_Geocaching *gp)
{
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   if(gp_data->marker &&
	 (enlil_geocaching_longitude_get(gp) == 360 || enlil_geocaching_latitude_get(gp)== 360))
     {
	elm_map_marker_remove(gp_data->marker);
	gp_data->marker = NULL;
     }
   else if(gp_data->marker)
     {
	elm_map_marker_update(gp_data->marker);
     }
   else if (enlil_geocaching_longitude_get(gp) != 360 && enlil_geocaching_latitude_get(gp) != 360)
     map_geocaching_add(map, gp);
}

void map_geocaching_remove(Map *map, Enlil_Geocaching *gp)
{
   Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   elm_map_marker_remove(gp_data->marker);
   gp_data->marker = NULL;
}

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
   Map *map = data;

   double val = elm_slider_value_get(map->sl);
   elm_map_zoom_set(map->map, val);
}

   static void
_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Map *map = data;
   Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;
   int zoom;
   //unset the mouse wheel
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   zoom = elm_map_zoom_get(map->map);

   if (ev->z > 0)
     zoom--;
   else
     zoom++;

   elm_map_zoom_mode_set(map->map, ELM_MAP_ZOOM_MODE_MANUAL);
   if (zoom >= 0 && zoom <= 18)
     {
	elm_map_zoom_set(map->map, zoom);
	elm_slider_value_set(map->sl, zoom);
     }
}

   static void
_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int x,y,w,h;
   Map *map = data;

   evas_object_geometry_get(map->map,&x,&y,&w,&h);
   evas_object_resize(map->rect,w,h);
   evas_object_move(map->rect,x,y);
}

static Evas_Object *_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   const char *s = NULL;
   Enlil_Photo *photo = (Enlil_Photo *) data;
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);

   Evas_Object *o = photo_object_add(obj);
   photo_object_theme_file_set(o, THEME, "photo");
   evas_object_size_hint_min_set(o, 128, 96);

   evas_object_smart_callback_add(o, "open", _open, photo);
   evas_object_smart_callback_add(o, "select", _photo_select_cb, photo);
   evas_object_smart_callback_add(o, "clicked,menu", _menu, photo);
   evas_object_smart_callback_add(o, "clicked,right", _menu, photo);

   if(enlil_photo_data->cant_create_thumb == 1)
     return o;

   s = enlil_thumb_photo_get(photo, Enlil_THUMB_FDO_NORMAL, thumb_done_cb, thumb_error_cb, NULL);

   evas_image_cache_flush (evas_object_evas_get(obj));

   if(s)
     photo_object_file_set(o, s , NULL);
   else
     photo_object_progressbar_set(o, EINA_TRUE);

   if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO)
     photo_object_camera_set(o, EINA_TRUE);

   photo_object_text_set(o, enlil_photo_name_get(photo));

   evas_object_show(o);
   return o;
}

static Evas_Object *_gp_marker_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   char buf[PATH_MAX];
   Evas_Object *tb, *o, *fr, *bx, *bt;
   Enlil_Geocaching *gp = data;
   //Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr, enlil_geocaching_url_name_get(gp));
   evas_object_show(fr);
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);

   tb = elm_table_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, -1.0);
   evas_object_show(tb);
   elm_frame_content_set(fr, tb);
   elm_table_padding_set(tb, 5, 5);

   snprintf(buf, PATH_MAX, D_("<b>Created by :</b> %s"), enlil_geocaching_gp_placed_by_get(gp));
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   evas_object_size_hint_align_set(o, -1.0, 0.5);
   elm_table_pack(tb, o, 0, 0, 1, 1);

   bt = elm_button_add(tb);
   elm_button_label_set(bt, enlil_geocaching_name_get(gp));
   o = elm_icon_add(tb);
   snprintf(buf, PATH_MAX, "icons/geocaching/%s", enlil_geocaching_gp_type_get(gp));
   elm_icon_file_set(o, THEME, buf);
   evas_object_smart_callback_add(bt, "clicked", _bt_geocaching_cb, gp);
   elm_button_icon_set(bt, o);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 1.0);
   elm_table_pack(tb, bt, 1, 0, 1, 1);


   bx = elm_box_add(tb);
   evas_object_show(bx);
   elm_box_horizontal_set(bx, EINA_TRUE);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, 1.0);
   elm_table_pack(tb, bx, 0, 1, 1, 1);

   o = elm_label_add(tb);
   elm_label_label_set(o, D_("<b>Difficulty :</b>"));
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 0.0, 1.0);
   evas_object_size_hint_align_set(o, 0.0, -1.0);
   elm_box_pack_end(bx, o);

   o = elm_layout_add(tb);
   elm_layout_file_set(o, THEME, "stars");
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_box_pack_end(bx, o);
   edje_object_signal_emit(elm_layout_edje_get(o), enlil_geocaching_gp_difficulty_get(gp), "");



   bx = elm_box_add(tb);
   evas_object_show(bx);
   elm_box_horizontal_set(bx, EINA_TRUE);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, 1.0);
   elm_table_pack(tb, bx, 1, 1, 1, 1);

   o = elm_label_add(tb);
   elm_label_label_set(o, D_("<b>Terrain :</b>"));
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 0.0, 1.0);
   evas_object_size_hint_align_set(o, 0.0, 1.0);
   elm_box_pack_end(bx, o);

   o = elm_layout_add(tb);
   elm_layout_file_set(o, THEME, "stars");
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 1.0);
   elm_box_pack_end(bx, o);
   edje_object_signal_emit(elm_layout_edje_get(o), enlil_geocaching_gp_terrain_get(gp), "");


   snprintf(buf, PATH_MAX, D_("<b>Date Hidden :</b> %s"), enlil_geocaching_time_get(gp));
   buf[21+10] = '\0';
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   evas_object_size_hint_align_set(o, -1.0, -1.0);
   elm_table_pack(tb, o, 0, 2, 1, 1);

   snprintf(buf, PATH_MAX, D_("<b>cache size :</b> %s"), enlil_geocaching_gp_container_get(gp));
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   evas_object_size_hint_align_set(o, -1.0, -1.0);
   elm_table_pack(tb, o, 1, 2, 1, 1);

   
   if(!strcmp(enlil_geocaching_gp_available_get(gp), "False"))
     {
	o = elm_label_add(tb);
	elm_label_label_set(o, D_("<b><color=#f00>Not available.<color></b>"));
	evas_object_show(o);
	evas_object_size_hint_weight_set(o, 1.0, 1.0);
	evas_object_size_hint_align_set(o, -1.0, -1.0);
	elm_table_pack(tb, o, 0, 3, 1, 1);
     }

   return fr;
}

static Evas_Object *_gp_marker_icon_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   char buf[PATH_MAX];
   Evas_Object *icon;
   Enlil_Geocaching *gp = data;
   Eina_Bool found = EINA_FALSE;
   //Geocaching_Data *gp_data = enlil_geocaching_user_data_get(gp);

   const char *s = eina_stringshare_add("Geocache Found");
   if( enlil_geocaching_sym_get(gp) == s)
     found = EINA_TRUE;
   EINA_STRINGSHARE_DEL(s);

   icon = elm_icon_add(obj);
   if(found)
     snprintf(buf, PATH_MAX, "icons/geocaching/found");
   else
     snprintf(buf, PATH_MAX, "icons/geocaching/%s", enlil_geocaching_gp_type_get(gp));

   elm_icon_file_set(icon, THEME, buf);

   evas_object_show(icon);
   return icon;
}

static Evas_Object *_gp_group_icon_get(Evas_Object *obj, void *data)
{
   Evas_Object *icon;

   icon = elm_icon_add(obj);
   elm_icon_file_set(icon, THEME, "icons/geocaching/geocaching");

   evas_object_show(icon);
   return icon;
}

static void _photo_select_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Photo *photo = (Enlil_Photo *) data;

   if(enlil_data->map->selected  && enlil_data->map->selected != photo)
     {
	Enlil_Photo_Data *photo_data_selected = enlil_photo_user_data_get(enlil_data->map->selected);
	Evas_Object *o = elm_map_marker_object_get(photo_data_selected->marker);
	if(o)
	  evas_object_smart_callback_call(o, "unselect", NULL);
     }
   enlil_data->map->selected = photo;
}


static void _open(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_Photo *photo = (Enlil_Photo *) data;
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

static void _menu(void *data, Evas_Object *obj, void *event_info)
{
   int x, y;
   Enlil_Photo *photo = data;
   Eina_List *photos = NULL;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   photos = eina_list_append(photos, photo);

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   Photo_Menu *photo_menu = photo_menu_new(photo_data->enlil_data->win->win, photo, photos);
   elm_menu_move(photo_menu->menu, x, y);
}

static void _tg_geocaching_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Map *map = data;

   if(elm_toggle_state_get(map->display_geocaching))
     {
	enlil_geocaching_get(geocaching_done_cb, NULL);
     }
   else
     {
	enlil_geocaching_get(geocaching_remove_marker_cb, NULL);
     }
}


static void _tg_photos_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Map *map = data;

   if(elm_toggle_state_get(map->display_photos))
     {
	elm_map_group_class_hide_set(map->map, itc_group, EINA_FALSE);
     }
   else
     {
	elm_map_group_class_hide_set(map->map, itc_group, EINA_TRUE);
     }
}

static void _bt_geocaching_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Geocaching *geo = data;
   Geocaching_Data *geo_data = enlil_geocaching_user_data_get(geo);

   if(geo_data->panel_geocaching) return ;

   panel_geocaching_new(obj, data);
}

