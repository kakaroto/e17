#include "Photo.h"

static void _cb_deactivate_post(void *data, E_Menu *m);
static void _cb_picture_info(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_picture_next(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_picture_prev(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_picture_setbg(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_picture_viewer(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_pause_toggle(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_configure_item(void *data, E_Menu *m, E_Menu_Item *mi);
static void _cb_configure_general(void *data, E_Menu *m, E_Menu_Item *mi);


/*
 * Public functions
 */

int photo_menu_show(Photo_Item *pi)
{
   E_Menu *mn, *mn2;
   E_Menu_Item *mi;
   char buf[4096];

   mn = e_menu_new();
   e_menu_post_deactivate_callback_set(mn, _cb_deactivate_post, pi);

   mi = e_menu_item_new(mn);
   if (pi->config->timer_active)
     {
        e_menu_item_label_set(mi, D_("Pause slideshow"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_PAUSE);
     }
   else
     {
        e_menu_item_label_set(mi, D_("Resume slideshow"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_RESUME);
     }
   e_menu_item_callback_set(mi, _cb_pause_toggle, pi);
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Next picture"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_NEXT);
   e_menu_item_callback_set(mi, _cb_picture_next, pi);
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Previous picture"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_PREVIOUS);
   e_menu_item_callback_set(mi, _cb_picture_prev, pi);

   photo_picture_histo_menu_append(pi, mn);

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Picture informations"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_INFOS);
   e_menu_item_callback_set(mi, _cb_picture_info, pi);
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Set as background"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_SETBG);
   e_menu_item_callback_set(mi, _cb_picture_setbg, pi);
   mi = e_menu_item_new(mn);
   snprintf(buf, sizeof(buf), "%s %s", D_("Open in"), photo->config->pictures_viewer);
   e_menu_item_label_set(mi, buf);
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_VIEWER);
   e_menu_item_callback_set(mi, _cb_picture_viewer, pi);

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Configure Photo module"));
   e_util_menu_item_theme_icon_set(mi, "preferences-system");
   e_menu_item_callback_set(mi, _cb_configure_general, NULL);
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Configure this Photo gadget"));
  e_util_menu_item_theme_icon_set(mi, "preferences-system");
   e_menu_item_callback_set(mi, _cb_configure_item, pi);

   mn2 = e_menu_new();
   e_gadcon_client_util_menu_items_append(pi->gcc, mn2, 0);
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, D_("Configure Gadget and Shelf"));
   e_menu_item_submenu_set(mi, mn2);

   pi->menu = mn;

   return 1;
}

void photo_menu_hide(Photo_Item *pi)
{
   e_menu_post_deactivate_callback_set(pi->menu, NULL, NULL);
   _cb_deactivate_post(pi, pi->menu);
   pi->menu = NULL;
}


/*
 * Private functions
 *
 */

static void
_cb_deactivate_post(void *data, E_Menu *m)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;
   if (!pi->menu) return;

   e_object_del(E_OBJECT(pi->menu));
   pi->menu = NULL;
}

static void
_cb_picture_info(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;

   photo_item_action_infos(pi);
}

static void
_cb_picture_next(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;

   photo_item_action_change(pi, 1);
   photo_item_timer_set(pi, pi->config->timer_active, 0);
}

static void
_cb_picture_prev(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;

   photo_item_action_change(pi, -1);
   photo_item_timer_set(pi, pi->config->timer_active, 0);
}

static void
_cb_picture_setbg(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;

   photo_item_action_setbg(pi);
}

static void
_cb_picture_viewer(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;

   photo_item_action_viewer(pi);
}

static void _cb_pause_toggle(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;

   photo_item_action_pause_toggle(pi);
}

static void
_cb_configure_item(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Photo_Item *pi;

   pi = data;
   if (!pi) return;
   if (pi->config_dialog) return;

   photo_config_dialog_item_show(pi);
}

static void
_cb_configure_general(void *data, E_Menu *m, E_Menu_Item *mi)
{
   if (!photo) return;
   if (photo->config_dialog) return;

   photo_config_dialog_show();
}
