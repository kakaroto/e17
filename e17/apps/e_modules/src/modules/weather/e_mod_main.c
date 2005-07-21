#include "e.h"
#include "e_mod_main.h"


static Weather *_weather_new ();
static void _weather_shutdown (Weather * weather);
static void _weather_config_menu_new (Weather * weather);

static Weather_Face *_weather_face_new (E_Container * con);
static void _weather_face_free (Weather_Face * face);
static void _weather_face_enable (Weather_Face * face);
static void _weather_face_disable (Weather_Face * face);

static void _weather_face_menu_new (Weather_Face * face);
static void _weather_face_cb_gmc_change (void *data, E_Gadman_Client * gmc,
                                         E_Gadman_Change change);
static void _weather_face_cb_mouse_down (void *data, Evas * e,
                                         Evas_Object * obj, void *event_info);
static void _weather_face_cb_menu_enabled (void *data, E_Menu * m,
                                           E_Menu_Item * mi);
static void _weather_face_cb_menu_edit (void *data, E_Menu * m,
                                        E_Menu_Item * mi);
static void _weather_face_level_set(Weather_Face *ef, double level);

static int  _weather_cb_check (void *data);

static int _weather_cb_download_html (weatherCb_t * weather);
static int _weather_cb_parse_html(weatherCb_t *weather);
static int _weather_cb_convert_degrees(weatherCb_t *weather, void *data);
static int _weather_cb_set_display(weatherCb_t *weather, Weather_Face *face, 
                                   int display);

static void _weather_menu_fast(void *data, E_Menu *m, E_Menu_Item *mi);
static void _weather_menu_medium(void *data, E_Menu *m, E_Menu_Item *mi);
static void _weather_menu_slow(void *data, E_Menu *m, E_Menu_Item *mi);

static void _weather_menu_display_simple(void *data, E_Menu *m, E_Menu_Item *mi);
static void _weather_menu_display_detailed(void *data, E_Menu *m, E_Menu_Item *mi);

static void _weather_menu_degrees_C(void *data, E_Menu *m, E_Menu_Item *mi);
static void _weather_menu_degrees_F(void *data, E_Menu *m, E_Menu_Item *mi);

static void _weather_menu_edit_url(void *data, E_Menu *m, E_Menu_Item *mi);

void cb_resize(Ecore_Evas *ee);


static int _weather_count;

static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

void *
e_modapi_init (E_Module * module)
{
   Weather *weather;

   if (module->api->version < E_MODULE_API_VERSION)
   {
      e_error_dialog_show (_("Module API Error"),
         _("Error initializing Module: test\n"
            "It requires a minimum module API version of: %i.\n"
            "The module API advertized by Enlightenment is: %i.\n"
            "Aborting module."), E_MODULE_API_VERSION, module->api->version);
      return NULL;
   }

   weather = _weather_new ();
   module->config_menu = weather->config_menu;
   return weather;
}

int
e_modapi_shutdown (E_Module * module)
{
   Weather *weather;

   if (module->config_menu)
      module->config_menu = NULL;

   weather = module->data;
   if (weather)
      _weather_shutdown (weather);

   return 1;
}

int
e_modapi_save (E_Module * module)
{
   Weather *weather;

   weather = module->data;
   e_config_domain_save ("module.weather", conf_edd, weather->conf);
   return 1;
}

int
e_modapi_info (E_Module * module)
{
   module->label = strdup (_("Weather"));
   module->icon_file = strdup (PACKAGE_LIB_DIR"/e_modules/weather/module_icon.png");
   return 1;
}

int
e_modapi_about (E_Module * module)
{
   e_error_dialog_show (_("Enlightenment Weather Module"),
                        _("A weather forecast module for Enlightenment."));
   return 1;
}

