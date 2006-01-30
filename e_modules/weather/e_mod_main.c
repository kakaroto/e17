/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include "config.h"

/*
 * TODO:
 * * Don't reload when changing from F to C
 * * Don't reload when changing display
 * * Store weather info in face
 * * Show weather time on mouseover
 */

static Weather     *_weather_new();
static void         _weather_free(Weather * weather);
static void         _weather_config_menu_new(Weather * weather);
static void         _weather_menu_cb_faces(void *data, E_Menu * m);

static Weather_Face *_weather_face_new(Weather * weather, E_Container * con);
static void         _weather_face_free(Weather_Face * face);

static void         _weather_face_menu_new(Weather_Face * face);
static void         _weather_face_cb_gmc_change(void *data,
                                                E_Gadman_Client * gmc,
                                                E_Gadman_Change change);
static void         _weather_face_cb_mouse_down(void *data, Evas * e,
                                                Evas_Object * obj,
                                                void *event_info);
static void         _weather_face_cb_menu_edit(void *data, E_Menu * m,
                                               E_Menu_Item * mi);

static E_Menu      *_weather_face_menu_choose_location(Weather_Face * face);
static E_Menu      *_weather_face_menu_continent(Weather_Face * face,
                                                 Evas_List * menus,
                                                 FILE * file);
static E_Menu      *_weather_face_menu_country(Weather_Face * face,
                                               FILE * file);
static void         _weather_face_del_hook(void *obj);
static void         _weather_face_cb_choose_location(void *data, E_Menu * m,
                                                     E_Menu_Item * mi);

static void         _weather_connect(Weather * weather);
static int          _weather_cb_check(void *data);
static int          _weather_net_server_add(void *data, int type, void *event);
static int          _weather_net_server_data(void *data, int type, void *event);
static int          _weather_net_server_del(void *data, int type, void *event);

static int          _weather_parse(Weather_Face * face);
static void         _weather_convert_degrees(Weather_Face * face, int degrees);
static void         _weather_display_set(Weather_Face * face, int display,
                                         int ok);

static void         _weather_menu_add_face(void *data, E_Menu * m,
                                           E_Menu_Item * mi);
static void         _weather_menu_remove_face(void *data, E_Menu * m,
                                              E_Menu_Item * mi);

static void         _weather_menu_fast(void *data, E_Menu * m,
                                       E_Menu_Item * mi);
static void         _weather_menu_medium(void *data, E_Menu * m,
                                         E_Menu_Item * mi);
static void         _weather_menu_slow(void *data, E_Menu * m,
                                       E_Menu_Item * mi);

static void         _weather_menu_display_simple(void *data, E_Menu * m,
                                                 E_Menu_Item * mi);
static void         _weather_menu_display_detailed(void *data, E_Menu * m,
                                                   E_Menu_Item * mi);

static void         _weather_menu_degrees_C(void *data, E_Menu * m,
                                            E_Menu_Item * mi);
static void         _weather_menu_degrees_F(void *data, E_Menu * m,
                                            E_Menu_Item * mi);

static int          _weather_count;

static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

EAPI E_Module_Api   e_modapi = {
   E_MODULE_API_VERSION,
   "Weather"
};

EAPI void          *
e_modapi_init(E_Module * module)
{
   Weather            *weather;

   weather = _weather_new();
   module->config_menu = weather->config_menu;
   return weather;
}

EAPI int
e_modapi_shutdown(E_Module * module)
{
   Weather            *weather;

   if (module->config_menu)
      module->config_menu = NULL;

   weather = module->data;
   if (weather)
      _weather_free(weather);

   return 1;
}

EAPI int
e_modapi_save(E_Module * module)
{
   Weather            *weather;

   weather = module->data;
   e_config_domain_save("module.weather", conf_edd, weather->conf);
   return 1;
}

