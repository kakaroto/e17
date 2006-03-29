#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <e.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

#ifdef HAVE_BSD
#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#endif

/* TODO List:
 *
 * * Listen to change of main e_app!
 *
 * * Create separate config for each bar
 * * Fix menu
 *
 * * icon labels & label tooltips supported for the name of the app
 * * use part list to know how many icons & where to put in the overlay of an icon
 * * description bubbles/tooltips for icons
 * * app subdirs - need to somehow handle these...
 * * use overlay object and repeat events for doing auto hide/show
 * * emit signals on hide/show due to autohide/show
 * * virtualise autoshow/hide to later allow for key bindings, mouse events elsewhere, ipc and other singals to show/hide
 *
 */

static int bar_count;
static E_Config_DD *conf_edd;
static E_Config_DD *conf_bar_edd;
static Ecore_Event_Handler *_mbar_exe_exit_handler = NULL;

static int drag, drag_start;
static int drag_x, drag_y;

/* const strings */
static const char *_mbar_main_orientation[] =
{"left", "right", "top", "bottom"};

/* module private routines */
static MBar   *_mbar_new();
static void    _mbar_free(MBar *mb);
static void    _mbar_app_change(void *data, E_App *a, E_App_Change ch);
static void    _mbar_config_menu_new(MBar *mb);

static MBar_Bar *_mbar_bar_new(MBar *mb, E_Container *con);
static void    _mbar_bar_free(MBar_Bar *mbb);
static void    _mbar_bar_menu_new(MBar_Bar *mbb);
static void    _mbar_bar_disable(MBar_Bar *mbb);
static void    _mbar_bar_frame_resize(MBar_Bar *mbb);
static void    _mbar_bar_edge_change(MBar_Bar *mbb, int edge);
static void    _mbar_bar_update_policy(MBar_Bar *mbb);
static void    _mbar_bar_motion_handle(MBar_Bar *mbb, Evas_Coord mx, Evas_Coord my);
static void    _mbar_bar_timer_handle(MBar_Bar *mbb);
static void    _mbar_bar_follower_reset(MBar_Bar *mbb);

static MBar_Icon *_mbar_icon_new(MBar_Bar *mbb, E_App *a);
static void       _mbar_icon_free(MBar_Icon *ic);
static MBar_Icon *_mbar_icon_find(MBar_Bar *mbb, E_App *a);
static MBar_Icon *_mbar_icon_pos_find(MBar_Bar *mbb, int x, int y);
static void       _mbar_icon_reorder_after(MBar_Icon *ic, MBar_Icon *after);

static void    _mbar_bar_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void    _mbar_bar_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _mbar_bar_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void    _mbar_bar_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_bar_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_bar_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_bar_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_bar_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int     _mbar_bar_cb_timer(void *data);
static int     _mbar_bar_cb_animator(void *data);

static void    _mbar_bar_cb_enter(void *data, const char *type, void *event);
static void    _mbar_bar_cb_move(void *data, const char *type, void *event);
static void    _mbar_bar_cb_leave(void *data, const char *type, void *event);

static void    _mbar_bar_cb_drop(void *data, const char *type, void *event);
static void    _mbar_bar_cb_finished(E_Drag *drag, int dropped);

static void    _mbar_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _mbar_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void    _mbar_icon_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_icon_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_icon_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_icon_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void    _mbar_icon_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);

#if 0
static void    _mbar_icon_reorder_before(MBar_Icon *ic, MBar_Icon *before);
#endif

static void    _mbar_bar_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _mbar_bar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void    _mbar_drag_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void    _mbar_drag_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);

/* Mount Point Protos */
static int     _mbar_is_mounted(const char *path);
static void    _mbar_mount_point(MBar_Icon *ic);
static void    _mbar_umount_point(MBar_Icon *ic);
static void    _mbar_eject_point(MBar_Icon *ic);
static void    _mbar_set_state(MBar_Icon *ic, int mounted);
static int     _mbar_exe_cb_exit(void *data, int type, void *event);
static int     _mbar_parse_file(char *file, const char *mntpath); 
static void    _mbar_parse_fstab(MBar *mb);
static void    _mbar_add_order(const char *dir, const char *name);
static void    _mbar_mtab_update(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path);

#ifdef HAVE_BSD
static int     _mbar_bsd_is_mounted(const char *path);
static int     _mbar_bsd_cb_timer(void *data);
#endif