/* module private routines */
static Weather *
_weather_new ()
{
   Weather *weather;
   Evas_List *managers, *l, *l2, *cl;
   E_Menu_Item *mi;

   _weather_count = 0;
   
   /* Define poll time for now in seconds */
   
   weather = E_NEW (Weather, 1);
   if (!weather)
      return NULL;

   conf_face_edd = E_CONFIG_DD_NEW ("Weather_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D conf_face_edd
   E_CONFIG_VAL (D, T, enabled, UCHAR);

   conf_edd = E_CONFIG_DD_NEW ("Weather_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST (D, T, faces, conf_face_edd);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, display, INT);
   E_CONFIG_VAL(D, T, degrees, INT);

   weather->conf = e_config_domain_load ("module.weather", conf_edd);
   if (!weather->conf)
   {
      weather->conf = E_NEW (Config, 1);
      weather->conf->poll_time = 900.0;
      weather->conf->display = 1;
      system(CREATE_DEFAULT_CONFIG_URL);
   }
   E_CONFIG_LIMIT(weather->conf->poll_time, 900.0, 3600.0);

   _weather_config_menu_new (weather);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time, _weather_cb_check, weather);
   
   managers = e_manager_list ();
   cl = weather->conf->faces;
   for (l = managers; l; l = l->next)
   {
      E_Manager *man;

      man = l->data;
      for (l2 = man->containers; l2; l2 = l2->next)
      {
         E_Container *con;
         Weather_Face *face;

         con = l2->data;
         face = _weather_face_new (con);
         if (face)
         {
            weather->faces = evas_list_append (weather->faces, face);
            /* Config */
            if (!cl)
            {
               face->conf = E_NEW (Config_Face, 1);
               face->conf->enabled = 1;
               weather->conf->faces =
                  evas_list_append (weather->conf->faces, face->conf);
            }
            else
            {
               face->conf = cl->data;
               cl = cl->next;
            }

            /* Menu */
            /* This menu must be initialized after conf */
            _weather_face_menu_new (face);

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
            
            mi = e_menu_item_new(face->menu);
            e_menu_item_label_set(mi, _("Edit URL file"));
            e_menu_item_callback_set(mi, _weather_menu_edit_url, weather);
            
            
            mi = e_menu_item_new (weather->config_menu);
            e_menu_item_label_set (mi, con->name);

            e_menu_item_submenu_set (mi, face->menu);

            /* Setup */
            if (!face->conf->enabled)
               _weather_face_disable (face);
         }
      }
   }
   _weather_cb_check(weather);
   
   return weather;
}

static void
_weather_shutdown (Weather * weather)
{
   Evas_List *list;

   E_CONFIG_DD_FREE (conf_edd);
   E_CONFIG_DD_FREE (conf_face_edd);
	
   //system("rm -f HTML_FILE");

   for (list = weather->faces; list; list = list->next)
      _weather_face_free (list->data);
   
   evas_list_free (weather->faces);
   e_object_del (E_OBJECT (weather->config_menu));
   e_object_del (E_OBJECT (weather->config_menu_poll));
   e_object_del (E_OBJECT (weather->config_menu_display));
   e_object_del (E_OBJECT (weather->config_menu_degrees));
   ecore_timer_del(weather->weather_check_timer);

   evas_list_free (weather->conf->faces);
   free (weather->conf);
   free (weather);
}

static void
_weather_config_menu_new (Weather * weather)
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   //weather->config_menu = e_menu_new();
   mn = e_menu_new();
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("15 minutes"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->poll_time == 900.0) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_fast, weather);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("30 minutes"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->poll_time == 1800.0) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_medium, weather);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("1 hour"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->poll_time == 3600.0) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_slow, weather);
   
   weather->config_menu_poll = mn;
   
   mn = e_menu_new();
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Simple"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->display == SIMPLE_DISPLAY) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_display_simple, weather);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Detailed"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->display == DETAILED_DISPLAY) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_display_detailed, weather);
   
   weather->config_menu_display = mn;
   
   mn = e_menu_new();
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Fahrenheit"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->degrees == DEGREES_F) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_degrees_F, weather);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Celcius"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 1);
   if (weather->conf->degrees == DEGREES_C) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _weather_menu_degrees_C, weather);
   
   weather->config_menu_degrees = mn;
   
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
   e_menu_item_label_set(mi, _("Edit URL file"));
   e_menu_item_callback_set(mi, _weather_menu_edit_url, weather);
   
   weather->config_menu = mn;
}

