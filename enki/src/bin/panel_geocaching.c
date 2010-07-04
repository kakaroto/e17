// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "main.h"

static void _bt_close_cb(void *data, Evas_Object *obj, void *event_info);

static void _panel_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item);

static void _map_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _map_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Evas_Object *_map_icon_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data);

Panel_Geocaching *panel_geocaching_new(Evas_Object *obj, Enlil_Geocaching *geocaching)
{
   char buf[PATH_MAX];
   Evas_Object *vbox, *bx, *bt, *main_bx, *lbl, *ly, *fr, *ic, *tb, *o, *entry, *sc, *map, *rect; 
   Enlil_Geocaching_Log *log;
   Eina_List *l;
   char *s = NULL, *s2;

   Panel_Geocaching *panel_geocaching = calloc(1, sizeof(Panel_Geocaching));
   Geocaching_Data *geo_data = enlil_geocaching_user_data_get(geocaching);
   geo_data->panel_geocaching = panel_geocaching;
   panel_geocaching->geocaching = geocaching;

   bx = elm_box_add(obj);
   main_bx = bx;
   elm_box_horizontal_set(bx, 1);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, -1.0);
   evas_object_show(bx);

   // left panel
   vbox = elm_box_add(obj);
   evas_object_size_hint_weight_set(vbox, 0.0, 1.0);
   evas_object_size_hint_align_set(vbox, -1.0, 0.0);
   evas_object_show(vbox);
   elm_box_pack_end(bx, vbox);

   bt = elm_button_add(obj);
   elm_button_label_set(bt, D_("Close the cache"));
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _bt_close_cb, panel_geocaching);
   evas_object_show(bt);
   elm_box_pack_end(vbox, bt);
   //

   
   fr = elm_frame_add(obj);
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);
   evas_object_show(fr);
   elm_box_pack_end(bx, fr);

   ly = elm_layout_add(obj);
   elm_layout_file_set(ly, THEME, "layout/panel/geocaching");
   evas_object_size_hint_weight_set(ly, 1.0, 1.0);
   evas_object_size_hint_align_set(ly, -1.0, -1.0);
   evas_object_show(ly);
   elm_frame_content_set(fr, ly);

   ic = elm_icon_add(obj);
   snprintf(buf, PATH_MAX, "icons/geocaching/%s", enlil_geocaching_gp_type_get(geocaching));
   elm_icon_file_set(ic, THEME, buf);
   evas_object_size_hint_weight_set(ic, 1.0, 1.0);
   evas_object_size_hint_align_set(ic, -1.0, 0.0);
   evas_object_show(ic);
   elm_layout_content_set(ly, "swallow.icon", ic);

   snprintf(buf, PATH_MAX, "<b><font_size=14>%s</font_size></b>", enlil_geocaching_url_name_get(geocaching));
   lbl = elm_label_add(obj);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 0.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   evas_object_show(lbl);
   elm_layout_content_set(ly, "swallow.title", lbl);

   snprintf(buf, PATH_MAX, "<b>A cache by %s</b>", enlil_geocaching_gp_owner_get(geocaching));
   lbl = elm_label_add(obj);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 0.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   evas_object_show(lbl);
   elm_layout_content_set(ly, "swallow.owner", lbl);

   ic = elm_icon_add(obj);
   elm_icon_file_set(ic, THEME, "icons/geocaching/geocaching");
   evas_object_size_hint_weight_set(ic, 1.0, 1.0);
   evas_object_size_hint_align_set(ic, -1.0, 0.0);
   evas_object_show(ic);
   elm_layout_content_set(ly, "swallow.icon2", ic);


   //
   tb = elm_table_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, -1.0);
   evas_object_show(tb);
   elm_table_padding_set(tb, 30, 5);
   elm_layout_content_set(ly, "swallow.header", tb);

   bx = elm_box_add(tb);
   evas_object_show(bx);
   elm_box_horizontal_set(bx, EINA_TRUE);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, 1.0);
   elm_table_pack(tb, bx, 0, 0, 1, 1);

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
   edje_object_signal_emit(elm_layout_edje_get(o), enlil_geocaching_gp_difficulty_get(geocaching), "");


   bx = elm_box_add(tb);
   evas_object_show(bx);
   elm_box_horizontal_set(bx, EINA_TRUE);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, 0.0);
   elm_table_pack(tb, bx, 1, 0, 1, 1);

   o = elm_label_add(tb);
   elm_label_label_set(o, D_("<b>Terrain :</b>"));
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 0.0, 1.0);
   evas_object_size_hint_align_set(o, 0.0, 0.0);
   elm_box_pack_end(bx, o);

   o = elm_layout_add(tb);
   elm_layout_file_set(o, THEME, "stars");
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_box_pack_end(bx, o);
   edje_object_signal_emit(elm_layout_edje_get(o), enlil_geocaching_gp_terrain_get(geocaching), "");


   snprintf(buf, PATH_MAX, D_("<b>Date Hidden :</b> %s"), enlil_geocaching_time_get(geocaching));
   buf[21+10] = '\0';
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_table_pack(tb, o, 0, 1, 1, 1);

   snprintf(buf, PATH_MAX, D_("<b>cache size :</b> %s"), enlil_geocaching_gp_container_get(geocaching));
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_table_pack(tb, o, 1, 1, 1, 1);

   snprintf(buf, PATH_MAX, D_("<b>Coordinates :</b> %f - %f"), enlil_geocaching_longitude_get(geocaching), enlil_geocaching_latitude_get(geocaching));
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_table_pack(tb, o, 0, 2, 1, 1);

   snprintf(buf, PATH_MAX, D_("<b>Hints :</b> %s"), enlil_geocaching_gp_hints_get(geocaching));
   o = elm_label_add(tb);
   elm_label_label_set(o, buf);
   evas_object_show(o);
   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_table_pack(tb, o, 0, 4, 2, 1);
   //


   //
   sc = elm_scroller_add(obj);
   elm_scroller_bounce_set(sc, 0, 0);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   evas_object_show(sc);
   elm_layout_content_set(ly, "swallow.description", sc);

  
   //replace <br /> and \n
   if(enlil_geocaching_gp_long_desc_get(geocaching))
     {
	s = calloc(strlen(enlil_geocaching_gp_long_desc_get(geocaching))+1, sizeof(char));
	strcpy(s, enlil_geocaching_gp_long_desc_get(geocaching));
	s2 = s;
	while( (s2 = strstr(s2, "<br />")) )
	  {
	     s2[0] = '<';
	     s2[1] = 'b';
	     s2[2] = 'r';
	     s2[3] = '>';
	     s2[4] = ' ';
	     s2[5] = ' ';
	  }

	s2 = s;
	while( (s2 = strstr(s2, "\n")) )
	  {
	     s2[0] = ' ';
	  }
     }
   else
     s = strdup("");

   entry = elm_entry_add(obj);
   elm_entry_entry_set(entry, s);
   elm_entry_editable_set(entry, EINA_FALSE);
   elm_entry_single_line_set(entry, 0);
   evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, 0.0);
   elm_scroller_content_set(sc,entry);
   evas_object_show(entry);

   FREE(s);
   //

   
   map = elm_map_add(obj);
   panel_geocaching->map = map;
   evas_object_size_hint_weight_set(map, 1.0, 1.0);
   evas_object_size_hint_align_set(map, -1.0, -1.0);
   evas_object_show(map);
   elm_layout_content_set(ly, "swallow.map", map);

   panel_geocaching->itc_group = elm_map_group_class_new(map);
   panel_geocaching->itc = elm_map_marker_class_new(map);
   elm_map_marker_class_icon_cb_set(panel_geocaching->itc, _map_icon_get);
   elm_map_marker_class_style_set(panel_geocaching->itc, "empty");

   panel_geocaching->marker = elm_map_marker_add(map,
	 enlil_geocaching_longitude_get(geocaching), enlil_geocaching_latitude_get(geocaching),
	 panel_geocaching->itc, panel_geocaching->itc_group, panel_geocaching);

   l = eina_list_append(NULL, panel_geocaching->marker);
   elm_map_markers_list_show(l);
   eina_list_free(l);

   //
   rect = evas_object_rectangle_add(evas_object_evas_get(obj));
   panel_geocaching->rect = rect;
   evas_object_color_set(rect, 0, 0, 0, 0);
   evas_object_repeat_events_set(rect,1);
   evas_object_show(rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_WHEEL, _map_mouse_wheel_cb, panel_geocaching);
   evas_object_raise(rect);
   evas_object_smart_member_add(panel_geocaching->rect, map);

   evas_object_event_callback_add(panel_geocaching->map ,EVAS_CALLBACK_RESIZE, _map_move_resize_cb, panel_geocaching);
   evas_object_event_callback_add(panel_geocaching->map, EVAS_CALLBACK_MOVE, _map_move_resize_cb, panel_geocaching);
   //
  
   // comments
   sc = elm_scroller_add(obj);
   elm_scroller_bounce_set(sc, 0, 1);
   evas_object_size_hint_weight_set(sc, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_layout_content_set(ly, "swallow.comments", sc);

   bx = elm_box_add(obj);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_scroller_content_set(sc, bx);

   EINA_LIST_FOREACH(enlil_geocaching_logs_get(geocaching), l, log)
     {
	Evas_Object *bb;

	bb = elm_bubble_add(obj);
	elm_bubble_label_set(bb, enlil_geocaching_log_finder_get(log));

	snprintf(buf, PATH_MAX, "%s", enlil_geocaching_log_date_get(log));
	buf[10] = '\0';
	elm_bubble_info_set(bb, buf);

	if(enlil_geocaching_log_text_get(log))
	  {
	     s = calloc(strlen(enlil_geocaching_log_text_get(log))+1, sizeof(char));
	     strcpy(s, enlil_geocaching_log_text_get(log));
	     s2 = s;
	     while( (s2 = strstr(s2, "<br />")) )
	       {
		  s2[0] = '<';
		  s2[1] = 'b';
		  s2[2] = 'r';
		  s2[3] = '>';
		  s2[4] = ' ';
		  s2[5] = ' ';
	       }

	     s2 = s;
	     while( (s2 = strstr(s2, "\n")) )
	       {
		  s2[0] = ' ';
	       }
	  }
	else
	  s = strdup("");

	o = elm_icon_add(obj);
	snprintf(buf, PATH_MAX, "icons/geocaching/%s", enlil_geocaching_log_type_get(log));
	elm_icon_file_set(o, THEME, buf);
	elm_bubble_icon_set(bb, o);
	evas_object_show(o);
	evas_object_size_hint_weight_set(bb, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(bb, EVAS_HINT_FILL, EVAS_HINT_FILL);
	entry = elm_entry_add(obj);
	elm_entry_editable_set(entry, EINA_FALSE);
	elm_label_label_set(entry, s);
	elm_bubble_content_set(bb,entry);
	evas_object_show(entry);
	elm_box_pack_end(bx, bb);
	evas_object_show(bb);

	FREE(s);
     }
   //


   panel_geocaching->tabpanel_item = tabpanel_item_add(enlil_data->tabpanel,
	 enlil_geocaching_name_get(geocaching), main_bx, _panel_select_cb, geocaching);

   return panel_geocaching;
}

void panel_geocaching_free(Panel_Geocaching **_panel_geocaching)
{
   Panel_Geocaching *panel_geocaching = *_panel_geocaching;
   Geocaching_Data *geo_data = enlil_geocaching_user_data_get( panel_geocaching->geocaching );

   elm_map_marker_remove(panel_geocaching->marker);
   evas_object_del(panel_geocaching->rect);

   tabpanel_item_del(panel_geocaching->tabpanel_item);
   panel_geocaching->tabpanel_item = NULL;

   geo_data->panel_geocaching = NULL;

   FREE(panel_geocaching);
}

static void _panel_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item)
{
   elm_map_bubbles_close(enlil_data->map->map);
}