EAPI int
e_modapi_info(E_Module * module)
{
   module->icon_file = strdup(PACKAGE_DATA_DIR "/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module * module)
{
   e_module_dialog_show(_("Enlightenment Weather Module"),
                        _("A weather forecast module for Enlightenment."));
   return 1;
}

/* module private routines */
static Weather     *
_weather_new()
{
   Weather            *weather;
   Evas_List          *cl;
   E_Menu_Item        *mi;

   _weather_count = 0;

   /* Define poll time for now in seconds */

   weather = E_NEW(Weather, 1);
   if (!weather)
      return NULL;

   conf_face_edd = E_CONFIG_DD_NEW("Weather_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D conf_face_edd
   E_CONFIG_VAL(D, T, container, INT);
   E_CONFIG_VAL(D, T, enabled, UCHAR);
   E_CONFIG_VAL(D, T, location, STR);
   E_CONFIG_VAL(D, T, url, STR);

   conf_edd = E_CONFIG_DD_NEW("Weather_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, display, INT);
   E_CONFIG_VAL(D, T, degrees, INT);
   E_CONFIG_VAL(D, T, host, STR);

   weather->conf = e_config_domain_load("module.weather", conf_edd);
   if (!weather->conf)
     {
        weather->conf = E_NEW(Config, 1);
        weather->conf->poll_time = 1800.0;
        weather->conf->display = DETAILED_DISPLAY;
        weather->conf->degrees = DEGREES_C;
        weather->conf->host =
            (char *)evas_stringshare_add("www.rssweather.com");
     }
   E_CONFIG_LIMIT(weather->conf->poll_time, 900.0, 3600.0);

   _weather_config_menu_new(weather);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time,
                                                  _weather_cb_check, weather);

   cl = weather->conf->faces;
   if (!cl)
     {
        E_Manager          *man;
        E_Container        *con;
        Weather_Face       *face;

        man = e_manager_current_get();
        con = e_container_current_get(man);

        face = _weather_face_new(weather, con);
        if (face)
          {
             weather->faces = evas_list_append(weather->faces, face);

             face->conf = E_NEW(Config_Face, 1);
             face->conf->container = con->num;
             face->conf->enabled = 1;
             face->conf->location =
                 (char *)evas_stringshare_add("Kirkenes Lufthavn");
             face->conf->url =
                 (char *)evas_stringshare_add("/icao/ENKR/rss.php");
             weather->conf->faces =
                 evas_list_append(weather->conf->faces, face->conf);

             /* Menu */
             /* This menu must be initialized after conf */
             _weather_face_menu_new(face);

             /* Add main menu to face menu */
             mi = e_menu_item_new(face->menu);
             e_menu_item_label_set(mi, _("Set Degrees"));
             e_menu_item_submenu_set(mi, weather->config_menu_degrees);

             mi = e_menu_item_new(face->menu);
             e_menu_item_label_set(mi, _("Set Display"));
             e_menu_item_submenu_set(mi, weather->config_menu_display);

             mi = e_menu_item_new(face->menu);
             e_menu_item_label_set(mi, _("Set Update Time"));
             e_menu_item_submenu_set(mi, weather->config_menu_poll);
          }
     }
   else
     {
        for (; cl; cl = cl->next)
          {
             E_Container        *con;
             Weather_Face       *face;
             Config_Face        *conf;

             conf = cl->data;
             con = e_util_container_number_get(conf->container);
             if (!con)
                continue;
             face = _weather_face_new(weather, con);
             if (!face)
                continue;
             weather->faces = evas_list_append(weather->faces, face);
             face->conf = conf;

             /* Menu */
             /* This menu must be initialized after conf */
             _weather_face_menu_new(face);

             /* Add main menu to face menu */
             mi = e_menu_item_new(face->menu);
             e_menu_item_label_set(mi, _("Set Degrees"));
             e_menu_item_submenu_set(mi, weather->config_menu_degrees);

             mi = e_menu_item_new(face->menu);
             e_menu_item_label_set(mi, _("Set Display"));
             e_menu_item_submenu_set(mi, weather->config_menu_display);

             mi = e_menu_item_new(face->menu);
             e_menu_item_label_set(mi, _("Set Update Time"));
             e_menu_item_submenu_set(mi, weather->config_menu_poll);
          }
     }
   _weather_connect(weather);
   _weather_cb_check(weather);

   return weather;
}

static void
_weather_free(Weather * weather)
{
   Evas_List          *list;

   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_face_edd);

   ecore_timer_del(weather->weather_check_timer);

   for (list = weather->faces; list; list = list->next)
      _weather_face_free(list->data);
   evas_list_free(weather->conf->faces);
   evas_list_free(weather->faces);

   e_object_del(E_OBJECT(weather->config_menu));
   e_object_del(E_OBJECT(weather->config_menu_faces));
   e_object_del(E_OBJECT(weather->config_menu_poll));
   e_object_del(E_OBJECT(weather->config_menu_display));
   e_object_del(E_OBJECT(weather->config_menu_degrees));
   ecore_event_handler_del(weather->add_handler);
   ecore_event_handler_del(weather->del_handler);
   ecore_event_handler_del(weather->data_handler);

   if (weather->conf->host)
      evas_stringshare_del(weather->conf->host);
   free(weather->conf);
   free(weather);
}

static void
_weather_config_menu_new(Weather * weather)
{
   E_Menu             *mn;
   E_Menu_Item        *mi;

   /* Poll menu */
   mn = e_menu_new();

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("15 minutes"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->poll_time == 900.0)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_fast, weather);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("30 minutes"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->poll_time == 1800.0)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_medium, weather);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("1 hour"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->poll_time == 3600.0)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_slow, weather);

   weather->config_menu_poll = mn;

   /* Display menu */
   mn = e_menu_new();

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Simple"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->display == SIMPLE_DISPLAY)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_display_simple, weather);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Detailed"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->display == DETAILED_DISPLAY)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_display_detailed, weather);

   weather->config_menu_display = mn;

   /* Degrees menu */
   mn = e_menu_new();

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Fahrenheit"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->degrees == DEGREES_F)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_degrees_F, weather);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Celcius"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->degrees == DEGREES_C)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_degrees_C, weather);

   weather->config_menu_degrees = mn;

   /* Faces menu */
   mn = e_menu_new();
   e_menu_pre_activate_callback_set(mn, _weather_menu_cb_faces, weather);
   weather->update_menu_faces = 1;
   weather->config_menu_faces = mn;

   /* Main menu */
   mn = e_menu_new();

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Set Degrees"));
   e_menu_item_submenu_set(mi, weather->config_menu_degrees);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Set Display"));
   e_menu_item_submenu_set(mi, weather->config_menu_display);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Set Update Time"));
   e_menu_item_submenu_set(mi, weather->config_menu_poll);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Faces"));
   e_menu_item_submenu_set(mi, weather->config_menu_faces);

   weather->config_menu = mn;
}

