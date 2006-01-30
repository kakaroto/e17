#include <e.h>
#include <E_Lib.h>
#include <Ecore.h>
#ifdef WANT_OSIRIS
# include <Ecore_File.h>
#endif
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

int idx, bg_id, bg_count;
static int slide_count;
Ecore_List *list;

static Slide *_slide_init(E_Module *m);
static void _slide_config_menu_new(Slide *e);
static void _slide_shutdown(Slide *e);

static int _slide_face_init(Slide_Face *sf);
static void _slide_face_free(Slide_Face *ef);
static void _slide_face_menu_new(Slide_Face *face);
static void _slide_face_enable(Slide_Face *face);
static void _slide_face_disable(Slide_Face *face);
static void _slide_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
static void _slide_face_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);

static void _slide_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,void *event_info);
static void _slide_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);

static int _slide_cb_check(void *data);

static void get_bg_count();
static void _set_bg(char *bg, Slide_Face *sf);

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
     "Slideshow"
};

EAPI void *
e_modapi_init(E_Module * m)
{
   Slide *e;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   /* actually init slide */
   e = _slide_init(m);
   m->config_menu = e->config_menu;

   if (!e->display)
     {
        char *tmp = getenv("DISPLAY");
        if (tmp) e->display = strdup(tmp);
     }

    /* make sure the display var is of the form name:0.0 or :0.0 */
   if (e->display)
     {
        char *p;
        char buf[1024];

        p = strrchr(e->display, ':');
        if (!p)
	  {
	     snprintf(buf, sizeof(buf), "%s:0.0", e->display);
	     free(e->display);
	     e->display = strdup(buf);
	  }
        else
	  {
	     p = strrchr(p, '.');
	     if (!p)
	       {
		  snprintf(buf, sizeof(buf), "%s.0", e->display);
		  free(e->display);
		  e->display = strdup(buf);
	       }
	  }
     }
   else
     e->display = strdup(":0.0");

   /* Init E Lib */
   if (e->display) e_lib_init(e->display);
   if (!e->display) e_lib_init(":0.0");

   return e;
}

EAPI int 
e_modapi_shutdown(E_Module *m)
{
   Slide *s;

   s = m->data;
   if (s)
     {
	if (m->config_menu)
	  {
	     e_menu_deactivate(m->config_menu);
	     e_object_del(E_OBJECT(m->config_menu));
	     m->config_menu = NULL;
	  }
	if (s->config_dialog) 
	  {
	     e_object_del(E_OBJECT(s->config_dialog));
	     s->config_dialog = NULL;
	  }
	_slide_shutdown(s);
     }

   e_lib_shutdown();
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m)
{
   Slide *e;

   e = m->data;
   if (e)
     e_config_domain_save("module.slideshow", e->conf_edd, e->conf);
   return 1;
}

EAPI int 
e_modapi_info(E_Module * m)
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

EAPI int 
e_modapi_about(E_Module * m)
{
   e_module_dialog_show(D_("Enlightenment Slide Show Module"),
			D_("This module is VERY simple and is used to cycle desktop backgrounds"));
   return 1;
}

EAPI int 
e_modapi_config(E_Module *m) 
{
   Slide *s;
   E_Container *con;
   
   s = m->data;
   if (!s) return 0;
   if (!s->face) return 0;
   
   con = e_container_current_get(e_manager_current_get());
   if (s->face->con == con) 
     _config_slideshow_module(con, s);
   return 1;
}

/* Begin Private Routines */

static Slide *
_slide_init(E_Module *m)
{
   Slide *e;
   E_Menu_Item *mi;
   Evas_List *managers, *l, *l2;

   e = E_NEW(Slide, 1);
   if (!e) return NULL;

   e->conf_edd = E_CONFIG_DD_NEW("Slide_Config", Config);
#undef T
#undef D
#define T Config
#define D e->conf_edd
#ifdef WANT_OSIRIS
   E_CONFIG_VAL(D, T, theme, STR);
#endif
   E_CONFIG_VAL(D, T, disable_timer, INT);
   E_CONFIG_VAL(D, T, cycle_time, DOUBLE);

   e->conf = e_config_domain_load("module.slideshow", e->conf_edd);
   if (!e->conf)
     {
	e->conf = E_NEW(Config, 1);
	#ifdef WANT_OSIRIS
	e->conf->theme = (char *)evas_stringshare_add("");
	#endif
	e->conf->disable_timer = 0;
	e->conf->cycle_time = 600;
     }

   E_CONFIG_LIMIT(e->conf->cycle_time, 5.0, 600.0);

   _slide_config_menu_new(e);

   /* Managers */
   managers = e_manager_list ();
   for (l = managers; l; l = l->next)
     {
	E_Manager *man;

	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;
	     Slide_Face *ef;

	     con = l2->data;
	     ef = E_NEW(Slide_Face, 1);
	     if (ef)
	       {
		  ef->conf_face_edd = E_CONFIG_DD_NEW("Slide_Config_Face", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D ef->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);

		  e->face = ef;
		  ef->slide = e;
		  ef->con = con;
		  ef->evas = con->bg_evas;

		  ef->conf = E_NEW(Config_Face, 1);
		  ef->conf->enabled = 1;

		  if (!_slide_face_init(ef)) return NULL;

		  /* Menu */
		  /* This menu must be initialized after conf */
		  _slide_face_menu_new(ef);

		  /* Add main menu to face menu */
		  mi = e_menu_item_new(e->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _slide_face_cb_menu_configure, ef);

		  mi = e_menu_item_new(e->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, ef->menu);

		  /* Setup */
		  if (!ef->conf->enabled)
		    {
		       _slide_face_disable(ef);
		    }
		  else
		    {
		       _slide_face_enable(ef);
		    }
	       }
	  }
     }
   return e;
}

