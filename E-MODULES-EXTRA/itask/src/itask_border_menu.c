#define D_(str) dgettext(PACKAGE, str)
#include "e.h"

/* taken from e_int_border_menu */

/* static void _e_mod_cb_border_menu_end(void *data, E_Menu *m); */
static void _itask_border_menu_cb_close(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_iconify(void *data, E_Menu *m, E_Menu_Item *mi);
/* static void _itask_border_menu_cb_kill(void *data, E_Menu *m, E_Menu_Item *mi); */
static void _itask_border_menu_cb_maximize(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_unmaximize(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_stick(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_on_top(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_normal(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_below(void *data, E_Menu *m, E_Menu_Item *mi);
/* static void _itask_border_menu_cb_fullscreen(void *data, E_Menu *m, E_Menu_Item *mi); */
/* static void _itask_border_menu_cb_skip_winlist(void *data, E_Menu *m, E_Menu_Item *mi); */
static void _itask_border_menu_cb_sendto_pre(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_border_menu_cb_sendto(void *data, E_Menu *m, E_Menu_Item *mi);
/* static void _itask_border_menu_cb_raise(void *data, E_Menu *m, E_Menu_Item *mi); */
/* static void _itask_border_menu_cb_lower(void *data, E_Menu *m, E_Menu_Item *mi); */

E_Menu *
itask_border_menu_get(E_Border *bd, E_Menu *mn)
{
   E_Menu_Item *mi;
   E_Menu *m, *m_stacking;
   if (bd->border_menu)
     return NULL;

   /* Only allow to change layer for windows in "normal" layers */
   if ((!bd->lock_user_stacking) &&
       ((bd->layer == 50) || (bd->layer == 100) || (bd->layer == 150)))
     {
	m = e_menu_new();

	e_menu_category_set(m,"border/stacking");
	e_menu_category_data_set("border/stacking",bd);


	mi = e_menu_item_new(m);
	e_menu_item_label_set(mi, D_("Always On Top"));
	e_menu_item_radio_set(mi, 1);
	e_menu_item_radio_group_set(mi, 2);
	e_menu_item_toggle_set(mi, (bd->layer == 150 ? 1 : 0));
	e_menu_item_callback_set(mi, _itask_border_menu_cb_on_top, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/stack_on_top"),
				  "widgets/border/default/stack_on_top");

	mi = e_menu_item_new(m);
	e_menu_item_label_set(mi, D_("Normal"));
	e_menu_item_radio_set(mi, 1);
	e_menu_item_radio_group_set(mi, 2);
	e_menu_item_toggle_set(mi, (bd->layer == 100 ? 1 : 0));
	e_menu_item_callback_set(mi, _itask_border_menu_cb_normal, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/stack_normal"),
				  "widgets/border/default/stack_normal");

	mi = e_menu_item_new(m);
	e_menu_item_label_set(mi, D_("Always Below"));
	e_menu_item_radio_set(mi, 1);
	e_menu_item_radio_group_set(mi, 2);
	e_menu_item_toggle_set(mi, (bd->layer == 50 ? 1 : 0));
	e_menu_item_callback_set(mi, _itask_border_menu_cb_below, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/stack_below"),
				  "widgets/border/default/stack_below");

	m_stacking = m;
     }

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Stacking"));
   e_menu_item_submenu_set(mi, m_stacking);
   e_menu_item_icon_edje_set(mi,
                             (char *)e_theme_edje_file_get("base/theme/borders",
                                                           "widgets/border/default/stacking"),
                             "widgets/border/default/stacking");

   if (!bd->sticky)
     {
	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Send to Desktop"));
	e_menu_item_submenu_pre_callback_set(mi, _itask_border_menu_cb_sendto_pre, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/sendto"),
				  "widgets/border/default/sendto");
     }


   if (!bd->lock_user_sticky && !bd->fullscreen) // sticky with fullscreen doesnt work correct
     {
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Sticky"));
	e_menu_item_check_set(mi, 1);
	e_menu_item_toggle_set(mi, (bd->sticky ? 1 : 0));
	e_menu_item_callback_set(mi, _itask_border_menu_cb_stick, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/stick"),
				  "widgets/border/default/stick");
     }

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);

   if (!bd->lock_close)
     {
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Close"));
	e_menu_item_callback_set(mi, _itask_border_menu_cb_close, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/close"),
				  "widgets/border/default/close");
     }

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);




   if (!(((bd->client.icccm.min_w == bd->client.icccm.max_w) &&
          (bd->client.icccm.min_h == bd->client.icccm.max_h)) ||
         (bd->lock_user_maximize)))
     {
	if(!bd->maximized)
	  {
	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, D_("Maximize"));
	     e_menu_item_callback_set(mi, _itask_border_menu_cb_maximize, bd);
	     e_menu_item_icon_edje_set(mi,
				       (char *)e_theme_edje_file_get("base/theme/borders",
								     "widgets/border/default/maximize"),
				       "widgets/border/default/maximize");
	  }
	else
	  {
	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, D_("Unmaximize"));
	     e_menu_item_callback_set(mi, _itask_border_menu_cb_unmaximize, bd);
	     e_menu_item_icon_edje_set(mi,
				       (char *)e_theme_edje_file_get("base/theme/borders",
								     "widgets/border/default/maximize"),
				       "widgets/border/default/maximize");
	  }
     }




   if ((!bd->lock_user_iconify) && (!bd->fullscreen) && (!bd->iconic))
     {
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Iconify"));
	e_menu_item_callback_set(mi, _itask_border_menu_cb_iconify, bd);
	e_menu_item_icon_edje_set(mi,
				  (char *)e_theme_edje_file_get("base/theme/borders",
								"widgets/border/default/minimize"),
				  "widgets/border/default/minimize");
     }

   return mn;
}




static void
_itask_border_menu_cb_close(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   if (!bd->lock_close)
     e_border_act_close_begin(bd);
}

static void
_itask_border_menu_cb_iconify(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   if (!bd->lock_user_iconify)
     {
	if (bd->iconic)
	  e_border_uniconify(bd);
	else
	  e_border_iconify(bd);
     }
}

static void
_itask_border_menu_cb_maximize(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;



   bd = data;
   if (!bd->lock_user_maximize)
     {
	if(e_desk_current_get(bd->zone) != bd->desk)
	  e_desk_show(bd->desk);

	if(bd->iconic)
	  e_border_uniconify(bd);
	else
	  e_border_raise(bd);

	e_border_focus_set(bd, 1, 1);
	e_border_maximize(bd, (e_config->maximize_policy & E_MAXIMIZE_TYPE) | E_MAXIMIZE_BOTH);
     }
}


static void
_itask_border_menu_cb_unmaximize(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   e_border_unmaximize(bd, E_MAXIMIZE_BOTH);
}

static void
_itask_border_menu_cb_stick(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   if (!bd->lock_user_sticky)
     {
	if (bd->sticky)
	  e_border_unstick(bd);
	else
	  e_border_stick(bd);
     }
}

static void
_itask_border_menu_cb_on_top(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   if (bd->layer != 150)
     {
	e_border_layer_set(bd, 150);
	e_hints_window_stacking_set(bd, E_STACKING_ABOVE);
     }
}

static void
_itask_border_menu_cb_below(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   if (bd->layer != 50)
     {
	e_border_layer_set(bd, 50);
	e_hints_window_stacking_set(bd, E_STACKING_BELOW);
     }
}

static void
_itask_border_menu_cb_normal(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;

   bd = data;
   if (bd->layer != 100)
     {
	e_border_layer_set(bd, 100);
	e_hints_window_stacking_set(bd, E_STACKING_NONE);
     }
}

static void
_itask_border_menu_cb_sendto_pre(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Menu *subm;
   E_Menu_Item *submi;
   E_Border *bd;
   E_Desk *desk_cur;
   int i;

   bd = data;
   desk_cur = e_desk_current_get(bd->zone);

   subm = e_menu_new();
   e_object_data_set(E_OBJECT(subm), bd);
   e_menu_item_submenu_set(mi, subm);

   for (i = 0; i < bd->zone->desk_x_count * bd->zone->desk_y_count; i++)
     {
	E_Desk *desk;

	desk = bd->zone->desks[i];
	submi = e_menu_item_new(subm);
	e_menu_item_label_set(submi, desk->name);
	e_menu_item_radio_set(submi, 1);
	e_menu_item_radio_group_set(submi, 2);
	e_menu_item_toggle_set(submi, (desk_cur == desk ? 1 : 0));
	e_menu_item_callback_set(submi, _itask_border_menu_cb_sendto, desk);
     }
}

static void
_itask_border_menu_cb_sendto(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Desk *desk;
   E_Border *bd;

   desk = data;
   bd = e_object_data_get(E_OBJECT(m));
   if ((bd) && (desk))
     {
	e_border_desk_set(bd, desk);
     }
}
