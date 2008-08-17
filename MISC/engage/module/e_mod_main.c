/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "math.h"
#include <unistd.h>

#include "config.h"


/* TODO List:
 *
 * Now managed online at: http://wiki.rectang.com/EngagePage
 *
 */

static int bar_count;
static E_Config_DD *conf_edd;
static E_Config_DD *conf_bar_edd;

/* const strings */
static const char *_engage_main_orientation[] =
{"left", "right", "top", "bottom"};

/* module private routines */
static Engage *_engage_new();
static void    _engage_free(Engage *e);
static void    _engage_app_change(void *data, E_App *a, E_App_Change ch);
static void    _engage_dotorder_app_add(Engage *e, const char *name);
static void    _engage_dotorder_app_del(Engage *e, const char *name);
static void    _engage_config_menu_new(Engage *e);

/* xdnd alpha code - this is a temp */
static int     _engage_cb_event_dnd_drop(void *data, int type, void *event);
static int     _engage_cb_event_dnd_position(void *data, int type, void *event);
static int     _engage_cb_event_dnd_selection(void *data, int type, void *event);

static int     _engage_cb_event_border_add(void *data, int type, void *event);
static int     _engage_cb_event_border_remove(void *data, int type, void *event);
static int     _engage_cb_event_border_iconify(void *data, int type, void *event);
static int     _engage_cb_event_border_uniconify(void *data, int type, void *event);
static int     _engage_cb_border_add(Engage_Bar *eb, E_Border *bd);

static void    _engage_cb_menu_clickfocus(void *data, E_Menu *m, E_Menu_Item *mi);

static Engage_Bar *_engage_bar_new(Engage_Bar *eb, E_Container *con);
static void    _engage_bar_free(Engage_Bar *eb);
static void    _engage_bar_menu_gen(Engage_Bar *eb);
static void    _engage_bar_enable(Engage_Bar *eb);
static void    _engage_bar_disable(Engage_Bar *eb);
void           _engage_bar_frame_resize(Engage_Bar *eb);
static void    _engage_bar_layout(Engage_Bar *eb);
static void    _engage_bar_edge_change(Engage_Bar *eb, int edge);
static void    _engage_bar_update_policy(Engage_Bar *eb);
static void    _engage_bar_motion_handle(Engage_Bar *eb, Evas_Coord mx, Evas_Coord my);
static void    _engage_bar_zoom_factor_set(Engage_Bar *eb, double zoom);
static void    _engage_bar_zoom_duration_set(Engage_Bar *eb, double duration);

static Engage_Icon *_engage_icon_new(Engage_Bar *eb, E_App *a);
static void    _engage_icon_free(Engage_Icon *ic);
static Engage_Icon *_engage_icon_find(Engage_Bar *eb, E_App *a);
static void    _engage_icon_reorder_after(Engage_Icon *ic, Engage_Icon *after);

static Engage_App_Icon *_engage_app_icon_new(Engage_Icon *ic, E_Border *bd, int min);
static void    _engage_app_icon_free(Engage_App_Icon *ai);
//static Engage_App_Icon *_engage_app_icon_find(Engage_Icon *ic, E_Border *bd);

static void    _engage_bar_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void    _engage_bar_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _engage_bar_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);

static void    _engage_bar_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_bar_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
//static void    _engage_bar_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_bar_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_bar_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void    _engage_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _engage_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void    _engage_icon_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_icon_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_icon_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_icon_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_icon_cb_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info);

#if 0
static void    _engage_icon_reorder_before(Engage_Icon *ic, Engage_Icon *before);
#endif
static void    _engage_app_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _engage_app_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void    _engage_app_icon_cb_intercept_show(void *data, Evas_Object *o);
static void    _engage_app_icon_cb_intercept_hide(void *data, Evas_Object *o);

static void    _engage_app_icon_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_app_icon_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _engage_app_icon_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);


static void    _engage_bar_cb_menu_enabled(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);

