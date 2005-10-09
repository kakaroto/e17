#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ltdl.h>

#include <e.h>
#include "embrace.h"
#include "e_mod_main.h"

/* module private routines */
static EmbraceModule *embrace_module_new (void);
static void           embrace_module_free (EmbraceModule *em);
static E_Menu        *embrace_config_menu_new (void);

static void           embrace_cb_gmc_change (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void           embrace_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void           embrace_cb_mouse_up (void *data, Evas *e, Evas_Object *obj, void *event_info);

#if 0
static void          _embrace_desk_cb_mouse_in (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_out (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_up (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void          _embrace_desk_cb_mouse_move (void *data, Evas *e, Evas_Object *obj, void *event_info);

static void          _embrace_desk_cb_intercept_move (void *data, Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void          _embrace_desk_cb_intercept_resize (void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
#endif

static int              embrace_count;

/* public module routines. all modules must have these */

E_Module_Api e_modapi = {
	E_MODULE_API_VERSION,
	"Embrace"
};

void *e_modapi_init (E_Module *module)
{
	EmbraceModule *em = NULL;

	/* actually init embrace */
	em = embrace_module_new ();
	module->config_menu = embrace_config_menu_new ();

	return em;
}

int e_modapi_shutdown (E_Module *module)
{
	EmbraceModule *em;

	if (module->config_menu)
		e_object_del (E_OBJECT (module->config_menu));

	em = module->data;
	if (em)
		embrace_module_free (em);

	return 1;
}

int e_modapi_save (E_Module *module)
{
	return 1;
}

int e_modapi_info (E_Module *module)
{
#if 0
	char buf[4096];

	snprintf (buf, sizeof (buf), "%s/module_icon.png", e_module_dir_get (module));
	module->icon_file = strdup (buf);
#endif
	return 1;
}

int e_modapi_about (E_Module *module)
{
	E_Dialog *dia;

	dia = e_dialog_new (e_container_current_get (e_manager_current_get ()));
	if (!dia)
		return 0;
	e_dialog_title_set (dia, "Enlightenment Embrace Module");
	e_dialog_icon_set (dia, "enlightenment/e", 64);
	e_dialog_text_set (dia, _("A module to check your email."));
	e_dialog_button_add (dia, _("Ok"), NULL, NULL, NULL);
	e_win_centered_set (dia->win, 1);
	e_dialog_show (dia);
	return 1;
}

/* module private routines */
static EmbraceModule *embrace_module_new (void)
{
	EmbraceModule *em;
	Evas_Object   *o;

	E_Manager     *man;
	E_Container   *con;

	embrace_count = 0;

	if (lt_dlinit ()) {
		fprintf (stderr, "Cannot initialize LTDL!\n");
		return NULL;
	}

	em = E_NEW (EmbraceModule, 1);
	if (!em) return NULL;

	em->embrace = embrace_new ();
	if (!em->embrace) {
		free (em);
		return NULL;
	}

	man = e_manager_current_get ();
	con = e_container_current_get (man);
	em->embrace->gui.evas = con->bg_evas;
	em->embrace->cfg.module = 1;

	embrace_init (em->embrace);

	o = evas_object_rectangle_add (em->embrace->gui.evas);
	em->event_object = o;
	evas_object_pass_events_set (o, 0);
	evas_object_color_set (o, 0, 0, 0, 0);
	evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN, embrace_cb_mouse_down, em);
	evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_UP, embrace_cb_mouse_up, em);
	evas_object_show (o);
	evas_object_stack_below (o, em->embrace->gui.edje);

	em->gmc = e_gadman_client_new (con->gadman);
	e_gadman_client_domain_set (em->gmc, "module.embrace", embrace_count++);
	e_gadman_client_policy_set (em->gmc,
			E_GADMAN_POLICY_ANYWHERE |
			E_GADMAN_POLICY_HMOVE |
			E_GADMAN_POLICY_VMOVE |
			E_GADMAN_POLICY_HSIZE |
			E_GADMAN_POLICY_VSIZE);
	e_gadman_client_min_size_set (em->gmc, 8, 8);
	e_gadman_client_max_size_set (em->gmc, 2000, 2000);
	e_gadman_client_auto_size_set (em->gmc, 186, 40);
	e_gadman_client_align_set (em->gmc, 0.0, 0.0);
	e_gadman_client_resize (em->gmc, 186, 40);
	e_gadman_client_change_func_set (em->gmc, embrace_cb_gmc_change, em);
	e_gadman_client_load (em->gmc);

	embrace_run (em->embrace);

	return em;
}

static void embrace_module_free (EmbraceModule *em)
{
	e_object_del (E_OBJECT (em->gmc));
	evas_object_del (em->event_object);

	embrace_stop (em->embrace);
	embrace_deinit (em->embrace);
	embrace_free (em->embrace);

	free (em);

	embrace_count--;

	lt_dlexit ();
}

static E_Menu *embrace_config_menu_new (void)
{
	E_Menu *m;
	E_Menu_Item *mi;

	m = e_menu_new ();

	mi = e_menu_item_new (m);
	e_menu_item_label_set (mi, _("(Empty)"));

	return m;
}

static void embrace_cb_gmc_change (void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   EmbraceModule *em;
   Evas_Coord  x, y, w, h;

   em = data;
   e_gadman_client_geometry_get (em->gmc, &x, &y, &w, &h);

   switch (change)
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	evas_object_move (em->embrace->gui.edje, x, y);
	evas_object_resize (em->embrace->gui.edje, w, h);
	evas_object_move (em->event_object, x, y);
	evas_object_resize (em->event_object, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise (em->embrace->gui.edje);
	evas_object_stack_below (em->event_object, em->embrace->gui.edje);
	break;
      default:
	break;
     }
}

static void embrace_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   EmbraceModule *em;

   em = data;
}

static void embrace_cb_mouse_up (void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   EmbraceModule *em;

   em = data;
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
