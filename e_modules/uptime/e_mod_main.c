#include <e.h>
#include "e_mod_main.h"

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <utmp.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include <syslog.h>

#ifdef __linux__
# include <linux/kernel.h>
# include <linux/unistd.h>
# include <sys/sysinfo.h>
#endif

#ifdef __FreeBSD__
# include <paths.h>
# include <sys/tty.h>
# include <sys/sysctl.h>
# include <sys/param.h>
#endif

typedef struct _Instance Instance;
typedef struct _Uptime Uptime;

struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *ut_obj;
  Uptime *ut;
  Ecore_Timer *monitor;
  time_t uptime;
  time_t counter;
  int nusers;
  double la[3];
  Config_Item *ci;
};

struct _Uptime
{
  Instance *inst;
  Evas_Object *ut_obj;
};

static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);
static const char  *_gc_id_new (void);

static void _ut_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
			       void *event_info);
static void _ut_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi);
static void _ut_menu_cb_post (void *data, E_Menu * m);
static Config_Item *_ut_config_item_get (const char *id);
static Uptime *_ut_new (Evas * evas);
static void _ut_free (Uptime * ut);
static int _ut_cb_check (void *data);
static void update_counters (Instance * inst);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *ut_config = NULL;

static const E_Gadcon_Client_Class _gc_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "uptime", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
  E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
  Evas_Object *o;
  E_Gadcon_Client *gcc;
  Instance *inst;
  Uptime *ut;

  inst = E_NEW (Instance, 1);
  inst->ci = _ut_config_item_get (id);

#ifdef UPTIME_DEBUG
  syslog (LOG_EMERG, "ii id= %s, inst->ci->update_interval = %d, ci = %d\n",
	  inst->ci->id, inst->ci->update_interval, inst->ci->check_interval);
#endif

  ut = _ut_new (gc->evas);
  ut->inst = inst;
  inst->ut = ut;

  o = ut->ut_obj;
  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;
  inst->gcc = gcc;
  inst->ut_obj = o;

  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				  _ut_cb_mouse_down, inst);
  ut_config->instances = evas_list_append (ut_config->instances, inst);

  (void) update_counters (inst);
  inst->counter = 0;

  if (!inst->monitor)
    inst->monitor = ecore_timer_add (inst->ci->update_interval, _ut_cb_check, inst);

  return gcc;
}

static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
  Instance *inst;
  Uptime *ut;

  if (!gcc)
    return;
  if (!gcc->data)
    return;

  inst = gcc->data;
  ut = inst->ut;
  if (!ut)
    return;

  if (inst->monitor != NULL)
    ecore_timer_del (inst->monitor);

  ut_config->instances = evas_list_remove (ut_config->instances, inst);
  evas_object_event_callback_del (ut->ut_obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _ut_cb_mouse_down);

  _ut_free (ut);
  free (inst);
  inst = NULL;
}

static void
_gc_orient (E_Gadcon_Client * gcc)
{
  e_gadcon_client_aspect_set (gcc, 16, 16);
  e_gadcon_client_min_size_set (gcc, 16, 16);
}

static char *
_gc_label (void)
{
  return D_ ("Uptime");
}

static Evas_Object *
_gc_icon (Evas * evas)
{
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/module.edj",
	    e_module_dir_get (ut_config->module));
  edje_object_file_set (o, buf, "icon");

  return o;
}

static const char *
_gc_id_new (void)
{
   Config_Item *ci;

   ci = _ut_config_item_get (NULL);
   return ci->id;
}

static void
_ut_cb_mouse_down (void *data, Evas * e, Evas_Object * obj, void *event_info)
{
  Instance *inst;
  Evas_Event_Mouse_Down *ev;

  if (ut_config->menu != NULL)
    return;

  inst = data;
  ev = event_info;

  if (ev->button == 3)
    {
      E_Menu *mn;
      E_Menu_Item *mi;
      int x, y, w, h;

      mn = e_menu_new ();
      e_menu_post_deactivate_callback_set (mn, _ut_menu_cb_post, inst);
      ut_config->menu = mn;

      mi = e_menu_item_new (mn);
      e_menu_item_label_set (mi, D_ ("Configuration"));
      e_util_menu_item_edje_icon_set (mi, "enlightenment/configuration");
      e_menu_item_callback_set (mi, _ut_menu_cb_configure, inst);

      mi = e_menu_item_new (mn);
      e_menu_item_separator_set (mi, 1);

      e_gadcon_client_util_menu_items_append (inst->gcc, mn, 0);
      e_gadcon_canvas_zone_geometry_get (inst->gcc->gadcon, &x, &y, &w, &h);
      e_menu_activate_mouse (mn,
			     e_util_zone_current_get (e_manager_current_get
						      ()), x + ev->output.x,
			     y + ev->output.y, 1, 1,
			     E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
      evas_event_feed_mouse_up (inst->gcc->gadcon->evas, ev->button,
				EVAS_BUTTON_NONE, ev->timestamp, NULL);
    }
}

static void
_ut_menu_cb_post (void *data, E_Menu * m)
{
  if (ut_config->menu == NULL)
    return;
  e_object_del (E_OBJECT (ut_config->menu));
  ut_config->menu = NULL;
}

static void
_ut_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi)
{
  Instance *inst;

  inst = data;
  _config_ut_module (inst->ci);
}