static void    _engage_bar_cb_menu_zoom(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_stretch(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_small(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_medium(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_large(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_huge(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_speed_fast(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_speed_normal(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_zoom_speed_slow(void *data, E_Menu *m, E_Menu_Item *mi);

static void    _engage_bar_cb_menu_tray(void *data, E_Menu *m, E_Menu_Item *mi);

static void    _engage_bar_cb_menu_edit_icon(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_keep_icon(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _engage_bar_cb_menu_remove_icon(void *data, E_Menu *m, E_Menu_Item *mi);

static int     _engage_zoom_function(double d, double *zoom, double *disp, Engage_Bar *eb);
static int     _engage_border_ignore(E_Border *bd);

static void    _engage_bar_cb_menu_context_change(void *data, E_Menu *m, E_Menu_Item *mi);

extern void    _engage_tray_init(Engage_Bar *eb);
extern void    _engage_tray_shutdown(Engage_Bar *eb);
extern void    _engage_tray_active_set(Engage_Bar *eb, int active);
extern void    _engage_tray_layout(Engage_Bar *eb);
extern void    _engage_tray_freeze(Engage_Bar *eb);
extern void    _engage_tray_thaw(Engage_Bar *eb);


static int      _engage_zoom_in_slave(void *data);
static int      _engage_zoom_out_slave(void *data);

E_App         *_engage_unmatched_app;

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi =
{
    E_MODULE_API_VERSION,
    "Engage"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   Engage *e;

   /* actually init engage */
   e = _engage_new();
//   m->config_menu = e->config_menu;
   e->module = m;
   return e;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Engage *e;

//   if (m->config_menu)
//     {
//	e_menu_deactivate(m->config_menu);
//	e_object_del(E_OBJECT(m->config_menu));
//	m->config_menu = NULL;
//     }

   e = m->data;
   if (e)
     _engage_free(e);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Engage *e;

   e = m->data;
   e_config_domain_save("module.engage", conf_edd, e->conf);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(m, _("Enlightenment Engage Module"),
		   _("This is the Engage module for Enlightenment.<br>"
		     "It is the native E17 version of engage.<br>"
		     "This version offers far greater features<br>"
		     "and will be the main focus of development from now on."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   Engage *e;
   Evas_List *l;
   E_Container *current;

   e = m->data;
   current = e_container_current_get(e_manager_current_get());
   for (l = e->bars; l; l = l->next)
     {
	Engage_Bar *eb;
	
	eb = l->data;
	if (eb->con == current)
	  {
	     _engage_module_config(current, eb);
	     break;
	  }	
     }
   return 1;
}

/* module private routines */
static Engage *
_engage_new()
{
   Engage *e;
   char buf[4096];
   Evas_List *managers, *l, *l2, *cl;

   bar_count = 0;
   e = E_NEW(Engage, 1);
   if (!e) return NULL;
   
   _engage_unmatched_app = e_app_new(PACKAGE_DATA_DIR "/icons/xapp.eap", 0);
   if (!_engage_unmatched_app)
     printf("ERROR: Engage could not locate default icon xapp.eap - please make install\n");

   conf_bar_edd = E_CONFIG_DD_NEW("Engage_Config_Bar", Config_Bar);
#undef T
#undef D
#define T Config_Bar
#define D conf_bar_edd
   E_CONFIG_VAL(D, T, enabled, INT);
   E_CONFIG_VAL(D, T, iconsize, INT);
   E_CONFIG_VAL(D, T, zoom, INT);
   E_CONFIG_VAL(D, T, zoom_factor, DOUBLE);
   E_CONFIG_VAL(D, T, zoom_duration, DOUBLE);
   E_CONFIG_VAL(D, T, zoom_stretch, INT);
   E_CONFIG_VAL(D, T, tray, INT);

   conf_edd = E_CONFIG_DD_NEW("Engage_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_VAL(D, T, appdir, STR);
   E_CONFIG_LIST(D, T, bars, conf_bar_edd);
   E_CONFIG_VAL(D, T, click_focus, UCHAR);
   /*
   E_CONFIG_VAL(D, T, handle, DOUBLE);
   E_CONFIG_VAL(D, T, autohide, UCHAR);
   */

   e->conf = e_config_domain_load("module.engage", conf_edd);
   if (!e->conf)
     {
	e->conf = E_NEW(Config, 1);
	e->conf->appdir = evas_stringshare_add("bar/engage");
	e->conf->click_focus = 1;
	/*
	e->conf->handle = 0.5;
	e->conf->autohide = 0;
	*/
     }
   /*
   E_CONFIG_LIMIT(e->conf->handle, 0.0, 1.0);
   E_CONFIG_LIMIT(e->conf->autohide, 0, 1);
   */

   _engage_config_menu_new(e);

   if (e->conf->appdir[0] != '/')
     {
	char *homedir;

	homedir = e_user_homedir_get();
	if (homedir)
	  {
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/%s", homedir, e->conf->appdir);
	     free(homedir);
	  }
     }
   else
     strcpy(buf, e->conf->appdir);

   e->apps = e_app_new(buf, 0);
   if (e->apps) e_app_subdir_scan(e->apps, 0);
   e_app_change_callback_add(_engage_app_change, e);

   managers = e_manager_list();
   cl = e->conf->bars;
   for (l = managers; l; l = l->next)
     {
	E_Manager *man;

	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;
	     Engage_Bar *eb;
	     /* Config */
	     con = l2->data;
	     eb = E_NEW(Engage_Bar, 1);
	     if (eb)
	       {
		  E_Menu_Item *mi;

		  eb->loaded = 0;
		  if (!cl)
		    {
		       eb->conf = E_NEW(Config_Bar, 1);
		       eb->conf->enabled = 1;
		       eb->conf->iconsize = 64;
		       eb->conf->zoom = 1;
		       eb->conf->zoom_factor = 2.0;
		       eb->conf->zoom_duration = 0.3;
		       eb->conf->zoom_stretch = 0;
		       eb->conf->tray = 1;
		       e->conf->bars = evas_list_append(e->conf->bars, eb->conf);
		    }
		  else
		    {
		       eb->conf = cl->data;
		       cl = cl->next;
		    }
		  E_CONFIG_LIMIT(eb->conf->iconsize, 2, 400);
		  E_CONFIG_LIMIT(eb->conf->zoom, 0, 1);
		  E_CONFIG_LIMIT(eb->conf->zoom_factor, 1.0, 4.0);
		  E_CONFIG_LIMIT(eb->conf->zoom_duration, 0.1, 0.5);
		  E_CONFIG_LIMIT(eb->conf->zoom_stretch, 0, 1);
		  E_CONFIG_LIMIT(eb->conf->tray, 0, 1);

		  eb->engage = e;
		  _engage_bar_new(eb, con);
		  /* Menu */
		  _engage_bar_menu_gen(eb);

		  /*add tray*/
		  _engage_tray_init(eb);

		  /* Add main menu to bar menu */
		  mi = e_menu_item_new(e->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, eb->menu);

		  /* include apps already running */
		  Evas_List *list;

		  for (list = e_border_focus_stack_get(); list; list = list->next)
		    {
		       _engage_cb_border_add(eb, list->data);
		    }

		  /* Setup */
		  if (!eb->conf->enabled)
		    _engage_bar_disable(eb);

		  /* We need to resize, the number of apps could have changed
		   *  since last startup */
		  eb->loaded = 1;
		  _engage_bar_frame_resize(eb);
	       }
	  }
     }
   return e;
}

static void
_engage_free(Engage *e)
{
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_bar_edd);

   while (e->bars)
     _engage_bar_free(e->bars->data);
   if (e->apps)
     e_object_unref(E_OBJECT(e->apps));

   if (e->conf->appdir) evas_stringshare_del(e->conf->appdir);
   e_app_change_callback_del(_engage_app_change, e);
   e_object_del(E_OBJECT(e->config_menu));
   evas_list_free(e->conf->bars);
   free(e->conf);
   free(e);
}

static void
_engage_app_change(void *data, E_App *a, E_App_Change ch)
{
   Engage *e;
   Evas_List *l, *ll;

   e = data;
   for (l = e->bars; l; l = l->next)
     {
	Engage_Bar *eb;

	eb = l->data;
	switch (ch)
	  {
	   case E_APP_ADD:
	     if (e_app_is_parent(e->apps, a))
	       {
		  Engage_Icon *ic, *ic2;

		  ic = _engage_icon_find(eb, a);
		  if (ic)
		    {
		       ic->dotorder = 1;
		    }
		  else
		    {
		       ic = _engage_icon_new(eb, a);
		       if (ic)
			 {
			    ic->dotorder = 1;
			    for (ll = e->apps->subapps; ll; ll = ll->next)
			      {
				 E_App *a2;

				 a2 = ll->data;
				 ic2 = _engage_icon_find(eb, a2);
				 if (ic2) _engage_icon_reorder_after(ic, ic2);
			      }
			 }
		       _engage_bar_frame_resize(eb);
		    }
	       }
	     break;
	   case E_APP_DEL:
	     if (e_app_is_parent(e->apps, a))
	       {
		  Engage_Icon *ic;

		  ic = _engage_icon_find(eb, a);
		  if (ic)
		    {
		       ic->dotorder = 0;
		       if (!ic->extra_icons)
		         _engage_icon_free(ic);
		    }
		  _engage_bar_frame_resize(eb);
	       }
	     break;
	   case E_APP_CHANGE:
	     if (e_app_is_parent(e->apps, a))
	       {
		  Engage_Icon *ic, *ic2;
		  Evas_List *extras = NULL;

		  ic = _engage_icon_find(eb, a);
		  if (ic)
		    {
		       extras = ic->extra_icons;
		       ic->extra_icons = NULL;
		       _engage_icon_free(ic);
		    }
		  evas_image_cache_flush(eb->evas);
		  evas_image_cache_reload(eb->evas);
		  ic = _engage_icon_new(eb, a);
		  if (ic)
		    {
		       ic->extra_icons = extras;
		       for (ll = e->apps->subapps; ll; ll = ll->next)
			 {
			    E_App *a2;

			    a2 = ll->data;
			    ic2 = _engage_icon_find(eb, a2);
			    if (ic2) _engage_icon_reorder_after(ic, ic2);
			 }
		       _engage_bar_frame_resize(eb);
		    }
	       }
	     break;
	   case E_APP_ORDER:
	     if (a == e->apps)
	       {
		  for (ll = e->apps->subapps; ll; ll = ll->next)
		    {
		       Engage_Icon *ic;
		       E_App *a2;

		       a2 = ll->data;
		       ic = _engage_icon_find(eb, a2);
		       if (ic) _engage_icon_reorder_after(ic, NULL);
		    }
		  Evas_List *nondots = NULL;
		  /* collect all ic that are not .order... */
		  for (ll = eb->icons; ll; ll = ll->next)
		    {
		       Engage_Icon *ic;

		       ic = ll->data;
		       if (ic->dotorder == 0) 
			 {
			    nondots = evas_list_append(nondots, ic);
			 }
		    }
		  /* ...and put them at the end*/
		  if (nondots) 
		    {
		       for (; nondots; nondots = nondots->next)
			 {
			    Engage_Icon *ic;

			    ic = nondots->data;
			    _engage_icon_reorder_after(ic, NULL);
			 }
		       evas_list_free(nondots);
		    }
		  _engage_bar_frame_resize(eb);
	       }
	     break;
	   case E_APP_EXEC:
	     break;
	   case E_APP_READY:
	     break;
	   case E_APP_EXIT:
	     break;
	   default:
	     break;
	  }
     }
}

static char*
_engage_dotorder_locate(Engage *e)
{
   char *homedir;
   char buf[4096];

   homedir = e_user_homedir_get();
   if (homedir)
     {
	snprintf(buf, sizeof(buf), "%s/.e/e/applications/%s/.order", homedir,
		 e->conf->appdir);
	free(homedir);
	return strdup(buf);
     }
   return NULL;
}

static void
_engage_dotorder_app_add(Engage *e, const char *name)
{
   FILE *f;
   char *dotorder;

   dotorder = _engage_dotorder_locate(e);
   if (!dotorder)
     return;
   f = fopen(dotorder, "ab");
   if (f)
     {
	fputs("\n", f);
	fputs(name, f);
	fclose(f);
     }
   free(dotorder);
}

static void
_engage_dotorder_app_del(Engage *e, const char *name)
{
   FILE *f;
   char buf[4096];
   char *buf_ptr;
   char *dotorder;
   Ecore_List *list = ecore_list_new();

   dotorder = _engage_dotorder_locate(e);
   if (!dotorder)
     return;

   if ((f = fopen(dotorder, "r+")) == NULL)
     return;

   while (fgets(buf, 4096, f))
     {
	if (strncmp(name, buf, strlen(name)) != 0)
	  ecore_list_append(list, strdup(buf));
     }
   fclose(f);
   ecore_list_first_goto(list);

   if ((f = fopen(dotorder, "w")) == NULL)
     return;

   while((buf_ptr = ecore_list_next(list)))
     {
	snprintf(buf, 4096, "%s", buf_ptr);
	fputs(buf_ptr, f);
	free(buf_ptr);
     }
   free(dotorder);
   fclose(f);
   ecore_list_destroy(list);

}

static Engage_Bar *
_engage_bar_new(Engage_Bar *eb, E_Container *con)
{
   Evas_List *l;
   Evas_Object *o;
   E_Gadman_Policy policy;

   eb->engage->bars = evas_list_append(eb->engage->bars, eb);

   eb->con = con;
   e_object_ref(E_OBJECT(con));
   eb->evas = con->bg_evas;
   
   eb->contexts = NULL;
   eb->tray = NULL;
   eb->selected_ic = NULL;
   
   eb->x = eb->y = eb->w = eb->h = -1;
   eb->zoom = 1.0;
   eb->zoom_start_time = 0.0;
   eb->cancel_zoom_in = 0;
   eb->cancel_zoom_out = 0;
   eb->state = ENGAGE_NORMAL;
   eb->mouse_out = -1;

   evas_event_freeze(eb->evas);
   o = edje_object_add(eb->evas);
   eb->bar_object = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/engage",
       "module/engage/main"))
     edje_object_file_set(o, PACKAGE_DATA_DIR "/themes/module.edj",
	 "module/engage/main");

   evas_object_show(o);
   edje_object_size_min_calc(o, &eb->bw, &eb->bh);

   o = evas_object_rectangle_add(eb->evas);
   eb->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,  _engage_bar_cb_mouse_in,  eb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _engage_bar_cb_mouse_out, eb);
   //evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _engage_bar_cb_mouse_down, eb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _engage_bar_cb_mouse_up, eb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _engage_bar_cb_mouse_move, eb);
   evas_object_show(o);

   o = evas_object_rectangle_add(eb->evas); 
   evas_object_repeat_events_set(o, 0);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_intercept_move_callback_add(o, _engage_bar_cb_intercept_move, eb);
   evas_object_intercept_resize_callback_add(o, _engage_bar_cb_intercept_resize,eb);
   eb->box_object = o;
   edje_object_part_swallow(eb->bar_object, "items", o);
   evas_object_show(o);

   if (eb->engage->apps)
     {
	for (l = eb->engage->apps->subapps; l; l = l->next)
	  {
	     E_App *a;
	     Engage_Icon *ic;

	     a = l->data;
	     ic = _engage_icon_new(eb, a);
	     if (ic)
	       ic->dotorder = 1;
	  }
     }

   eb->align_req = 0.5;
   eb->align = 0.5;

   eb->gmc = e_gadman_client_new(eb->con->gadman);
   e_gadman_client_domain_set(eb->gmc, "module.engage", bar_count++);
   policy = E_GADMAN_POLICY_EDGES | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;
   e_gadman_client_policy_set(eb->gmc, policy);
   e_gadman_client_min_size_set(eb->gmc, 16, 16);
   e_gadman_client_max_size_set(eb->gmc, 800, 136);
   e_gadman_client_auto_size_set(eb->gmc, -1, -1);
   e_gadman_client_align_set(eb->gmc, 0.5, 1.0);
   e_gadman_client_resize(eb->gmc, 400, 40);
   e_gadman_client_edge_set(eb->gmc, E_GADMAN_EDGE_BOTTOM);
   e_gadman_client_change_func_set(eb->gmc, _engage_bar_cb_gmc_change, eb);
   e_gadman_client_load(eb->gmc);
   /* update for appropriate bar we loaded on */
   _engage_bar_update_policy(eb);

   evas_event_thaw(eb->evas);

   /*
   edje_object_signal_emit(eb->bar_object, "passive", "");
   */


   eb->add_handler = ecore_event_handler_add(E_EVENT_BORDER_ADD,
	 _engage_cb_event_border_add, eb);
   eb->remove_handler = ecore_event_handler_add(E_EVENT_BORDER_REMOVE,
	 _engage_cb_event_border_remove, eb);
   eb->iconify_handler = ecore_event_handler_add(E_EVENT_BORDER_ICONIFY,
	 _engage_cb_event_border_iconify, eb);
   eb->uniconify_handler = ecore_event_handler_add(E_EVENT_BORDER_UNICONIFY,
	 _engage_cb_event_border_uniconify, eb);
   
   /* xdnd alpha code - this shouldnt hurt or break anything */
   //e_hints_window_visible_set(eb->con->bg_win);   
   ecore_x_dnd_aware_set(eb->con->bg_win, 1);
   ecore_event_handler_add(ECORE_X_EVENT_XDND_DROP, _engage_cb_event_dnd_drop , eb);
   ecore_event_handler_add(ECORE_X_EVENT_XDND_POSITION, _engage_cb_event_dnd_position, eb);
   ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, _engage_cb_event_dnd_selection, eb);

   /* search for available contexts to switch to */
   if(eb->engage->conf->appdir)
     {
	Ecore_List *cons = NULL;
	char buf[4096];
	char *homedir;
	char *dir;
	
	homedir = e_user_homedir_get();
	if (homedir)
	  {
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/%s", homedir,
		       eb->engage->conf->appdir);
	     free(homedir);
	  }	
	cons = ecore_file_ls(buf);
	while((dir = ecore_list_next(cons)))
	  {
	     char context[4096];
	     snprintf(context, sizeof(context), "%s/%s", buf, dir);
	     if(ecore_file_is_dir(context))
	       {
		  char dotorder[4096];
		  snprintf(dotorder, sizeof(dotorder), "%s/%s", context, ".order");
		  if(ecore_file_exists(dotorder))
		    {
		       eb->contexts = evas_list_append(eb->contexts, dir);
		    }
	       }
	  }
     }
   return eb;
}

static void
_engage_bar_free(Engage_Bar *eb)
{
   e_object_unref(E_OBJECT(eb->con));
   evas_object_intercept_move_callback_del(eb->box_object, _engage_bar_cb_intercept_move);
   evas_object_intercept_resize_callback_del(eb->box_object, _engage_bar_cb_intercept_resize);

   e_object_del(E_OBJECT(eb->zoom_size_menu));
   e_object_del(E_OBJECT(eb->zoom_speed_menu));
   e_object_del(E_OBJECT(eb->icon_menu));
   e_object_del(E_OBJECT(eb->menu));

   while (eb->icons)
     _engage_icon_free(eb->icons->data);

   evas_object_del(eb->bar_object);
   evas_object_del(eb->box_object);
   evas_object_del(eb->event_object);
	 
   _engage_tray_shutdown(eb);

   e_gadman_client_save(eb->gmc);
   e_object_del(E_OBJECT(eb->gmc));

   eb->engage->bars = evas_list_remove(eb->engage->bars, eb);

   ecore_event_handler_del(eb->add_handler);
   ecore_event_handler_del(eb->remove_handler);
   ecore_event_handler_del(eb->iconify_handler);
   ecore_event_handler_del(eb->uniconify_handler);

   if (eb->cfd)
     {
	e_object_del(E_OBJECT(eb->cfd));
	eb->cfd = NULL;
     }
   free(eb->conf);
   free(eb);
   bar_count--;

}

static void
_engage_bar_menu_gen(Engage_Bar *eb)
{
   E_Menu *mn;
   E_Menu_Item *mi;
   Evas_List *l;
   
   mn = e_menu_new();
   eb->zoom_size_menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Small");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_small, eb);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Medium");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_medium, eb);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Large");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_large, eb);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Huge");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_huge, eb);
   
   mn = e_menu_new();
   eb->zoom_speed_menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Fast");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_speed_fast, eb);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Normal");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_speed_normal, eb);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Slow");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_speed_slow, eb);

   mn = e_menu_new();
   eb->icon_menu = mn;

   if (eb->selected_ic)
     {
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, "Edit Icon");
	e_menu_item_callback_set(mi, _engage_bar_cb_menu_edit_icon, eb);

	if (!eb->selected_ic->dotorder)
	  {
	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, "Keep Icon");
	     e_menu_item_callback_set(mi, _engage_bar_cb_menu_keep_icon, eb);
	  }
	else
	  {
	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, "Remove Icon");
	     e_menu_item_callback_set(mi, _engage_bar_cb_menu_remove_icon, eb);
	  }
     }

   mn = e_menu_new();
   eb->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Zoom Icons");
   e_menu_item_check_set(mi, 1);
   if (eb->conf->zoom == 1)
     e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom, eb);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Zoom Level");
   e_menu_item_submenu_set(mi, eb->zoom_size_menu);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Zoom Speed");
   e_menu_item_submenu_set(mi, eb->zoom_speed_menu);
   
   mi = e_menu_item_new(mn); 
   e_menu_item_separator_set(mi, 1);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Stretch Bar");
   e_menu_item_check_set(mi, 1);
   if (eb->conf->zoom_stretch)
     e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_zoom_stretch, eb);

	 
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "System Tray");
   e_menu_item_check_set(mi, 1);
   if (eb->conf->tray)
     e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_tray, eb);

   mi = e_menu_item_new(mn); 
   e_menu_item_separator_set(mi, 1);
   
   if (eb->selected_ic && (eb->selected_ic->app != _engage_unmatched_app))
     {
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, "Icon Options");
	e_menu_item_submenu_set(mi, eb->icon_menu);
	if (eb->selected_ic)
	  e_menu_item_icon_edje_set(mi, eb->selected_ic->app->path, "icon");

	mi = e_menu_item_new(mn); 
	e_menu_item_separator_set(mi, 1);
     }
	 
   /* Enabled */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Enabled");
   e_menu_item_check_set(mi, 1);
   if (eb->conf->enabled) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_enabled, eb);

   /* Edit */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Edit Mode");
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_edit, eb);

   
   l = eb->contexts;
   if(l)
     {
	mi = e_menu_item_new(mn); 
	e_menu_item_separator_set(mi, 1);
	
	eb->context_menu = e_menu_new();   
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, "Context");
	e_menu_item_submenu_set(mi, eb->context_menu);      
	
	
	while(l)
	  {	   
	     char *context;
	     context = l->data;
	     if(context[0] == '.')
	       context = &context[1];
	     mi = e_menu_item_new(eb->context_menu);
	     e_menu_item_label_set(mi, context);
	     e_menu_item_callback_set(mi, _engage_bar_cb_menu_context_change, eb);
	     l = l->next;
	  }      
     }

   /* Configuration */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Configuration");
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _engage_bar_cb_menu_configure, eb);
}

