#include <Elementary.h>
#include "tsuite.h"
#define SOURCE_MAX 10
#define MARKER_MAX 1000
#define NAME_ENTRY_TEXT "Enter freeform address"

typedef struct Map_Source
{
   Evas_Object *map;
   char *source_name;
} Map_Source;

static Elm_Map_Marker_Class *itc1, *itc2, *itc_parking;
static Elm_Map_Group_Class *itc_group1, *itc_group2, *itc_group_parking;

static Evas_Object *rect, *menu, *fs_win;
static int nb_elts;
/*static Elm_Map_Marker *markers[MARKER_MAX];*/
static Elm_Map_Marker *route_from, *route_to;
static Elm_Map_Route *route;
static Elm_Map_Name *name;
static Evas_Object *track;
static const char **source_names = NULL;
static Evas_Coord old_x, old_y;
static Evas_Coord old_d;
static Map_Source ms[SOURCE_MAX];

struct _Test_Data
{
   char *data1;
   char *data2;
   char *data3;
   char *data4;
   char *data5;
   char *data6;
   char *data7;
   char *data8;
   char *data9;
   char *data10;
   char *data11;
};
typedef struct _Test_Data Test_Data;

static char *data3;  /* used in _marker_get */

static Evas_Object * _marker_get(Evas_Object *obj, Elm_Map_Marker *marker __UNUSED__, void *data);
static Evas_Object * _group_icon_get(Evas_Object *obj, void *data);