/* Config Updated Function Protos */
static void    _mbar_bar_cb_width_auto(void *data);
static void    _mbar_bar_cb_follower(void *data);
static void    _mbar_bar_cb_iconsize_change(void *data);

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
     "MBar"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   MBar *mb;

   /* actually init mbar */
   mb = _mbar_new();
   m->config_menu = mb->config_menu;
   return mb;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   MBar *mb;

   if (m->config_menu)
     m->config_menu = NULL;

   mb = m->data;
   if (!mb)
     return 1;
   
   if (mb->config_dialog) 
     {
	e_object_del(E_OBJECT(mb->config_dialog));
	mb->config_dialog = NULL;
     }
   _mbar_free(mb);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   MBar *mb;

   mb = m->data;
   e_config_domain_save("module.mbar", conf_edd, mb->conf);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m)
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(_("Enlightenment Mount Bar Module"),
			_("This is the Mount Bar module for Enlightenment."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   MBar *mb;
   Evas_List *l;
   
   mb = m->data;
   if (!mb) return 0;
   if (!mb->bars) return 0;
   for (l = mb->bars; l; l = l->next) 
     {
	MBar_Bar *mbb;
	
	mbb = l->data;
	if (!mbb) return 0;
	if (mbb->con == e_container_current_get(e_manager_current_get())) 
	  { 
	     _config_mbar_module(mbb->con, mb);
	     break;
	  }	
     }
   return 1;
}

/* module private routines */
static MBar *
_mbar_new()
{
   MBar *mb;
   char buf[4096];
   Evas_List *managers, *l, *l2, *cl;

   bar_count = 0;
   mb = E_NEW(MBar, 1);
   if (!mb) 
     return NULL;

   conf_bar_edd = E_CONFIG_DD_NEW("MBar_Config_Bar", Config_Bar);
#undef T
#undef D
#define T Config_Bar
#define D conf_bar_edd
   E_CONFIG_VAL(D, T, enabled, UCHAR);

   conf_edd = E_CONFIG_DD_NEW("MBar_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_VAL(D, T, appdir, STR);
   E_CONFIG_VAL(D, T, follower, INT);
   E_CONFIG_VAL(D, T, follow_speed, DOUBLE);
   E_CONFIG_VAL(D, T, autoscroll_speed, DOUBLE);
   E_CONFIG_VAL(D, T, iconsize, INT);
   E_CONFIG_VAL(D, T, width, INT);
   E_CONFIG_VAL(D, T, allow_overlap, INT);
   E_CONFIG_LIST(D, T, bars, conf_bar_edd);

   mb->conf = e_config_domain_load("module.mbar", conf_edd);   
   if (!mb->conf)
     {
	mb->conf = E_NEW(Config, 1);
	mb->conf->appdir = evas_stringshare_add("mbar");
	mb->conf->follower = 1;
	mb->conf->follow_speed = 0.9;
	mb->conf->autoscroll_speed = 0.95;
	mb->conf->iconsize = 24;
	mb->conf->allow_overlap = 0;
	mb->conf->width = MBAR_WIDTH_AUTO;
	snprintf(buf, sizeof(buf), "%s/.e/e/applications/%s", getenv("HOME"), mb->conf->appdir);
	if (!ecore_file_exists(buf))
	  ecore_file_mkdir(buf);	
     }
   E_CONFIG_LIMIT(mb->conf->follow_speed, 0.01, 1.0);
   E_CONFIG_LIMIT(mb->conf->autoscroll_speed, 0.01, 1.0);
   E_CONFIG_LIMIT(mb->conf->iconsize, 2, 400);
   E_CONFIG_LIMIT(mb->conf->width, -2, -1);
   E_CONFIG_LIMIT(mb->conf->allow_overlap, 0, 1);
   
   _mbar_config_menu_new(mb);

   if (mb->conf->appdir[0] != '/')
     {
	char *homedir;

	homedir = e_user_homedir_get();
	if (homedir)
	  {
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/%s", homedir, mb->conf->appdir);
	     free(homedir);
	  }
     }
   else
     strcpy(buf, mb->conf->appdir);

   mb->apps = e_app_new(buf, 0);
   _mbar_parse_fstab(mb);   
   if (mb->apps) 
     e_app_subdir_scan(mb->apps, 0);
   e_app_change_callback_add(_mbar_app_change, mb);

   managers = e_manager_list();
   cl = mb->conf->bars;
   for (l = managers; l; l = l->next)
     {
	E_Manager *man;

	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;
	     MBar_Bar *mbb;
	     /* Config */
	     con = l2->data;
	     mbb = _mbar_bar_new(mb, con);
	     if (mbb)
	       {
		  E_Menu_Item *mi;

		  if (!cl)
		    {
		       mbb->conf = E_NEW(Config_Bar, 1);
		       mbb->conf->enabled = 1;
		       mb->conf->bars = evas_list_append(mb->conf->bars, mbb->conf);
		    }
		  else
		    {
		       mbb->conf = cl->data;
		       cl = cl->next;
		    }
		  /* Menu */
		  _mbar_bar_menu_new(mbb);

		  mi = e_menu_item_new(mb->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");		  
		  e_menu_item_callback_set(mi, _mbar_bar_cb_menu_configure, mbb);

		  mi = e_menu_item_new(mb->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, mbb->menu);

		  /* Setup */
		  if (!mbb->conf->enabled)
		    _mbar_bar_disable(mbb);
	       }
	  }
     }

#ifdef HAVE_LINUX   
   /* Add File Monitor for /etc/mtab */
   mb->mon = ecore_file_monitor_add(MTAB, _mbar_mtab_update, mb);
#endif
#ifdef HAVE_BSD
   mb->mon_timer = ecore_timer_add(2.0, _mbar_bsd_cb_timer, mb);
#endif
   
   return mb;
}

static void
_mbar_free(MBar *mb)
{
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_bar_edd);

   while (mb->bars)
     _mbar_bar_free(mb->bars->data);
   if (mb->apps)
     e_object_unref(E_OBJECT(mb->apps));

   if (mb->conf->appdir) evas_stringshare_del(mb->conf->appdir);
   e_app_change_callback_del(_mbar_app_change, mb);
   e_object_del(E_OBJECT(mb->config_menu));

#ifdef HAVE_LINUX   
   if (mb->mon)
     ecore_file_monitor_del(mb->mon);
#endif
#ifdef HAVE_BSD
   if (mb->mon_timer)
     ecore_timer_del(mb->mon_timer);
#endif
   
   evas_list_free(mb->conf->bars);
   free(mb->conf);
   free(mb);
}

static void
_mbar_app_change(void *data, E_App *a, E_App_Change ch)
{
   MBar *mb;
   Evas_List *l, *ll;

   mb = data;
   for (l = mb->bars; l; l = l->next)
     {
	MBar_Bar *mbb;

	mbb = l->data;
	switch (ch)
	  {
	   case E_APP_ADD:
	     if (e_app_is_parent(mb->apps, a))
	       {
		  MBar_Icon *ic;

		  ic = _mbar_icon_new(mbb, a);
		  if (ic)
		    {
		       edje_object_part_text_set(ic->bg_object, "label", a->comment);		       
		       for (ll = mb->apps->subapps; ll; ll = ll->next)
			 {
			    E_App *a2;
			    
			    a2 = ll->data;
			    ic = _mbar_icon_find(mbb, a2);
			    if (ic) 
			      _mbar_icon_reorder_after(ic, NULL);
			 }
		       _mbar_bar_frame_resize(mbb);
		    }
	       }
	     break;
	   case E_APP_DEL:
	     if (e_app_is_parent(mb->apps, a))
	       {
		  MBar_Icon *ic;

		  ic = _mbar_icon_find(mbb, a);
		  if (ic) 
		    _mbar_icon_free(ic);
		  _mbar_bar_frame_resize(mbb);
	       }
	     break;
	   case E_APP_CHANGE:
	     if (e_app_is_parent(mb->apps, a))
	       {
		  MBar_Icon *ic;

		  ic = _mbar_icon_find(mbb, a);
		  if (ic) 
		    _mbar_icon_free(ic);
		  evas_image_cache_flush(mbb->evas);
		  evas_image_cache_reload(mbb->evas);
		  ic = _mbar_icon_new(mbb, a);
		  if (ic)
		    {
		       edje_object_part_text_set(ic->bg_object, "label", a->comment);
		       for (ll = mb->apps->subapps; ll; ll = ll->next)
			 {
			    E_App *a2;

			    a2 = ll->data;
			    ic = _mbar_icon_find(mbb, a2);
			    if (ic) 
			      _mbar_icon_reorder_after(ic, NULL);
			 }
		       _mbar_bar_frame_resize(mbb);
		    }
	       }
	     break;
	   case E_APP_ORDER:
	     if (a == mb->apps)
	       {
		  for (ll = mb->apps->subapps; ll; ll = ll->next)
		    {
		       MBar_Icon *ic;
		       E_App *a2;

		       a2 = ll->data;
		       ic = _mbar_icon_find(mbb, a2);
		       if (ic) _mbar_icon_reorder_after(ic, NULL);
		    }
	       }
	     break;
	   case E_APP_EXEC:
	     if (e_app_is_parent(mb->apps, a))
	       {
		  MBar_Icon *ic;

		  ic = _mbar_icon_find(mbb, a);
		  if (ic)
		    {
		       if (a->startup_notify)
			 {
			    edje_object_signal_emit(ic->icon_object, "start", "");
			    for (ll = ic->extra_icons; ll; ll = ll->next) edje_object_signal_emit(ll->data, "start", "");
			    edje_object_signal_emit(ic->bg_object, "start", "");
			    edje_object_signal_emit(ic->overlay_object, "start", "");
			    if (ic->mbb->overlay_object)
			      edje_object_signal_emit(ic->mbb->overlay_object, "start", "");
			 }
		       else
			 {
			    edje_object_signal_emit(ic->icon_object, "exec", "");
			    for (ll = ic->extra_icons; ll; ll = ll->next) edje_object_signal_emit(ll->data, "exec", "");
			    edje_object_signal_emit(ic->bg_object, "exec", "");
			    edje_object_signal_emit(ic->overlay_object, "exec", "");
			    if (ic->mbb->overlay_object)
			      edje_object_signal_emit(ic->mbb->overlay_object, "exec", "");
			 }
		    }
	       }
	     break;
	   case E_APP_READY:
	   case E_APP_READY_EXPIRE:
	   case E_APP_EXIT:
	     if (e_app_is_parent(mb->apps, a))
	       {
		  if (a->startup_notify)
		    {
		       MBar_Icon *ic;

		       ic = _mbar_icon_find(mbb, a);
		       if (ic)
			 {
			    edje_object_signal_emit(ic->icon_object, "stop", "");
			    for (ll = ic->extra_icons; ll; ll = ll->next) edje_object_signal_emit(ll->data, "stop", "");
			    edje_object_signal_emit(ic->bg_object, "stop", "");
			    edje_object_signal_emit(ic->overlay_object, "stop", "");
			    if (ic->mbb->overlay_object)
			      edje_object_signal_emit(ic->mbb->overlay_object, "stop", "");
			 }
		    }
	       }
	     break;
	   default:
	     break;
	  }
     }
}

static MBar_Bar *
_mbar_bar_new(MBar *mb, E_Container *con)
{
   MBar_Bar *mbb;
   Evas_List *l;
   Evas_Object *o;
   E_Gadman_Policy policy;
   Evas_Coord x, y, w, h;
   const char *drop[] = { "enlightenment/eapp" };
   char buf[4096];

   mbb = E_NEW(MBar_Bar, 1);
   if (!mbb) 
     return NULL;
   mbb->mbar = mb;
   mb->bars = evas_list_append(mb->bars, mbb);

   mbb->con = con;
   e_object_ref(E_OBJECT(con));
   mbb->evas = con->bg_evas;

   mbb->x = mbb->y = mbb->w = mbb->h = -1;

   evas_event_freeze(mbb->evas);
   o = edje_object_add(mbb->evas);
   mbb->bar_object = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/main"))
   {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");
	edje_object_file_set(o, buf, "modules/mbar/main");
   }
   evas_object_show(o);

   if (mbb->mbar->conf->follower)
     {
	o = edje_object_add(mbb->evas);
	mbb->overlay_object = o;
	evas_object_layer_set(o, 2);
	if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/follower"))
	  {
	     snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
	     edje_object_file_set(o, buf, "modules/mbar/follower");
	  }
	evas_object_show(o);
     }
   
   o = evas_object_rectangle_add(mbb->evas);
   mbb->event_object = o;
   evas_object_layer_set(o, 3);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,  _mbar_bar_cb_mouse_in,  mbb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _mbar_bar_cb_mouse_out, mbb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _mbar_bar_cb_mouse_down, mbb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _mbar_bar_cb_mouse_up, mbb);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _mbar_bar_cb_mouse_move, mbb);
   evas_object_show(o);

   o = e_box_add(mbb->evas);
   mbb->box_object = o;
   evas_object_intercept_move_callback_add(o, _mbar_bar_cb_intercept_move, mbb);
   evas_object_intercept_resize_callback_add(o, _mbar_bar_cb_intercept_resize, mbb);
   e_box_freeze(o);
   edje_object_part_swallow(mbb->bar_object, "items", o);
   evas_object_show(o);

   if (mbb->mbar->apps)
     {
	for (l = mbb->mbar->apps->subapps; l; l = l->next)
	  {
	     E_App *a;
	     MBar_Icon *ic;

	     a = l->data;
	     ic = _mbar_icon_new(mbb, a);
	  }
     }
   mbb->align_req = 0.5;
   mbb->align = 0.5;
   e_box_align_set(mbb->box_object, 0.5, 0.5);

   e_box_thaw(mbb->box_object);

   evas_object_resize(mbb->bar_object, 1000, 1000);
   edje_object_calc_force(mbb->bar_object);
   edje_object_part_geometry_get(mbb->bar_object, "items", &x, &y, &w, &h);
   mbb->bar_inset.l = x;
   mbb->bar_inset.r = 1000 - (x + w);
   mbb->bar_inset.t = y;
   mbb->bar_inset.b = 1000 - (y + h);

   o = edje_object_add(mbb->evas);
   if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/icon"))
   {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
	edje_object_file_set(o, buf, "modules/mbar/icon");
   }
   evas_object_resize(o, 100, 100);
   edje_object_calc_force(o);
   edje_object_part_geometry_get(o, "item", &x, &y, &w, &h);
   mbb->icon_inset.l = x;
   mbb->icon_inset.r = 100 - (x + w);
   mbb->icon_inset.t = y;
   mbb->icon_inset.b = 100 - (y + h);
   evas_object_del(o);

   mbb->drop_handler = e_drop_handler_add(mbb,
					  _mbar_bar_cb_enter, _mbar_bar_cb_move,
					  _mbar_bar_cb_leave, _mbar_bar_cb_drop,
					  drop, 1,
					  mbb->x + mbb->bar_inset.l, mbb->y + mbb->bar_inset.t,
					  mbb->w - (mbb->bar_inset.l + mbb->bar_inset.r),
					  mbb->h - (mbb->bar_inset.t + mbb->bar_inset.b));

   mbb->gmc = e_gadman_client_new(mbb->con->gadman);
   e_gadman_client_domain_set(mbb->gmc, "module.mbar", bar_count++);
   policy = E_GADMAN_POLICY_EDGES | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;
   if (mbb->mbar->conf->width == MBAR_WIDTH_FIXED)
     policy |= E_GADMAN_POLICY_HSIZE;

   if (mbb->mbar->conf->allow_overlap == 0)
     policy &= ~E_GADMAN_POLICY_ALLOW_OVERLAP;
   else
     policy |= E_GADMAN_POLICY_ALLOW_OVERLAP;

   e_gadman_client_policy_set(mbb->gmc, policy);
   e_gadman_client_min_size_set(mbb->gmc, 8, 8);
   e_gadman_client_max_size_set(mbb->gmc, 3200, 3200);
   e_gadman_client_auto_size_set(mbb->gmc, -1, -1);
   e_gadman_client_align_set(mbb->gmc, 0.5, 1.0);
   e_gadman_client_resize(mbb->gmc, 400, 40);
   e_gadman_client_change_func_set(mbb->gmc, _mbar_bar_cb_gmc_change, mbb);
   e_gadman_client_edge_set(mbb->gmc, E_GADMAN_EDGE_BOTTOM);
   e_gadman_client_load(mbb->gmc);

   evas_event_thaw(mbb->evas);

   /* We need to resize, if the width is auto and the number
    * of apps has changed since last startup */
   _mbar_bar_frame_resize(mbb);

   return mbb;
}