static void
_engage_bar_enable(Engage_Bar *eb)
{
   eb->conf->enabled = 1;
   evas_object_show(eb->bar_object);
   evas_object_show(eb->box_object);
   evas_object_show(eb->event_object);
   _engage_tray_active_set(eb, eb->conf->tray);
   e_config_save_queue();
}

static void
_engage_bar_disable(Engage_Bar *eb)
{
   eb->conf->enabled = 0;
   _engage_tray_active_set(eb, 0);
   evas_object_hide(eb->bar_object);
   evas_object_hide(eb->box_object);
   evas_object_hide(eb->event_object);
   e_config_save_queue();
}

static Engage_Icon *
_engage_icon_new(Engage_Bar *eb, E_App *a)
{
   Engage_Icon *ic;
   Evas_Object *o;
   Evas_Coord size;

   ic = E_NEW(Engage_Icon, 1);
   if (!ic) return NULL;
   ic->eb = eb;
   ic->app = a;
   ic->scale = 1.0;
   ic->dotorder = 0;
   ic->selected_app = NULL;
   e_object_ref(E_OBJECT(a));
   eb->icons = evas_list_append(eb->icons, ic);

   o = evas_object_rectangle_add(eb->evas);
   ic->event_object = o;
   evas_object_layer_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_repeat_events_set(o, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,  _engage_icon_cb_mouse_in,  ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _engage_icon_cb_mouse_out, ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _engage_icon_cb_mouse_down, ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _engage_icon_cb_mouse_up, ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_WHEEL, _engage_icon_cb_mouse_wheel, ic);
   evas_object_show(o);

   size = eb->conf->iconsize;
   o = edje_object_add(eb->evas);
   ic->bg_object = o;
   edje_extern_object_min_size_set(o, size, size);
   edje_extern_object_max_size_set(o, size, size);
   evas_object_intercept_move_callback_add(o, _engage_icon_cb_intercept_move, ic);
   evas_object_intercept_resize_callback_add(o, _engage_icon_cb_intercept_resize, ic);
   if (!e_theme_edje_object_set(o, "base/theme/modules/engage",
       "module/engage/icon"))
     edje_object_file_set(o, PACKAGE_DATA_DIR "/themes/module.edj",
	 "module/engage/icon");
   evas_object_show(o);

   o = e_app_icon_add(eb->evas, a);
   ic->icon_object = o;
   edje_object_part_swallow(ic->bg_object, "item", o);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   evas_object_raise(ic->event_object);

   edje_object_signal_emit(ic->bg_object, "passive", "");
   if (eb->gmc)
     {
	edje_object_signal_emit(ic->bg_object, "set_orientation", 
	   _engage_main_orientation[e_gadman_client_edge_get(eb->gmc)]);
     }
   return ic;
}

