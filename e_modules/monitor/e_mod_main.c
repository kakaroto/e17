#include "e_mod_main.h"
#include <sys/utsname.h>
#include <stdio.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

/* module private routines */

static Monitor *_monitor_new();
static void _monitor_shutdown(Monitor *monitor);
static void _monitor_config_menu_new(Monitor *monitor);

static Monitor_Face *_monitor_face_new(E_Container *con, Config *config);
static void _monitor_face_free(Monitor_Face *face);
static void _monitor_face_menu_new(Monitor_Face *face);
static void _monitor_face_cb_gmc_change(void *data,
                                        E_Gadman_Client *gmc,
                                        E_Gadman_Change change);
Config_Face *_monitor_face_config_init(Config_Face *conf);
static int _monitor_face_config_cb_timer(void *data);

static void _monitor_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
static void _monitor_cpu_text_update_callcack(Flow_Chart *chart, void *data);
static void _monitor_mem_real_text_update_callback(Flow_Chart *chart,
                                                   void *data);
static void _monitor_mem_swap_text_update_callback(Flow_Chart *chart,
                                                   void *data);
static void _monitor_net_in_text_update_callcack(Flow_Chart *chart, void *data);
static void _monitor_net_out_text_update_callcack(Flow_Chart *chart,
                                                  void *data);
static void _monitor_wlan_link_text_update_callcack(Flow_Chart *chart,
                                                    void *data);
static void _monitor_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _add_sensor(Monitor_Face *face, Evas_Object *o, int VerHor);
static int _date_cb_check(void *data);

static void _monitor_face_cb_mouse_down(void *data, Evas *e,
                                        Evas_Object *obj, void *event_info);

static int _monitor_count;
static int num_sensors;

static E_Config_DD *conf_edd;
static E_Config_DD *conf_face_edd;

static Flow_Chart *flow_chart_cpu;
static Flow_Chart *flow_chart_net_in;
static Flow_Chart *flow_chart_net_out;
static Flow_Chart *flow_chart_mem_real;
static Flow_Chart *flow_chart_mem_swap;
static Flow_Chart *flow_chart_wlan_link;

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Monitor"
};

EAPI void *
e_modapi_init(E_Module *module)
{
   Monitor *monitor;

   /* actually init buttons */
   monitor = _monitor_new();
   module->config_menu = monitor->config_menu;
   return monitor;
}

EAPI int
e_modapi_shutdown(E_Module *module)
{
   Monitor *monitor;

   if (module->config_menu)
      module->config_menu = NULL;

   monitor = module->data;
   if (monitor)
      _monitor_shutdown(monitor);

   return 1;
}

EAPI int
e_modapi_save(E_Module *module)
{
   Monitor *monitor;

   monitor = module->data;
   e_config_domain_save("module.monitor", conf_edd, monitor->conf);
   return 1;
}