static void
_weather_menu_cb_faces(void *data, E_Menu * m)
{
   Weather            *weather;
   E_Menu_Item        *mi;
   Evas_List          *l;

   weather = data;
   if (!weather->update_menu_faces)
      return;

   /* Delete old items */
   for (l = m->items; l;)
     {
        E_Object           *obj;

        obj = l->data;
        l = l->next;
        e_object_del(obj);
     }
   evas_list_free(m->items);
   m->items = NULL;

   /* New face */
   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, _("Add Face"));
   e_menu_item_callback_set(mi, _weather_menu_add_face, weather);

   /* Add faces to the config menu */
   for (l = weather->faces; l; l = l->next)
     {
        Weather_Face       *face;
        char                buf[1024];

        face = l->data;
        snprintf(buf, sizeof(buf), "%s (%d)", face->conf->location,
                 face->conf->container);
        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, buf);

        e_menu_item_submenu_set(mi, face->menu);
     }

   weather->update_menu_faces = 0;
}

static Weather_Face *
_weather_face_new(Weather * weather, E_Container * con)
{
   Weather_Face       *face;
   Evas_Object        *o;

   face = E_NEW(Weather_Face, 1);
   if (!face)
      return NULL;

   face->con = con;
   e_object_ref(E_OBJECT(con));
   face->weather = weather;

   face->temp = 0;
   face->degrees = 'C';
   strcat(face->conditions, "");
   strcat(face->icon, "na.png");

   evas_event_freeze(con->bg_evas);

   o = edje_object_add(con->bg_evas);
   face->weather_object = o;
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/weather", "modules/weather/main"))
      edje_object_file_set(o, PACKAGE_DATA_DIR "/weather.edj",
                           "modules/weather/main");
   evas_object_show(o);

   o = evas_object_rectangle_add(con->bg_evas);
   face->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                  _weather_face_cb_mouse_down, face);
   evas_object_show(o);

   face->gmc = e_gadman_client_new(con->gadman);
   e_gadman_client_domain_set(face->gmc, "module.weather", _weather_count++);
   e_gadman_client_policy_set(face->gmc, E_GADMAN_POLICY_ANYWHERE |
                              E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE |
                              E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE);
   //e_gadman_client_min_size_set(face->gmc, 160, 60);
   //e_gadman_client_max_size_set(face->gmc, 220, 96);
   e_gadman_client_auto_size_set(face->gmc, 200, 40);
   e_gadman_client_align_set(face->gmc, 1.0, 1.0);
   e_gadman_client_resize(face->gmc, 200, 40);
   e_gadman_client_change_func_set(face->gmc, _weather_face_cb_gmc_change,
                                   face);
   e_gadman_client_load(face->gmc);

   edje_object_part_text_set(face->weather_object, "location", "");
   face->icon_object = e_icon_add(con->bg_evas);

   evas_event_thaw(con->bg_evas);

   return face;
}

