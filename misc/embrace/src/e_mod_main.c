#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ltdl.h>

#include <e.h>
#include "e_mod_main.h"
#include "embrace.h"

/* module private routines */
static Embrace      *_embrace_new ();
static void          _embrace_free (Embrace *embrace);
static E_Menu       *_embrace_config_menu_new (void);

#if 0
static void          _embrace_face_cb_gmc_change (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);

static void          _embrace_desk_cb_mouse_in (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_out (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_up (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_move (void *data, Evas *e, Evas_Object *obj, void *event_info);

static void          _embrace_desk_cb_intercept_move (void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void          _embrace_desk_cb_intercept_resize (void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
#endif

static int           _embrace_count;

/* public module routines. all modules must have these */
void *e_modapi_init (E_Module *module) {
	Embrace *embrace = NULL;

	/* check module api version */
	if (module->api->version < E_MODULE_API_VERSION) {
		e_error_dialog_show ( _("Module API Error"),
				      _("Error initializing Module: Embrace\n"
					"It requires a minimum module API version of: %i.\n"
				       	"The module API advertized by Enlightenment is: %i.\n"
				       	"Aborting module."),
				      E_MODULE_API_VERSION,
				      module->api->version);
		return NULL;
	}
	/* actually init embrace */
	embrace = _embrace_new (module);
	module->config_menu = _embrace_config_menu_new ();


	return embrace;
}

int e_modapi_shutdown (E_Module *module) {
	Embrace *embrace;

	if (module->config_menu)
		e_object_del (E_OBJECT (module->config_menu));

	embrace = module->data;
	if (embrace)
		_embrace_free (embrace);

	return 1;
}

int e_modapi_save (E_Module *module) {
	return 1;
}

int e_modapi_info (E_Module *module) {
#if 0
	char buf[4096];
#endif

	module->label = strdup ( _("Embrace"));
#if 0
	snprintf (buf, sizeof (buf), "%s/module_icon.png", e_module_dir_get (module));
	module->icon_file = strdup (buf);
#endif
	return 1;
}

int e_modapi_about (E_Module *module) {
	e_error_dialog_show ( _("Enlightenment Embrace Module"),
			      _("A module to check your email."));
	return 1;
}

/* module private routines */
static Embrace *_embrace_new () {
	Embrace       *embrace;

#if 0
	Evas_List   *managers, *l, *l2, *l3;
#endif
	E_Manager   *man;
	E_Container *con;
#if 0
	E_Zone      *zone;
	E_Menu      *mn;
	E_Menu_Item *mi;
#endif

	_embrace_count = 0;

	if (lt_dlinit ()) {
		fprintf (stderr, "Cannot initialize LTDL!\n");
		return NULL;
	}

	embrace = embrace_new ();
	if (!embrace) return NULL;

	man = e_manager_current_get();
	con = e_container_current_get(man);
	embrace->gui.evas = con->bg_evas;
	embrace->cfg.module = 1;

	embrace_init (embrace);

#if 0
	managers = e_manager_list ();
	for (l = managers; l; l = l->next) {
		man = l->data;

		for (l2 = man->containers; l2; l2 = l2->next) {
			con = l2->data;

			mi = e_menu_item_new (embrace->config_menu);
			e_menu_item_label_set (mi, con->name);

			mn = e_menu_new ();
			e_menu_item_submenu_set (mi, mn);
			embrace->menus = evas_list_append (embrace->menus, mn);

			for (l3 = con->zones; l3; l3 = l3->next) {
				zone = l3->data;

				face = _embrace_face_new (embrace, zone);
				if (face) {
					embrace->faces = evas_list_append (embrace->faces, face);

					/* Menu */
					_embrace_face_menu_new (face);

					mi = e_menu_item_new (mn);
					e_menu_item_label_set (mi, zone->name);
					e_menu_item_submenu_set (mi, face->menu);
				}
			}
		}
	}
#endif
	embrace_run (embrace);
	return embrace;
}

static void _embrace_free (Embrace *embrace) {
#if 0
	Evas_List *l;
#endif

	embrace_stop (embrace);
	embrace_deinit (embrace);

#if 0
	for (l = embrace->faces; l; l = l->next)
		_embrace_face_free (l->data);
	evas_list_free (embrace->faces);
#endif

#if 0
	for (l = embrace->menus; l; l = l->next)
		e_object_del (E_OBJECT (l->data));
	evas_list_free (embrace->menus);
#endif

	embrace_free (embrace);

	lt_dlexit ();
}

static E_Menu *_embrace_config_menu_new (void)
{
	E_Menu *m;
	E_Menu_Item *mi;

	m = e_menu_new ();

	mi = e_menu_item_new(m);
	e_menu_item_label_set(mi, _("(Empty)"));

	return m;
}

#if 0
static Embrace_Face *_embrace_face_new (Embrace *embrace, E_Zone *zone) {
	Embrace_Face  *face;
	Evas_Object *o;
	Evas_Coord   x, y, w, h;

	face = E_NEW (Embrace_Face, 1);
	if (!face) return NULL;

	/* store what evas we live in */
	face->evas = zone->container->bg_evas;

	/* the bg */
	o = edje_object_add (face->evas);
	face->mailbox_object = o;
	edje_object_file_set (o, embrace->conf->theme, "MailBox");
	evas_object_show (o);

	o = e_box_add (face->evas);
	face->table_object = o;
	e_table_homogenous_set (o, 1);   
	edje_object_part_swallow (face->embrace_object, "items", face->table_object);
	evas_object_show (o);

	face->gmc = e_gadman_client_new (zone->container->gadman);

	e_gadman_client_domain_set (face->gmc, "module.embrace", _embrace_count++);
	e_gadman_client_zone_set (face->gmc, face->zone);
	e_gadman_client_policy_set (face->gmc,
			//			      E_GADMAN_POLICY_FIXED_ZONE | 
			E_GADMAN_POLICY_ANYWHERE |
			E_GADMAN_POLICY_HMOVE |
			E_GADMAN_POLICY_VMOVE |
			E_GADMAN_POLICY_HSIZE |
			E_GADMAN_POLICY_VSIZE);
	e_gadman_client_min_size_set (face->gmc, 8, 8);
	e_gadman_client_max_size_set (face->gmc, 600, 600);
	e_gadman_client_auto_size_set (face->gmc, 186, 40);
	e_gadman_client_align_set (face->gmc, 0.0, 1.0);
	e_gadman_client_resize (face->gmc, 186, 40);
	e_gadman_client_change_func_set (face->gmc, _embrace_face_cb_gmc_change, face);
	e_gadman_client_load (face->gmc);

	return face;
}

void
_embrace_face_free (Embrace_Face *face)
{
   if (face->embrace_object) evas_object_del (face->embrace_object);
   if (face->table_object) evas_object_del (face->table_object);
   e_gadman_client_save (face->gmc);
   e_object_del (E_OBJECT (face->gmc));

   e_drop_handler_del (face->drop_handler);

   _embrace_face_zone_unset (face);
   ecore_event_handler_del (face->ev_handler_border_resize);
   ecore_event_handler_del (face->ev_handler_border_move);
   ecore_event_handler_del (face->ev_handler_border_add);
   ecore_event_handler_del (face->ev_handler_border_remove);
   ecore_event_handler_del (face->ev_handler_border_iconify);
   ecore_event_handler_del (face->ev_handler_border_uniconify);
   ecore_event_handler_del (face->ev_handler_border_stick);
   ecore_event_handler_del (face->ev_handler_border_unstick);
   ecore_event_handler_del (face->ev_handler_border_desk_set);
   ecore_event_handler_del (face->ev_handler_border_raise);
   ecore_event_handler_del (face->ev_handler_border_lower);
   ecore_event_handler_del (face->ev_handler_border_icon_change);
   ecore_event_handler_del (face->ev_handler_zone_desk_count_set);
   ecore_event_handler_del (face->ev_handler_desk_show);
   ecore_event_handler_del (face->ev_handler_container_resize);

   e_object_del (E_OBJECT (face->menu));

   free (face->conf);
   free (face);
   
   _embrace_count--;
}

static void
_embrace_face_menu_new (Embrace_Face *face)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new ();
   face->menu = mn;

   mi = e_menu_item_new (mn);
   e_menu_item_label_set (mi, _("Edit Mode"));
   e_menu_item_callback_set (mi, _embrace_face_cb_menu_edit, face);
}

static void
_embrace_face_cb_gmc_change (void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   Embrace_Face *face;
   Evas_Coord  x, y, w, h;

   face = data;
   e_gadman_client_geometry_get (face->gmc, &x, &y, &w, &h);
   face->fx = x;
   face->fy = y;
   face->fw = w;
   face->fh = h;
   e_drop_handler_geometry_set (face->drop_handler,
			       face->fx + face->inset.l, face->fy + face->inset.t,
			       face->fw - (face->inset.l + face->inset.r),
			       face->fh - (face->inset.t + face->inset.b));
   switch (change)
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	evas_object_move (face->embrace_object, face->fx, face->fy);
	evas_object_resize (face->embrace_object, face->fw, face->fh);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise (face->embrace_object);
	break;
      default:
	break;
     }
}

/*****/

static void
_embrace_face_cb_menu_edit (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Embrace_Face *face;

   face = data;
   e_gadman_mode_set (face->gmc->gadman, E_GADMAN_MODE_EDIT);
}

/*****/

static void
_embrace_desk_cb_mouse_in (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Embrace_Desk *desk;
   
   ev = event_info;
   desk = data;
}

static void
_embrace_desk_cb_mouse_out (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Embrace_Desk *desk;
   
   ev = event_info;
   desk = data;
}

static void
_embrace_desk_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Embrace_Desk *pd;
   
   ev = event_info;
   pd = data;
   if (ev->button == 3)
     {
	e_menu_activate_mouse (pd->face->menu, pd->face->zone,
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later (pd->face->zone->container);
     }
   else if (ev->button == 1)
     {
	e_desk_show (pd->desk);
     }
}

static void
_embrace_desk_cb_mouse_up (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Embrace_Desk *desk;
   
   ev = event_info;
   desk = data;
}

static void
_embrace_desk_cb_mouse_move (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   Embrace_Desk *desk;
   
   ev = event_info;
   desk = data;
}

/*****/

static void
_embrace_desk_cb_intercept_move (void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Embrace_Desk *desk;
   
   desk = data;
   evas_object_move (o, x, y);
   evas_object_move (desk->event_object, x, y);
}

static void
_embrace_desk_cb_intercept_resize (void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Embrace_Desk *desk;
   
   desk = data;
   evas_object_resize (o, w, h);
   evas_object_resize (desk->event_object, w, h);
}
#endif