static void
_mbar_bar_free(MBar_Bar *mbb)
{
   e_object_unref(E_OBJECT(mbb->con));

   e_object_del(E_OBJECT(mbb->menu));

   while (mbb->icons)
     _mbar_icon_free(mbb->icons->data);

   e_drop_handler_del(mbb->drop_handler);

   if (mbb->timer) 
     ecore_timer_del(mbb->timer);
   if (mbb->animator) 
     ecore_animator_del(mbb->animator);
   evas_object_del(mbb->bar_object);
   if (mbb->overlay_object) 
     evas_object_del(mbb->overlay_object);
   evas_object_del(mbb->box_object);
   evas_object_del(mbb->event_object);
   if (mbb->drag_object) 
     evas_object_del(mbb->drag_object);
   mbb->drag_object = NULL;
   if (mbb->drag_object_overlay) 
     evas_object_del(mbb->drag_object_overlay);
   mbb->drag_object_overlay = NULL;

   e_gadman_client_save(mbb->gmc);
   e_object_del(E_OBJECT(mbb->gmc));

   mbb->mbar->bars = evas_list_remove(mbb->mbar->bars, mbb);

   free(mbb->conf);
   free(mbb);
   bar_count--;
}

static void
_mbar_bar_menu_new(MBar_Bar *mbb)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   mbb->menu = mn;

   /* Config */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");   
   e_menu_item_callback_set(mi, _mbar_bar_cb_menu_configure, mbb);

   /* Edit */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");   
   e_menu_item_callback_set(mi, _mbar_bar_cb_menu_edit, mbb);
}

static void
_mbar_bar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   MBar_Bar *mbb;

   mbb = data;
   if (!mbb) return;
   _config_mbar_module(mbb->con, mbb->mbar);
}

static void
_mbar_bar_disable(MBar_Bar *mbb)
{
   mbb->conf->enabled = 0;
   evas_object_hide(mbb->bar_object);
   if (mbb->overlay_object) 
     evas_object_hide(mbb->overlay_object);
   evas_object_hide(mbb->box_object);
   evas_object_hide(mbb->event_object);
   e_config_save_queue();
}

static MBar_Icon *
_mbar_icon_new(MBar_Bar *mbb, E_App *a)
{
   MBar_Icon *ic;
   Evas_Object *o;
   Evas_Coord w, h;
   char buf[4096];
   Evas_Object *obj;
   int ow, oh;

   ic = E_NEW(MBar_Icon, 1);
   if (!ic) 
     return NULL;
   ic->mbb = mbb;
   ic->app = a;
   e_object_ref(E_OBJECT(a));
   mbb->icons = evas_list_append(mbb->icons, ic);

   o = evas_object_rectangle_add(mbb->evas);
   ic->event_object = o;
   evas_object_layer_set(o, 1);
   evas_object_clip_set(o, evas_object_clip_get(mbb->box_object));
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_repeat_events_set(o, 1);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN,  _mbar_icon_cb_mouse_in,  ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _mbar_icon_cb_mouse_out, ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _mbar_icon_cb_mouse_down, ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _mbar_icon_cb_mouse_up, ic);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _mbar_icon_cb_mouse_move, ic);
   evas_object_show(o);

   o = edje_object_add(mbb->evas);
   ic->bg_object = o;
   evas_object_intercept_move_callback_add(o, _mbar_icon_cb_intercept_move, ic);
   evas_object_intercept_resize_callback_add(o, _mbar_icon_cb_intercept_resize, ic);
   if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/icon"))
   {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
	edje_object_file_set(o, buf, "modules/mbar/icon");
   }
   evas_object_show(o);

   edje_object_part_text_set(ic->bg_object, "label", a->comment);
   
   o = edje_object_add(mbb->evas);
   ic->icon_object = o;
   evas_object_resize(o, mbb->mbar->conf->iconsize, mbb->mbar->conf->iconsize);
   edje_object_file_set(o, ic->app->path, "icon");
   edje_object_part_swallow(ic->bg_object, "item", o);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   o = edje_object_add(mbb->evas);
   ic->overlay_object = o;
   evas_object_intercept_move_callback_add(o, _mbar_icon_cb_intercept_move, ic);
   evas_object_intercept_resize_callback_add(o, _mbar_icon_cb_intercept_resize, ic);
   if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/icon_overlay"))
   {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
	edje_object_file_set(o, buf, "modules/mbar/icon_overlay");
   }
   evas_object_show(o);

   o = edje_object_add(mbb->evas);
   ic->extra_icons = evas_list_append(ic->extra_icons, o);
   edje_object_file_set(o, ic->app->path, "icon");
   edje_object_part_swallow(ic->overlay_object, "item", o);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
	
   evas_object_raise(ic->event_object);

   w = mbb->mbar->conf->iconsize + mbb->icon_inset.l + mbb->icon_inset.r;
   h = mbb->mbar->conf->iconsize + mbb->icon_inset.t + mbb->icon_inset.b;
   e_box_pack_end(mbb->box_object, ic->bg_object);
   e_box_pack_options_set(ic->bg_object,
			  1, 1, /* fill */
			  0, 0, /* expand */
			  0.5, 0.5, /* align */
			  w, h, /* min */
			  w, h /* max */
			  );

   edje_object_signal_emit(ic->bg_object, "passive", "");
   edje_object_signal_emit(ic->icon_object, "passive", "");   
   edje_object_signal_emit(ic->overlay_object, "passive", "");
   
   #ifdef HAVE_LINUX
   _mbar_set_state(ic, _mbar_is_mounted(ic->app->generic));
   #endif
   #ifdef HAVE_BSD
   _mbar_set_state(ic, _mbar_bsd_is_mounted(ic->app->generic));   
   #endif
   return ic;
}