static void 
_slide_shutdown(Slide *e)
{
   if (list) ecore_list_destroy(list);

   _slide_face_free(e->face);

   if (e->cycle_timer) 
     {
	e->cycle_timer = ecore_timer_del(e->cycle_timer);
	e->cycle_timer = NULL;
     }
   if (e->display) free(e->display);

   free(e->conf);
   E_CONFIG_DD_FREE(e->conf_edd);
   free(e);
}

static void 
_slide_config_menu_new(Slide * e)
{
   E_Menu *mn;

   mn = e_menu_new();
   e->config_menu = mn;
}

static int 
_slide_face_init(Slide_Face *sf)
{
   Evas_Object *o;
   char buff[4096];
   
   evas_event_freeze(sf->evas);
   o = edje_object_add(sf->evas);
   sf->slide_object = o;

   snprintf(buff, sizeof(buff), PACKAGE_DATA_DIR"/slideshow.edj");
   if (!e_theme_edje_object_set(o, "base/theme/modules/slideshow", "modules/slideshow/main"))
     edje_object_file_set(o, buff, "modules/slideshow/main");
   evas_object_show(o);

   o = evas_object_rectangle_add(sf->evas);
   sf->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,_slide_face_cb_mouse_down, sf);
   evas_object_show(o);

   sf->gmc = e_gadman_client_new(sf->con->gadman);
   e_gadman_client_domain_set(sf->gmc, "module.slideshow", slide_count++);
   e_gadman_client_policy_set(sf->gmc,E_GADMAN_POLICY_ANYWHERE | E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_VMOVE | E_GADMAN_POLICY_HSIZE | E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(sf->gmc, 4, 4);
   e_gadman_client_max_size_set(sf->gmc, 128, 128);
   e_gadman_client_auto_size_set(sf->gmc, 40, 40);
   e_gadman_client_align_set(sf->gmc, 1.0, 1.0);
   e_gadman_client_resize(sf->gmc, 40, 40);
   e_gadman_client_change_func_set(sf->gmc, _slide_face_cb_gmc_change, sf);
   e_gadman_client_load(sf->gmc);
   evas_event_thaw(sf->evas);

   return 1;
}

static void 
_slide_face_free(Slide_Face * ef)
{
   if (ef->menu) e_object_del(E_OBJECT(ef->menu));
   if (ef->event_object) evas_object_del(ef->event_object);
   if (ef->slide_object) evas_object_del(ef->slide_object);
   if (ef->gmc) e_gadman_client_save(ef->gmc);
   if (ef->gmc) e_object_del(E_OBJECT(ef->gmc));

   E_FREE(ef->conf);
   E_FREE(ef);
   slide_count--;
}

static void 
_slide_face_menu_new(Slide_Face * face)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   face->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _slide_face_cb_menu_configure, face);
   /* Edit */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _slide_face_cb_menu_edit, face);
}

static void 
_slide_face_enable(Slide_Face * face)
{
   face->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(face->slide_object);
   evas_object_show(face->event_object);
   if (!face->slide->conf->disable_timer) 
     {	
	if (face->slide->cycle_timer)
	  {
	     if (face->slide->conf->cycle_time != 0)
	       {
		  ecore_timer_interval_set(face->slide->cycle_timer, face->slide->conf->cycle_time);
	       }
	     else
	       {
		  face->slide->cycle_timer = ecore_timer_del(face->slide->cycle_timer);
		  face->slide->cycle_timer = NULL;
	       }
	  }
	else
	  {
	     face->slide->cycle_timer = ecore_timer_add(face->slide->conf->cycle_time, _slide_cb_check, face);
	  }
     }
   else 
     {
	if (face->slide->cycle_timer) 
	  {
	     face->slide->cycle_timer = ecore_timer_del(face->slide->cycle_timer);
	     face->slide->cycle_timer = NULL;
	  }
     }
}

static void 
_slide_face_disable(Slide_Face * face)
{
   face->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(face->slide_object);
   evas_object_hide(face->event_object);
   if (face->slide->cycle_timer) 
     {
	face->slide->cycle_timer = ecore_timer_del(face->slide->cycle_timer);
	face->slide->cycle_timer = NULL;
     }
}

