#include <e.h>
#include "e_mod_main.h"
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
#include <libmpd/libmpdclient.h>

#define MAX_SONG_LENGTH 255

/* Func Proto Requirements for Gadcon */
static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);
static const char *_gc_id_new(void);

/* Module Protos */
static void _mpdule_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
				   void *event_info);
static void _mpdule_menu_cb_configure (void *data, E_Menu * m,
				       E_Menu_Item * mi);
static void _mpdule_menu_cb_post (void *data, E_Menu * m);
static int _mpdule_cb_check (void *data);
static Config_Item *_mpdule_config_item_get (const char *id);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mpdule_config = NULL;

/* Define the class and gadcon functions this module provides */
static const E_Gadcon_Client_Class _gc_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "mpdule", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
  E_GADCON_CLIENT_STYLE_PLAIN
};

typedef struct _Instance Instance;
struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *mpdule;
  mpd_Connection *mpd;
  Ecore_Timer *update_timer;
  Config_Item *ci;
};


static void _mpdule_connect (Instance * inst);
static void _mpdule_disconnect (Instance * inst);
static void _mpdule_update_song (Instance * inst);
static void _mpdule_update_song_cb (void *data);

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
  Evas_Object *o;
  E_Gadcon_Client *gcc;
  Instance *inst;
  char buf[4096];

  inst = E_NEW (Instance, 1);
	
  inst->ci = _mpdule_config_item_get (id);
  if (!inst->ci->id)
    inst->ci->id = evas_stringshare_add (id);

  o = edje_object_add (gc->evas);
  snprintf (buf, sizeof (buf), "%s/mpdule.edj",
	    e_module_dir_get (mpdule_config->module));
  if (!e_theme_edje_object_set
      (o, "base/theme/modules/mpdule", "modules/mpdule/main"))
    edje_object_file_set (o, buf, "modules/mpdule/main");
  evas_object_show (o);

  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;
  inst->gcc = gcc;
  inst->mpdule = o;

  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				  _mpdule_cb_mouse_down, inst);
  _mpdule_connect(inst);
  _mpdule_update_song(inst);
  inst->update_timer = ecore_timer_add((double)inst->ci->poll_time,
		  _mpdule_update_song_cb, inst);


  mpdule_config->instances =
    evas_list_append (mpdule_config->instances, inst);
  return gcc;
}

static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
  Instance *inst;

  inst = gcc->data;
  if (inst->update_timer)
  ecore_timer_del(inst->update_timer);
  _mpdule_disconnect(inst);
  mpdule_config->instances =
    evas_list_remove (mpdule_config->instances, inst);

  evas_object_event_callback_del (inst->mpdule, EVAS_CALLBACK_MOUSE_DOWN,
				  _mpdule_cb_mouse_down);

  evas_object_del (inst->mpdule);
  free (inst);
  inst = NULL;
}

static void
_gc_orient (E_Gadcon_Client * gcc)
{
  Instance *inst;
  Evas_Coord mw, mh;

  inst = gcc->data;
  edje_object_size_min_calc (inst->mpdule, &mw, &mh);
  e_gadcon_client_min_size_set (gcc, mw, mh);
}

static char *
_gc_label (void)
{
  return D_ ("MPDule");
}

static Evas_Object *
_gc_icon (Evas * evas)
{
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/e-module-mpdule.edj",
	    e_module_dir_get (mpdule_config->module));
  edje_object_file_set (o, buf, "icon");
  return o;
}

static const char *
_gc_id_new(void)
{
   Config_Item *ci;

   ci = _mpdule_config_item_get(NULL);
   return ci->id;
}