static void
_engage_icon_free(Engage_Icon *ic)
{
   ic->eb->icons = evas_list_remove(ic->eb->icons, ic);
   if (ic->bg_object) evas_object_del(ic->bg_object);
   if (ic->icon_object) evas_object_del(ic->icon_object);
   if (ic->event_object) evas_object_del(ic->event_object);
   while (ic->extra_icons)
     {
	Engage_App_Icon *ai;

	ai = ic->extra_icons->data;
	ic->extra_icons = evas_list_remove_list(ic->extra_icons, ic->extra_icons);
	_engage_app_icon_free(ai);
     }
   e_object_unref(E_OBJECT(ic->app));
   free(ic);
}

static Engage_Icon *
_engage_icon_find(Engage_Bar *eb, E_App *a)
{
   Evas_List *l;

   if (!a)
     return NULL;
   for (l = eb->icons; l; l = l->next)
     {
	Engage_Icon *ic;

	ic = l->data;
	if (ic->app == a) return ic;
	if (((E_App *)ic->app)->orig == a) return ic;

/* FIXME - this could be slow - is there a reason we need to check the path -
 * why would the other two fail if we are referencing the same eap? */
	if (((E_App *) ic->app)->path && a->path &&
	    !strcmp(((E_App *)ic->app)->path, a->path)) return ic;
     }
   return NULL;
}

static Engage_App_Icon *
_engage_app_icon_new(Engage_Icon *ic, E_Border *bd, int min)
{
   Engage_App_Icon *ai;
   Evas_Object *o;

   if (_engage_border_ignore(bd))
     return NULL;
   ai = E_NEW(Engage_App_Icon, 1);
   if (!ai) return NULL;
   
   ai->ic = ic;
   ai->border = bd;
   e_object_ref(E_OBJECT(bd));
   ai->min = min?1:0;
   ic->extra_icons = evas_list_append(ic->extra_icons, ai);

   o = evas_object_rectangle_add(ic->eb->evas);
   ai->event_object = o;
   evas_object_layer_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_repeat_events_set(o, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,  _engage_app_icon_cb_mouse_in,  ai);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _engage_app_icon_cb_mouse_out, ai);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _engage_app_icon_cb_mouse_down, ai);
//   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _engage_app_icon_cb_mouse_up, ai);

   o = edje_object_add(ic->eb->evas);
   ai->bg_object = o;
   evas_object_intercept_move_callback_add(o, _engage_app_icon_cb_intercept_move, ai);
   evas_object_intercept_resize_callback_add(o, _engage_app_icon_cb_intercept_resize, ai);
   evas_object_intercept_show_callback_add(o, _engage_app_icon_cb_intercept_show, ai);
   evas_object_intercept_hide_callback_add(o, _engage_app_icon_cb_intercept_hide, ai);
   if (!e_theme_edje_object_set(o, "base/theme/modules/engage",
      "module/engage/icon"))
     edje_object_file_set(o, PACKAGE_DATA_DIR "/themes/module.edj",
	 "module/engage/icon");

   o = e_app_icon_add(ic->eb->evas, ic->app);
   ai->icon_object = o;
   edje_object_part_swallow(ai->bg_object, "item", o);
   evas_object_pass_events_set(o, 1);

   evas_object_raise(ai->event_object);
   evas_object_resize(ai->bg_object, ic->eb->conf->iconsize / 2, ic->eb->conf->iconsize / 2);

   edje_object_signal_emit(ai->bg_object, "passive", "");
   if (ai->min)
     {
	edje_object_signal_emit(ai->bg_object, "iconify", "");
     }
   return ai;
}

static void
_engage_app_icon_free(Engage_App_Icon *ai)
{
  // If this app_icon is selected_app, then try to make next the
   // selected_app, then the previous, or make it null if was only app.
   if(ai->ic->selected_app)
     {
       if(ai->ic->selected_app->data == ai)
	 {
	   if(ai->ic->selected_app->next)
	     ai->ic->selected_app = ai->ic->selected_app->next;
	   else if(ai->ic->selected_app->prev)
	     ai->ic->selected_app = ai->ic->selected_app->prev;
	   else
	     ai->ic->selected_app = NULL;
	 }
     }

   ai->ic->extra_icons = evas_list_remove(ai->ic->extra_icons, ai);
   if (ai->bg_object) evas_object_del(ai->bg_object);
   if (ai->icon_object) evas_object_del(ai->icon_object);
   if (ai->event_object) evas_object_del(ai->event_object);
   e_object_unref(E_OBJECT(ai->border));
   free(ai);
}

#if 0 
# not used yet, removed to stop warnings
static Engage_App_Icon *
_engage_app_icon_find(Engage_Icon *ic, E_Border *bd)
{
   Evas_List *l;

   for (l = ic->extra_icons; l; l = l->next)
     {
	Engage_App_Icon *ai;

	ai = l->data;
	if (ai->border == bd) return ai;
     }
   return NULL;
}
#endif

void
_engage_config_menu_new(Engage *e)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Focus borders on click");
   e_menu_item_check_set(mi, 1);
   if (e->conf->click_focus) e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _engage_cb_menu_clickfocus, e);
/*
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Auto hide");
   e_menu_item_check_set(mi, 1);
   if (e->conf->autohide == 0) e_menu_item_toggle_set(mi, 1);

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "More Options...");
*/

   e->config_menu = mn;
}