static void
_slide_face_cb_gmc_change(void *data, E_Gadman_Client * gmc, E_Gadman_Change change)
{
   Slide_Face *ef;
   Evas_Coord x, y, w, h;

   ef = data;
   switch (change)
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(ef->gmc, &x, &y, &w, &h);
	evas_object_move(ef->slide_object, x, y);
	evas_object_move(ef->event_object, x, y);
	evas_object_resize(ef->slide_object, w, h);
	evas_object_resize(ef->event_object, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(ef->slide_object);
	evas_object_raise(ef->event_object);
	break;
      case E_GADMAN_CHANGE_ZONE:
	break;
      case E_GADMAN_CHANGE_EDGE:
	break;
     }
}

static void
_slide_face_cb_mouse_down(void *data, Evas * e, Evas_Object * obj,void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Slide_Face *ef;
   Slide *es;

   ev = event_info;
   ef = data;
   es = ef->slide;

   if (ev->button == 3)
     {
	E_Zone *zone;
	
	zone = e_zone_current_get(ef->con);
	e_menu_activate_mouse(ef->menu, zone, ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(ef->con);
     }
   else if (ev->button == 2)
     {
	if (es->conf->disable_timer) return;
	if (es->cycle_timer)
	  {
	     es->cycle_timer = ecore_timer_del(es->cycle_timer);
	     es->cycle_timer = NULL;
	  }
	else
	  {
	     es->cycle_timer = ecore_timer_add(es->conf->cycle_time, _slide_cb_check, ef);
	  }
     }
   else if (ev->button == 1)
     {
	_slide_cb_check(ef);
     }
}

static int 
_slide_cb_check(void *data)
{
   char *bg;
   Slide_Face *ef = data;
   
#ifdef WANT_OSIRIS
   Slide *e;   
   e = ef->slide;
   get_bg_count(e->conf->theme);
#else
   get_bg_count(NULL);
#endif

   if (!ef) return 0;
   if (!ef->conf) return 0;
   if (!ef->con) return 0;

   if (idx > bg_count) idx = 0;
   if (idx <= bg_count)
     {
	bg = ecore_list_goto_index(list, idx);
	if (bg == NULL)
	  {
	     idx = 0;
	     bg = ecore_list_goto_index(list, idx);
	  }
	if (bg != NULL)
	  {
	     _set_bg(bg, ef);
	     idx++;
	  }
     }
   if (ef->conf->enabled == 0) return 0;
   return 1;
}

static void 
_slide_face_cb_menu_edit(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Slide_Face *face;

   face = data;
   e_gadman_mode_set(face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
get_bg_count(char *name)
{
   char *list_item;
   char *home;
   char buffer[PATH_MAX];

   home = e_user_homedir_get();
#ifdef WANT_OSIRIS
   if (name == NULL)
     {
	snprintf(buffer, sizeof(buffer), "%s/.e/e/backgrounds", home);
     }
   else
     {
	snprintf(buffer, sizeof(buffer), "%s/.e/e/backgrounds/%s", home, name);
     }
#else
   snprintf(buffer, sizeof(buffer), "%s/.e/e/backgrounds", home);
#endif
   bg_count = 0;
   list = ecore_file_ls(buffer);
   ecore_list_goto_first(list);
   while ((list_item = (char *) ecore_list_next(list)) != NULL)
     bg_count++;
}

static void 
_set_bg(char *bg, Slide_Face *sf)
{
   char buffer[4096];
   char *home;
   
   home = e_user_homedir_get();

#ifdef WANT_OSIRIS
   Slide *e;
   e = sf->slide;
   if (e->conf->theme == NULL)
     {
	snprintf(buffer, sizeof(buffer), "%s/.e/e/backgrounds/%s", home, bg);
     }
   else
     {
	snprintf(buffer, sizeof(buffer), "%s/.e/e/backgrounds/%s/%s", home, e->conf->theme, bg);
     }
#else
   snprintf(buffer, sizeof(buffer), "%s/.e/e/backgrounds/%s", home, bg);
#endif

   if (buffer) e_lib_background_set(buffer);
}

static void 
_slide_face_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Slide_Face *sf;
   
   sf = data;
   if (!sf) return;
   _config_slideshow_module(sf->con, sf->slide);
}

void 
_slide_cb_config_updated(void *data) 
{
   Slide *s;
   
   s = data;
   if (s->conf->disable_timer == 1) 
     { 
	if (s->cycle_timer) 
	  {
	     s->cycle_timer = ecore_timer_del(s->cycle_timer);
	     s->cycle_timer = NULL;
	  }
	return;
     }
   
   if (s->conf->cycle_time == 0) 
     {
	if (s->cycle_timer) 
	  {
	     s->cycle_timer = ecore_timer_del(s->cycle_timer);
	     s->cycle_timer = NULL;
	  }
     }
   else 
     {
	if (s->cycle_timer) 
	  {
	     ecore_timer_interval_set(s->cycle_timer, s->conf->cycle_time);
	  }
	 else 
	  {
	     s->cycle_timer = ecore_timer_add(s->conf->cycle_time, _slide_cb_check, s->face);  
	  }	
     }
}