static void
_mbar_icon_free(MBar_Icon *ic)
{
   ic->mbb->icons = evas_list_remove(ic->mbb->icons, ic);
   if (ic->bg_object) evas_object_del(ic->bg_object);
   if (ic->overlay_object) evas_object_del(ic->overlay_object);
   if (ic->icon_object) evas_object_del(ic->icon_object);
   if (ic->event_object) evas_object_del(ic->event_object);
   while (ic->extra_icons)
     {
	Evas_Object *o;

	o = ic->extra_icons->data;
	ic->extra_icons = evas_list_remove_list(ic->extra_icons, ic->extra_icons);
	evas_object_del(o);
     }
   e_object_unref(E_OBJECT(ic->app));
   free(ic);
}

static MBar_Icon *
_mbar_icon_find(MBar_Bar *mbb, E_App *a)
{
   Evas_List *l;

   for (l = mbb->icons; l; l = l->next)
     {
	MBar_Icon *ic;

	ic = l->data;
	if (e_app_equals(ic->app, a))
	  return ic;
     }
   return NULL;
}

static MBar_Icon *
_mbar_icon_pos_find(MBar_Bar *mbb, int x, int y)
{
   MBar_Icon *ic;
   double pos, iw;
   int w, h;

   x = x - (mbb->x + mbb->bar_inset.l);
   y = y - (mbb->y + mbb->bar_inset.t);
   w = mbb->w - (mbb->bar_inset.l + mbb->bar_inset.r);
   h = mbb->h - (mbb->bar_inset.t + mbb->bar_inset.b);

   if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_BOTTOM) ||
       (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_TOP))
     {
	iw = w / (double) e_box_pack_count_get(mbb->box_object);
	pos = x / iw;
     }
   else if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_RIGHT))
     {
	iw = h / (double) e_box_pack_count_get(mbb->box_object);
	pos = y / iw;
     }

   ic = evas_list_nth(mbb->icons, pos);
   return ic;
}

void
_mbar_config_menu_new(MBar *mb)
{
   E_Menu *mn;

   mn = e_menu_new();
   mb->config_menu = mn;
}

#if 0
static void
_mbar_icon_reorder_before(MBar_Icon *ic, MBar_Icon *before)
{
   Evas_Coord bw, bh;

   e_box_freeze(ic->mbb->box_object);
   e_box_unpack(ic->bg_object);
   ic->mbb->icons = evas_list_remove(ic->mbb->icons, ic);
   if (before)
     {
	ic->mbb->icons = evas_list_prepend_relative(ic->mbb->icons, ic, before);
	e_box_pack_before(ic->mbb->box_object, ic->bg_object, before->bg_object);
     }
   else
     {
	ic->mbb->icons = evas_list_prepend(ic->mbb->icons, ic);
	e_box_pack_start(ic->mbb->box_object, ic->bg_object);
     }
   edje_object_size_min_calc(ic->bg_object, &bw, &bh);
   e_box_pack_options_set(ic->bg_object,
			  1, 1, /* fill */
			  0, 0, /* expand */
			  0.5, 0.5, /* align */
			  bw, bh, /* min */
			  bw, bh /* max */
			  );
   e_box_thaw(ic->mbb->box_object);
}
#endif

static void
_mbar_icon_reorder_after(MBar_Icon *ic, MBar_Icon *after)
{
   Evas_Coord w, h;

   e_box_freeze(ic->mbb->box_object);
   e_box_unpack(ic->bg_object);
   ic->mbb->icons = evas_list_remove(ic->mbb->icons, ic);
   if (after)
     {
	ic->mbb->icons = evas_list_append_relative(ic->mbb->icons, ic, after);
	e_box_pack_after(ic->mbb->box_object, ic->bg_object, after->bg_object);
     }
   else
     {
	ic->mbb->icons = evas_list_append(ic->mbb->icons, ic);
	e_box_pack_end(ic->mbb->box_object, ic->bg_object);
     }
   w = ic->mbb->mbar->conf->iconsize + ic->mbb->icon_inset.l + ic->mbb->icon_inset.r;
   h = ic->mbb->mbar->conf->iconsize + ic->mbb->icon_inset.t + ic->mbb->icon_inset.b;
   e_box_pack_options_set(ic->bg_object,
			  1, 1, /* fill */
			  0, 0, /* expand */
			  0.5, 0.5, /* align */
			  w, h, /* min */
			  w, h /* max */
			  );
   e_box_thaw(ic->mbb->box_object);
}

static void
_mbar_bar_frame_resize(MBar_Bar *mbb)
{
   Evas_Coord w, h, bw, bh;
   /* Not finished loading config yet! */
   if ((mbb->x == -1) || (mbb->y == -1) ||
       (mbb->w == -1) || (mbb->h == -1))
     return;

   evas_event_freeze(mbb->evas);
   e_box_freeze(mbb->box_object);

   e_box_min_size_get(mbb->box_object, &w, &h);
   edje_extern_object_min_size_set(mbb->box_object, w, h);
   edje_object_part_swallow(mbb->bar_object, "items", mbb->box_object);
   edje_object_size_min_calc(mbb->bar_object, &bw, &bh);
   edje_extern_object_min_size_set(mbb->box_object, 0, 0);
   edje_object_part_swallow(mbb->bar_object, "items", mbb->box_object);

   e_box_thaw(mbb->box_object);
   evas_event_thaw(mbb->evas);

   if (mbb->mbar->conf->width == MBAR_WIDTH_AUTO)
     e_gadman_client_resize(mbb->gmc, bw, bh);
   else
     {
	if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_RIGHT))
	  e_gadman_client_resize(mbb->gmc, bw, mbb->h);
	else if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_TOP) ||
		 (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_BOTTOM))
	  e_gadman_client_resize(mbb->gmc, mbb->w, bh);
     }
}

static void
_mbar_bar_edge_change(MBar_Bar *mbb, int edge)
{
   Evas_List *l;
   Evas_Coord w, h, tmp;
   Evas_Object *o;
   E_Gadman_Policy policy;
   int changed;

   evas_event_freeze(mbb->evas);
   o = mbb->bar_object;
   edje_object_signal_emit(o, "set_orientation", _mbar_main_orientation[edge]);
   edje_object_message_signal_process(o);

   if (mbb->overlay_object)
     {
	o = mbb->overlay_object;
	edje_object_signal_emit(o, "set_orientation", _mbar_main_orientation[edge]);
	edje_object_message_signal_process(o);
     }

   e_box_freeze(mbb->box_object);

   for (l = mbb->icons; l; l = l->next)
     {
	MBar_Icon *ic;

	ic = l->data;
	o = ic->bg_object;
	edje_object_signal_emit(o, "set_orientation", _mbar_main_orientation[edge]);
	edje_object_message_signal_process(o);

	o = ic->overlay_object;
	edje_object_signal_emit(o, "set_orientation", _mbar_main_orientation[edge]);
	edje_object_message_signal_process(o);

	w = mbb->mbar->conf->iconsize + mbb->icon_inset.l + mbb->icon_inset.r;
	h = mbb->mbar->conf->iconsize + mbb->icon_inset.t + mbb->icon_inset.b;
	e_box_pack_options_set(ic->bg_object,
			       1, 1, /* fill */
			       0, 0, /* expand */
			       0.5, 0.5, /* align */
			       w, h, /* min */
			       w, h /* max */
			       );
     }

   mbb->align_req = 0.5;
   mbb->align = 0.5;
   e_box_align_set(mbb->box_object, 0.5, 0.5);

   policy = E_GADMAN_POLICY_EDGES | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;
   if (mbb->mbar->conf->allow_overlap == 0)
     policy &= ~E_GADMAN_POLICY_ALLOW_OVERLAP;
   else
     policy |= E_GADMAN_POLICY_ALLOW_OVERLAP;

   if ((edge == E_GADMAN_EDGE_BOTTOM) ||
       (edge == E_GADMAN_EDGE_TOP))
     {
	changed = (e_box_orientation_get(mbb->box_object) != 1);
	if (changed)
	  {
	     e_box_orientation_set(mbb->box_object, 1);
	     if (mbb->mbar->conf->width == MBAR_WIDTH_FIXED)
	       policy |= E_GADMAN_POLICY_HSIZE;
	     e_gadman_client_policy_set(mbb->gmc, policy);
	     tmp = mbb->w;
	     mbb->w = mbb->h;
	     mbb->h = tmp;
	  }
     }
   else if ((edge == E_GADMAN_EDGE_LEFT) ||
	    (edge == E_GADMAN_EDGE_RIGHT))
     {
	changed = (e_box_orientation_get(mbb->box_object) != 0);
	if (changed)
	  {
	     e_box_orientation_set(mbb->box_object, 0);
	     if (mbb->mbar->conf->width == MBAR_WIDTH_FIXED)
	       policy |= E_GADMAN_POLICY_VSIZE;
	     e_gadman_client_policy_set(mbb->gmc, policy);
	     tmp = mbb->w;
	     mbb->w = mbb->h;
	     mbb->h = tmp;
	  }
     }

   e_box_thaw(mbb->box_object);
   evas_event_thaw(mbb->evas);

   _mbar_bar_frame_resize(mbb);
}