/* xdnd alpha code */
static int
_engage_cb_event_dnd_position(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Position *ev;
   Ecore_X_Rectangle rect;
   Engage_Bar *eb;

   eb = data;
   ev = event;
   e_gadman_client_geometry_get(eb->gmc, &rect.x, &rect.y, &rect.width, &rect.height);
   if (ev->position.x >= rect.x && ev->position.x <= rect.x + rect.width &&
       ev->position.y >= rect.y && ev->position.y <= rect.y + rect.height)
     ecore_x_dnd_send_status(1, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
   else
     ecore_x_dnd_send_status(0, 1, rect, ECORE_X_DND_ACTION_PRIVATE);
   return 1;
}

static int
_engage_cb_event_dnd_drop(void *data, int type, void *event)
{
   Ecore_X_Event_Xdnd_Drop *ev;

   ev = event;
   /* FIXME - we do not really want plain text files! */
   ecore_x_selection_xdnd_request(ev->win, "text/uri-list");
   return 1;
}

int
_engage_cb_event_dnd_selection(void *data, int type, void *event)
{
   Ecore_X_Event_Selection_Notify *ev;
   Ecore_X_Selection_Data_Files   *files;
   int i;
   Engage_Bar *eb;
   
   ev = event;
   eb = data;

   if (ev->selection != ECORE_X_SELECTION_XDND)
     return 1;
   files = ev->data;
   for (i = 0; i < files->num_files; i++)
     {
	char *path, *ext;
	const char *name;

	ext = strstr(files->files[i], ".eap");
	if (!ext)
	  continue;
	path = ecore_file_dir_get(files->files[i]);
	/* FIXME test here, we might need to copy it to .../all/ */
	name = ecore_file_file_get(files->files[i]);

	_engage_dotorder_app_add(eb->engage, name);
     }
   ecore_x_dnd_send_finished();
   return 1;
}

static int
_engage_cb_border_add(Engage_Bar *eb, E_Border *bd)
{
   Engage_Icon *ic;
   Engage_App_Icon *ai;
   E_App *app;
   const char *title;
   
   if (bd->zone->container != eb->con)
     return 1;
   
   /* border not fully evaluated - can't use that*/
   if (bd->new_client)
     return 1;

   if (_engage_border_ignore(bd))
     return 1;
   title = e_border_name_get(bd);
   app = e_app_window_name_class_title_role_find(bd->client.icccm.name,
						 bd->client.icccm.class,
						 title,
						 bd->client.icccm.window_role);
   if (!app)
     app = _engage_unmatched_app;
   ic = _engage_icon_find(eb, app);
   if (!ic)
     {
	ic = _engage_icon_new(eb, app);
	if (ic)
	  {
	     ic->dotorder = 0;
	  }
     }
   if (ic)
     {
	ai = _engage_app_icon_new(ic, bd, 0);
	if (ai && bd->iconic)
	  {
	     ai->min = 1;
	     edje_object_signal_emit(ai->bg_object, "iconify", "");
	  }				       
	edje_object_signal_emit(ic->bg_object, "running", "");
     }
   _engage_bar_frame_resize(eb);
   return 1;
}

static void
_engage_cb_menu_clickfocus(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage        *e;
   unsigned char  enabled;

   e = data;
   enabled = e_menu_item_toggle_get(mi);
   e->conf->click_focus = enabled;
   e_config_save_queue();
}

static int
_engage_cb_event_border_add(void *data, int type, void *event)
{
   Engage_Bar *eb;
   E_Event_Border_Add *e;

   e = event;
   eb = data;

   return _engage_cb_border_add(eb, e->border);
}

static int
_engage_cb_event_border_remove(void *data, int type, void *event)
{
   Engage_Bar *eb;
   Engage_Icon *ic;
   Engage_App_Icon *ai;
   E_Event_Border_Remove *e;
   E_App *app;
   Evas_List *icons;
   const char *title;

   e = event;
   eb = data;

   if (e->border->zone->container != eb->con)
     return 1;

   title = e_border_name_get(e->border);
   app = e_app_window_name_class_title_role_find(e->border->client.icccm.name,
						 e->border->client.icccm.class,
						 title,
						 e->border->client.icccm.window_role);
   if (!app)
     app = _engage_unmatched_app;
   ic = _engage_icon_find(eb, app);
   if (!ic)
     return 1;

   icons = ic->extra_icons;
   while (icons)
     {
	ai = icons->data;
	if (ai->border == e->border)
	  {
	      _engage_app_icon_free(ai);
	      if (!ic->extra_icons)
		{
		   if (ic->dotorder == 0)
		     {
			_engage_icon_free(ic);
			_engage_bar_frame_resize(eb);
		     }
		   else
		     edje_object_signal_emit(ic->bg_object, "notrunning", "");
		}
	      break;
	  }
	icons = icons->next;
     }
   return 1;
}

static int
_engage_cb_event_border_iconify(void *data, int type, void *event)
{
   Engage_Bar *eb;
   Engage_Icon *ic;
   Engage_App_Icon *ai;
   E_Event_Border_Hide *e;
   E_App *app;
   Evas_List *icons;
   const char *title;

   e = event;
   eb = data;
   if (e->border->zone->container != eb->con)
     return 1;

   title = e_border_name_get(e->border);
   app = e_app_window_name_class_title_role_find(e->border->client.icccm.name,
						 e->border->client.icccm.class,
						 title,
						 e->border->client.icccm.window_role);
   if (!app)
     app = _engage_unmatched_app;
   ic = _engage_icon_find(eb, app);
   if (!ic)
     {
	ic = _engage_icon_new(eb, app);
	if (ic)
	  {
	     ic->dotorder = 0;
	     _engage_bar_frame_resize(eb);
	  }
     }
   if (!ic)
     return 1;

   icons = ic->extra_icons;
   while (icons)
     {
	ai = icons->data;
	if (ai->border == e->border)
	  {
	     ai->min = 1;
	     edje_object_signal_emit(ai->bg_object, "iconify", "");
	     return 0;
	  }
	icons = icons->next;
     }
   return 1;
}

static int
_engage_cb_event_border_uniconify(void *data, int type, void *event)
{
   Engage_Bar *eb;
   Engage_Icon *ic;
   Engage_App_Icon *ai;
   E_Event_Border_Show *e;
   E_App *app;
   Evas_List *icons;
   const char *title;

   e = event;
   eb = data;

   if (e->border->zone->container != eb->con)
     return 1;

   title = e_border_name_get(e->border);
   app = e_app_window_name_class_title_role_find(e->border->client.icccm.name,
						 e->border->client.icccm.class,
						 title,
						 e->border->client.icccm.window_role);
   if (!app)
     app = _engage_unmatched_app;
   ic = _engage_icon_find(eb, app);
   if (!ic)
     return 1;

   icons = ic->extra_icons;
   while (icons)
     {
	ai = icons->data;
	if (ai->min && ai->border == e->border)
	  {
	      ai->min = 0;
	      edje_object_signal_emit(ai->bg_object, "uniconify", "");
	      return 1;
	  }
	icons = icons->next;
     }
   return 1;
}


#if 0
static void
_engage_icon_reorder_before(Engage_Icon *ic, Engage_Icon *before)
{
   Evas_Coord bw, bh;

   ic->eb->icons = evas_list_remove(ic->eb->icons, ic);
   if (before)
     ic->eb->icons = evas_list_prepend_relative(ic->eb->icons, ic, before);
   else
     ic->eb->icons = evas_list_prepend(ic->eb->icons, ic);
   edje_object_size_min_calc(ic->bg_object, &bw, &bh);
}
#endif

static void
_engage_icon_reorder_after(Engage_Icon *ic, Engage_Icon *after)
{
   Evas_Coord bw, bh;

   ic->eb->icons = evas_list_remove(ic->eb->icons, ic);
   if (after)
     ic->eb->icons = evas_list_append_relative(ic->eb->icons, ic, after);
   else
     ic->eb->icons = evas_list_append(ic->eb->icons, ic);
   edje_object_size_min_calc(ic->bg_object, &bw, &bh);
}

static void
_engage_bar_layout(Engage_Bar *eb)
{
   Evas_Coord x, y;
   Evas_List *l;
   int edge;

   if (!eb->gmc)
     return;
   edge = e_gadman_client_edge_get(eb->gmc);
   evas_object_geometry_get(eb->box_object, &x, &y, NULL, NULL);
   for (l = eb->icons; l; l = l->next)
     {
	Engage_Icon *ic;
	
	ic = l->data;
	evas_object_resize(ic->bg_object, eb->conf->iconsize, eb->conf->iconsize);
	evas_object_move(ic->bg_object, x, y);
	if ((edge == E_GADMAN_EDGE_LEFT) || (edge == E_GADMAN_EDGE_RIGHT))
	  y += eb->conf->iconsize;
	else
	  x += eb->conf->iconsize;	
     }
}

void
_engage_bar_frame_resize(Engage_Bar *eb)
{
   Evas_Coord w, h;
   int edge;

   /* Not finished loading config yet! */
   if (!eb->loaded)
     return;

   evas_event_freeze(eb->evas);

   edge = e_gadman_client_edge_get(eb->gmc);
   if ((edge == E_GADMAN_EDGE_LEFT) || (edge == E_GADMAN_EDGE_RIGHT))
     {
	w = eb->conf->iconsize;
	h = evas_list_count(eb->icons) * eb->conf->iconsize;
     }
   else
     {
	w = evas_list_count(eb->icons) * eb->conf->iconsize;
	h = eb->conf->iconsize;
     }

   _engage_tray_freeze(eb);
   edje_object_part_unswallow(eb->bar_object, eb->box_object);
   _engage_tray_thaw(eb);
   edje_extern_object_min_size_set(eb->box_object, w, h);
   edje_extern_object_max_size_set(eb->box_object, w, h);
   edje_object_part_swallow(eb->bar_object, "items", eb->box_object);
   _engage_tray_layout(eb);

   edje_object_size_min_calc(eb->bar_object, &w, &h);
   e_gadman_client_resize(eb->gmc, w, h);
   _engage_bar_layout(eb);

   evas_event_thaw(eb->evas);
}

static void
_engage_bar_edge_change(Engage_Bar *eb, int edge)
{
   Evas_List *l;
   Evas_Coord bw, bh, tmp;
   Evas_Object *o;
   E_Gadman_Policy policy;

   evas_event_freeze(eb->evas);
   o = eb->bar_object;
   edje_object_signal_emit(o, "set_orientation", _engage_main_orientation[edge]);
   edje_object_message_signal_process(o);

   _engage_tray_freeze(eb);
   if (eb->tray)
     edje_object_part_unswallow(eb->bar_object, eb->tray->tray);
   edje_object_part_unswallow(eb->bar_object, eb->box_object);
   edje_object_size_min_calc(o, &eb->bw, &eb->bh);
   edje_object_part_swallow(eb->bar_object, "items", eb->box_object);
   if (eb->tray)
     edje_object_part_swallow(eb->bar_object, "tray", eb->tray->tray);
   _engage_tray_thaw(eb);
   _engage_tray_layout(eb);

   for (l = eb->icons; l; l = l->next)
     {
	Engage_Icon *ic;

	ic = l->data;
	o = ic->bg_object;
	edje_object_signal_emit(o, "set_orientation", _engage_main_orientation[edge]);
	edje_object_message_signal_process(o);
	edje_object_size_min_calc(ic->bg_object, &bw, &bh);
     }

   eb->align_req = 0.5;
   eb->align = 0.5;

   policy = E_GADMAN_POLICY_EDGES | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;
   if ((edge == E_GADMAN_EDGE_BOTTOM) ||
       (edge == E_GADMAN_EDGE_TOP))
     {
	policy |= E_GADMAN_POLICY_VSIZE;
	e_gadman_client_policy_set(eb->gmc, policy);
	tmp = eb->w;
	eb->w = eb->h;
	eb->h = tmp;

	_engage_bar_frame_resize(eb);
     }
   else if ((edge == E_GADMAN_EDGE_LEFT) ||
	    (edge == E_GADMAN_EDGE_RIGHT))
     {
	policy |= E_GADMAN_POLICY_HSIZE;
	e_gadman_client_policy_set(eb->gmc, policy);
	tmp = eb->w;
	eb->w = eb->h;
	eb->h = tmp;

	_engage_bar_frame_resize(eb);
     }

   evas_event_thaw(eb->evas);

   _engage_bar_frame_resize(eb);
}

static void
_engage_bar_update_policy(Engage_Bar *eb)
{
   E_Gadman_Policy policy;

   policy = E_GADMAN_POLICY_EDGES | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;
   if ((e_gadman_client_edge_get(eb->gmc) == E_GADMAN_EDGE_BOTTOM) ||
       (e_gadman_client_edge_get(eb->gmc) == E_GADMAN_EDGE_TOP))
     {
	policy |= E_GADMAN_POLICY_VSIZE;
	e_gadman_client_policy_set(eb->gmc, policy);
     }
   else if ((e_gadman_client_edge_get(eb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(eb->gmc) == E_GADMAN_EDGE_RIGHT))
     {
	policy |= E_GADMAN_POLICY_HSIZE;
	e_gadman_client_policy_set(eb->gmc, policy);
     }
}

static void
_engage_bar_motion_handle(Engage_Bar *eb, Evas_Coord mx, Evas_Coord my)
{
   Evas_Coord x, y, w, h, md, md2, xx, yy, app_size, halfapp_size;
   double relx, rely;
   Evas_List *items, *extras;
   int counter;
   Engage_Icon *prev;
   E_Gadman_Edge edge;
              
   evas_object_geometry_get(eb->event_object, &x, &y, &w, &h);
   if (mx < x)
     mx = x;
   if (mx > x + w)
     mx = x + w;
   if (my < y)
     my = y;
   if (my > y + h)
     my = y + y;

   evas_object_geometry_get(eb->box_object, &x, &y, &w, &h);
   if (w > 0) relx = (double)(mx - x) / (double)w;
   else relx = 0.0;
   if (h > 0) rely = (double)(my - y) / (double)h;
   else rely = 0.0;
   edge = e_gadman_client_edge_get(eb->gmc);
   if ((edge == E_GADMAN_EDGE_LEFT) || (edge == E_GADMAN_EDGE_RIGHT))
     eb->align_req = 1.0 - rely;
   else
     eb->align_req = 1.0 - relx;

   if (edge == E_GADMAN_EDGE_LEFT || edge == E_GADMAN_EDGE_RIGHT)
     {
	md = my;
	if (edge == E_GADMAN_EDGE_LEFT)
	  md2 = mx;
	else
	  md2 = x + w - mx;
	counter = y;
     }
   else
     {
	md = mx;
	if (edge == E_GADMAN_EDGE_TOP)
	  md2 = my;
	else
	  md2 = y + h - my;
	counter = x;
     }
   app_size = eb->conf->iconsize / 1.5;
   halfapp_size = app_size / 2;
   counter += (eb->conf->iconsize / 2) + 1;
   items = eb->icons;
   while (items)
     {
	Engage_Icon *icon;
	double       distance, new_zoom, relative, size, halfsize;
	int          do_zoom, offset;
	Evas_Coord   cx, cy;

	relative = 0; // compiler warning supression
	icon = (Engage_Icon *) items->data;
	if (eb->mouse_out != -1)
	  distance = (double) (counter - eb->mouse_out) / (eb->conf->iconsize);
	else
	  distance = (double) (counter - md) / (eb->conf->iconsize);

	do_zoom = _engage_zoom_function(distance, &new_zoom, &relative, eb);
	size = icon->scale * new_zoom * eb->conf->iconsize;
	halfsize = size / 2;

	evas_object_image_fill_set(icon->icon_object, 0.0, 0.0, size, size);
	evas_object_resize(icon->bg_object, size, size);

	xx = x;
	yy = y;

	if (eb->conf->zoom_stretch)
	  {
	     if (eb->mouse_out == -1)
	       offset = md + relative;
	     else
	       offset = eb->mouse_out + relative;
	  }
	else
	  {
	     offset = counter;
	  }

	if (edge == E_GADMAN_EDGE_LEFT)
	  yy = offset - halfsize;
	else if (edge == E_GADMAN_EDGE_RIGHT)
	  {
	     xx = x + w - size;
	     yy = offset - halfsize;
	  }
	else if (edge == E_GADMAN_EDGE_TOP)
	  xx = offset - halfsize;
	else
	  {
	     xx = offset - halfsize;
	     yy = y + h - size;
	  }
	evas_object_move(icon->bg_object, xx, yy);
	cx = xx + halfsize;
	cy = yy + halfsize;

	if (do_zoom && -0.5 < distance && distance < 0.5)
	  {
	     double radius, arc, theta;
	     if (md2 > size)
	       {
		  /* only freeze if we can choose subicons */
		  if ((eb->mouse_out == -1) && icon->extra_icons)
		    eb->mouse_out = md;
	       }
	     else
	       /* give us more space to choose subicons */
	       if (md2 <= size - halfapp_size)
		 eb->mouse_out = -1;

	     radius = sqrt(2) * new_zoom / 2;
	     if (edge == E_GADMAN_EDGE_LEFT || edge == E_GADMAN_EDGE_RIGHT)
	       radius *= w;
	     else
	       radius *= h;

	     evas_object_raise(icon->bg_object);
	     evas_object_show(icon->event_object);

	     if (evas_list_count(icon->extra_icons) == 0)
	       {
		  // nothing
	       }
	     else if (evas_list_count(icon->extra_icons) == 1)
	       {
		  Engage_App_Icon *ai;

		  ai = icon->extra_icons->data;
		  evas_object_resize(ai->bg_object, app_size, app_size);
		  if (edge == E_GADMAN_EDGE_LEFT)
		    evas_object_move(ai->bg_object, cx + radius - halfapp_size, cy - halfapp_size);
		  else if (edge == E_GADMAN_EDGE_RIGHT)
		    evas_object_move(ai->bg_object, cx - radius - halfapp_size, cy - halfapp_size);
		  else if (edge == E_GADMAN_EDGE_TOP)
		    evas_object_move(ai->bg_object, cx - halfapp_size, cy + radius - halfapp_size);
		  else
		    evas_object_move(ai->bg_object, cx - halfapp_size, cy - radius - halfapp_size);
		  evas_object_show(ai->bg_object);
	       }
	     else
	       {
		  int i = 0, selected_pos = 0, app_cnt;

		  app_cnt = evas_list_count(icon->extra_icons);

		  extras = icon->extra_icons;

		  // if first time through, then set to first app
		  if(!icon->selected_app)
		    icon->selected_app = icon->extra_icons;

		  // get the position we are currently in
		  while(extras)
		    {
		      if(icon->selected_app == extras)
			break;

		      selected_pos++;
		      extras = extras->next;
		    }

		  // divide the 120 degrees of a circle into the proper number of
		  // divisions to make it possible to rotate the app icons around
		  // in their 60 degree arc
		  arc = ((2 * M_PI) / 3) / (app_cnt * 2 - 2);
		  theta = (-2 * M_PI) / 6;

		  // start at back of list so that the app_icon for the 
		  // first item will be placed directly above the main engage icon
		  extras = evas_list_last(icon->extra_icons);

		  // loop through twice as many times (-1) times the number of apps
		  // so that we have the proper number of places to put icons
		  while(i < app_cnt * 2 - 1)
		    {
		       Engage_App_Icon *ai;
		       Evas_Coord x, y, ax, ay;

		       // if we've placed all the icons, then break
		       if(!extras)
			 break;

		       ai = extras->data;
		       x = radius * sin(theta);
		       y = radius * cos(theta);

		       evas_object_resize(ai->bg_object, app_size, app_size);

		       if (edge == E_GADMAN_EDGE_LEFT)
			 {
			   ax = cx + y - halfapp_size;
			   ay = cy + x - halfapp_size;
			 }
		       else if (edge == E_GADMAN_EDGE_RIGHT)
			 {
			   ax = cx - y - halfapp_size;
			   ay = cy - x - halfapp_size;
			 }	     
		       else if (edge == E_GADMAN_EDGE_TOP)
			 {
			   ax = cx + x - halfapp_size;
			   ay = cy + y - halfapp_size;
			 }
		       else
			 {
			   ax = cx - x - halfapp_size;
			   ay = cy - y - halfapp_size;
			 }
			 
		       // if we are currently in a position that is above the number of
		       // the currently selected app icon but is not past the slots where 
		       // we place the app icons
		       if(i >= selected_pos && i <= app_cnt + selected_pos)
			 {
			   evas_object_move(ai->bg_object, ax, ay);
			   evas_object_show(ai->bg_object);
			       
			   // raise the app icon we are on above the others
			   if(x == 0)
			     evas_object_layer_set(ai->bg_object, 1);
			   else
			     evas_object_layer_set(ai->bg_object, 0);

			   extras = extras->prev;
			 }		       
 		       theta += arc;
		       i++;
		    }
	       }
	  }
	else
	  {
	     evas_object_hide(icon->event_object);
	     for (extras = icon->extra_icons; extras; extras = extras->next)
	       {
		   Engage_App_Icon *ai;

		   ai = extras->data;
		   evas_object_hide(ai->bg_object);
	       }
	  }

	prev = icon;
	items = items->next;
	counter += eb->conf->iconsize;
     }

}

static void
_engage_bar_zoom_factor_set(Engage_Bar *eb, double zoom)
{
   eb->conf->zoom_factor = zoom;
   e_config_save_queue();
}

static void
_engage_bar_zoom_duration_set(Engage_Bar *eb, double duration)
{
   eb->conf->zoom_duration = duration;
   e_config_save_queue();
}

static void
_engage_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Engage_Icon *ic;

   ic = data;
   evas_object_move(o, x, y);
   evas_object_move(ic->event_object, x, y);
}

static void
_engage_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Engage_Icon *ic;

   ic = data;
   evas_object_resize(o, w, h);
   evas_object_resize(ic->event_object, w, h);
}