static void
_weather_face_free(Weather_Face * face)
{
   Evas_List          *l, *l2;

   e_object_unref(E_OBJECT(face->con));
   e_object_del(E_OBJECT(face->gmc));
   evas_object_del(face->weather_object);
   evas_object_del(face->icon_object);
   evas_object_del(face->event_object);
   e_object_del(E_OBJECT(face->menu));
   l = e_object_data_get(E_OBJECT(face->menu_location));
   for (l2 = l; l; l = l->next)
      e_object_del(E_OBJECT(l2->data));
   evas_list_free(l);
   e_object_del(E_OBJECT(face->menu_location));

   if (face->conf->location)
      evas_stringshare_del(face->conf->location);
   if (face->conf->url)
      evas_stringshare_del(face->conf->url);
   free(face->conf);
   free(face);
   _weather_count--;
}

static void
_weather_face_menu_new(Weather_Face * face)
{
   E_Menu             *mn, *subm;
   E_Menu_Item        *mi;

   mn = e_menu_new();
   face->menu = mn;

   /* Edit */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _weather_face_cb_menu_edit, face);

   /* New face */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Add Face"));
   e_menu_item_callback_set(mi, _weather_menu_add_face, face->weather);

   /* Remove face */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Remove Face"));
   e_menu_item_callback_set(mi, _weather_menu_remove_face, face);

   /* Choose location */
   subm = _weather_face_menu_choose_location(face);
   if (subm)
     {
        face->menu_location = subm;
        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, _("Choose Continent"));
        e_menu_item_submenu_set(mi, subm);
     }
}

static void
_weather_face_cb_gmc_change(void *data, E_Gadman_Client * gmc,
                            E_Gadman_Change change)
{
   Weather_Face       *face;
   Evas_Coord          x, y, w, h;

   face = data;
   switch (change)
     {
       case E_GADMAN_CHANGE_MOVE_RESIZE:
          e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);
          evas_object_move(face->weather_object, x, y);
          evas_object_move(face->event_object, x, y);
          evas_object_resize(face->weather_object, w, h);
          evas_object_resize(face->event_object, w, h);
          break;
       case E_GADMAN_CHANGE_RAISE:
          evas_object_raise(face->weather_object);
          evas_object_raise(face->event_object);
          break;
       case E_GADMAN_CHANGE_EDGE:
       case E_GADMAN_CHANGE_ZONE:
          /* FIXME
           * Must we do something here?
           */
          break;
     }
}