static Config_Item *
_ut_config_item_get (const char *id)
{
   Evas_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (ut_config->items)
	  {
	     const char *p;
	     ci = evas_list_last (ut_config->items)->data;
	     p = strrchr (ci->id, '.');
	     if (p) num = atoi (p + 1) + 1;
	  }
	snprintf (buf, sizeof (buf), "%s.%d", _gc_class.name, num);
	id = buf;
     }
   else
     {
	for (l = ut_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if (ci->id == NULL)
	       continue;
	     if (strcmp (ci->id, id) == 0)
	       return ci;
	  }
     }

   ci = E_NEW (Config_Item, 1);
   ci->id = evas_stringshare_add (id);
   ci->check_interval = 60.0;
   ci->update_interval = 60.0;

   ut_config->items = evas_list_append (ut_config->items, ci);

   return ci;
}

void
_ut_config_updated (Config_Item *ci)
{
  Evas_List *l;

  if (ut_config == NULL)
    return;

  for (l = ut_config->instances; l; l = l->next)
    {
      Instance *inst;

      inst = l->data;
      if (inst->ci != ci) continue;
      if (inst->monitor != NULL)
	ecore_timer_del (inst->monitor);
      inst->monitor =
	      ecore_timer_add (ci->update_interval, _ut_cb_check, inst);
    }
}

EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Uptime"
};

