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
   E_Menu *ma, *mg;
   E_Menu_Item *mi;
   char buf[4096];

   ma = e_menu_new();
   e_menu_post_deactivate_callback_set(ma, _cb_deactivate_post, pi);
   pi->menu = ma;

   mi = e_menu_item_new(ma);
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
   mi = e_menu_item_new(ma);
   e_menu_item_label_set(mi, D_("Next picture"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_NEXT);
   e_menu_item_callback_set(mi, _cb_picture_next, pi);
   mi = e_menu_item_new(ma);
   e_menu_item_label_set(mi, D_("Previous picture"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_PREVIOUS);
   e_menu_item_callback_set(mi, _cb_picture_prev, pi);

   photo_picture_histo_menu_append(pi, ma);

   mi = e_menu_item_new(ma);
   e_menu_item_separator_set(mi, 1);

   mi = e_menu_item_new(ma);
   e_menu_item_label_set(mi, D_("Picture informations"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_INFOS);
   e_menu_item_callback_set(mi, _cb_picture_info, pi);
   mi = e_menu_item_new(ma);
   e_menu_item_label_set(mi, D_("Set as background"));
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_SETBG);
   e_menu_item_callback_set(mi, _cb_picture_setbg, pi);
   mi = e_menu_item_new(ma);
   snprintf(buf, sizeof(buf), "%s %s", D_("Open in"), photo->config->pictures_viewer);
   e_menu_item_label_set(mi, buf);
   photo_util_menu_icon_set(mi, PHOTO_THEME_ICON_VIEWER);
   e_menu_item_callback_set(mi, _cb_picture_viewer, pi);

   mg = e_menu_new();

   mi = e_menu_item_new(mg);
   e_menu_item_label_set(mi, D_("Module Settings"));
   e_util_menu_item_theme_icon_set(mi, "preferences-system");
   e_menu_item_callback_set(mi, _cb_configure_general, NULL);
   mi = e_menu_item_new(mg);
   e_menu_item_separator_set(mi, 1);
   mi = e_menu_item_new(mg);
   e_menu_item_label_set(mi, D_("Settings"));
   e_util_menu_item_theme_icon_set(mi, "preferences-system");
   e_menu_item_callback_set(mi, _cb_configure_item, pi);

   e_gadcon_client_util_menu_items_append(pi->gcc, ma, mg, 0);

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