static void
_mbar_bar_update_policy(MBar_Bar *mbb)
{
   E_Gadman_Policy policy;

   policy = E_GADMAN_POLICY_EDGES | E_GADMAN_POLICY_HMOVE | E_GADMAN_POLICY_VMOVE;

   if (mbb->mbar->conf->allow_overlap == 0)
     policy &= ~E_GADMAN_POLICY_ALLOW_OVERLAP;
   else
     policy |= E_GADMAN_POLICY_ALLOW_OVERLAP;

   if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_BOTTOM) ||
       (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_TOP))
     {
	if (mbb->mbar->conf->width == MBAR_WIDTH_FIXED)
	  policy |= E_GADMAN_POLICY_HSIZE;
	e_gadman_client_policy_set(mbb->gmc, policy);
     }
   else if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_RIGHT))
     {
	if (mbb->mbar->conf->width == MBAR_WIDTH_FIXED)
	  policy |= E_GADMAN_POLICY_VSIZE;
	e_gadman_client_policy_set(mbb->gmc, policy);
     }
}

static void
_mbar_bar_motion_handle(MBar_Bar *mbb, Evas_Coord mx, Evas_Coord my)
{
   Evas_Coord x, y, w, h;
   double relx, rely;

   evas_object_geometry_get(mbb->box_object, &x, &y, &w, &h);
   if (w > 0) relx = (double)(mx - x) / (double)w;
   else relx = 0.0;
   if (h > 0) rely = (double)(my - y) / (double)h;
   else rely = 0.0;

   if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_BOTTOM) ||
       (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_TOP))
     {
	mbb->align_req = 1.0 - relx;
	mbb->follow_req = relx;
     }
   else if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_RIGHT))
     {
	mbb->align_req = 1.0 - rely;
	mbb->follow_req = rely;
     }
}

static void
_mbar_bar_timer_handle(MBar_Bar *mbb)
{
   if (!mbb->timer)
     mbb->timer = ecore_timer_add(0.01, _mbar_bar_cb_timer, mbb);
   if (!mbb->animator)
     mbb->animator = ecore_animator_add(_mbar_bar_cb_animator, mbb);
}

static void
_mbar_bar_follower_reset(MBar_Bar *mbb)
{
   Evas_Coord ww, hh, bx, by, bw, bh, d1, d2, mw, mh;

   if (!mbb->overlay_object) 
     return;

   evas_output_viewport_get(mbb->evas, NULL, NULL, &ww, &hh);
   evas_object_geometry_get(mbb->box_object, &bx, &by, &bw, &bh);
   edje_object_size_min_get(mbb->overlay_object, &mw, &mh);
   if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_BOTTOM) ||
       (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_TOP))
     {
	d1 = bx;
	d2 = ww - (bx + bw);
	if (bw > 0)
	  {
	     if (d1 < d2)
	       mbb->follow_req = -((double)(d1 + (mw * 4)) / (double)bw);
	     else
	       mbb->follow_req = 1.0 + ((double)(d2 + (mw * 4)) / (double)bw);
	  }
     }
   else if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_RIGHT))
     {
	d1 = by;
	d2 = hh - (by + bh);
	if (bh > 0)
	  {
	     if (d1 < d2)
	       mbb->follow_req = -((double)(d1 + (mh * 4)) / (double)bh);
	     else
	       mbb->follow_req = 1.0 + ((double)(d2 + (mh * 4)) / (double)bh);
	  }
     }
}

static void
_mbar_icon_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   MBar_Icon *ic;

   ic = data;
   evas_object_move(o, x, y);
   evas_object_move(ic->event_object, x, y);
   evas_object_move(ic->overlay_object, x, y);
}

static void
_mbar_icon_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   MBar_Icon *ic;

   ic = data;
   evas_object_resize(o, w, h);
   evas_object_resize(ic->event_object, w, h);
   evas_object_resize(ic->overlay_object, w, h);
}

static void
_mbar_bar_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   MBar_Bar *mbb;

   mbb = data;
   evas_object_move(o, x, y);
   evas_object_move(mbb->event_object, x, y);
}

static void
_mbar_bar_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   MBar_Bar *mbb;

   mbb = data;

   evas_object_resize(o, w, h);
   evas_object_resize(mbb->event_object, w, h);
}

static void
_mbar_icon_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   MBar_Icon *ic;

   ev = event_info;
   ic = data;
   evas_event_freeze(ic->mbb->evas);
   evas_object_raise(ic->event_object);
   if (ic->raise_on_hilight)
     evas_object_stack_below(ic->bg_object, ic->event_object);
   evas_object_stack_below(ic->overlay_object, ic->event_object);
   evas_event_thaw(ic->mbb->evas);
   edje_object_signal_emit(ic->bg_object, "active", "");
   edje_object_signal_emit(ic->icon_object, "active", "");
   edje_object_signal_emit(ic->overlay_object, "active", "");   
   if (ic->mbb->overlay_object)
     edje_object_signal_emit(ic->mbb->overlay_object, "active", "");
	edje_object_part_text_set(ic->bg_object, "label", ic->app->comment);
}

static void
_mbar_icon_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   MBar_Icon *ic;

   ev = event_info;
   ic = data;
   edje_object_signal_emit(ic->bg_object, "passive", "");
   edje_object_signal_emit(ic->icon_object, "passive", "");   
   edje_object_signal_emit(ic->overlay_object, "passive", "");
   if (ic->mbb->overlay_object)
     edje_object_signal_emit(ic->mbb->overlay_object, "passive", "");
	edje_object_part_text_set(ic->bg_object, "label", "");
}

static void
_mbar_icon_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   MBar_Icon *ic;

   ev = event_info;
   ic = data;
   if (ev->button == 1)
     {
	drag_x = ev->output.x;
	drag_y = ev->output.y;
	drag_start = 1;
	drag = 0;
     }
}

static void
_mbar_icon_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   MBar_Icon *ic;
   int ret, mounted;
   
   ev = event_info;
   ic = data;
   if (ev->button == 1)
     {
	if (!drag)
	  {	
	     _mbar_exe_exit_handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _mbar_exe_cb_exit, NULL);
	     #ifdef HAVE_LINUX
	     mounted = _mbar_is_mounted(ic->app->generic);
	     #endif
	     #ifdef HAVE_BSD
	     mounted = _mbar_bsd_is_mounted(ic->app->generic);	     
	     #endif
	     _mbar_set_state(ic, mounted);
	     if (!mounted)
	       _mbar_mount_point(ic);
	     else 
	       {
#ifdef HAVE_EJECT
		  _mbar_eject_point(ic);
#else
		  _mbar_umount_point(ic);
#endif
	       }
	  }
	drag = 0;
	drag_start = 0;
     }
}

static void
_mbar_icon_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   MBar_Icon *ic;

   ev = event_info;
   ic = data;
   
   if (drag_start)
     {
	int dx, dy;

	dx = ev->cur.output.x - drag_x;
	dy = ev->cur.output.y - drag_y;
	if (((dx * dx) + (dy * dy)) >
	    (e_config->drag_resist * e_config->drag_resist))
	  {
	     E_Drag *d;
	     Evas_Object *o;
	     Evas_Coord x, y, w, h;
	     const char *drag_types[] = { "enlightenment/eapp" };

	     drag = 1;
	     drag_start = 0;

	     evas_object_geometry_get(ic->icon_object,
				      &x, &y, &w, &h);
	     d = e_drag_new(ic->mbb->con, x, y, drag_types, 1,
			    ic->app, -1, _mbar_bar_cb_finished);
	     o = edje_object_add(e_drag_evas_get(d));
	     edje_object_file_set(o, ic->app->path, "icon");
	     e_drag_object_set(d, o);

	     e_drag_resize(d, w, h);
	     e_drag_start(d, drag_x, drag_y);
	     evas_event_feed_mouse_up(ic->mbb->evas, 1, EVAS_BUTTON_NONE, ev->timestamp, NULL);
	     e_app_remove(ic->app);
	  }
     }
}