EAPI int
e_modapi_info(E_Module *module)
{
   module->icon_file = strdup(PACKAGE_DATA_DIR "/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *module)
{
   e_module_dialog_show(_("Enlightenment Monitor Module"),
                        _
                        ("A simple module to give E17 a usage monitor for some resources."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *module)
{
   Monitor *mon;
   Evas_List *l;

   mon = module->data;
   if (!mon)
      return 0;
   for (l = mon->faces; l; l = l->next)
     {
        Monitor_Face *f;

        f = l->data;
        if (!f)
           return 0;
        if (f->con == e_container_current_get(e_manager_current_get()))
          {
             _config_monitor_module(f->con, f);
             break;
          }
     }
   return 1;
}

static int
_monitor_face_config_cb_timer(void *data)
{
   e_error_dialog_show(_("Configuration Upgraded"), data);
   return 0;
}

Config_Face *
_monitor_face_config_init(Config_Face *conf)
{
   if (!conf)
      return NULL;
   conf->config_version = MONITOR_CONFIG_VERSION;
   conf->enabled = 1;
   conf->cpu_interval = 1.0;
   conf->mem_interval = 1.0;
   conf->net_interval = 1.0;
   conf->net_interface = (char *)evas_stringshare_add("eth0");
   conf->wlan_interval = 1.0;
   conf->wlan_interface = (char *)evas_stringshare_add("wlan0");
   conf->mem_real_ignore_cached = 0;
   conf->mem_real_ignore_buffers = 0;

   return conf;
}

/* module private routines */
static Monitor *
_monitor_new()
{
   Monitor *monitor;
   Evas_List *managers, *l, *l2, *cl;
   E_Menu_Item *mi;

   _monitor_count = 0;
   monitor = E_NEW(Monitor, 1);

   if (!monitor)
      return NULL;

   conf_face_edd = E_CONFIG_DD_NEW("Monitor_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D conf_face_edd
   E_CONFIG_VAL(D, T, config_version, INT);
   E_CONFIG_VAL(D, T, enabled, INT);
   E_CONFIG_VAL(D, T, cpu_interval, DOUBLE);
   E_CONFIG_VAL(D, T, mem_interval, DOUBLE);
   E_CONFIG_VAL(D, T, net_interval, DOUBLE);
   E_CONFIG_VAL(D, T, net_interface, STR);
   E_CONFIG_VAL(D, T, wlan_interval, DOUBLE);
   E_CONFIG_VAL(D, T, wlan_interface, STR);
   E_CONFIG_VAL(D, T, mem_real_ignore_cached, INT);
   E_CONFIG_VAL(D, T, mem_real_ignore_buffers, INT);

   conf_edd = E_CONFIG_DD_NEW("Monitor_Config", Config);

#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);
   E_CONFIG_VAL(D, T, cpu, INT);
   E_CONFIG_VAL(D, T, mem, INT);
   E_CONFIG_VAL(D, T, net, INT);
   E_CONFIG_VAL(D, T, wlan, INT);
   E_CONFIG_VAL(D, T, Horz, INT);
   E_CONFIG_VAL(D, T, hostname, INT);
   E_CONFIG_VAL(D, T, uptime, INT);
   E_CONFIG_VAL(D, T, time, INT);

   monitor->conf = e_config_domain_load("module.monitor", conf_edd);
   if (!monitor->conf)
     {
        monitor->conf = E_NEW(Config, 1);

        monitor->conf->cpu = 0;
        monitor->conf->mem = 0;
        monitor->conf->net = 0;
        monitor->conf->wlan = 0;
        monitor->conf->Horz = 0;
        monitor->conf->hostname = 1;
        monitor->conf->uptime = 1;
        monitor->conf->time = 0;
     }

   _monitor_config_menu_new(monitor);

   managers = e_manager_list();
   cl = monitor->conf->faces;
   for (l = managers; l; l = l->next)
     {
        E_Manager *man;

        man = l->data;
        for (l2 = man->containers; l2; l2 = l2->next)
          {
             E_Container *con;
             Monitor_Face *face;

             con = l2->data;
             num_sensors = 0;
             face = _monitor_face_new(con, monitor->conf);
             if (face)
               {
                  face->mon = monitor;
                  monitor->faces = evas_list_append(monitor->faces, face);
                  /* Config */
                  if (!cl)
                    {
                       face->conf = E_NEW(Config_Face, 1);

                       face->conf = _monitor_face_config_init(face->conf);
                       monitor->conf->faces =
                          evas_list_append(monitor->conf->faces, face->conf);
                    }
                  else
                    {
                       face->conf = cl->data;
                       /*  if (face->conf->config_version != MONITOR_CONFIG_VERSION)
                        * {
                        * face->conf = E_NEW(Config_Face, 1);
                        * face->conf = _monitor_face_config_init(face->conf);
                        * ecore_timer_add(1.0, _monitor_face_config_cb_timer,
                        * _("Configuration data needed upgrading. Your old configuration\n"
                        * "has been removed. Please reset your setting for the monitor\n"
                        * "module. Sorry for the inconvenience.\n"));
                        * } */
                       cl = cl->next;
                    }

                  /* Menu */
                  /* This menu must be initialized after conf */
                  _monitor_face_menu_new(face);

                  mi = e_menu_item_new(monitor->config_menu);
                  e_menu_item_label_set(mi, _("Configuration"));
                  e_menu_item_callback_set(mi, _monitor_menu_cb_configure,
                                           face);

                  mi = e_menu_item_new(monitor->config_menu);
                  e_menu_item_label_set(mi, con->name);
                  e_menu_item_submenu_set(mi, face->menu);

                  /* 
                   * Now that init is done, set up intervals,
                   * interfaces, and memory monitor parameters
                   * according to the configuration.
                   */
                  mem_real_ignore_buffers_set(face->conf->
                                              mem_real_ignore_buffers);
                  mem_real_ignore_cached_set(face->conf->
                                             mem_real_ignore_cached);
                  net_interface_set(face->conf->net_interface);
                  wlan_interface_set(face->conf->wlan_interface);

                  if (monitor->conf->cpu)
                     flow_chart_update_rate_set(flow_chart_cpu,
                                                face->conf->cpu_interval);
                  if (monitor->conf->mem)
                     flow_chart_update_rate_set(flow_chart_mem_real,
                                                face->conf->mem_interval);
                  if (monitor->conf->mem)
                     flow_chart_update_rate_set(flow_chart_mem_swap,
                                                face->conf->mem_interval);
                  if (monitor->conf->net)
                     flow_chart_update_rate_set(flow_chart_net_in,
                                                face->conf->net_interval);
                  if (monitor->conf->net)
                     flow_chart_update_rate_set(flow_chart_net_out,
                                                face->conf->net_interval);
                  if (monitor->conf->wlan)
                     flow_chart_update_rate_set(flow_chart_wlan_link,
                                                face->conf->wlan_interval);

                  rebuild_monitor(face);
               }
          }
     }
   return monitor;
}

static void
_monitor_shutdown(Monitor *monitor)
{
   Evas_List *list;

   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_face_edd);

   for (list = monitor->faces; list; list = list->next)
      _monitor_face_free(list->data);
   evas_list_free(monitor->faces);

   e_object_del(E_OBJECT(monitor->config_menu));

   evas_list_free(monitor->conf->faces);
   free(monitor->conf);
   free(monitor);
}

static void
_monitor_config_menu_new(Monitor *monitor)
{
   monitor->config_menu = e_menu_new();
}

static Monitor_Face *
_monitor_face_new(E_Container *con, Config *config)
{
   Monitor_Face *face;
   Evas_Object *o;
   struct utsname u_buf;
   char u_date_time[256];
   struct sysinfo s_info;

   sysinfo(&s_info);

   long minute = 60;
   long hour = minute * 60;
   long day = hour * 24;
   double megabyte = 1024 * 1024;

   uname(&u_buf);
   /* 
    * Configuration cannot be used yet as the face config 
    * is not initialized. Everything will be updated after 
    * the init is complete.
    */
   double tmp_cpu_interval = 1.0;
   double tmp_mem_interval = 1.0;
   double tmp_net_interval = 1.0;
   double tmp_wlan_interval = 1.0;

   Chart_Container *chart_con;

   face = E_NEW(Monitor_Face, 1);

   if (!face)
      return NULL;

   face->con = con;
   e_object_ref(E_OBJECT(con));

   evas_event_freeze(con->bg_evas);

   /* setup monitor object */
   o = edje_object_add(con->bg_evas);
   face->monitor_object = o;
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/monitor", "modules/monitor/main"))
      edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj", "monitor/main");
   evas_object_show(o);

   /* setup res table */
   o = e_table_add(con->bg_evas);
   face->table_object = o;
   e_table_homogenous_set(o, 1);
   edje_object_part_swallow(face->monitor_object, "items", face->table_object);
   evas_object_show(o);

   /*setup hostname sensor */
   if (config->hostname)
     {
        face->hostname = edje_object_add(con->bg_evas);
        if (!e_theme_edje_object_set
            (face->hostname, "base/theme/modules/monitor",
             "modules/monitor/host"))
           edje_object_file_set(face->hostname, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/host");
        _add_sensor(face, face->hostname, config->Horz);
        edje_object_part_text_set(face->hostname, "sysname", u_buf.sysname);
        edje_object_part_text_set(face->hostname, "release", u_buf.release);
        edje_object_part_text_set(face->hostname, "version", u_buf.version);
        edje_object_part_text_set(face->hostname, "machine", u_buf.machine);
        //edje_object_part_text_set(face->hostname,"node",u_buf.node);
     }

   /* setup cpu */
   if (config->cpu)
     {
        o = edje_object_add(con->bg_evas);
        face->cpu = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/cpu"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/cpu");
        _add_sensor(face, face->cpu, config->Horz);

        /* add cpu chart */
        chart_con = chart_container_new(con->bg_evas, 0, 0, 0, 0);
        flow_chart_cpu = flow_chart_new();
        flow_chart_color_set(flow_chart_cpu, 33, 100, 220, 125);
        flow_chart_get_value_function_set(flow_chart_cpu, cpu_usage_get);
        flow_chart_update_rate_set(flow_chart_cpu, tmp_cpu_interval);
        chart_container_chart_add(chart_con, flow_chart_cpu);
        face->chart_cpu = chart_con;
        flow_chart_callback_set(flow_chart_cpu,
                                _monitor_cpu_text_update_callcack, face);
     }

   if (config->mem)
     {
        /* setup mem */
        o = edje_object_add(con->bg_evas);
        face->mem = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/mem"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/mem");
        _add_sensor(face, face->mem, config->Horz);

        /* add mem charts */
        chart_con = chart_container_new(con->bg_evas, 0, 0, 0, 0);
        flow_chart_mem_real = flow_chart_new();
        flow_chart_color_set(flow_chart_mem_real, 213, 91, 91, 125);
        flow_chart_get_value_function_set(flow_chart_mem_real,
                                          mem_real_usage_get);
        flow_chart_update_rate_set(flow_chart_mem_real, tmp_mem_interval);
        chart_container_chart_add(chart_con, flow_chart_mem_real);
        face->chart_mem = chart_con;
        flow_chart_callback_set(flow_chart_mem_real,
                                _monitor_mem_real_text_update_callback, face);

        flow_chart_mem_swap = flow_chart_new();
        flow_chart_color_set(flow_chart_mem_swap, 51, 181, 69, 125);
        flow_chart_get_value_function_set(flow_chart_mem_swap,
                                          mem_swap_usage_get);
        flow_chart_update_rate_set(flow_chart_mem_swap, tmp_mem_interval);
        flow_chart_alignment_set(flow_chart_mem_swap, 0);
        chart_container_chart_add(chart_con, flow_chart_mem_swap);
        flow_chart_callback_set(flow_chart_mem_swap,
                                _monitor_mem_swap_text_update_callback, face);
     }

   if (config->net)
     {
        /* setup net */
        o = edje_object_add(con->bg_evas);
        face->net = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/net"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/net");
        _add_sensor(face, face->net, config->Horz);

        /* add net charts */
        chart_con = chart_container_new(con->bg_evas, 0, 0, 0, 0);
        flow_chart_net_in = flow_chart_new();
        flow_chart_color_set(flow_chart_net_in, 213, 91, 91, 125);
        flow_chart_get_value_function_set(flow_chart_net_in, net_in_usage_get);
        flow_chart_update_rate_set(flow_chart_net_in, tmp_net_interval);
        chart_container_chart_add(chart_con, flow_chart_net_in);
        face->chart_net = chart_con;
        flow_chart_callback_set(flow_chart_net_in,
                                _monitor_net_in_text_update_callcack, face);

        flow_chart_net_out = flow_chart_new();
        flow_chart_color_set(flow_chart_net_out, 51, 181, 69, 125);
        flow_chart_get_value_function_set(flow_chart_net_out,
                                          net_out_usage_get);
        flow_chart_update_rate_set(flow_chart_net_out, tmp_net_interval);
        flow_chart_alignment_set(flow_chart_net_out, 0);
        chart_container_chart_add(chart_con, flow_chart_net_out);
        flow_chart_callback_set(flow_chart_net_out,
                                _monitor_net_out_text_update_callcack, face);
     }

   if (config->wlan)
     {
        /* setup wlan */
        o = edje_object_add(con->bg_evas);
        face->wlan = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/wlan"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/wlan");
        _add_sensor(face, face->wlan, config->Horz);

        /* add wlan charts */
        chart_con = chart_container_new(con->bg_evas, 0, 0, 0, 0);
        flow_chart_wlan_link = flow_chart_new();
        flow_chart_color_set(flow_chart_wlan_link, 33, 100, 220, 125);
        flow_chart_get_value_function_set(flow_chart_wlan_link, wlan_link_get);
        flow_chart_update_rate_set(flow_chart_wlan_link, tmp_wlan_interval);
        chart_container_chart_add(chart_con, flow_chart_wlan_link);
        face->chart_wlan = chart_con;
        flow_chart_callback_set(flow_chart_wlan_link,
                                _monitor_wlan_link_text_update_callcack, face);
     }

   if (config->uptime)
     {
        face->uptime = edje_object_add(con->bg_evas);
        if (!e_theme_edje_object_set
            (face->uptime, "base/theme/modules/monitor",
             "modules/monitor/uptime"))
           edje_object_file_set(face->uptime, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/uptime");
        _add_sensor(face, face->uptime, config->Horz);
        sprintf(u_date_time, "uptime: %ld days, %ld:%02ld:%02ld",
                s_info.uptime / day, (s_info.uptime % day) / hour,
                (s_info.uptime % hour) / minute, s_info.uptime % minute);
        edje_object_part_text_set(face->uptime, "uptime", u_date_time);
        face->date_check_timer = ecore_timer_add(1, _date_cb_check, face);
     }

   if (config->time)
     {
        time_t now;
        struct tm date;

        time(&now);
        char curr_time[12];

        date = *localtime(&now);
        face->time = edje_object_add(con->bg_evas);
        if (!e_theme_edje_object_set
            (face->time, "base/theme/modules/monitor", "modules/monitor/time"))
           edje_object_file_set(face->time, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/time");
        _add_sensor(face, face->time, config->Horz);
        sprintf(curr_time, "%02d:%02d:%02d", date.tm_hour, date.tm_min,
                date.tm_sec);
        edje_object_part_text_set(face->time, "time", curr_time);
     }

   face->monitor_cover_obj = evas_object_rectangle_add(face->con->bg_evas);
   evas_object_color_set(face->monitor_cover_obj, 255, 255, 255, 0);
   evas_object_event_callback_add(face->monitor_cover_obj,
                                  EVAS_CALLBACK_MOUSE_DOWN,
                                  _monitor_face_cb_mouse_down, face);
   evas_object_show(face->monitor_cover_obj);

   /* setup gadman */
   face->gmc = e_gadman_client_new(con->gadman);
   e_gadman_client_domain_set(face->gmc, "module.monitor", _monitor_count++);
   e_gadman_client_policy_set(face->gmc,
                              E_GADMAN_POLICY_ANYWHERE |
                              E_GADMAN_POLICY_HMOVE |
                              E_GADMAN_POLICY_VMOVE |
                              E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(face->gmc, 14, 7);
   e_gadman_client_align_set(face->gmc, 1.0, 1.0);
   e_gadman_client_resize(face->gmc, 160, 40);
   e_gadman_client_change_func_set(face->gmc, _monitor_face_cb_gmc_change,
                                   face);
   e_gadman_client_load(face->gmc);

   evas_event_thaw(con->bg_evas);
   return face;
}

static void
_monitor_cpu_text_update_callcack(Flow_Chart *chart, void *data)
{
   Monitor_Face *face;
   char buf[64];

   face = data;

   snprintf(buf, 64, "%i%%", chart->current_value);
   edje_object_part_text_set(face->cpu, "cpu-text", buf);
}

static void
_monitor_mem_real_text_update_callback(Flow_Chart *chart, void *data)
{
   Monitor_Face *face;
   char buf[64];

   long kbytes = mem_real_get();

   face = data;

   if (kbytes > 1048576)
      snprintf(buf, 64, "%ldGB", kbytes / 1048576);
   else if (kbytes > 1024 && kbytes < 1048576)
      snprintf(buf, 64, "%ldMB", kbytes / 1024);
   else
      snprintf(buf, 64, "%ldKB", kbytes);

   edje_object_part_text_set(face->mem, "mem-real-text", buf);
}

static void
_monitor_mem_swap_text_update_callback(Flow_Chart *chart, void *data)
{
   Monitor_Face *face;
   char buf[64];

   long kbytes = mem_swap_get();

   face = data;

   if (kbytes > 1048576)
      snprintf(buf, 64, "%ldGB", kbytes / 1048576);
   else if (kbytes > 1024 && kbytes < 1048576)
      snprintf(buf, 64, "%ldMB", kbytes / 1024);
   else
      snprintf(buf, 64, "%ldKB", kbytes);

   edje_object_part_text_set(face->mem, "mem-swap-text", buf);
}

static void
_monitor_net_in_text_update_callcack(Flow_Chart *chart, void *data)
{
   Monitor_Face *face;
   char buf[64];

   long bytes = net_bytes_in_get();

   face = data;

   if (bytes > 1048576)
      snprintf(buf, 64, "%ldMB", bytes / 1048576);
   else if (bytes > 1024 && bytes < 1048576)
      snprintf(buf, 64, "%ldKB", bytes / 1024);
   else
      snprintf(buf, 64, "%ldB", bytes);

   edje_object_part_text_set(face->net, "net-in-text", buf);
}

static void
_monitor_net_out_text_update_callcack(Flow_Chart *chart, void *data)
{
   Monitor_Face *face;
   char buf[64];

   long bytes = net_bytes_out_get();

   face = data;

   if (bytes > 1048576)
      snprintf(buf, 64, "%ldMB", bytes / 1048576);
   else if (bytes > 1024 && bytes < 1048576)
      snprintf(buf, 64, "%ldKB", bytes / 1024);
   else
      snprintf(buf, 64, "%ldB", bytes);

   edje_object_part_text_set(face->net, "net-out-text", buf);
}

static void
_monitor_wlan_link_text_update_callcack(Flow_Chart *chart, void *data)
{
   Monitor_Face *face;
   char buf[64];
   long link = wlan_link_get();

   face = data;
   snprintf(buf, 64, "LNK: %ld", link);
   edje_object_part_text_set(face->wlan, "wlan-link-text", buf);
}

static void
_monitor_face_free(Monitor_Face *face)
{
   e_object_unref(E_OBJECT(face->con));
   e_object_del(E_OBJECT(face->gmc));
   if (face->date_check_timer)
      ecore_timer_del(face->date_check_timer);

   if (face->cpu)
      evas_object_del(face->cpu);
   if (face->mem)
      evas_object_del(face->mem);
   if (face->net)
      evas_object_del(face->net);
   if (face->wlan)
      evas_object_del(face->wlan);
   if (face->hostname)
      evas_object_del(face->hostname);
   if (face->uptime)
      evas_object_del(face->uptime);
   if (face->time)
      evas_object_del(face->time);

   chart_container_del(face->chart_cpu);
   chart_container_del(face->chart_mem);
   chart_container_del(face->chart_net);
   chart_container_del(face->chart_wlan);

   if (face->monitor_object)
      evas_object_del(face->monitor_object);
   if (face->table_object)
      evas_object_del(face->table_object);

   e_object_del(E_OBJECT(face->menu));

   if (face->conf->wlan_interface)
      evas_stringshare_del(face->conf->wlan_interface);
   if (face->conf->net_interface)
      evas_stringshare_del(face->conf->net_interface);
   free(face->conf);
   free(face);
   _monitor_count--;
}

static void
_monitor_face_cb_gmc_change(void *data, E_Gadman_Client *gmc,
                            E_Gadman_Change change)
{
   Monitor_Face *face;
   Evas_Coord x, y, w, h;

   face = data;
   switch (change)
     {
     case E_GADMAN_CHANGE_MOVE_RESIZE:
        e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);

        evas_object_move(face->monitor_object, x, y);
        evas_object_resize(face->monitor_object, w, h);

        evas_object_move(face->table_object, x, y);
        evas_object_resize(face->table_object, w, h);

        evas_object_move(face->monitor_cover_obj, x, y);
        evas_object_resize(face->monitor_cover_obj, w, h);

        if (face->cpu)
           evas_object_geometry_get(face->cpu, &x, &y, &w, &h);
        if (face->cpu)
           chart_container_move(face->chart_cpu, x + 2, y + 2);
        if (face->cpu)
           chart_container_resize(face->chart_cpu, w - 4, h - 4);

        if (face->mem)
           evas_object_geometry_get(face->mem, &x, &y, &w, &h);
        if (face->mem)
           chart_container_move(face->chart_mem, x + 2, y + 2);
        if (face->mem)
           chart_container_resize(face->chart_mem, w - 4, h - 4);

        if (face->net)
           evas_object_geometry_get(face->net, &x, &y, &w, &h);
        if (face->net)
           chart_container_move(face->chart_net, x + 2, y + 2);
        if (face->net)
           chart_container_resize(face->chart_net, w - 4, h - 4);

        if (face->wlan)
           evas_object_geometry_get(face->wlan, &x, &y, &w, &h);
        if (face->wlan)
           chart_container_move(face->chart_wlan, x + 2, y + 2);
        if (face->wlan)
           chart_container_resize(face->chart_wlan, w - 4, h - 4);

        break;
     case E_GADMAN_CHANGE_RAISE:
        //evas_object_raise(face->exit_event_object);
        break;
     case E_GADMAN_CHANGE_EDGE:
     case E_GADMAN_CHANGE_ZONE:
        /* FIXME
         * Must we do something here?
         */
        break;
     }
   e_gadman_client_save(face->gmc);
}

static void
_monitor_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Monitor_Face *face;

   face = data;
   e_gadman_mode_set(face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void
_monitor_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
                            void *event_info)
{
   Monitor_Face *face;
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
   /*
    * else if(ev->button == 1)
    * {
    * Evas_Coord x, y, w, h;
    * e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);
    * 
    * if(ev->output.x < x + w / 2)
    * restart = 1;
    * ecore_main_loop_quit();    
    * }
    */
}
/*
static void
_monitor_face_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, 
			  void *event_info)
{
}

static void
_monitor_face_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, 
			   void *event_info)
{
}
*/

static void
_monitor_face_menu_new(Monitor_Face *face)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   face->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _monitor_menu_cb_configure, face);

   mi = e_menu_item_new(face->menu);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _monitor_face_cb_menu_edit, face);
}