static void
_engage_app_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Engage_App_Icon *ai;

   ai = data;
   evas_object_move(o, x, y);
   evas_object_move(ai->event_object, x, y);
}

static void
_engage_app_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Engage_App_Icon *ai;

   ai = data;
   evas_object_resize(o, w, h);
   evas_object_resize(ai->event_object, w, h);
}

static void
_engage_app_icon_cb_intercept_show(void *data, Evas_Object *o)
{
   Engage_App_Icon *ai;

   ai = data;
   evas_object_show(o);
   evas_object_show(ai->event_object);
}

static void
_engage_app_icon_cb_intercept_hide(void *data, Evas_Object *o)
{
   Engage_App_Icon *ai;

   ai = data;
   evas_object_hide(o);
   evas_object_hide(ai->event_object);
}

static void
_engage_bar_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Engage_Bar *eb;

   eb = data;
   evas_object_move(o, x, y);
   evas_object_move(eb->event_object, x, y);
   _engage_bar_layout(eb);
}

static void
_engage_bar_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Engage_Bar *eb;
   E_Gadman_Edge edge;

   eb = data;
   if (w <= eb->bw || h <= eb->bh)
     return;

   evas_object_resize(o, w, h);
   evas_object_resize(eb->event_object, w, h);
   edje_extern_object_min_size_set(eb->box_object, w, h);
   
   if (eb->gmc)
     edge = e_gadman_client_edge_get(eb->gmc);
   else
     edge = E_GADMAN_EDGE_BOTTOM;

   _engage_bar_frame_resize(eb);
}