static void
_weather_face_cb_mouse_down(void *data, Evas * e, Evas_Object * obj,
                            void *event_info)
{
   Weather_Face       *face;
   Evas_Event_Mouse_Down *ev;

   face = data;
   ev = event_info;

   if (ev->button == 3)
     {
        e_menu_activate_mouse(face->menu, e_zone_current_get(face->con),
                              ev->output.x, ev->output.y, 1, 1,
                              E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        e_util_container_fake_mouse_up_all_later(face->con);
     }
}

static void
_weather_face_cb_menu_edit(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather_Face       *face;

   face = data;
   e_gadman_mode_set(face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static E_Menu      *
_weather_face_menu_choose_location(Weather_Face * face)
{
   E_Menu             *mn;
   Evas_List          *menus = NULL;
   FILE               *file;
   char                buf[2048];

   mn = e_menu_new();

   file = fopen(PACKAGE_DATA_DIR "/dir.xml", "rb");
   if (!file)
      return NULL;
   while ((fgets(buf, sizeof(buf), file)))
     {
        char               *needle, *name;

        needle = strstr(buf, "<");
        if (!needle)
           continue;
        if (!strncmp(needle, "<continent>", 11))
          {
             E_Menu             *subm;
             E_Menu_Item        *mi;

             fgets(buf, sizeof(buf), file);
             name = strstr(buf, ">");
             if (!name)
                continue;
             name++;
             needle = strstr(name, "<");
             if (!needle)
                continue;
             needle[0] = 0;

             mi = e_menu_item_new(mn);
             e_menu_item_label_set(mi, name);
             subm = _weather_face_menu_continent(face, menus, file);
             e_menu_item_submenu_set(mi, subm);
             menus = evas_list_append(menus, subm);
          }
     }
   fclose(file);

   e_object_data_set(E_OBJECT(mn), menus);
   return mn;
}

static E_Menu      *
_weather_face_menu_continent(Weather_Face * face, Evas_List * menus,
                             FILE * file)
{
   E_Menu             *mn;
   char                buf[2048];

   mn = e_menu_new();
   while ((fgets(buf, sizeof(buf), file)))
     {
        char               *needle, *name;

        needle = strstr(buf, "<");
        if (!needle)
           continue;
        if (!strncmp(needle, "<country>", 9))
          {
             E_Menu             *subm;
             E_Menu_Item        *mi;

             fgets(buf, sizeof(buf), file);
             name = strstr(buf, ">");
             if (!name)
                continue;
             name++;
             needle = strstr(name, "<");
             if (!needle)
                continue;
             needle[0] = 0;

             mi = e_menu_item_new(mn);
             e_menu_item_label_set(mi, name);
             subm = _weather_face_menu_country(face, file);
             e_menu_item_submenu_set(mi, subm);
             menus = evas_list_append(menus, subm);
          }
        else if (!strncmp(needle, "</continent>", 12))
           break;
     }
   return mn;
}

static E_Menu      *
_weather_face_menu_country(Weather_Face * face, FILE * file)
{
   E_Menu             *mn;
   char                buf[2048];

   mn = e_menu_new();
   while ((fgets(buf, sizeof(buf), file)))
     {
        char               *needle, *name, *url;

        needle = strstr(buf, "<");
        if (!needle)
           continue;
        if (!strncmp(needle, "<location>", 10))
          {
             E_Menu_Item        *mi;

             fgets(buf, sizeof(buf), file);
             name = strstr(buf, ">");
             if (!name)
                continue;
             name++;
             needle = strstr(name, "<");
             if (!needle)
                continue;
             needle[0] = 0;
             name = strdup(name);

             fgets(buf, sizeof(buf), file);
             url = strstr(buf, ">");
             if (!url)
                continue;
             url++;
             needle = strstr(url, "<");
             if (!needle)
                continue;
             needle[0] = 0;
             url = strdup(url);

             mi = e_menu_item_new(mn);
             e_menu_item_label_set(mi, name);
             e_menu_item_callback_set(mi, _weather_face_cb_choose_location,
                                      face);
             e_object_data_set(E_OBJECT(mi), url);
             e_object_del_attach_func_set(E_OBJECT(mi), _weather_face_del_hook);
             free(name);
          }
        else if (!strncmp(needle, "</country>", 10))
           break;
     }
   return mn;
}

static void
_weather_face_del_hook(void *obj)
{
   free(e_object_data_get(E_OBJECT(obj)));
}

static void
_weather_face_cb_choose_location(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather_Face       *face;
   char               *url;

   face = data;
   url = e_object_data_get(E_OBJECT(mi));

   if (face->conf->url)
      evas_stringshare_del(face->conf->url);
   face->conf->url = (char *)evas_stringshare_add(url);
   if (face->conf->location)
      evas_stringshare_del(face->conf->location);
   face->conf->location = (char *)evas_stringshare_add(mi->label);

   face->weather->update_menu_faces = 1;

   _weather_cb_check(face->weather);
   e_config_save_queue();
}

static void
_weather_connect(Weather * weather)
{
   char               *env;

   env = getenv("http_proxy");
   if (!env)
      env = getenv("HTTP_PROXY");
   if ((env) && !strncmp(env, "http://", 7))
     {
        /* Use a proxy */
        char               *host = NULL, *p;
        int                 port = 0;

        host = strchr(env, ':');
        host += 3;
        p = strchr(host, ':');
        if (p)
          {
             *p = 0;
             p++;
             if (sscanf(p, "%d", &port) != 1)
                port = 0;
          }
#if 0
        else
          {
             p = strchr(host, '/');
             if (p)
                *p = 0;
          }
#endif
        if ((host) && (port))
          {
             weather->proxy.host = strdup(host);
             weather->proxy.port = port;
          }
     }
   weather->add_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
                                                  _weather_net_server_add,
                                                  weather);
   weather->del_handler =
       ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
                               _weather_net_server_del, weather);
   weather->data_handler =
       ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                               _weather_net_server_data, weather);
}