static void
_mpdule_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
		       void *event_info)
{
  Instance *inst;
  Evas_Event_Mouse_Down *ev;

  inst = data;
  ev = event_info;
  if ((ev->button == 3) && (!mpdule_config->menu))
    {
      E_Menu *mn;
      E_Menu_Item *mi;
      int x, y, w, h;

      mn = e_menu_new ();
      e_menu_post_deactivate_callback_set (mn, _mpdule_menu_cb_post, inst);
      mpdule_config->menu = mn;

      mi = e_menu_item_new (mn);
      e_menu_item_label_set (mi, D_ ("Configuration"));
      e_util_menu_item_edje_icon_set (mi, "enlightenment/configuration");
      e_menu_item_callback_set (mi, _mpdule_menu_cb_configure, inst);

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
_mpdule_menu_cb_post (void *data, E_Menu * m)
{
  if (!mpdule_config->menu)
    return;
  e_object_del (E_OBJECT (mpdule_config->menu));
  mpdule_config->menu = NULL;
}

static void
_mpdule_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi)
{
  Instance *inst;
	
  inst = data;
  _config_mpdule_module (inst->ci);
}

void
_mpdule_config_updated (Config_Item *ci)
{
  Evas_List *l;

  if (!mpdule_config)
    return;
  for (l = mpdule_config->instances; l; l = l->next)
    {
      Instance *inst;

      inst = l->data;
      if (!inst->ci != ci)
	continue;
      _mpdule_disconnect(inst);
      _mpdule_connect(inst);
      _mpdule_update_song(inst);
      if (inst->update_timer)
        ecore_timer_interval_set (inst->update_timer,
				      (double) ci->poll_time);
      else
        inst->update_timer =
          ecore_timer_add ((double) ci->poll_time, _mpdule_update_song_cb,
			       inst);
      break;
    }
}


static Config_Item *
_mpdule_config_item_get (const char *id)
{
  Evas_List *l;
  Config_Item *ci;
   char buf[128];


   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (mpdule_config->items)
	  {
	     const char *p;
	     ci = evas_list_last(mpdule_config->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
	id = buf;
     }
   else
     {
         for (l = mpdule_config->items; l; l = l->next)
          {
            ci = l->data;
            if (!ci->id)
	      continue;
            if (!strcmp (ci->id, id))
	      return ci;
          }
      }

  ci = E_NEW (Config_Item, 1);
  ci->id = evas_stringshare_add (id);
  ci->poll_time = 1.0;
  ci->hostname = evas_stringshare_add ("localhost");
  ci->port = 6600;

  mpdule_config->items = evas_list_append (mpdule_config->items, ci);
  return ci;
}

EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "MPDule"
};

static void
_mpdule_connect(Instance *inst)
{
	mpd_Connection *mpd;
        Config_Item *ci;

	ci = inst->ci;
	mpd = mpd_newConnection(ci->hostname, ci->port, 3.0);
	inst->mpd = mpd;
}

static void
_mpdule_disconnect(Instance *inst)
{
	if (inst->mpd) {
		mpd_closeConnection(inst->mpd);
		inst->mpd = NULL;
	}
}

static void
_mpdule_update_song_cb(void *data)
{
	Instance *inst;
	
	inst = data;
	_mpdule_update_song(inst);
	return 0;
}

static void
_mpdule_update_song(Instance *inst)
{
	mpd_Connection *mpd;
	Evas_Object *mpdule;
	
	if (!inst->mpd)
		return;
	mpd = inst->mpd;
	mpdule = inst->mpdule;
	mpd_sendStatusCommand(mpd);
	if (mpd->error == 0)
	{
		mpd_Status *status = mpd_getStatus(mpd);
		
		if (status)
		{
			if (status->state == MPD_STATUS_STATE_UNKNOWN)
			{
				edje_object_part_text_set (mpdule, "mpdule.status", D_ ("Unknown"));
			}
			else if (status->state == MPD_STATUS_STATE_STOP)
			{
				edje_object_part_text_set (mpdule, "mpdule.status", D_ ("Stopped"));
			}
			else if (status->state == MPD_STATUS_STATE_PLAY)
			{
				edje_object_part_text_set (mpdule, "mpdule.status", D_ ("Playing"));
			}
			else if (status->state == MPD_STATUS_STATE_PAUSE)
			{
				edje_object_part_text_set (mpdule, "mpdule.status", D_ ("Paused"));
			}

			if (status->state > MPD_STATUS_STATE_STOP)
			{
				mpd_sendCurrentSongCommand(mpd);
				mpd_InfoEntity *entity = NULL;
				
				while ( (entity = mpd_getNextInfoEntity(mpd)) )
				{
					if ( entity->type == MPD_INFO_ENTITY_TYPE_SONG &&
							entity->info.song->id == status->songid )
					{
						mpd_Song *song = entity->info.song;
						
						if (song->artist)
						{
							edje_object_part_text_set (mpdule, "mpdule.artist", song->artist);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.artist", "");
						}
						if (song->title)
						{
							edje_object_part_text_set (mpdule, "mpdule.title", song->title);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.title", "");
						}
						if (song->album)
						{
							edje_object_part_text_set (mpdule, "mpdule.album", song->album);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.album", "");
						}
						if (song->track)
						{
							edje_object_part_text_set (mpdule, "mpdule.track", song->track);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.track", "");
						}
						if (song->date)
						{
							edje_object_part_text_set (mpdule, "mpdule.date", song->date);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.date", "");
						}
						if (song->genre)
						{
							edje_object_part_text_set (mpdule, "mpdule.genre", song->genre);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.genre", "");
						}
						if (song->composer)
						{
							edje_object_part_text_set (mpdule, "mpdule.composer", song->composer);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.composer", "");
						}
						if (song->time)
						{
							char * songtime;
							sprintf(songtime, "%i", song->time);
							edje_object_part_text_set (mpdule, "mpdule.time", songtime);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.time", "");
						}
						if (song->file)
						{
							edje_object_part_text_set (mpdule, "mpdule.file", song->file);
							
						}
						else
						{
							edje_object_part_text_set (mpdule, "mpdule.file", "");
						}
					}
					
					mpd_freeInfoEntity(entity);
				}
			}
	
			mpd_freeStatus(status);
		}
	}
	else
	{
		_mpdule_disconnect(inst);
		_mpdule_connect(inst);
	}
}

EAPI void *
e_modapi_init (E_Module * m)
{
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");

  conf_item_edd = E_CONFIG_DD_NEW ("MPDule_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
  E_CONFIG_VAL (D, T, id, STR);
  E_CONFIG_VAL (D, T, poll_time, DOUBLE);
  E_CONFIG_VAL (D, T, hostname, STR);
  E_CONFIG_VAL (D, T, port, INT);

  conf_edd = E_CONFIG_DD_NEW ("MPDule_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
  E_CONFIG_LIST (D, T, items, conf_item_edd);

  mpdule_config = e_config_domain_load ("module.mpdule", conf_edd);
  if (!mpdule_config)
    {
      Config_Item *ci;

      mpdule_config = E_NEW (Config, 1);

      ci = E_NEW (Config_Item, 1);
      ci->id = evas_stringshare_add ("0");
      ci->poll_time = 1.0;
      ci->hostname = evas_stringshare_add ("localhost");
      ci->port = 6600;

      mpdule_config->items = evas_list_append (mpdule_config->items, ci);
    }
  mpdule_config->module = m;

  e_gadcon_provider_register (&_gc_class);
  return m;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  mpdule_config->module = NULL;
  e_gadcon_provider_unregister (&_gc_class);

  if (mpdule_config->config_dialog)
    e_object_del (E_OBJECT (mpdule_config->config_dialog));
  if (mpdule_config->menu)
    {
      e_menu_post_deactivate_callback_set (mpdule_config->menu, NULL, NULL);
      e_object_del (E_OBJECT (mpdule_config->menu));
      mpdule_config->menu = NULL;
    }

  while (mpdule_config->items)
    {
      Config_Item *ci;

      ci = mpdule_config->items->data;
      mpdule_config->items =
	evas_list_remove_list (mpdule_config->items, mpdule_config->items);
      if (ci->id)
	evas_stringshare_del (ci->id);
      free (ci);
      ci = NULL;
    }

  free (mpdule_config);
  mpdule_config = NULL;
  E_CONFIG_DD_FREE (conf_item_edd);
  E_CONFIG_DD_FREE (conf_edd);
  return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
  e_config_domain_save ("module.mpdule", conf_edd, mpdule_config);
  return 1;
}
