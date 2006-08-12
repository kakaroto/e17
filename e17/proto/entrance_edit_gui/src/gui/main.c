#include <etk/Etk.h>
#include <Entrance_Widgets.h>
#include "Egui.h"

static void _main_dialog_show(void);
static void _close_button_cb(void *, void *);

static Entrance_Dialog dialog;

    
int
main(int argc, char **argv)
{
   entrance_edit_init(NULL);
   ew_init(&argc, &argv);
   
   _main_dialog_show();
   
   ew_main();
   ew_shutdown();
   entrance_edit_shutdown();
   
   return 0;
}

static void
_main_dialog_show()
{
   dialog = ew_dialog_new(_("Entrance Configuration"), EW_TRUE);   
   const char* edjefile = etk_theme_icon_theme_get();

   Entrance_List tree = ew_edjelist_new("<b>Configuration</b>", 320, 240, 52, 90);
   ew_edjelist_add(tree, _("Theme"), edjefile, "apps/preferences-desktop-theme_48", NULL, 0, egui_theme_dialog_show);
   ew_edjelist_add(tree, _("Background"), edjefile, "apps/preferences-desktop-wallpaper_48", NULL, 0, egui_background_dialog_show);
   ew_edjelist_add(tree, _("Layout"), edjefile, "categories/preferences-desktop_48", NULL, 0,  egui_layout_dialog_show);
   ew_edjelist_add(tree, _("Behavior"), edjefile, "categories/preferences-system_48", NULL, 0,  egui_behavior_dialog_show);
   ew_edjelist_add(tree, _("Sessions"),edjefile, "apps/preferences-system-session_48", NULL, 0,  egui_sessions_dialog_show);
   ew_edjelist_add(tree, _("X settings"), edjefile, "devices/video-display_48",NULL, 0, egui_x_settings_dialog_show);

   Entrance_Widget group = ew_dialog_group_add(dialog, _("Configuration"));
   ew_group_add(group, tree);

   ew_dialog_close_button_add(dialog, _close_button_cb, NULL);

   ew_dialog_show(dialog);
}

static void
_close_button_cb(void *sender, void *data)
{
	ew_dialog_destroy(dialog);
	ew_main_quit();
}