static int
_weather_cb_check(void *data)
{
   Weather            *weather;
   Evas_List          *l;

   weather = data;
   for (l = weather->faces; l; l = l->next)
     {
        Weather_Face       *face;

        face = l->data;
        if (face->server)
           continue;

        face->bufsize = 4096;
        face->cursize = 0;
        face->buffer = malloc(face->bufsize);
        face->buffer[0] = 0;
        if (weather->proxy.port)
          {
             face->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
                                                     weather->proxy.host,
                                                     weather->proxy.port, face);
          }
        else
          {
             face->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
                                                     weather->conf->host, 80,
                                                     face);
          }
     }
   return 1;
}

static int
_weather_net_server_add(void *data, int type, void *event)
{
   Weather            *weather;
   Weather_Face       *face;
   Ecore_Con_Event_Server_Add *e;
   Evas_List          *l;
   char                buf[1024];

   weather = data;
   e = event;
   face = NULL;
   for (l = weather->faces; l; l = l->next)
     {
        face = l->data;
        if (face->server == e->server)
           break;
     }
   if ((!face) || (e->server != face->server))
      return 1;

   snprintf(buf, sizeof(buf), "GET http://%s%s HTTP/1.1\r\nHost: %s\r\n\r\n",
            weather->conf->host, face->conf->url, weather->conf->host);
   ecore_con_server_send(face->server, buf, strlen(buf));
   return 0;
}

static int
_weather_net_server_data(void *data, int type, void *event)
{
   Weather            *weather;
   Weather_Face       *face;
   Ecore_Con_Event_Server_Data *e;
   Evas_List          *l;

   weather = data;
   e = event;
   face = NULL;
   for (l = weather->faces; l; l = l->next)
     {
        face = l->data;
        if (face->server == e->server)
           break;
     }
   if ((!face) || (e->server != face->server))
      return 1;

   while ((face->cursize + e->size) >= face->bufsize)
     {
        face->bufsize += 4096;
        face->buffer = realloc(face->buffer, face->bufsize);
     }
   memcpy(face->buffer + face->cursize, e->data, e->size);
   face->cursize += e->size;
   face->buffer[face->cursize] = 0;
   return 0;
}

static int
_weather_net_server_del(void *data, int type, void *event)
{
   Weather            *weather;
   Weather_Face       *face;
   Ecore_Con_Event_Server_Del *e;
   Evas_List          *l;
   int                 ok;

   weather = data;
   e = event;
   face = NULL;
   for (l = weather->faces; l; l = l->next)
     {
        face = l->data;
        if (face->server == e->server)
           break;
     }
   if ((!face) || (e->server != face->server))
      return 1;

   ecore_con_server_del(face->server);
   face->server = NULL;

   ok = _weather_parse(face);
   _weather_convert_degrees(face, weather->conf->degrees);
   _weather_display_set(face, weather->conf->display, ok);

   face->bufsize = 0;
   face->cursize = 0;
   free(face->buffer);
   face->buffer = NULL;
   return 0;
}