static void
_mbar_bar_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   MBar_Bar *mbb;

   ev = event_info;
   mbb = data;
   if (mbb->overlay_object)
     edje_object_signal_emit(mbb->overlay_object, "active", "");
   _mbar_bar_motion_handle(mbb, ev->canvas.x, ev->canvas.y);
   _mbar_bar_timer_handle(mbb);
}

static void
_mbar_bar_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   MBar_Bar *mbb;

   ev = event_info;
   mbb = data;
   if (mbb->overlay_object)
     edje_object_signal_emit(mbb->overlay_object, "passive", "");
   _mbar_bar_follower_reset(mbb);
   _mbar_bar_timer_handle(mbb);
}

static void
_mbar_bar_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   MBar_Bar *mbb;

   ev = event_info;
   mbb = data;
   if (ev->button == 3)
     {
	e_menu_activate_mouse(mbb->menu, e_zone_current_get(mbb->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_later(mbb->con, 3);
     }
}

static void
_mbar_bar_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   MBar_Bar *mbb;

   ev = event_info;
   mbb = data;
}

static void
_mbar_bar_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   MBar_Bar *mbb;

   ev = event_info;
   mbb = data;
   _mbar_bar_motion_handle(mbb, ev->cur.canvas.x, ev->cur.canvas.y);
   _mbar_bar_timer_handle(mbb);
}

static int
_mbar_bar_cb_timer(void *data)
{
   MBar_Bar *mbb;
   double dif, dif2;
   double v;

   mbb = data;
   v = mbb->mbar->conf->autoscroll_speed;
   mbb->align = (mbb->align_req * (1.0 - v)) + (mbb->align * v);
   v = mbb->mbar->conf->follow_speed;
   mbb->follow = (mbb->follow_req * (1.0 - v)) + (mbb->follow * v);

   dif = mbb->align - mbb->align_req;
   if (dif < 0) dif = -dif;
   if (dif < 0.001) mbb->align = mbb->align_req;

   dif2 = mbb->follow - mbb->follow_req;
   if (dif2 < 0) dif2 = -dif2;
   if (dif2 < 0.001) mbb->follow = mbb->follow_req;

   if ((dif < 0.001) && (dif2 < 0.001))
     {
	mbb->timer = NULL;
	return 0;
     }
   return 1;
}

static int
_mbar_bar_cb_animator(void *data)
{
   MBar_Bar *mbb;
   Evas_Coord x, y, w, h, mw, mh;

   mbb = data;

   if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_BOTTOM) ||
       (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_TOP))
     {
	e_box_min_size_get(mbb->box_object, &mw, &mh);
	evas_object_geometry_get(mbb->box_object, NULL, NULL, &w, &h);
	if (mw > w)
	  e_box_align_set(mbb->box_object, mbb->align, 0.5);
	else
	  e_box_align_set(mbb->box_object, 0.5, 0.5);

	if (mbb->overlay_object)
	  {
	     evas_object_geometry_get(mbb->box_object, &x, &y, &w, &h);
	     edje_object_size_min_get(mbb->overlay_object, &mw, &mh);
	     evas_object_resize(mbb->overlay_object, mw, h);
	     evas_object_move(mbb->overlay_object, x + (w * mbb->follow) - (mw / 2), y);
	  }
     }
   else if ((e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_LEFT) ||
	    (e_gadman_client_edge_get(mbb->gmc) == E_GADMAN_EDGE_RIGHT))
     {
	e_box_min_size_get(mbb->box_object, &mw, &mh);
	evas_object_geometry_get(mbb->box_object, NULL, NULL, &w, &h);
	if (mh > h)
	  e_box_align_set(mbb->box_object, 0.5, mbb->align);
	else
	  e_box_align_set(mbb->box_object, 0.5, 0.5);

	if (mbb->overlay_object)
	  {
	     evas_object_geometry_get(mbb->box_object, &x, &y, &w, &h);
	     edje_object_size_min_get(mbb->overlay_object, &mw, &mh);
	     evas_object_resize(mbb->overlay_object, w, mh);
	     evas_object_move(mbb->overlay_object, x, y + (h * mbb->follow) - (mh / 2));
	  }
     }
   if (mbb->timer) 
     return 1;
   mbb->animator = NULL;
   return 0;
}

static void
_mbar_bar_cb_enter(void *data, const char *type, void *event)
{
   E_Event_Dnd_Enter *ev;
   Evas_Object *o, *o2;
   MBar_Bar *mbb;
   MBar_Icon *ic;
   int w, h;
   char buf[4096];
   
   ev = event;
   mbb = data;

   o = edje_object_add(mbb->evas);
   mbb->drag_object = o;
   o2 = edje_object_add(mbb->evas);
   mbb->drag_object_overlay = o2;
   evas_object_intercept_move_callback_add(o, _mbar_drag_cb_intercept_move, o2);
   evas_object_intercept_resize_callback_add(o, _mbar_drag_cb_intercept_resize, o2);
   if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/drop"))
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
	edje_object_file_set(o, buf, "modules/mbar/drop");
     }
   if (!e_theme_edje_object_set(o2, "base/theme/modules/mbar", "modules/mbar/drop_overlay"))
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
	edje_object_file_set(o2, buf, "modules/mbar/drop_overlay");
     }
   edje_object_signal_emit(o, "set_orientation",
			   _mbar_main_orientation[e_gadman_client_edge_get(mbb->gmc)]);
   edje_object_signal_emit(o2, "set_orientation",
			   _mbar_main_orientation[e_gadman_client_edge_get(mbb->gmc)]);
   evas_object_resize(o, mbb->mbar->conf->iconsize, mbb->mbar->conf->iconsize);

   ic = _mbar_icon_pos_find(mbb, ev->x, ev->y);
   
   e_box_freeze(mbb->box_object);
   evas_object_show(mbb->drag_object);
   evas_object_show(mbb->drag_object_overlay);
   if (ic)
     {
	/* Add new eapp before this icon */
	e_box_pack_before(mbb->box_object, mbb->drag_object, ic->bg_object);
     }
   else
     {
	/* Add at the end */
	e_box_pack_end(mbb->box_object, mbb->drag_object);
     }
   w = mbb->mbar->conf->iconsize + mbb->icon_inset.l + mbb->icon_inset.r;
   h = mbb->mbar->conf->iconsize + mbb->icon_inset.t + mbb->icon_inset.b;
   e_box_pack_options_set(mbb->drag_object,
			  1, 1, /* fill */
			  0, 0, /* expand */
			  0.5, 0.5, /* align */
			  w, h, /* min */
			  w, h /* max */
			  );
   e_box_thaw(mbb->box_object);

   _mbar_bar_frame_resize(mbb);
}

static void
_mbar_bar_cb_move(void *data, const char *type, void *event)
{
   E_Event_Dnd_Move *ev;
   MBar_Bar *mbb;
   MBar_Icon *ic;
   int w, h;

   ev = event;
   mbb = data;

   ic = _mbar_icon_pos_find(mbb, ev->x, ev->y);

   e_box_freeze(mbb->box_object);
   evas_object_show(mbb->drag_object);
   e_box_unpack(mbb->drag_object);
   if (ic)
     {
	/* Add new eapp before this icon */
	e_box_pack_before(mbb->box_object, mbb->drag_object, ic->bg_object);
     }
   else
     {
	/* Add at the end */
	e_box_pack_end(mbb->box_object, mbb->drag_object);
     }
   w = mbb->mbar->conf->iconsize + mbb->icon_inset.l + mbb->icon_inset.r;
   h = mbb->mbar->conf->iconsize + mbb->icon_inset.t + mbb->icon_inset.b;
   e_box_pack_options_set(mbb->drag_object,
			  1, 1, /* fill */
			  0, 0, /* expand */
			  0.5, 0.5, /* align */
			  w, h, /* min */
			  w, h /* max */
			  );
   e_box_thaw(mbb->box_object);

   _mbar_bar_frame_resize(mbb);
}

static void
_mbar_bar_cb_leave(void *data, const char *type, void *event)
{
   E_Event_Dnd_Leave *ev;
   MBar_Bar *mbb;

   ev = event;
   mbb = data;

   e_box_freeze(mbb->box_object);
   e_box_unpack(mbb->drag_object);
   evas_object_del(mbb->drag_object);
   mbb->drag_object = NULL;
   evas_object_del(mbb->drag_object_overlay);
   mbb->drag_object_overlay = NULL;
   e_box_thaw(mbb->box_object);

   _mbar_bar_frame_resize(mbb);
}