static void _bt_close_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Geocaching *panel_geocaching = data;

   panel_geocaching_free(&panel_geocaching);
}

   static void
_map_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Panel_Geocaching *panel_geocaching = data;
   Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;
   int zoom;
   //unset the mouse wheel
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   zoom = elm_map_zoom_get(panel_geocaching->map);

   if (ev->z > 0)
     zoom--;
   else
     zoom++;

   elm_map_zoom_mode_set(panel_geocaching->map, ELM_MAP_ZOOM_MODE_MANUAL);
   if (zoom >= 0 && zoom <= 18)
     {
	elm_map_zoom_set(panel_geocaching->map, zoom);
     }
}

   static void
_map_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int x,y,w,h;
   Panel_Geocaching *panel_geocaching = data;

   evas_object_geometry_get(panel_geocaching->map,&x,&y,&w,&h);
   evas_object_resize(panel_geocaching->rect,w,h);
   evas_object_move(panel_geocaching->rect,x,y);
}

static Evas_Object *_map_icon_get(Evas_Object *obj, Elm_Map_Marker *marker, void *data)
{
   char buf[PATH_MAX];
   Evas_Object *icon;
   Panel_Geocaching *panel_geocaching = data;
   Enlil_Geocaching *gp = panel_geocaching->geocaching;
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