static int
_weather_parse(Weather_Face * face)
{
   char               *needle, *ext;

   needle = strstr(face->buffer, "<content:encoded>");
   if (!needle)
      goto error;

   /* Get the icon */
   needle = strstr(needle, "<img");
   if (!needle)
      goto error;
   needle = strstr(needle, "id=");
   if (!needle)
      goto error;
   sscanf(needle, "id=\"%[^\"]\"", face->icon);
   ext = strstr(face->icon, ".");
   if (!strcmp(ext, ".gif"))
      strcpy(ext, ".png");

   /* Get the conditions */
   needle = strstr(needle, "class=\"sky\"");
   if (!needle)
      goto error;
   needle = strstr(needle, ">");
   if (!needle)
      goto error;
   sscanf(needle, ">%[^<]<", face->conditions);

   /* Get the temp */
   needle = strstr(needle, "class=\"temp\"");
   if (!needle)
      goto error;
   needle = strstr(needle, ">");
   if (!needle)
      goto error;
   sscanf(needle, ">%d", &face->temp);
   needle = strstr(needle, "<");
   if (!needle)
      goto error;
   needle--;
   face->degrees = needle[0];

   return 1;
 error:
   printf("ERROR: Couldn't parse info from rssweather.com\n");
   printf("%s\n", face->buffer);
   return 0;
}

static void
_weather_convert_degrees(Weather_Face * face, int degrees)
{
   /* Check if degrees is in C or F */
   if ((face->degrees == 'F') && (degrees == DEGREES_C))
     {
        face->temp = (face->temp - 32) * 5.0 / 9.0;
        face->degrees = 'C';
     }
   if ((face->degrees == 'C') && (degrees == DEGREES_F))
     {
        face->temp = (face->temp * 9.0 / 5.0) + 32;
        face->degrees = 'F';
     }
}

static void
_weather_display_set(Weather_Face * face, int display, int ok)
{
   char                buf[PATH_MAX];
   char               *utf8;

   if (!face)
      return;

   /* If _get_weather fails, blank out text and set icon to unknown */
   if (!ok)
     {
        e_icon_file_set(face->icon_object, PACKAGE_DATA_DIR "/images/na.png");
        edje_object_part_swallow(face->weather_object, "icon",
                                 face->icon_object);

        edje_object_part_text_set(face->weather_object, "location",
                                  face->conf->location);
        edje_object_part_text_set(face->weather_object, "temp", "");
        edje_object_part_text_set(face->weather_object, "conditions", "");
        if (display == DETAILED_DISPLAY)
           edje_object_signal_emit(face->weather_object, "set_style",
                                   "detailed");
        else
           edje_object_signal_emit(face->weather_object, "set_style", "simple");
     }
   else if (display == DETAILED_DISPLAY)
     {
        /* Detailed display */
        edje_object_signal_emit(face->weather_object, "set_style", "detailed");
        snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR "/images/%s", face->icon);
        e_icon_file_set(face->icon_object, buf);
        edje_object_part_swallow(face->weather_object, "icon",
                                 face->icon_object);

        edje_object_part_text_set(face->weather_object, "location",
                                  face->conf->location);
#if 0
        snprintf(buf, sizeof(buf), "%d\u00b0%c", face->temp, face->degrees);
        edje_object_part_text_set(face->weather_object, "temp", buf);
#else
        snprintf(buf, sizeof(buf), "%d°%c", face->temp, face->degrees);
        utf8 = ecore_txt_convert("iso-8859-1", "utf-8", buf);
        edje_object_part_text_set(face->weather_object, "temp", utf8);
        free(utf8);
#endif
        edje_object_part_text_set(face->weather_object, "conditions",
                                  face->conditions);
     }
   else
     {
        /* Simple display */
        edje_object_signal_emit(face->weather_object, "set_style", "simple");
        snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR "/images/%s", face->icon);
        e_icon_file_set(face->icon_object, buf);
        edje_object_part_swallow(face->weather_object, "icon",
                                 face->icon_object);

        edje_object_part_text_set(face->weather_object, "location",
                                  face->conf->location);
#if 0
        snprintf(buf, sizeof(buf), "%d\u00b0%c", face->temp, face->degrees);
        edje_object_part_text_set(face->weather_object, "temp", buf);
