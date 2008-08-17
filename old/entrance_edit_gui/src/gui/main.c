#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Entrance_Smart.h>
#include "Egui.h"
#include "config.h"

static void _main_dialog_show(void);
static void _close_button_cb(void *, void *);

static Entrance_Dialog dialog;

    
int
main(int argc, char **argv)
{
   ecore_init();
   entrance_edit_init(NULL);
   ew_init(&argc, &argv);
   entrance_smart_init();
   
   _main_dialog_show();
   
   ew_main();
   ew_shutdown();
   entrance_edit_shutdown();
   ecore_shutdown();
   
   return 0;
}

static void
_main_dialog_show()
{
   dialog = ew_dialog_new(_("Entrance Configuration"), EW_TRUE);   
   const char* edjefile = PACKAGE_DATA_DIR"/icons/icons.edj";

   Entrance_List tree = ew_edjelist_new("<b>Configuration</b>", 320, 240, 52, 90);
   ew_edjelist_add(tree, _("Theme"), edjefile, "icons/main/theme", NULL, 0, egui_theme_dialog_show, NULL);
   ew_edjelist_add(tree, _("Background"), edjefile, "icons/main/background", NULL, 0, egui_background_dialog_show, NULL);
   ew_edjelist_add(tree, _("Behavior"), edjefile, "icons/main/behavior", NULL, 0,  egui_behavior_dialog_show, NULL);
   ew_edjelist_add(tree, _("X settings"), edjefile, "icons/main/xsettings",NULL, 0, egui_x_settings_dialog_show, NULL);
/*   ew_edjelist_add(tree, _("Sessions"),edjefile, "icons/main/sessions", NULL, 0,  egui_sessions_dialog_show, NULL);*/
   ew_edjelist_add(tree, _("Layout"), edjefile, "icons/main/layout", NULL, 0,  egui_layout_dialog_show, NULL);
/*   ew_edjelist_add(tree, _("Modules"), edjefile, "icons/main/modules",NULL, 0, NULL, NULL);*/

   Entrance_Widget group = ew_dialog_group_add(dialog, _("Configuration"), EW_GROUP_VERTICAL);
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