static void
_mbar_bar_cb_drop(void *data, const char *type, void *event)
{
   E_Event_Dnd_Drop *ev;
   E_App *app = NULL;
   Evas_List *l = NULL;
   MBar_Bar *mbb;
   MBar_Icon *ic;
   
   ev = event;
   mbb = data;
   if (!strcmp(type, "enlightenment/eapp"))
     app = ev->data;
   else
     return;

   /* add dropped element */
   ic = _mbar_icon_pos_find(mbb, ev->x, ev->y);

   /* remove drag marker */
   e_box_freeze(mbb->box_object);
   e_box_unpack(mbb->drag_object);
   evas_object_del(mbb->drag_object);
   mbb->drag_object = NULL;
   evas_object_del(mbb->drag_object_overlay);
   mbb->drag_object_overlay = NULL;
   e_box_thaw(mbb->box_object);

   _mbar_bar_frame_resize(mbb);

   if (ic)
     {
	/* Add new eapp before this icon */
	if (app)
	  e_app_prepend_relative(app, ic->app);
	else if (l)
	  e_app_files_prepend_relative(l, ic->app);
     }
   else
     {
	/* Add at the end */
	if (app)
	  e_app_append(app, mbb->mbar->apps);
	else if (l)
	  e_app_files_append(l, mbb->mbar->apps);
     }
}

static void
_mbar_bar_cb_finished(E_Drag *drag, int dropped)
{
   /* Unref the object so it will be deleted. */
   if (!dropped)
     e_object_unref(E_OBJECT(drag->data));
}

static void
_mbar_bar_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   MBar_Bar *mbb;

   mbb = data;
   switch (change)
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(mbb->gmc, &mbb->x, &mbb->y, &mbb->w, &mbb->h);

	edje_extern_object_min_size_set(mbb->box_object, 0, 0);
	edje_object_part_swallow(mbb->bar_object, "items", mbb->box_object);

	evas_object_move(mbb->bar_object, mbb->x, mbb->y);
	evas_object_resize(mbb->bar_object, mbb->w, mbb->h);

	_mbar_bar_follower_reset(mbb);
	_mbar_bar_timer_handle(mbb);

	e_drop_handler_geometry_set(mbb->drop_handler,
				    mbb->x + mbb->bar_inset.l, mbb->y + mbb->bar_inset.t,
				    mbb->w - (mbb->bar_inset.l + mbb->bar_inset.r),
				    mbb->h - (mbb->bar_inset.t + mbb->bar_inset.b));
	break;
      case E_GADMAN_CHANGE_EDGE:
	_mbar_bar_edge_change(mbb, e_gadman_client_edge_get(mbb->gmc));
	break;
      case E_GADMAN_CHANGE_RAISE:
      case E_GADMAN_CHANGE_ZONE:
	 /* FIXME
	  * Must we do something here?
	  */
	break;
     }
}

void 
_mbar_bar_cb_config_updated(void *data) 
{   
   /* Call Any Needed Funcs To Let Module Handle Config Changes */
   _mbar_bar_cb_follower(data);
   _mbar_bar_cb_width_auto(data);
   _mbar_bar_cb_iconsize_change(data);
}

static void
_mbar_bar_cb_width_auto(void *data)
{
   MBar          *mb;
   MBar_Bar      *mbb;
   Evas_List     *l;

   mb = data;
   for (l = mb->bars; l; l = l->next)
     {
	mbb = l->data;
	_mbar_bar_update_policy(mbb);
	_mbar_bar_frame_resize(mbb);
     }
}

static void
_mbar_bar_cb_follower(void *data)
{
   MBar          *mb;
   MBar_Bar      *mbb;
   unsigned char  enabled;
   Evas_List     *l;
   char buf[4096];

   mb = data;
   enabled = mb->conf->follower;
   if (enabled)
     {
	for (l = mb->bars; l; l = l->next)
	  {
	     Evas_Object *o;

	     mbb = l->data;
	     if (mbb->overlay_object) 
	       continue;
	     o = edje_object_add(mbb->evas);
	     mbb->overlay_object = o;
	     evas_object_layer_set(o, 2);
	     if (!e_theme_edje_object_set(o, "base/theme/modules/mbar", "modules/mbar/follower"))
	       {
		  snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mbar.edj");	
		  edje_object_file_set(o, buf, "modules/mbar/follower");
	       }
	     edje_object_signal_emit(o, "set_orientation", _mbar_main_orientation[e_gadman_client_edge_get(mbb->gmc)]);
	     edje_object_message_signal_process(o);

	     evas_object_show(o);
	  }
     }
   else if (!enabled)
     {
	for (l = mb->bars; l; l = l->next)
	  {
	     mbb = l->data;
	     if (!mbb->overlay_object) 
	       continue;
	     evas_object_del(mbb->overlay_object);
	     mbb->overlay_object = NULL;
	  }
     }
}

static void
_mbar_bar_cb_iconsize_change(void *data)
{
   MBar *mb;
   Evas_List *l, *ll;

   mb = data;
   for (l = mb->bars; l; l = l->next)
     {
	MBar_Bar *mbb;

	mbb = l->data;

	e_box_freeze(mbb->box_object);
	for (ll = mbb->icons; ll; ll = ll->next)
	  {
	     MBar_Icon *ic;
	     Evas_Object *o;
	     Evas_Coord w, h;

	     ic = ll->data;
	     o = ic->icon_object;

	     evas_object_resize(o, mbb->mbar->conf->iconsize, mbb->mbar->conf->iconsize);
	     edje_object_part_swallow(ic->bg_object, "item", o);

	     w = mbb->mbar->conf->iconsize + mbb->icon_inset.l + mbb->icon_inset.r;
	     h = mbb->mbar->conf->iconsize + mbb->icon_inset.t + mbb->icon_inset.b;
	     e_box_pack_options_set(ic->bg_object,
				    1, 1, /* fill */
				    0, 0, /* expand */
				    0.5, 0.5, /* align */
				    w, h, /* min */
				    w, h /* max */
				    );
	  }
	e_box_thaw(mbb->box_object);
	_mbar_bar_frame_resize(mbb);
     }
}