static void
#ifdef ELM_EMAP
my_map_gpx_fileselector_done(void *data, Evas_Object *obj __UNUSED__, void *event_info)
#else
my_map_gpx_fileselector_done(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
#endif
{
   const char *selected = event_info;

   if (selected)
     {
        printf("Selected file: %s\n", selected);
#ifdef ELM_EMAP
        EMap_Route *emap = emap_route_gpx_new(selected);
        track = elm_map_track_add(data, emap);
#else
        printf("libEMap is required !\n");
#endif
     }
   evas_object_del(fs_win);
}

static char *
_get_str(char *dest, const char *str)
{  /* dest should be not null and big enough */
   sprintf(dest, "%s/%s", elm_app_data_dir_get(), str);
   return dest;
}

static void
my_map_clicked(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("clicked\n");
}

static void
my_map_press(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("press\n");
}

static void
my_map_longpressed(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   double lon, lat;
   Evas_Coord ox, oy, x, y, w, h;
   int zoom;
   Evas_Event_Mouse_Down *down = (Evas_Event_Mouse_Down *)event_info;
   if (!down) return;

   evas_object_geometry_get(data, &ox, &oy, &w, &h);
   zoom = elm_map_zoom_get(data);
   elm_map_geo_region_get(obj, &lon, &lat);
   elm_map_utils_convert_geo_into_coord(obj, lon, lat, pow(2.0, zoom) * 256, &x, &y);
   x += down->canvas.x - (w / 2) - ox;
   y += down->canvas.y - (h / 2) - oy;
   elm_map_utils_convert_coord_into_geo(obj, x, y, pow(2.0, zoom) * 256, &lon, &lat);

   name = elm_map_utils_convert_coord_into_name(data, lon, lat);
}

static void
my_map_clicked_double(void *data, Evas_Object *obj, void *event_info)
{
   double lon, lat;
   double flon, flat, tlon, tlat;
   Evas_Coord ox, oy, x, y, w, h, rx, ry, tx, ty;
   double d;
   int zoom;
   char buf[1024];
   Evas_Coord size;
   Evas_Event_Mouse_Down *down = (Evas_Event_Mouse_Down *)event_info;
   if (!down) return;

   evas_object_geometry_get(data, &ox, &oy, &w, &h);
   zoom = elm_map_zoom_get(data);
   if (zoom < 5) return;
   size = pow(2.0, zoom) * 256;
   elm_map_geo_region_get(obj, &lon, &lat);
   elm_map_utils_convert_geo_into_coord(obj, lon, lat, size, &x, &y);

   rx = x;
   ry = y;
   x += down->canvas.x - ((float)w * 0.5) - ox;
   y += down->canvas.y - ((float)h * 0.5) - oy;
   elm_map_rotate_get(data, &d, NULL, NULL);
   elm_map_utils_rotate_coord(data, x, y, rx, ry, -d, &tx, &ty);
   elm_map_utils_convert_coord_into_geo(obj, tx, ty, size, &lon, &lat);

   itc1 = elm_map_marker_class_new(data);

   elm_map_marker_class_del_cb_set(itc1, NULL);

   itc_group1 = elm_map_group_class_new(data);
   elm_map_group_class_icon_cb_set(itc_group1, _group_icon_get);
   elm_map_group_class_data_set(itc_group1, _get_str(buf,"images/bubble.png"));
   elm_map_group_class_style_set(itc_group1, "empty");
   elm_map_group_class_zoom_displayed_set(itc_group1, 5);

   if (route_from && route_to)
     {
        elm_map_marker_remove(route_from);
        route_from = NULL;
        elm_map_marker_remove(route_to);
        route_to = NULL;
        elm_map_route_remove(route);
     }

   if (!route_from) route_from = elm_map_marker_add(data, lon, lat, itc1, itc_group1, NULL);
   else route_to = elm_map_marker_add(data, lon, lat, itc1, itc_group1, NULL);

   if (route_from && route_to)
     {
        elm_map_marker_region_get(route_from, &flon, &flat);
        elm_map_marker_region_get(route_to, &tlon, &tlat);
        route = elm_map_route_add(data, ELM_MAP_ROUTE_TYPE_MOTOCAR, ELM_MAP_ROUTE_METHOD_FASTEST, flon, flat, tlon, tlat);
        elm_map_route_color_set(route, 255, 0, 0, 255);
     }
}

static void
my_map_load_detail(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("load,detail\n");
}

static void
my_map_loaded_detail(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("loaded,detail\n");
}

static void
my_map_zoom_start(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("zoom,start\n");
}

static void
my_map_zoom_stop(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("zoom,stop\n");
}

static void
my_map_zoom_change(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("zoom,change\n");
}

static void
my_map_anim_start(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("anim,start\n");
}

static void
my_map_anim_stop(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("anim,stop\n");
}

static void
my_map_drag_start(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_smart_callback_del(data, "longpressed", my_map_longpressed);
}

static void
my_map_drag_stop(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_smart_callback_add(data, "longpressed", my_map_longpressed, data);
}

static void
my_map_scroll(void *data __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   double lon, lat;
   elm_map_geo_region_get(obj, &lon, &lat);
   printf("scroll longitude : %f latitude : %f\n", lon, lat);
}

static void
my_map_downloaded(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int try_num, finish_num;
   elm_map_utils_downloading_status_get(data, &try_num, &finish_num);
   printf("downloaded : %d / %d\n", finish_num, try_num);
}

static void
my_map_route_load(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("route_load\n");
}

static void
my_map_route_loaded(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   double d;
   const char *w, *n;
   d = elm_map_route_distance_get(route);
   printf("route distance = %lf km\n", d);

   w = elm_map_route_waypoint_get(route);
   if (w) printf("[waypoints]\n%s\n", w);

   n = elm_map_route_node_get(route);
   if (n) printf("[nodes]\n%s\n", n);
}

static void
my_map_name_load(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   printf("name_load\n");
}

static void
my_map_name_loaded(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   if (!name) return;
   double lon, lat;
   const char *addr = elm_map_name_address_get(name);
   elm_map_name_region_get(name, &lon, &lat);
   if (addr)
     {
        printf("name of [lon = %lf, lat = %lf] is %s\n", lon, lat, addr);
        if ((lon != 0.0) && (lat !=0.0))
          {
             Eina_Bool b = elm_map_paused_get(data);
             elm_map_paused_set(data, EINA_TRUE);
             elm_map_zoom_mode_set(data, ELM_MAP_ZOOM_MODE_MANUAL);
             elm_map_geo_region_show(data, lon, lat);
             elm_map_zoom_set(data, 18);
             elm_map_paused_set(data, b);
          }
     }
   elm_map_name_remove(name);
}

static void
map_show_urmatt(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Eina_Bool b = elm_map_paused_get(data);
   elm_map_paused_set(data, EINA_TRUE);
   elm_map_zoom_mode_set(data, ELM_MAP_ZOOM_MODE_MANUAL);
   elm_map_geo_region_show(data,7.325201, 48.526813);
   elm_map_zoom_set(data, 12);
   elm_map_paused_set(data, b);
}

static void
map_bring_seoul(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_geo_region_bring_in(data, 126.977969, 37.566535);
}

static void
map_paused_set(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_paused_set(data, EINA_TRUE);
}

static void
map_paused_unset(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_paused_set(data, EINA_FALSE);
}

static void
map_zoom_in(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int zoom;

   zoom = elm_map_zoom_get(data) + 1;
   elm_map_zoom_mode_set(data, ELM_MAP_ZOOM_MODE_MANUAL);
   elm_map_zoom_set(data, zoom);
}

static void
map_zoom_out(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int zoom;

   zoom = elm_map_zoom_get(data) - 1;
   elm_map_zoom_mode_set(data, ELM_MAP_ZOOM_MODE_MANUAL);
   elm_map_zoom_set(data, zoom);
}

static void
map_zoom_fit(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_zoom_mode_set(data, ELM_MAP_ZOOM_MODE_AUTO_FIT);
}

static void
map_zoom_fill(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_zoom_mode_set(data, ELM_MAP_ZOOM_MODE_AUTO_FILL);
}

static void
map_track_add(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *fs, *bg, *vbox, *hbox, *sep;

   fs_win = elm_win_add(NULL, "fileselector", ELM_WIN_BASIC);
   elm_win_title_set(fs_win, "File Selector");
   elm_win_autodel_set(fs_win, 1);

   bg = elm_bg_add(fs_win);
   elm_win_resize_object_add(fs_win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   vbox = elm_box_add(fs_win);
   elm_win_resize_object_add(fs_win, vbox);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);

   fs = elm_fileselector_add(fs_win);
   elm_fileselector_is_save_set(fs, EINA_TRUE);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", my_map_gpx_fileselector_done, data);

   sep = elm_separator_add(fs_win);
   elm_separator_horizontal_set(sep, EINA_TRUE);
   elm_box_pack_end(vbox, sep);
   evas_object_show(sep);

   hbox = elm_box_add(fs_win);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   elm_box_pack_end(vbox, hbox);
   evas_object_show(hbox);

   evas_object_resize(fs_win, 240, 350);
   evas_object_show(fs_win);
}


static void
map_track_remove(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_track_remove(data, track);
}

static void
map_rotate_cw(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   double d;
   Evas_Coord x, y, w, h;
   float half_w, half_h;
   evas_object_geometry_get(data, &x, &y, &w, &h);
   half_w = (float)w * 0.5;
   half_h = (float)h * 0.5;

   elm_map_rotate_get(data, &d, NULL, NULL);
   d += 15.0;
   elm_map_rotate_set(data, d, x + half_w, y + half_h);
}

static void
map_rotate_ccw(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   double d;
   Evas_Coord x, y, w, h;
   float half_w, half_h;
   evas_object_geometry_get(data, &x, &y, &w, &h);
   half_w = (float)w * 0.5;
   half_h = (float)h * 0.5;

   elm_map_rotate_get(data, &d, NULL, NULL);
   d -= 15.0;
   elm_map_rotate_set(data, d, x + half_w, y + half_h);
}

static void
map_rotate_reset(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Coord x, y, w, h;
   float half_w, half_h;
   evas_object_geometry_get(data, &x, &y, &w, &h);
   half_w = (float)w * 0.5;
   half_h = (float)h * 0.5;

   elm_map_rotate_set(data, 0.0, x + half_w, y + half_h);
}

static void
map_zoom_min_set(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_source_zoom_min_set(data, 1);
}

static void
map_zoom_max_set(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_map_source_zoom_max_set(data, 10);
}

static void
map_source(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Map_Source *s = data;

   if (!s) return;
   elm_map_source_name_set(s->map, s->source_name);
}

static Evas_Object *
_marker_get(Evas_Object *obj, Elm_Map_Marker *marker __UNUSED__, void *data)
{
   char *d = data;

   Evas_Object *bx = elm_box_add(obj);
   evas_object_show(bx);

   if (d == data3)
     {
        Evas_Object *icon = elm_icon_add(obj);
        elm_icon_file_set(icon, d, NULL);
        evas_object_show(icon);

        Evas_Object *o = elm_button_add(obj);
        elm_object_part_content_set(o, "icon", icon);
        evas_object_show(o);
        elm_box_pack_end(bx, o);
     }
   else
     {
        Evas_Object *o = evas_object_image_add(evas_object_evas_get(obj));
        evas_object_image_file_set(o, d, NULL);
        evas_object_image_filled_set(o, EINA_TRUE);
        evas_object_size_hint_min_set(o, 64, 64);
        evas_object_show(o);
        elm_box_pack_end(bx, o);

        Evas_Object *lbl = elm_label_add(obj);
        elm_object_text_set(lbl, "Wolves Go !");
        evas_object_show(lbl);
        elm_box_pack_end(bx, lbl);
     }

   return bx;
}

static Evas_Object *
_icon_get(Evas_Object *obj, Elm_Map_Marker *marker __UNUSED__, void *data)
{
   char *d = data;

   Evas_Object *icon = elm_icon_add(obj);
   elm_icon_file_set(icon, d, NULL);
   evas_object_show(icon);

   return icon;
}

static Evas_Object *
_group_icon_get(Evas_Object *obj, void *data)
{
   char *file = data;

   Evas_Object *icon = elm_icon_add(obj);
   elm_icon_file_set(icon, file, NULL);
   evas_object_show(icon);

   return icon;
}

static void
_map_move_resize_cb(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   int x,y,w,h;

   evas_object_geometry_get(data,&x,&y,&w,&h);
   evas_object_resize(rect,w,h);
   evas_object_move(rect,x,y);
}

static void
_populate(void *data, Elm_Object_Item *menu_it)
{
   int idx;

   if ((!data) || (!menu_it) || (!source_names)) return;
   for (idx = 0; source_names[idx]; idx++)
     {
        if (idx >= SOURCE_MAX) break;
        ms[idx].map = data;
        ms[idx].source_name = strdup(source_names[idx]);
        elm_menu_item_add(menu, menu_it, "", source_names[idx], map_source,
                          &ms[idx]);
     }
}

static void
_map_mouse_down(void *data, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *down = event_info;
   Elm_Object_Item *menu_it;
   if (!down) return;

   if (down->button == 2)
     {
        old_x = down->canvas.x;
        old_y = down->canvas.y;
        old_d = 0.0;
     }
   else if (down->button == 3)
     {
        menu = elm_menu_add(obj);
        menu_it = elm_menu_item_add(menu, NULL, NULL, "Source", NULL, NULL);
        _populate(data, menu_it);
        elm_menu_item_add(menu, NULL, NULL, "Show Urmatt", map_show_urmatt, data);
        elm_menu_item_add(menu, NULL, NULL, "Bring Seoul", map_bring_seoul, data);
        elm_menu_item_add(menu, NULL, NULL, "Paused Set", map_paused_set, data);
        elm_menu_item_add(menu, NULL, NULL, "Paused Unset", map_paused_unset, data);
        elm_menu_item_add(menu, NULL, NULL, "Zoom +", map_zoom_in, data);
        elm_menu_item_add(menu, NULL, NULL, "Zoom -", map_zoom_out, data);
        elm_menu_item_add(menu, NULL, NULL, "Zoom Fit", map_zoom_fit, data);
        elm_menu_item_add(menu, NULL, NULL, "Zoom Fill", map_zoom_fill, data);
        elm_menu_item_add(menu, NULL, NULL, "Add Track", map_track_add, data);
        elm_menu_item_add(menu, NULL, NULL, "Remove Track", map_track_remove, data);
        elm_menu_item_add(menu, NULL, NULL, "Add Marker", NULL, NULL);
        elm_menu_item_add(menu, NULL, NULL, "Rotate CW", map_rotate_cw, data);
        elm_menu_item_add(menu, NULL, NULL, "Rotate CCW", map_rotate_ccw, data);
        elm_menu_item_add(menu, NULL, NULL, "Reset Rotate", map_rotate_reset, data);
        elm_menu_item_add(menu, NULL, NULL, "Set Zoom Min to 1", map_zoom_min_set, data);
        elm_menu_item_add(menu, NULL, NULL, "Set Zoom Max to 10", map_zoom_max_set, data);

        elm_menu_move(menu, down->canvas.x, down->canvas.y);
        evas_object_show(menu);
     }
}

static void
_map_mouse_move(void *data, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Mouse_Move *move = event_info;
   Evas_Coord x, y, w, h;
   float half_w, half_h;
   int d, d_diff;
   double cur_d;
   if (!move) return;

   if (move->buttons == 2)
     {
        evas_object_geometry_get(data, &x, &y, &w, &h);
        half_w = (float)w * 0.5;
        half_h = (float)h * 0.5;
        elm_map_rotate_get(data, &cur_d, NULL, NULL);

        d = move->cur.canvas.x - old_x;
        if (!old_d) old_d = d;
        else
          {
             d_diff = old_d - d;
             if (d_diff > 0)
               {
                  old_d --;
                  cur_d += 1.0;
               }
             else if (d_diff < 0)
               {
                  old_d ++;
                  cur_d -= 1.0;
               }
             old_d = d;
             elm_map_rotate_set(data, cur_d, x + half_w, y + half_h);
          }
     }
}

static void
_map_mouse_up(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Mouse_Up *up = event_info;
   if (!up) return;

   if (up->button == 2)
     {
        old_x = 0;
        old_y = 0;
     }
}

static void
_win_del(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Test_Data *tdata = data;
   if (!data) return;

   free(tdata->data1);
   free(tdata->data2);
   free(tdata->data3);
   free(tdata->data4);
   free(tdata->data5);
   free(tdata->data6);
   free(tdata->data7);
   free(tdata->data8);
   free(tdata->data9);
   free(tdata->data10);
   free(tdata->data11);

   free(tdata);
}

TEST_START(test_map)
{
   Evas_Object *bg, *map;
   int idx = 0;
   char buf[1024];

   Test_Data *tdata = malloc(sizeof(Test_Data));
   tdata->data1 = strdup(_get_str(buf, "images/logo.png"));
   tdata->data2 = strdup(_get_str(buf, "images/logo_small.png"));
   data3 = tdata->data3 = strdup(_get_str(buf, "images/panel_01.jpg"));
   tdata->data4 = strdup(_get_str(buf, "images/plant_01.jpg"));
   tdata->data5 = strdup(_get_str(buf, "images/rock_01.jpg"));
   tdata->data6 = strdup(_get_str(buf, "images/rock_02.jpg"));
   tdata->data7 = strdup(_get_str(buf, "images/sky_01.jpg"));
   tdata->data8 = strdup(_get_str(buf, "images/sky_02.jpg"));
   tdata->data9 = strdup(_get_str(buf, "images/sky_03.jpg"));
   tdata->data10 = strdup(_get_str(buf, "images/sky_03.jpg"));
   tdata->data11= strdup(_get_str(buf, "images/wood_01.jpg"));

   evas_object_smart_callback_add(win, "delete,request", _win_del, tdata);
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   map = elm_map_add(win);
   if (map)
     {
        srand(time(NULL));

        source_names = elm_map_source_names_get(map);

        if (!source_names) return;
        printf("map sources [ ");
        for (idx = 0; source_names[idx] ; idx++) printf("%s ", source_names[idx]);
        printf("]\n");

        evas_object_size_hint_weight_set(map, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, map);
        evas_object_data_set(map, "window", win);

        //
        itc1 = elm_map_marker_class_new(map);
        elm_map_marker_class_get_cb_set(itc1, _marker_get);
        elm_map_marker_class_del_cb_set(itc1, NULL);

        itc2 = elm_map_marker_class_new(map);
        elm_map_marker_class_get_cb_set(itc2, _marker_get);
        elm_map_marker_class_del_cb_set(itc2, NULL);
        elm_map_marker_class_style_set(itc2, "radio2");

        itc_parking = elm_map_marker_class_new(map);
        elm_map_marker_class_get_cb_set(itc_parking, _marker_get);
        elm_map_marker_class_del_cb_set(itc_parking, NULL);
        elm_map_marker_class_icon_cb_set(itc_parking, _icon_get);
        elm_map_marker_class_style_set(itc_parking, "empty");
        //

        //
        itc_group1 = elm_map_group_class_new(map);
        elm_map_group_class_data_set(itc_group1,
              _get_str(buf, "images/plant_01.jpg"));

        itc_group2 = elm_map_group_class_new(map);
        elm_map_group_class_style_set(itc_group2, "radio2");
        elm_map_group_class_zoom_displayed_set(itc_group1, 3);

        itc_group_parking = elm_map_group_class_new(map);
        elm_map_group_class_icon_cb_set(itc_group_parking, _group_icon_get);
        elm_map_group_class_data_set(itc_group_parking,
           _get_str(buf, "images/parking.png"));
        elm_map_group_class_style_set(itc_group_parking, "empty");
        elm_map_group_class_zoom_displayed_set(itc_group_parking, 5);
        //

        rect = evas_object_rectangle_add(evas_object_evas_get(win));
        evas_object_color_set(rect, 0, 0, 0, 0);
        evas_object_repeat_events_set(rect,1);
        evas_object_show(rect);
        evas_object_raise(rect);

        evas_object_event_callback_add(map, EVAS_CALLBACK_RESIZE,
                                       _map_move_resize_cb, map);
        evas_object_event_callback_add(map, EVAS_CALLBACK_MOVE,
                                       _map_move_resize_cb, map);
        evas_object_event_callback_add(map, EVAS_CALLBACK_MOUSE_DOWN,
                                       _map_mouse_down, map);
        evas_object_event_callback_add(map, EVAS_CALLBACK_MOUSE_MOVE,
                                       _map_mouse_move, map);
        evas_object_event_callback_add(map, EVAS_CALLBACK_MOUSE_UP,
                                       _map_mouse_up, map);

        elm_map_marker_add(map, 2.352, 48.857, itc1, itc_group1, tdata->data1);
        elm_map_marker_add(map, 2.355, 48.857, itc1, itc_group1, tdata->data3);
        elm_map_marker_add(map, 3, 48.857, itc2, itc_group1, tdata->data2);
        elm_map_marker_add(map, 2.352, 49, itc2, itc_group1, tdata->data1);

        elm_map_marker_add(map, 7.31451, 48.857127, itc1, itc_group1, tdata->data10);
        elm_map_marker_add(map, 7.314704, 48.857119, itc1, itc_group1, tdata->data4);
        elm_map_marker_add(map, 7.314704, 48.857119, itc2, itc_group1, tdata->data5);
        elm_map_marker_add(map, 7.31432, 48.856785, itc2, itc_group1, tdata->data6);
        elm_map_marker_add(map, 7.3148, 48.85725, itc1, itc_group2, tdata->data7);
        elm_map_marker_add(map, 7.316445, 48.8572210000694, itc1, itc_group1, tdata->data8);
        elm_map_marker_add(map, 7.316527000125, 48.85609, itc2, itc_group2, tdata->data9);
        elm_map_marker_add(map, 7.3165409990833, 48.856078, itc2, itc_group1, tdata->data11);
        elm_map_marker_add(map, 7.319812, 48.856561, itc2, itc_group2, tdata->data10);

        nb_elts = 13;

        evas_object_smart_callback_add(map, "clicked", my_map_clicked, win);
        evas_object_smart_callback_add(map, "press", my_map_press, win);
        evas_object_smart_callback_add(map, "longpressed", my_map_longpressed, map);
        evas_object_smart_callback_add(map, "clicked,double", my_map_clicked_double, map);
        evas_object_smart_callback_add(map, "load,detail", my_map_load_detail, win);
        evas_object_smart_callback_add(map, "loaded,detail", my_map_loaded_detail, win);
        evas_object_smart_callback_add(map, "zoom,start", my_map_zoom_start, win);
        evas_object_smart_callback_add(map, "zoom,stop", my_map_zoom_stop, win);
        evas_object_smart_callback_add(map, "zoom,change", my_map_zoom_change, win);
        evas_object_smart_callback_add(map, "scroll,anim,start", my_map_anim_start, win);
        evas_object_smart_callback_add(map, "scroll,anim,stop", my_map_anim_stop, win);
        evas_object_smart_callback_add(map, "scroll,drag,start", my_map_drag_start, map);
        evas_object_smart_callback_add(map, "scroll,drag,stop", my_map_drag_stop, map);
        evas_object_smart_callback_add(map, "scroll", my_map_scroll, win);
        evas_object_smart_callback_add(map, "downloaded", my_map_downloaded, map);
        evas_object_smart_callback_add(map, "route,load", my_map_route_load, map);
        evas_object_smart_callback_add(map, "route,loaded", my_map_route_loaded, map);
        evas_object_smart_callback_add(map, "name,load", my_map_name_load, map);
        evas_object_smart_callback_add(map, "name,loaded", my_map_name_loaded, map);

        evas_object_show(map);
     }

   evas_object_resize(win, 800, 800);
   evas_object_show(win);
}
TEST_END