void
_monitor_cb_config_updated(void *data)
{
   Monitor_Face *face;

   face = data;

   if (face->cpu)
      flow_chart_update_rate_set(flow_chart_cpu, face->conf->cpu_interval);

   if (face->mem)
     {
        mem_real_ignore_cached_set(face->conf->mem_real_ignore_cached);
        mem_real_ignore_buffers_set(face->conf->mem_real_ignore_buffers);
        flow_chart_update_rate_set(flow_chart_mem_real,
                                   face->conf->mem_interval);
        flow_chart_update_rate_set(flow_chart_mem_swap,
                                   face->conf->mem_interval);
     }

   if (face->wlan)
     {
        wlan_interface_set(face->conf->wlan_interface);
        flow_chart_update_rate_set(flow_chart_wlan_link,
                                   face->conf->wlan_interval);
     }

   if (face->net)
     {
        net_interface_set(face->conf->net_interface);
        flow_chart_update_rate_set(flow_chart_net_in, face->conf->net_interval);
        flow_chart_update_rate_set(flow_chart_net_out,
                                   face->conf->net_interval);
     }
}

static void
_monitor_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Monitor_Face *f;

   f = data;
   if (!f)
      return;
   _config_monitor_module(f->con, f);
}

static void
_add_sensor(Monitor_Face *face, Evas_Object *o, int VerHor)
{
   if (VerHor)
      e_table_pack(face->table_object, o, num_sensors, 0, 1, 1);
   else
      e_table_pack(face->table_object, o, 0, num_sensors, 1, 1);
   e_table_pack_options_set(o, 1, 1, 1, 1, 0.5, 0.5, 0, 0, -1, -1);
   evas_object_layer_set(o, evas_object_layer_get(face->monitor_object) + 1);
   evas_object_show(o);
   num_sensors++;
}