static void
_engage_app_icon_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Engage_App_Icon *ai;

   ev = event_info;
   ai = data;
   if (ev->button == 1)
     {
	edje_object_signal_emit(ai->bg_object, "start", "");
	if (ai->min)
	  e_border_uniconify(ai->border);
	e_border_raise(ai->border);
	e_desk_show(ai->border->desk);
	if (ai->ic->eb->engage->conf->click_focus)
	  e_border_focus_set(ai->border, 1, 1);
     }
   if (ev->button == 3)
     {
	e_int_border_menu_show(ai->border,
			       ev->output.x,
			       ev->output.y, 0,
			       ev->timestamp);
	e_util_container_fake_mouse_up_all_later(ai->ic->eb->con);
     }
}

static void
_engage_app_icon_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Engage_App_Icon *ai;
   const char *title;

   ev = event_info;
   ai = data;
   
   title = e_border_name_get(ai->border);
   if (title)
     edje_object_part_text_set(ai->ic->bg_object, "EngageIconText", title);
   edje_object_signal_emit(ai->ic->bg_object, "active", "");
}

static void
_engage_app_icon_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Engage_App_Icon *ai;

   ev = event_info;
   ai = data;
   edje_object_signal_emit(ai->ic->bg_object, "passive", "");
}

static void
_engage_icon_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Engage_Icon *ic;

   ev = event_info;
   ic = data;
   evas_event_freeze(ic->eb->evas);
   evas_object_raise(ic->event_object);
   evas_object_stack_above(ic->bg_object, ic->event_object);
   evas_event_thaw(ic->eb->evas);
   edje_object_signal_emit(ic->bg_object, "active", "");
   edje_object_part_text_set(ic->bg_object, "EngageIconText", ic->app->name);
}

static void
_engage_icon_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Engage_Icon *ic;

   ev = event_info;
   ic = data;
   edje_object_signal_emit(ic->bg_object, "passive", "");
}

static void
_engage_icon_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Engage_Icon *ic;

   ev = event_info;
   ic = data;
   if (ev->button == 1)
     {
	edje_object_signal_emit(ic->bg_object, "start", "");
	e_zone_app_exec(ic->eb->gmc->zone, ic->app);
     }
   else if (ev->button == 3)
     {
	ic->eb->selected_ic = ic;

	//I just dont like this:
	//generating the menu on the icon and not eb itself
	//needed because if we activate on the bar thi mouse_down has not been
	//executed to set the selected_ic
	_engage_bar_menu_gen(ic->eb);
	e_menu_activate_mouse(ic->eb->menu, e_zone_current_get(ic->eb->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(ic->eb->con);
     }
}

static void
_engage_icon_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Engage_Icon *ic;

   ev = event_info;
   ic = data;
   if (ev->button == 1)
     {
	edje_object_signal_emit(ic->bg_object, "start_end", "");
     }
}

static void
_engage_icon_cb_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev;
   Engage_Icon *ic;
   Engage_App_Icon *ai;
  
   ev = event_info;
   ic = data;

   if(!ic->extra_icons)
     return;

   if (ev->z > 0) // Wheel Down, traverse clockwise
     {
	if (!ic->selected_app)
	  ic->selected_app = ic->extra_icons;
	else
	  {
	     if (ic->selected_app->next)
	       ic->selected_app = ic->selected_app->next;
	  }
     }
   else // Wheel Up, traverse counterclockwise
     {
	if (!ic->selected_app)
	  ic->selected_app = evas_list_last(ic->extra_icons);
	else
	  {
	     if (ic->selected_app->prev)
	       ic->selected_app = ic->selected_app->prev;
	  }
    }
  
    ai = ic->selected_app->data;
    edje_object_signal_emit(ai->bg_object, "start", "");
    if (ai->min)
      e_border_uniconify(ai->border);
    e_border_raise(ai->border);
    e_desk_show(ai->border->desk);
    if (ic->eb->engage->conf->click_focus)
      e_border_focus_set(ai->border, 1, 1);

    _engage_bar_motion_handle(ic->eb, ev->canvas.x, ev->canvas.y);
}


static void
_engage_bar_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Engage_Bar *eb;
   Evas_Coord x, y, w, h;
   E_Gadman_Edge edge;
   double multiplier;

   ev = event_info;
   eb = data;

   if (eb->state == ENGAGE_ZOOMING)
     return;
   _engage_zoom_in_slave(eb);
   ecore_timer_add(0.05, _engage_zoom_in_slave, eb);
   evas_object_geometry_get(eb->box_object, &x, &y, &w, &h);
   edge = e_gadman_client_edge_get(eb->gmc);

   if (eb->conf->zoom)
     multiplier = eb->conf->zoom_factor + ((eb->conf->zoom_factor)
        * sqrt(2) / 2);
   else
     multiplier = 1.0;

   evas_event_freeze(eb->evas);
   if (edge == E_GADMAN_EDGE_LEFT)
     {
	evas_object_resize(eb->event_object, w * multiplier, h );
     }
   else if (edge == E_GADMAN_EDGE_RIGHT)
     {
	evas_object_resize(eb->event_object, w * multiplier, h );
	evas_object_move(eb->event_object, x - (w * (multiplier - 1)) + 1, y);
     }
   else if (edge == E_GADMAN_EDGE_TOP)
     {
	evas_object_resize(eb->event_object, w , h * multiplier);
     }
   else
     {
	evas_object_resize(eb->event_object, w , h * multiplier);
	evas_object_move(eb->event_object, x, y - (h * (multiplier - 1)) + 1);
     }
   evas_event_thaw(eb->evas);
   _engage_bar_motion_handle(eb, ev->canvas.x, ev->canvas.y);
}

static void
_engage_bar_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Engage_Bar *eb;

   ev = event_info;
   eb = data;

   if (eb->state == ENGAGE_UNZOOMING)
     return;
   _engage_zoom_out_slave(eb);
   ecore_timer_add(0.05, _engage_zoom_out_slave, eb);
   eb->mouse_out = -1;
   _engage_bar_motion_handle(eb, ev->canvas.x, ev->canvas.y);
}

static int
_engage_zoom_in_slave(void *data)
{
   Evas_Coord x, y;
   Engage_Bar *eb;

   eb = data;
   if (eb->cancel_zoom_in)
     {
	eb->cancel_zoom_in = 0;
	return 0;
     }
   if (eb->state == ENGAGE_NORMAL)
     {
	eb->state = ENGAGE_ZOOMING;
	eb->zoom_start_time = ecore_time_get();
     }
   else if (eb->state == ENGAGE_UNZOOMING)
     {
	eb->cancel_zoom_out = 1;
	eb->state = ENGAGE_ZOOMING;
	eb->zoom_start_time = ecore_time_get() - (eb->zoom - 1.0) /
	  (eb->conf->zoom_factor - 1.0) * eb->conf->zoom_duration;
     }

   eb->zoom = (eb->conf->zoom_factor - 1.0) *
	       ((ecore_time_get() - eb->zoom_start_time)
		/ eb->conf->zoom_duration) + 1.0;

   evas_pointer_canvas_xy_get(eb->evas, &x, &y);
   if (eb->zoom >= eb->conf->zoom_factor)
     {
	eb->zoom = eb->conf->zoom_factor;
	eb->state = ENGAGE_ZOOMED;
	_engage_bar_motion_handle(eb, x, y);
	return 0;
     }
   _engage_bar_motion_handle(eb, x, y);
   return 1;
}