static void
_mbar_bar_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi)
{
   MBar_Bar *mbb;

   mbb = data;
   e_gadman_mode_set(mbb->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void
_mbar_drag_cb_intercept_move(void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   evas_object_move(o, x, y);
   evas_object_move(data, x, y);
}

static void
_mbar_drag_cb_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   evas_object_resize(o, w, h);
   evas_object_resize(data, w, h);
}

static int 
_mbar_is_mounted(const char *path) 
{
   int mounted;
   
   mounted = _mbar_parse_file(PROCMOUNTS, path);
   if (mounted <= 0) 
     {
	mounted = _mbar_parse_file(MTAB, path);
	if (mounted <= 0)
	  mounted = 0;
     }
   return mounted;
}

static void 
_mbar_mount_point(MBar_Icon *ic) 
{
   char buf[1024];
   Ecore_Exe *x;
   Evas_List *ll;
   
   edje_object_signal_emit(ic->icon_object, "start", "");
   for (ll = ic->extra_icons; ll; ll = ll->next) 
     edje_object_signal_emit(ll->data, "start", "");
   edje_object_signal_emit(ic->bg_object, "start", "");
   edje_object_signal_emit(ic->overlay_object, "start", "");
   if (ic->mbb->overlay_object)
     edje_object_signal_emit(ic->mbb->overlay_object, "start", "");
   
   snprintf(buf, sizeof(buf), MOUNT" %s", ic->app->generic);
   x = ecore_exe_run(buf, ic);
   if (x)
     ecore_exe_tag_set(x, "Mount");
}

static void 
_mbar_umount_point(MBar_Icon *ic) 
{
   char buf[1024];
   Ecore_Exe *x;
   Evas_List *ll;
   
   edje_object_signal_emit(ic->icon_object, "start", "");
   for (ll = ic->extra_icons; ll; ll = ll->next) 
     edje_object_signal_emit(ll->data, "start", "");
   edje_object_signal_emit(ic->bg_object, "start", "");
   edje_object_signal_emit(ic->overlay_object, "start", "");
   if (ic->mbb->overlay_object)
     edje_object_signal_emit(ic->mbb->overlay_object, "start", "");
   
   snprintf(buf, sizeof(buf), UMOUNT" %s", ic->app->generic);
   x = ecore_exe_run(buf, ic);
   if (x)
     ecore_exe_tag_set(x, "Unount");   
}

static void 
_mbar_eject_point(MBar_Icon *ic) 
{
   char buf[1024];
   Ecore_Exe *x;
   
   snprintf(buf, sizeof(buf), EJECT" %s", ic->app->generic);
   x = ecore_exe_run(buf, ic);
   if (x)
     ecore_exe_tag_set(x, "Eject");   
}

static void 
_mbar_set_state(MBar_Icon *ic, int mounted) 
{
   if (mounted)
     edje_object_signal_emit(ic->overlay_object, "set_mounted", "");
   else
     edje_object_signal_emit(ic->overlay_object, "set_unmounted", "");     
}

static int 
_mbar_exe_cb_exit(void *data, int type, void *event) 
{
   Ecore_Exe_Event_Del *ev;
   Ecore_Exe *x;
   Evas_List *ll;
   MBar_Icon *ic;
   int mounted;
   char *tag;
   
   ev = event;
   if (!ev->exe)
     return 1;
   x = ev->exe;
   if (!x)
     return 1;
   
   ic = ecore_exe_data_get(x);
   tag = ecore_exe_tag_get(x);
   #ifdef HAVE_LINUX
   mounted = _mbar_is_mounted(ic->app->generic);
   #endif
   #ifdef HAVE_BSD
   mounted = _mbar_bsd_is_mounted(ic->app->generic);   
   #endif
   x = NULL;
   ecore_event_handler_del(_mbar_exe_exit_handler);
   
   if (!strcmp(tag, "Unmount")) 
     {
	if (mounted)
	  e_module_dialog_show(_("Mount Bar Module"), _("Unmount Failed."));
     }
   else if (!strcmp(tag, "Mount")) 
     {
	if (!mounted)
	  e_module_dialog_show(_("Mount Bar Module"), _("Mount Failed."));
	else 
	  {
	     if (ic->app->exe) 
	       {
		  e_zone_app_exec(ic->mbb->gmc->zone, ic->app);
		  e_exehist_add("mbar", ic->app->exe);
	       }
	  }
     }
   else if (!strcmp(tag, "Eject")) 
     {
	if (mounted)
	  e_module_dialog_show(_("Mount Bar Module"), _("Eject Failed."));
     }
   _mbar_set_state(ic, mounted);
   edje_object_signal_emit(ic->icon_object, "stop", "");
   for (ll = ic->extra_icons; ll; ll = ll->next) 
     edje_object_signal_emit(ll->data, "stop", "");
   edje_object_signal_emit(ic->bg_object, "stop", "");
   edje_object_signal_emit(ic->overlay_object, "stop", "");
   if (ic->mbb->overlay_object)
     edje_object_signal_emit(ic->mbb->overlay_object, "stop", "");
   
   return 0;
}

static int 
_mbar_parse_file(char *file, const char *mntpath) 
{
   FILE *f;
   char s[1024];
   char *token, *device, *path = NULL;
   int mounted;
   
   mounted = 0;
   if ((f = fopen(file, "r")) == NULL) 
     return -1;
   
   *s = 0;
   for (; fgets(s, sizeof(s), f);) 
     {
	/* Skip Comments */
	if (!(*s) || (*s == '\n') || (*s == '#')) 
	  continue;
	token = strtok(s, " \t");
	if (token) 
	  device = strdup(token);
	token = NULL;
	token = strtok(NULL, " \t");
	if (token) 
	  path = strdup(token);
	if (path) 
	  {
	     if (!strcmp(mntpath, path)) mounted = 1;
	  }
     }
   fclose(f);
   free(device);
   free(path);
   return mounted;
}

static void 
_mbar_parse_fstab(MBar *mb) 
{
   FILE *f;
   char s[1024];
   char *info[4];
   char *p;
   int i, u;
   char *token = NULL;
   
   if ((f = fopen(FSTAB, "r")) == NULL) 
     return;
   
   *s = 0;
   for (; fgets(s, sizeof(s), f);) 
     {
	p = s;
	while (*p && isspace(*p))
	  p++;
	
	if (!(*p) || (*p == '\n') || (*p == '#'))
	  continue;
	
	for (i = 0; i < 4; i++) 
	  info[i] = NULL;
	
	i = 0;
	token = strtok(p, " \t");
	if (!token)
	  continue;
	
	info[i++] = strdup(token);
	while ((token = strtok(NULL, " \t")) && (i < 4))
	  info[i++] = strdup(token);
	if ((i != 4) || (!(*info[--i])))
	  continue;

	u = 0;
	for (p = strtok(info[3], ","); p; p = strtok(NULL, ",")) 
	  {
	     if (!strcmp(p, "nouser"))
	       u = 0;
	     else if ((!strcmp(p, "user")) || (!strcmp(p, "users")))
	       u = 1;
	     else if (!strcmp(p, "owner")) 
	       {
		  struct stat st;
		  if (stat(info[0], &st) == 0 && st.st_uid == geteuid()) 
		    {
		       u = 1;
		       break;
		    }		  
	       }
	     else if (!strcmp(p, "group")) 
	       {
		  struct stat st;
		  if (stat(info[0], &st) == 0) 
		    {
		       gid_t gids[NGROUPS_MAX];
		       int gidn;
		       if ((gidn = getgroups(NGROUPS_MAX, gids)) != -1) 
			 {
			    while (gidn >= 0 && st.st_gid != gids[gidn])
			      gidn--;
			    if (gidn != -1) 
			      {
				 u = 1;
				 break;
			      }
			 }
		    }
	       }
	  }
	
	if (u) 
	  {
	     char icon[4096];
	     char path[4096];
	     E_App *a;

	     snprintf(path, sizeof(path), 
		      "%s/.e/e/applications/all/%s.eap", 
		      getenv("HOME"), basename(info[0]));
	     if (!ecore_file_exists(path)) 
	       {
		  a = e_app_raw_new();
		  snprintf(icon, sizeof(icon), PACKAGE_DATA_DIR"/e.png");
		  a->path = evas_stringshare_add(path);
		  a->name = evas_stringshare_add(info[0]);
		  a->generic = evas_stringshare_add(info[1]);
		  a->image = evas_stringshare_add(PACKAGE_DATA_DIR"/module_icon.png");	     
		  a->startup_notify = 1;
		  e_app_fields_save(a);
		  e_app_append(a, mb->apps);
	     
		  /* Add to order */
		  char eap[1024];
		  snprintf(eap, sizeof(eap), "%s.eap", basename(info[0]));
		  _mbar_add_order(mb->conf->appdir, eap);
	       }
	  }
	
	for (i = 0; i < 4; i++) 
	  {
	     if (info[i])
	       free(info[i]);
	  }
     }
   fclose(f);
}

static void 
_mbar_add_order(const char *dir, const char *name) 
{
   FILE *f;
   char path[4096];
   
   snprintf(path, sizeof(path), 
	    "%s/.e/e/applications/%s/.order", getenv("HOME"), dir);

   if (!ecore_file_exists(path)) 
     {
	f = fopen(path, "w");
	if (!f)
	  return;
	fclose(f);
     }
   
   f = fopen(path, "w+");
   if (!f)
     return;
   fwrite(name, sizeof(char), strlen(name), f);
   fclose(f);
}

static void 
_mbar_mtab_update(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path) 
{
   MBar *mb;
   Evas_List *l, *il;
   const char *file;
   
   mb = data;
   if (!mb)
     return;
   
   file = ecore_file_get_file((char *)path);
   if (!strcmp(file, "mtab")) 
     {
	if (event == ECORE_FILE_EVENT_MODIFIED) 
	  {
	     for (l = mb->bars; l; l = l->next) 
	       {
		  MBar_Bar *mbb;
		  mbb = l->data;
		  if (!mbb)
		    continue;
		  for (il = mbb->icons; il; il = il->next) 
		    {
		       MBar_Icon *ic;
		       int mounted;
		       
		       ic = il->data;
		       #ifdef HAVE_LINUX
		       mounted = _mbar_is_mounted(ic->app->generic);
		       #endif
		       #ifdef HAVE_BSD
		       mounted = _mbar_bsd_is_mounted(ic->app->generic);
		       #endif
		       _mbar_set_state(ic, mounted);
		    }		  
	       }
	  }
     }
}

#ifdef HAVE_BSD
static int 
_mbar_bsd_is_mounted(const char *path) 
{
   struct statfs *mnts;
   int num, i, mounted;
   
   mounted = 0;
   num = getmntinfo(&mnts, MNT_NOWAIT);
   for (i = 0; i < num; i++) 
     {
	if (!strcmp(mnts[i].f_mntonname, path)) 
	  mounted = 1;	
     }
   return mounted;
}

static int 
_mbar_bsd_cb_timer(void *data) 
{
   MBar *mb;
   Evas_List *l, *il;
   
   mb = data;
   if (!mb)
     return;
   
   for (l = mb->bars; l; l = l->next) 
     {
	MBar_Bar *mbb;
	mbb = l->data;
	if (!mbb)
	  continue;
	for (il = mbb->icons; il; il = il->next) 
	  {
	     MBar_Icon *ic;
	     int mounted;
	     
	     ic = il->data;
	     mounted = _mbar_bsd_is_mounted(ic->app->generic);
	     _mbar_set_state(ic, mounted);
	  }		  
     }
}
#endif