void
rebuild_monitor(Monitor_Face *face)
{
   struct utsname u_buf;

   uname(&u_buf);

   struct sysinfo s_info;

   sysinfo(&s_info);

   long minute = 60;
   long hour = minute * 60;
   long day = hour * 24;
   double megabyte = 1024 * 1024;

   Chart_Container *chart_con;
   Monitor *mon;
   Monitor_Face *f;

   num_sensors = 0;

   e_object_del(E_OBJECT(face->menu));

   Evas_Object *o;

   evas_event_freeze(face->con->bg_evas);

   if (face->gmc)
      e_gadman_client_save(face->gmc);

   /* setup monitor object */
   if (face->monitor_object)
      evas_object_del(face->monitor_object);
   o = edje_object_add(face->con->bg_evas);
   face->monitor_object = o;
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/monitor", "modules/monitor/main"))
      edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                           "modules/monitor/main");
   evas_object_show(o);

   /* setup res table */
   if (face->table_object)
      evas_object_del(face->table_object);
   o = e_table_add(face->con->bg_evas);
   face->table_object = o;
   e_table_homogenous_set(o, 1);
   edje_object_part_swallow(face->monitor_object, "items", face->table_object);
   evas_object_show(o);

   /*setup hostname */
   if (face->mon->conf->hostname)
     {
        if (face->hostname)
           evas_object_del(face->hostname);
        face->hostname = edje_object_add(face->con->bg_evas);
        if (!e_theme_edje_object_set
            (face->hostname, "base/theme/modules/monitor",
             "modules/monitor/host"))
           edje_object_file_set(face->hostname, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/host");
        _add_sensor(face, face->hostname, face->mon->conf->Horz);
        edje_object_part_text_set(face->hostname, "sysname", u_buf.sysname);
        edje_object_part_text_set(face->hostname, "release", u_buf.release);
        edje_object_part_text_set(face->hostname, "version", u_buf.version);
        edje_object_part_text_set(face->hostname, "machine", u_buf.machine);
        //edje_object_part_text_set(face->hostname,"node",u_buf.node);
     }
   else if (face->hostname)
     {
        evas_object_del(face->hostname);
        face->hostname = NULL;
     }

   /* setup cpu */
   if (face->mon->conf->cpu)
     {
        if (face->cpu)
           evas_object_del(face->cpu);
        if (face->chart_cpu)
           chart_container_del(face->chart_cpu);
        o = edje_object_add(face->con->bg_evas);
        face->cpu = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/cpu"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/cpu");
        _add_sensor(face, face->cpu, face->mon->conf->Horz);
        /* add cpu chart */
        chart_con = chart_container_new(face->con->bg_evas, 0, 0, 0, 0);
        flow_chart_cpu = flow_chart_new();
        flow_chart_color_set(flow_chart_cpu, 33, 100, 220, 125);
        flow_chart_get_value_function_set(flow_chart_cpu, cpu_usage_get);
        flow_chart_update_rate_set(flow_chart_cpu, face->conf->cpu_interval);
        chart_container_chart_add(chart_con, flow_chart_cpu);
        face->chart_cpu = chart_con;
        flow_chart_callback_set(flow_chart_cpu,
                                _monitor_cpu_text_update_callcack, face);

     }
   else if (face->cpu)
     {
        evas_object_del(face->cpu);
        if (face->chart_cpu)
           chart_container_del(face->chart_cpu);
        face->chart_cpu = NULL;
        face->cpu = NULL;
     }

   if (face->mon->conf->mem)
     {
        if (face->mem)
           evas_object_del(face->mem);
        if (face->chart_mem)
           chart_container_del(face->chart_mem);

        /* setup mem */
        o = edje_object_add(face->con->bg_evas);
        face->mem = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/mem"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/mem");
        _add_sensor(face, face->mem, face->mon->conf->Horz);
        /* add mem charts */
        chart_con = chart_container_new(face->con->bg_evas, 0, 0, 0, 0);
        flow_chart_mem_real = flow_chart_new();
        flow_chart_color_set(flow_chart_mem_real, 213, 91, 91, 125);
        flow_chart_get_value_function_set(flow_chart_mem_real,
                                          mem_real_usage_get);
        flow_chart_update_rate_set(flow_chart_mem_real,
                                   face->conf->mem_interval);
        chart_container_chart_add(chart_con, flow_chart_mem_real);
        face->chart_mem = chart_con;
        flow_chart_callback_set(flow_chart_mem_real,
                                _monitor_mem_real_text_update_callback, face);

        flow_chart_mem_swap = flow_chart_new();
        flow_chart_color_set(flow_chart_mem_swap, 51, 181, 69, 125);
        flow_chart_get_value_function_set(flow_chart_mem_swap,
                                          mem_swap_usage_get);
        flow_chart_update_rate_set(flow_chart_mem_swap,
                                   face->conf->mem_interval);
        flow_chart_alignment_set(flow_chart_mem_swap, 0);
        chart_container_chart_add(chart_con, flow_chart_mem_swap);
        flow_chart_callback_set(flow_chart_mem_swap,
                                _monitor_mem_swap_text_update_callback, face);
     }
   else if (face->mem)
     {
        evas_object_del(face->mem);
        if (face->chart_mem)
           chart_container_del(face->chart_mem);
        face->chart_mem = NULL;
        face->mem = NULL;
     }

   if (face->mon->conf->net)
     {
        if (face->net)
           evas_object_del(face->net);
        if (face->chart_net)
           chart_container_del(face->chart_net);

        /* setup net */
        o = edje_object_add(face->con->bg_evas);
        face->net = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/net"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/net");
        _add_sensor(face, face->net, face->mon->conf->Horz);
        /* add net charts */
        chart_con = chart_container_new(face->con->bg_evas, 0, 0, 0, 0);
        flow_chart_net_in = flow_chart_new();
        flow_chart_color_set(flow_chart_net_in, 213, 91, 91, 125);
        flow_chart_get_value_function_set(flow_chart_net_in, net_in_usage_get);
        flow_chart_update_rate_set(flow_chart_net_in, face->conf->net_interval);
        chart_container_chart_add(chart_con, flow_chart_net_in);
        face->chart_net = chart_con;
        flow_chart_callback_set(flow_chart_net_in,
                                _monitor_net_in_text_update_callcack, face);

        flow_chart_net_out = flow_chart_new();
        flow_chart_color_set(flow_chart_net_out, 51, 181, 69, 125);
        flow_chart_get_value_function_set(flow_chart_net_out,
                                          net_out_usage_get);
        flow_chart_update_rate_set(flow_chart_net_out,
                                   face->conf->net_interval);
        flow_chart_alignment_set(flow_chart_net_out, 0);
        chart_container_chart_add(chart_con, flow_chart_net_out);
        flow_chart_callback_set(flow_chart_net_out,
                                _monitor_net_out_text_update_callcack, face);
     }
   else if (face->net)
     {
        evas_object_del(face->net);
        if (face->chart_net)
           chart_container_del(face->chart_net);
        face->chart_net = NULL;
        face->net = NULL;
     }

   if (face->mon->conf->wlan)
     {
        if (face->wlan)
           evas_object_del(face->wlan);
        if (face->chart_wlan)
           chart_container_del(face->chart_wlan);

        /* setup wlan */
        o = edje_object_add(face->con->bg_evas);
        face->wlan = o;
        if (!e_theme_edje_object_set
            (o, "base/theme/modules/monitor", "modules/monitor/wlan"))
           edje_object_file_set(o, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/wlan");
        _add_sensor(face, face->wlan, face->mon->conf->Horz);
        /* add wlan charts */
        chart_con = chart_container_new(face->con->bg_evas, 0, 0, 0, 0);
        flow_chart_wlan_link = flow_chart_new();
        flow_chart_color_set(flow_chart_wlan_link, 33, 100, 220, 125);
        flow_chart_get_value_function_set(flow_chart_wlan_link, wlan_link_get);
        flow_chart_update_rate_set(flow_chart_wlan_link,
                                   face->conf->wlan_interval);
        chart_container_chart_add(chart_con, flow_chart_wlan_link);
        face->chart_wlan = chart_con;
        flow_chart_callback_set(flow_chart_wlan_link,
                                _monitor_wlan_link_text_update_callcack, face);
     }
   else if (face->wlan)
     {
        evas_object_del(face->wlan);
        if (face->chart_wlan)
           chart_container_del(face->chart_wlan);
        face->chart_wlan = NULL;
        face->wlan = NULL;
     }

   if (face->mon->conf->uptime)
     {
        if (face->uptime)
           evas_object_del(face->uptime);

        int num_days, num_hours, num_min;
        char u_date_time[256];

        face->uptime = edje_object_add(face->con->bg_evas);
        if (!e_theme_edje_object_set
            (face->uptime, "base/theme/modules/monitor",
             "modules/monitor/uptime"))
           edje_object_file_set(face->uptime, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/uptime");
        sprintf(u_date_time, "uptime: %ld days, %ld:%02ld:%02ld",
                s_info.uptime / day, (s_info.uptime % day) / hour,
                (s_info.uptime % hour) / minute, s_info.uptime % minute);
        edje_object_part_text_set(face->uptime, "uptime", u_date_time);
        face->date_check_timer = ecore_timer_add(1, _date_cb_check, face);
        _add_sensor(face, face->uptime, face->mon->conf->Horz);
     }
   else if (face->uptime)
     {
        evas_object_del(face->uptime);
        face->uptime = NULL;
     }

   if (face->mon->conf->time)
     {
        if (face->uptime)
           evas_object_del(face->uptime);

        time_t now;
        struct tm date;

        time(&now);
        char curr_time[12];

        date = *localtime(&now);
        face->time = edje_object_add(face->con->bg_evas);
        if (!e_theme_edje_object_set
            (face->time, "base/theme/modules/monitor", "modules/monitor/time"))
           edje_object_file_set(face->time, PACKAGE_DATA_DIR "/monitor.edj",
                                "modules/monitor/time");
        _add_sensor(face, face->time, face->mon->conf->Horz);
        sprintf(curr_time, "%02d:%02d:%02d", date.tm_hour, date.tm_min,
                date.tm_sec);
        edje_object_part_text_set(face->time, "time", curr_time);
     }
   else if (face->uptime)
     {
        evas_object_del(face->uptime);
        face->time = NULL;
     }

   if (face->monitor_cover_obj)
      evas_object_del(face->monitor_cover_obj);

   face->monitor_cover_obj = evas_object_rectangle_add(face->con->bg_evas);
   evas_object_color_set(face->monitor_cover_obj, 255, 255, 255, 0);
   evas_object_event_callback_add(face->monitor_cover_obj,
                                  EVAS_CALLBACK_MOUSE_DOWN,
                                  _monitor_face_cb_mouse_down, face);
   evas_object_show(face->monitor_cover_obj);

   int x, y, w, h;

   e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);

   evas_object_move(face->monitor_object, x, y);
   evas_object_resize(face->monitor_object, w, h);

   evas_object_move(face->table_object, x, y);
   evas_object_resize(face->table_object, w, h);

   evas_object_move(face->monitor_cover_obj, x, y);
   evas_object_resize(face->monitor_cover_obj, w, h);

   if (face->cpu)
      evas_object_geometry_get(face->cpu, &x, &y, &w, &h);
   if (face->cpu)
      chart_container_move(face->chart_cpu, x + 2, y + 2);
   if (face->cpu)
      chart_container_resize(face->chart_cpu, w - 4, h - 4);

   if (face->mem)
      evas_object_geometry_get(face->mem, &x, &y, &w, &h);
   if (face->mem)
      chart_container_move(face->chart_mem, x + 2, y + 2);
   if (face->mem)
      chart_container_resize(face->chart_mem, w - 4, h - 4);

   if (face->net)
      evas_object_geometry_get(face->net, &x, &y, &w, &h);
   if (face->net)
      chart_container_move(face->chart_net, x + 2, y + 2);
   if (face->net)
      chart_container_resize(face->chart_net, w - 4, h - 4);

   if (face->wlan)
      evas_object_geometry_get(face->wlan, &x, &y, &w, &h);
   if (face->wlan)
      chart_container_move(face->chart_wlan, x + 2, y + 2);
   if (face->wlan)
      chart_container_resize(face->chart_wlan, w - 4, h - 4);

   /* setup gadman
    * face->gmc = e_gadman_client_new(face->con->gadman);
    * e_gadman_client_domain_set(face->gmc, "module.monitor", _monitor_count++);
    * e_gadman_client_policy_set(face->gmc,
    * E_GADMAN_POLICY_ANYWHERE |
    * E_GADMAN_POLICY_HMOVE |
    * E_GADMAN_POLICY_VMOVE |
    * E_GADMAN_POLICY_HSIZE |
    * E_GADMAN_POLICY_VSIZE);
    * e_gadman_client_min_size_set(face->gmc, 14, 7);
    * e_gadman_client_align_set(face->gmc, 1.0, 1.0);
    * e_gadman_client_resize(face->gmc, 160, 40);
    * e_gadman_client_change_func_set(face->gmc, _monitor_face_cb_gmc_change, face);
    * e_gadman_client_load(face->gmc);
    */
   evas_event_thaw(face->con->bg_evas);
}

static int
_date_cb_check(void *data)
{
   Monitor_Face *face;

   face = data;

   //Update uptime
   char u_date_time[256];
   struct sysinfo s_info;

   sysinfo(&s_info);

   long minute = 60;
   long hour = minute * 60;
   long day = hour * 24;
   double megabyte = 1024 * 1024;

   sprintf(u_date_time, "uptime: %ld days, %ld:%02ld:%02ld",
           s_info.uptime / day, (s_info.uptime % day) / hour,
           (s_info.uptime % hour) / minute, s_info.uptime % minute);
   edje_object_part_text_set(face->uptime, "uptime", u_date_time);

   //Update time
   time_t now;
   struct tm date;

   time(&now);
   char curr_time[12];

   date = *localtime(&now);
   sprintf(curr_time, "%02d:%02d:%02d", date.tm_hour, date.tm_min, date.tm_sec);
   edje_object_part_text_set(face->time, "time", curr_time);

   return 1;
}