#else
        snprintf(buf, sizeof(buf), "%d°%c", face->temp, face->degrees);
        utf8 = ecore_txt_convert("iso-8859-1", "utf-8", buf);
        edje_object_part_text_set(face->weather_object, "temp", utf8);
        free(utf8);
#endif
        edje_object_part_text_set(face->weather_object, "conditions",
                                  face->conditions);
     }
}

static void
_weather_menu_add_face(void *data, E_Menu * m, E_Menu_Item * mi)
{
   E_Menu             *root;
   Weather            *weather;
   Weather_Face       *face;

   root = e_menu_root_get(m);
   if (!root->zone)
      return;

   weather = data;
   face = _weather_face_new(weather, root->zone->container);
   if (face)
     {
        weather->faces = evas_list_append(weather->faces, face);

        face->conf = E_NEW(Config_Face, 1);
        face->conf->container = root->zone->container->num;
        face->conf->enabled = 1;
        face->conf->location =
            (char *)evas_stringshare_add("Kirkenes Lufthavn");
        face->conf->url = (char *)evas_stringshare_add("/icao/ENKR/rss.php");
        weather->conf->faces =
            evas_list_append(weather->conf->faces, face->conf);

        /* Menu */
        /* This menu must be initialized after conf */
        _weather_face_menu_new(face);

        /* Add main menu to face menu */
        mi = e_menu_item_new(face->menu);
        e_menu_item_label_set(mi, _("Set Degrees"));
        e_menu_item_submenu_set(mi, weather->config_menu_degrees);

        mi = e_menu_item_new(face->menu);
        e_menu_item_label_set(mi, _("Set Display"));
        e_menu_item_submenu_set(mi, weather->config_menu_display);

        mi = e_menu_item_new(face->menu);
        e_menu_item_label_set(mi, _("Set Update Time"));
        e_menu_item_submenu_set(mi, weather->config_menu_poll);

        weather->update_menu_faces = 1;
     }
   _weather_cb_check(weather);
}

static void
_weather_menu_remove_face(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather_Face       *face;

   face = data;
   face->weather->faces = evas_list_remove(face->weather->faces, face);
   face->weather->conf->faces =
       evas_list_remove(face->weather->conf->faces, face->conf);

   face->weather->update_menu_faces = 1;

   _weather_face_free(face);
}

static void
_weather_menu_fast(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;

   weather = data;
   weather->conf->poll_time = 900.0;
   ecore_timer_del(weather->weather_check_timer);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time,
                                                  _weather_cb_check, weather);
   e_config_save_queue();
}

static void
_weather_menu_medium(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;

   weather = data;
   weather->conf->poll_time = 1800.0;
   ecore_timer_del(weather->weather_check_timer);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time,
                                                  _weather_cb_check, weather);
   e_config_save_queue();
}
static void
_weather_menu_slow(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;

   weather = data;
   weather->conf->poll_time = 3600.0;
   ecore_timer_del(weather->weather_check_timer);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time,
                                                  _weather_cb_check, weather);
   e_config_save_queue();
}

static void
_weather_menu_display_simple(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;

   weather = data;
   weather->conf->display = SIMPLE_DISPLAY;
   _weather_cb_check(data);
   e_config_save_queue();
}

static void
_weather_menu_display_detailed(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;

   weather = data;
   weather->conf->display = DETAILED_DISPLAY;
   _weather_cb_check(data);
   e_config_save_queue();
}

static void
_weather_menu_degrees_F(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;
   Evas_List          *l;

   weather = data;
   weather->conf->degrees = DEGREES_F;
   for (l = weather->faces; l; l = l->next)
     {
        Weather_Face       *face;

        face = l->data;
        _weather_convert_degrees(face, weather->conf->degrees);
        _weather_display_set(face, weather->conf->display, 1);
     }
   e_config_save_queue();
}

static void
_weather_menu_degrees_C(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather            *weather;
   Evas_List          *l;

   weather = data;
   weather->conf->degrees = DEGREES_C;
   for (l = weather->faces; l; l = l->next)
     {
        Weather_Face       *face;

        face = l->data;
        _weather_convert_degrees(face, weather->conf->degrees);
        _weather_display_set(face, weather->conf->display, 1);
     }
   e_config_save_queue();
}