static Weather_Face *
_weather_face_new (E_Container * con)
{
   int w, h;

   Weather_Face *face;
   Evas_Object *o;

   face = E_NEW (Weather_Face, 1);
   if (!face)
      return NULL;

   face->con = con;
   e_object_ref (E_OBJECT (con));

   evas_event_freeze (con->bg_evas);

   o = edje_object_add (con->bg_evas);
   face->weather_object = o;
   edje_object_file_set (o, PACKAGE_LIB_DIR"/e_modules/weather/weather.edj", 
                         "weather/main");
   evas_object_show (o);

   o = evas_object_rectangle_add (con->bg_evas);
   face->event_object = o;
   evas_object_layer_set (o, 2);
   evas_object_color_set (o, 0, 0, 0, 0);
   evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
                                   _weather_face_cb_mouse_down, face);
   evas_object_show (o);

   face->gmc = e_gadman_client_new (con->gadman);
   e_gadman_client_domain_set (face->gmc, "module.weather", _weather_count++);
   e_gadman_client_policy_set (face->gmc, E_GADMAN_POLICY_ANYWHERE |
                               E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE | 
                               E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set (face->gmc, 160, 60);
   e_gadman_client_max_size_set (face->gmc, 220, 96);
   e_gadman_client_auto_size_set (face->gmc, 145, 60);
   e_gadman_client_align_set (face->gmc, 1.0, 1.0);
   //e_gadman_client_aspect_set(face->gmc, 1.0, 1.0);
   e_gadman_client_resize (face->gmc, 145, 60);
   e_gadman_client_change_func_set (face->gmc, _weather_face_cb_gmc_change,
                                    face);
   e_gadman_client_load (face->gmc);

   evas_event_thaw (con->bg_evas);

   return face;
}

static void
_weather_face_free (Weather_Face * face)
{
   e_object_unref (E_OBJECT (face->con));
   e_object_del (E_OBJECT (face->gmc));
   evas_object_del (face->weather_object);
   evas_object_del (face->event_object);
   e_object_del (E_OBJECT (face->menu));

   free (face->conf);
   free (face);
   _weather_count--;
}

static void
_weather_face_enable (Weather_Face * face)
{
   face->conf->enabled = 1;
   evas_object_show (face->weather_object);
   evas_object_show (face->event_object);
   e_config_save_queue ();
}

static void
_weather_face_disable (Weather_Face * face)
{
   face->conf->enabled = 0;
   evas_object_hide (face->weather_object);
   evas_object_hide (face->event_object);
   e_config_save_queue ();
}

static void
_weather_face_menu_new (Weather_Face * face)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new ();
   face->menu = mn;

   /* Enabled */
   /*
    * mi = e_menu_item_new(mn);
    * e_menu_item_label_set(mi, _("Enabled"));
    * e_menu_item_check_set(mi, 1);
    * if (face->conf->enabled) e_menu_item_toggle_set(mi, 1);
    * e_menu_item_callback_set(mi, _weather_face_cb_menu_enabled, face);
    */

   /* Edit */
   mi = e_menu_item_new (mn);
   e_menu_item_label_set (mi, _("Edit Mode"));
   e_menu_item_callback_set (mi, _weather_face_cb_menu_edit, face);
}