EAPI void *
e_modapi_init (E_Module * m)
{
  char buf[4096];

  snprintf (buf, sizeof (buf), "%s/locale", e_module_dir_get (m));
  bindtextdomain (PACKAGE, buf);
  bind_textdomain_codeset (PACKAGE, "UTF-8");

  conf_item_edd = E_CONFIG_DD_NEW ("Uptime_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
  E_CONFIG_VAL (D, T, id, STR);
  E_CONFIG_VAL (D, T, check_interval, INT);
  E_CONFIG_VAL (D, T, update_interval, INT);

  conf_edd = E_CONFIG_DD_NEW ("Uptime_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
  E_CONFIG_LIST (D, T, items, conf_item_edd);

  ut_config = e_config_domain_load ("module.uptime", conf_edd);
  if (ut_config == NULL)
    {
      Config_Item *ci;

      ut_config = E_NEW (Config, 1);
      ci = E_NEW (Config_Item, 1);
      ci->id = evas_stringshare_add ("0");
      ci->check_interval = 60.0;
      ci->update_interval = 60.0;
      ut_config->items = evas_list_append (ut_config->items, ci);
    }
  ut_config->module = m;
  e_gadcon_provider_register (&_gc_class);

  return m;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  ut_config->module = NULL;
  e_gadcon_provider_unregister (&_gc_class);

  if (ut_config->config_dialog != NULL)
    e_object_del (E_OBJECT (ut_config->config_dialog));

  if (ut_config->menu)
    {
      e_menu_post_deactivate_callback_set (ut_config->menu, NULL, NULL);
      e_object_del (E_OBJECT (ut_config->menu));
      ut_config->menu = NULL;
    }
  while (ut_config->items)
    {
      Config_Item *ci;

      ci = ut_config->items->data;
      ut_config->items =
	evas_list_remove_list (ut_config->items, ut_config->items);

      if (ci->id != NULL)
	evas_stringshare_del (ci->id);

      free (ci);
      ci = NULL;
    }

  free (ut_config);
  ut_config = NULL;
  E_CONFIG_DD_FREE (conf_item_edd);
  E_CONFIG_DD_FREE (conf_edd);

  return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
  e_config_domain_save ("module.uptime", conf_edd, ut_config);
  return 1;
}

static Uptime *
_ut_new (Evas * evas)
{
  Uptime *ut;
  char buf[4096];

  ut = E_NEW (Uptime, 1);
  snprintf (buf, sizeof (buf), "%s/uptime.edj",
	    e_module_dir_get (ut_config->module));

  ut->ut_obj = edje_object_add (evas);
  if (!e_theme_edje_object_set
      (ut->ut_obj, "base/theme/modules/uptime", "modules/uptime/main"))
    edje_object_file_set (ut->ut_obj, buf, "modules/uptime/main");

  evas_object_show (ut->ut_obj);
  return ut;
}

static void
_ut_free (Uptime * ut)
{
  evas_object_del (ut->ut_obj);
  free (ut);
  ut = NULL;
}

static void
update_counters (Instance * inst)
{
  int ret;
#ifdef __linux__
  struct sysinfo s_info;
  struct utmp *utmpstruct;
#elif defined(__FreeBSD__)
  int mib[2];
  size_t size;
  time_t uptime;
  int days, hrs, i, mins, secs;
  struct timeval boottime;
  time_t now;

  FILE *utmp;
  struct timespec tp;
  struct utmp ut;
  char ttybuf[MAXPATHLEN];
  struct stat st;
#endif

  if (!inst)
    return;

#ifdef __linux__
  sysinfo (&s_info);
  inst->uptime = s_info.uptime;
  inst->la[0] = -1;
# ifdef HAVE_GETLOADAVG
  ret = getloadavg (inst->la, sizeof (inst->la) / sizeof (inst->la[0]));
  if (ret < 0)
    inst->la[0] = -1;
# endif
  inst->nusers = 0;
  setutent ();
  while ((utmpstruct = getutent ()))
    {
      if ((utmpstruct->ut_type == USER_PROCESS) &&
	  (utmpstruct->ut_name[0] != '\0'))
	inst->nusers++;
    }
  endutent ();
#elif defined(__FreeBSD__)
  /* retrive uptime info */
  (void) time (&now);
  mib[0] = CTL_KERN;
  mib[1] = KERN_BOOTTIME;
  size = sizeof (boottime);

  if (sysctl (mib, 2, &boottime, &size, NULL, 0) != -1 &&
      boottime.tv_sec != 0)
    {
      uptime = now - boottime.tv_sec;
      if (uptime > 60)
	uptime += 30;
      inst->uptime = uptime;
    }
  else
    inst->uptime = -1;

  /* retrive load averages */
  inst->la[0] = -1;
# ifdef HAVE_GETLOADAVG
  ret = getloadavg (inst->la, sizeof (inst->la) / sizeof (inst->la[0]));
  if (ret < 0)
    warn ("getloadavg()");
# endif

  /* retrive number of active users */
  utmp = fopen (_PATH_UTMP, "r");
  if (utmp == NULL)
    inst->nusers = -1;
  else
    {
      for (inst->nusers = 0; fread (&ut, sizeof (ut), 1, utmp);)
	{
	  if (ut.ut_name[0] == '\0')
	    continue;
	  (void) snprintf (ttybuf, sizeof (ttybuf), "%s%.*s", _PATH_DEV,
			   UT_LINESIZE, ut.ut_line);
	  if (stat (ttybuf, &st) != 0)
	    continue;		/* broken record */

	  inst->nusers++;
	}
      fclose (utmp);
    }
#else /* unsupported */
  inst->uptime = -1;
  inst->nusers = -1;
  inst->la[0] = -1;
#endif
}

static int
_ut_cb_check (void *data)
{
  Instance *inst;
  int days, hours, minutes;
  char u_date_time[256] = "up: N/A";
  char load_avg[256] = "la: N/A";
  char users[256] = "users: N/A";
  time_t uptime;

  inst = data;
  if (!inst)
    return 0;

#ifdef UPTIME_DEBUG
  syslog (LOG_EMERG, "counter = %d  update = %d\n", inst->counter,
	  inst->ci->check_interval);
#endif

  inst->counter += inst->ci->update_interval;
  if (inst->counter >= inst->ci->check_interval)
    {
      inst->counter = 0;
      (void) update_counters (inst);
    }

  uptime = inst->uptime + inst->counter;

  if (uptime != -1)
    {
      days = uptime / (24 * 3600);
      uptime %= 24 * 3600;

      hours = uptime / 3600;
      uptime %= 3600;

      minutes = uptime / 60;
      uptime %= 60;

      snprintf (u_date_time, sizeof (u_date_time),
		D_ ("up: %d days, %d:%02d:%02ld"),
		days, hours, minutes, uptime);
    }

  if (inst->la[0] != -1)
    {
      snprintf (load_avg, sizeof (load_avg),
		D_ ("la: %.2f %.2f %.2f"),
		inst->la[0], inst->la[1], inst->la[2]);
    }

  if (inst->nusers != -1)
    snprintf (users, sizeof (users), D_ ("users: %d"), inst->nusers);

  edje_object_part_text_set (inst->ut->ut_obj, "uptime", u_date_time);
  edje_object_part_text_set (inst->ut->ut_obj, "load_average", load_avg);
  edje_object_part_text_set (inst->ut->ut_obj, "nusers", users);

  return 1;
}