static int
_engage_zoom_out_slave(void *data)
{
   Evas_Coord x, y, bx, by, bw, bh;
   Engage_Bar *eb;

   eb = data;
   if (eb->cancel_zoom_out)
     {
	eb->cancel_zoom_out = 0;
	return 0;
     }
   if (eb->state == ENGAGE_ZOOMED)
     {
	eb->state = ENGAGE_UNZOOMING;
	eb->zoom_start_time = ecore_time_get();
     }
   else if (eb->state == ENGAGE_ZOOMING)
     {
	eb->cancel_zoom_in = 1;
	eb->state = ENGAGE_UNZOOMING;
	eb->zoom_start_time = ecore_time_get() - (eb->conf->zoom_factor - eb->zoom) /
	   (eb->conf->zoom_factor - 1.0) * eb->conf->zoom_duration;
     }

   eb->zoom = (eb->conf->zoom_factor - 1.0) * (1.0 - (ecore_time_get()
			   - eb->zoom_start_time) / eb->conf->zoom_duration) + 1.0;

   evas_pointer_canvas_xy_get(eb->evas, &x, &y);
   if (eb->zoom <= 1.0)
     {
	eb->zoom = 1.0;
	evas_object_geometry_get(eb->box_object, &bx, &by, &bw, &bh);
	evas_event_freeze(eb->evas);
	evas_object_move(eb->event_object, bx, by);
	evas_object_resize(eb->event_object, bw, bh);
	evas_event_thaw(eb->evas);

	eb->state = ENGAGE_NORMAL;
	_engage_bar_motion_handle(eb, x, y);
	return 0;
     }
   _engage_bar_motion_handle(eb, x, y);
   return 1;
}

/*
static void
_engage_bar_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Engage_Bar *eb;

   ev = event_info;
   eb = data;
   if (ev->button == 3)
     {
	e_menu_activate_mouse(eb->menu, e_zone_current_get(eb->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(eb->con);
     }
}
*/

static void
_engage_bar_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Engage_Bar *eb;

   ev = event_info;
   eb = data;
}

static void
_engage_bar_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   Engage_Bar *eb;

   ev = event_info;
   eb = data;
   _engage_bar_motion_handle(eb, ev->cur.canvas.x, ev->cur.canvas.y);
}

static void
_engage_bar_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   Engage_Bar *eb;
   int         edge;

   eb = data;
   switch (change)
     {
	case E_GADMAN_CHANGE_MOVE_RESIZE:
	  e_gadman_client_geometry_get(eb->gmc, &eb->x, &eb->y, &eb->w, &eb->h);

	   edge = e_gadman_client_edge_get(eb->gmc);
	   if (edge == E_GADMAN_EDGE_TOP || edge == E_GADMAN_EDGE_BOTTOM)
	     eb->conf->iconsize = eb->h - eb->bh;
	   else
	     eb->conf->iconsize = eb->w - eb->bw;

	  evas_object_move(eb->bar_object, eb->x, eb->y);
	  evas_object_resize(eb->bar_object, eb->w, eb->h);

	  break;
	case E_GADMAN_CHANGE_EDGE:
	  _engage_bar_edge_change(eb, e_gadman_client_edge_get(eb->gmc));
	  break;
	case E_GADMAN_CHANGE_RAISE:
	case E_GADMAN_CHANGE_ZONE:
	  /* FIXME
	   * Must we do something here?
	   */
	  break;
     }
}

static void
_engage_bar_cb_menu_enabled(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;
   unsigned char enabled;

   eb = data;
   enabled = e_menu_item_toggle_get(mi);
   if ((eb->conf->enabled) && (!enabled))
     {  
	_engage_bar_disable(eb);
     }
   else if ((!eb->conf->enabled) && (enabled))
     { 
	_engage_bar_enable(eb);
     }
}

static void
_engage_bar_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   e_gadman_mode_set(eb->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void
_engage_bar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   if (!eb)
     return;
   _engage_module_config(eb->con, eb);
}

static void
_engage_bar_cb_menu_zoom(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   eb->conf->zoom = e_menu_item_toggle_get(mi);
   e_config_save_queue();
}

static void
_engage_bar_cb_menu_zoom_stretch(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   eb->conf->zoom_stretch = e_menu_item_toggle_get(mi);
   e_config_save_queue();
}

static void
_engage_bar_cb_menu_tray(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_tray_shutdown(eb);
   eb->conf->tray = e_menu_item_toggle_get(mi);
   _engage_tray_init(eb);
   _engage_tray_layout(eb);
   _engage_bar_frame_resize(eb);
   e_config_save_queue();
}

static void
_engage_bar_cb_menu_edit_icon(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;
   eb = data;

   e_eap_edit_show(eb->con, eb->selected_ic->app);
}

static void
_engage_bar_cb_menu_keep_icon(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;
   eb = data;
   const char *file;

   file = ecore_file_file_get(eb->selected_ic->app->path);
   _engage_dotorder_app_add(eb->engage, file);
}

static void
_engage_bar_cb_menu_remove_icon(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;
   eb = data;
   const char *file;
   
   file = ecore_file_file_get(eb->selected_ic->app->path);
   _engage_dotorder_app_del(eb->engage, file);
}

static void
_engage_bar_cb_menu_context_change(void *data, E_Menu *m, E_Menu_Item *mi)
{  
   Engage_Bar *eb;
   char *homedir;
   char buf[4096];
   char dotorder[4096];
   char context[4096];
   
   eb = data;
   homedir = e_user_homedir_get();
   if (homedir)
     {
	snprintf(buf, sizeof(buf), "%s/.e/e/applications/%s", homedir,
		 eb->engage->conf->appdir);
	free(homedir);
     }   
   
   snprintf(dotorder, sizeof(dotorder), "%s/.order", buf);
   snprintf(context, sizeof(context), "%s/.%s/.order", buf, mi->label);
   unlink(dotorder);
   link(context, dotorder);
}

static void
_engage_bar_cb_menu_zoom_small(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_factor_set(eb, 1.5);
}

static void
_engage_bar_cb_menu_zoom_medium(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_factor_set(eb, 2.0);
}

static void
_engage_bar_cb_menu_zoom_large(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_factor_set(eb, 2.3);
}

static void
_engage_bar_cb_menu_zoom_huge(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_factor_set(eb, 2.6);
}

static void
_engage_bar_cb_menu_zoom_speed_fast(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_duration_set(eb, 0.1);
}

static void
_engage_bar_cb_menu_zoom_speed_normal(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_duration_set(eb, 0.3);
}

static void
_engage_bar_cb_menu_zoom_speed_slow(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_zoom_duration_set(eb, 0.5);
}

/* engage ported functions */


static int
_engage_zoom_function(double d, double *zoom, double *disp, Engage_Bar *eb)
{
   double          range, f, x;
   double          ff, sqrt_ffxx, sqrt_ff_1;
   int             zooming;

   zooming = (eb->state == ENGAGE_ZOOMING || eb->state == ENGAGE_UNZOOMING
	 || eb->state == ENGAGE_ZOOMED);
   if (eb->conf->zoom_stretch)
     {
	range = 2.5;
     }
   else
     {
	range = 1.0;
     }
   f = 1.5;
   x = d / range;

   /* some more vars to save computing things over and over */
   ff = f * f;
   sqrt_ffxx = sqrt(ff - x * x);
   sqrt_ff_1 = sqrt(ff - 1.0);

   if (!zooming || !eb->conf->zoom)
     {
	*disp = d * eb->conf->iconsize;
	*zoom = 1.0;
	return zooming;
     }

   if (d > -range && d < range)
     {
	*zoom = (eb->zoom - 1.0) * (eb->conf->zoom_factor - 1.0) *
	    ((sqrt_ff_1 - sqrt_ffxx) / (sqrt_ff_1 - f)) + 1.0;

	if (eb->conf->zoom_stretch)
	  {
	     *disp = (eb->conf->iconsize) *
	       ((eb->zoom - 1.0) * (eb->conf->zoom_factor - 1.0) *
	         (range * (x * (2 * sqrt_ff_1 - sqrt_ffxx) -
		   ff * atan(x / sqrt_ffxx)) / (2.0 * (sqrt_ff_1 - f))) + d);
	  }
      } else {
	*zoom = 1.0;
	if (eb->conf->zoom_stretch)
	  {
	     *disp = (eb->conf->iconsize) *
	       ((eb->zoom - 1.0) * (eb->conf->zoom_factor - 1.0) *
	         (range * (sqrt_ff_1 - ff * atan(1.0 / sqrt_ff_1)) /
		   (2.0 * (sqrt_ff_1 - f))) + range + fabs(d) - range);
	     if (d < 0.0)
	       *disp = -(*disp);
	  }
      }
   return 1;
}

static int
_engage_border_ignore(E_Border *bd)
{
   /* FIXME - this needs to be saved in config */
   static char *ignores[] = { "Gkrellm2", "trayer", "_dialog", NULL};
   char       **cur;
   Ecore_X_Window_State *state, *tmp;
   unsigned int num, counter;
   
   ecore_x_netwm_window_state_get(bd->win, &state, &num);
   counter = 0;
   for (tmp = state; counter < num; tmp++ && counter++)
     if (*tmp == ECORE_X_WINDOW_STATE_SKIP_TASKBAR)
       return 1;
   
   for (cur = ignores; *cur; cur++)
     if (bd->client.icccm.class && strcmp(bd->client.icccm.class, *cur) == 0)
       return 1;
   
   return 0;
}

void
_engage_cb_config_updated(void *data)
{
   Engage_Bar *eb;

   eb = data;
   _engage_bar_frame_resize(eb);
}