static void
_weather_face_cb_gmc_change (void *data, E_Gadman_Client * gmc,
   E_Gadman_Change change)
{
   Weather_Face *face;
   Evas_Coord x, y, w, h;

   face = data;
   switch (change)
   {
   case E_GADMAN_CHANGE_MOVE_RESIZE:
      e_gadman_client_geometry_get (face->gmc, &x, &y, &w, &h);
      evas_object_move (face->weather_object, x, y);
      evas_object_move (face->event_object, x, y);
      evas_object_resize (face->weather_object, w, h);
      evas_object_resize (face->event_object, w, h);
      break;
   case E_GADMAN_CHANGE_RAISE:
      evas_object_raise (face->weather_object);
      evas_object_raise (face->event_object);
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
_weather_face_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
   void *event_info)
{
   Weather_Face *face;
   Evas_Event_Mouse_Down *ev;

   face = data;
   ev = event_info;

   if (ev->button == 3)
   {
      e_menu_activate_mouse (face->menu, e_zone_current_get (face->con),
         ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN);
      e_util_container_fake_mouse_up_all_later (face->con);
   }
}

static void
_weather_face_cb_menu_enabled (void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather_Face *face;
   unsigned char enabled;

   face = data;
   enabled = e_menu_item_toggle_get (mi);
   if ((face->conf->enabled) && (!enabled))
   {
      _weather_face_disable (face);
   }
   else if ((!face->conf->enabled) && (enabled))
   {
      _weather_face_enable (face);
   }
}

static void
_weather_face_cb_menu_edit (void *data, E_Menu * m, E_Menu_Item * mi)
{
   Weather_Face *face;

   face = data;
   e_gadman_mode_set (face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static int
_weather_cb_check (void *data)
{
   int rc;
   int display;

   weatherCb_t w;
   Weather *ef;
   Weather_Face *face;
   Evas_List *l;
   ef = data;
   
   memset(&w, 0, sizeof(weatherCb_t));
	
   display = ef->conf->display;

   for (l = ef->faces; l; l = l->next)
   {
      face = l->data;

      rc = _weather_cb_download_html (&w);
	  if ( rc != GET_WEATHER_FAILURE )
	  {
	     rc = _weather_cb_parse_html (&w);
		 if ( rc != GET_WEATHER_FAILURE )
		 {
	        _weather_cb_convert_degrees(&w, ef);
		 }
	  }
	  
	  if ( rc == GET_WEATHER_FAILURE )
      {
		 display = GET_WEATHER_FAILURE;
	  }
	  
	  _weather_cb_set_display(&w, face , display);
  }

   
   return 1;

}

static int
_weather_cb_download_html(weatherCb_t *weather)
{
   int rc;
   FILE *fd;
   char exec_str[300];
   char *home_dir;
   char config_file_str[120];
   void *parsedStr;

   /* Check for NULL pointer */
   if (weather == NULL)
   {
	  return (GET_WEATHER_FAILURE);
   }
   
   memset(exec_str, 0, sizeof(exec_str));
   
   /* Get current home directory */
   home_dir = malloc (100);
   home_dir = getenv("HOME");
   sprintf(config_file_str, "%s%s", home_dir, CONFIG_FILE);
   free(home_dir);
   
   /* Get URL from config file */
   fd = fopen (config_file_str, "r");

   if ( fd == NULL )
   {
      return (GET_WEATHER_FAILURE);
   }

   parsedStr = malloc (sizeof (weather->URL));
   fgets(parsedStr, sizeof(weather->URL), fd);
   sprintf (exec_str, "wget -r -t 1 -O %s %s", HTML_FILE, parsedStr);
   free(parsedStr);
   fclose (fd);

   /* Execute wget command string */
   system(exec_str);
   
   /* Open the HTML file */
   fd = fopen (HTML_FILE, "r");
   if ( fd == NULL )
   {
      return (GET_WEATHER_FAILURE);
   }
   rc = fread (weather->file, sizeof (weather->file), 1, fd);
   if ( rc < 1 )
   {
      rc = GET_WEATHER_FAILURE;
   }
   fclose (fd);

   return (rc);
}

static int
_weather_cb_parse_html (weatherCb_t * weather)
{
   int rc, rc_hi_low;
   void *parsedStr;
   void *pNeedle;
   char location_str[40];
   char state_str[40];
   char cond_str[20];
   char cond_str2[20];
   char temp[1];
   char icon[1];

   /* Check for NULL pointer */
   if (weather == NULL)
   {
	  return (GET_WEATHER_FAILURE);
   }

   /* Open the config file and download current weather */
   
   /* Clean these out! */
   memset(location_str, 0, sizeof(location_str));
   memset(state_str, 0, sizeof(state_str));
   memset(cond_str, 0, sizeof(cond_str));

   /* Start parsing data */

   /* Allocate memory for the parsed string */
   parsedStr = malloc (sizeof (weather->file));
   
   /* Get current temp */
   pNeedle = strstr (weather->file, "temp=");
   snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
   sscanf (parsedStr, "temp=%d&", temp);
   sprintf (weather->currentTemp, "%d", temp[0]);
   
    /* Get icon and conditions */
    pNeedle = strstr(weather->file, "wxicons/"); 
    snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
    sscanf(parsedStr, "wxicons/%d/%d.gif", temp, icon);
    sprintf(weather->icon, "%d", icon[0]);
    
   /* Get location */
   pNeedle = strstr(weather->file, "Forecast for");
   snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
   sscanf(parsedStr, "Forecast for %[^,]%39s ", location_str, state_str);
   sprintf(weather->location, "%s", location_str);
   
   /* Get conditions */
   pNeedle = strstr(weather->file, "wxicons");
   snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
   pNeedle = strstr(weather->file, "TextA");
   snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
   sscanf(parsedStr, "TextA>%[^<]%s", cond_str, cond_str2);
   sprintf(weather->conditions, "%s", cond_str);
   
   /* Get hi/low */

   /* Get hi temp */
   pNeedle = strstr (weather->file, "templ1=");
   snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
   rc_hi_low = sscanf (parsedStr, "templ1=%d&", temp);

   if (rc_hi_low > 0)
   {
      sprintf (weather->lowTemp, "%d", temp[0]);
   }
   else
   {
      sprintf (weather->lowTemp, "--");
   }
   
   /* Get low temp */
   pNeedle = strstr (weather->file, "temph1=");
   snprintf(parsedStr, sizeof(weather->file), "%s", pNeedle);
   rc_hi_low = sscanf (parsedStr, "temph1=%d&", temp);

   if (rc_hi_low > 0)
   {
      sprintf (weather->highTemp, "%d", temp[0]);
   }
   else
   {
      sprintf (weather->highTemp, "--");
   }
   
   /* Complete the hi_low string */
   sprintf(weather->hi_low, "Hi/Lo: %s / %s", weather->highTemp, weather->lowTemp);
   
   /*
   ** Future spot for forecast stuff.
   */
   
   /* Free up memory */
   free(parsedStr);

   return ( 0 );
}

static int
_weather_cb_convert_degrees(weatherCb_t *weather, void *data)
{
   int temp;
	
   Weather *ef;
   ef = data;
	
   if ( (weather == NULL) || (data == NULL) )
   {
	  return (GET_WEATHER_FAILURE);
   }
   
   /* Check if degrees is in C or F */
   if ( ef->conf->degrees == DEGREES_C )
   {
      strcpy(weather->degrees, "C");
	  temp = atoi(weather->currentTemp);
      temp = (temp - 32) * 5 / 9;
      sprintf(weather->currentTemp, "%d", temp);
      
      if (strcmp(weather->highTemp, "--"))
      {
         temp = atoi(weather->highTemp);
         temp = (temp - 32) * 5 / 9;
         sprintf(weather->highTemp, "%d", temp);
      }
      if (strcmp(weather->lowTemp, "--"))
      {
         temp = atoi(weather->lowTemp);
         temp = (temp - 32) * 5 / 9;
         sprintf(weather->lowTemp, "%d", temp);
      }
   }
   else
   {
      strcpy(weather->degrees, "F");
   }

   sprintf(weather->hi_low, "Hi/Lo: %s / %s", weather->highTemp, weather->lowTemp);
   
   return (0);
}

static int
_weather_cb_set_display(weatherCb_t *weather, Weather_Face *face,
                        int display)
{
   Weather_Face *f;
   f = face;
   int rc;
	
   if ( (weather == NULL) || (face == NULL) )
   {
	  return (GET_WEATHER_FAILURE);
   }
   /* If _get_weather fails, blank out text and set icon to unknown */
      if ( display == GET_WEATHER_FAILURE )
      {
         edje_object_signal_emit (f->weather_object, UNKNOWN_WEATHER_ICON, "");
         edje_object_part_text_set (f->weather_object, "current_temp", "");
         edje_object_part_text_set (f->weather_object, "location", "");
         edje_object_part_text_set (f->weather_object, "conditions", "");
         edje_object_part_text_set (f->weather_object, "hi_low", "");
         edje_object_part_text_set (f->weather_object, "simple_location", "");
         edje_object_part_text_set (f->weather_object, "degrees", "");
         _weather_face_level_set(f, 1.0);
      }
      else if ( display == DETAILED_DISPLAY )
      {
         /* Detailed display */
         edje_object_signal_emit (f->weather_object, weather->icon, "");
         edje_object_part_text_set (f->weather_object, "current_temp", weather->currentTemp);
         edje_object_part_text_set (f->weather_object, "location", weather->location);
         edje_object_part_text_set (f->weather_object, "conditions", weather->conditions);
         edje_object_part_text_set (f->weather_object, "hi_low", weather->hi_low);
         edje_object_part_text_set (f->weather_object, "simple_location", "");
         edje_object_part_text_set (f->weather_object, "degrees", weather->degrees);
         _weather_face_level_set(f, 0.0);
      }
      else
      {
         /* Simple display */
         edje_object_signal_emit (f->weather_object, weather->icon, "");
         edje_object_part_text_set (f->weather_object, "current_temp", weather->currentTemp);
         edje_object_part_text_set (f->weather_object, "simple_location", weather->location);
         edje_object_part_text_set (f->weather_object, "location", "");
         edje_object_part_text_set (f->weather_object, "conditions", "");
         edje_object_part_text_set (f->weather_object, "hi_low", "");
         edje_object_part_text_set (f->weather_object, "degrees", weather->degrees);
         _weather_face_level_set(f, 1.0);
      }
	  
	  return (0);
  }


static void
_weather_face_level_set(Weather_Face *ef, double level)
{
   Edje_Message_Float msg;

   if (level < 0.0) level = 0.0;
   else if (level > 1.0) level = 1.0;
   msg.val = level;
   edje_object_message_send(ef->weather_object, EDJE_MESSAGE_FLOAT, 1, &msg);
}

static void
_weather_menu_fast(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->poll_time = 900.0;
   ecore_timer_del(weather->weather_check_timer);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time, _weather_cb_check, weather);
   e_config_save_queue();
}
static void
_weather_menu_medium(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->poll_time = 1800.0;
   ecore_timer_del(weather->weather_check_timer);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time, _weather_cb_check, weather);
   e_config_save_queue();
}
static void
_weather_menu_slow(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->poll_time = 3600.0;
   ecore_timer_del(weather->weather_check_timer);
   weather->weather_check_timer = ecore_timer_add(weather->conf->poll_time, _weather_cb_check, weather);
   e_config_save_queue();
}

static void
_weather_menu_display_simple(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->display = 0;
   _weather_cb_check(data);
   e_config_save_queue();
}

static void
_weather_menu_display_detailed(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->display = 1;
   _weather_cb_check(data);
   e_config_save_queue();
}

static void
_weather_menu_degrees_F(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->degrees = 0;
   _weather_cb_check(data);
   e_config_save_queue();
}

static void
_weather_menu_degrees_C(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;

   weather = data;
   weather->conf->degrees = 1;
   _weather_cb_check(data);
   e_config_save_queue();
}


static void
_weather_menu_edit_url(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Weather *weather;
   
   weather = data;
   system("xterm -bg lightyellow -fg black -e vi $HOME/.weather.config &");   
}
