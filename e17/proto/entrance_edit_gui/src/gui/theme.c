#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Etk.h>
#include <Entrance_Widgets.h>
//#include "Egui.h"

static void _egui_theme_cb_selected(void);
static void _egui_theme_cb_ok(void *object, void *data);
static void _egui_theme_cb_apply(void *object, void *data);
static void _egui_theme_cb_close(void *object, void *data);

static Entrance_Dialog win;
static Entrance_Widget img_preview;
static Entrance_List list_thumbs;
static Entrance_Widget group_themes;
static Entrance_Widget group_preview;
static Entrance_Widget group_options;

void
egui_theme_dialog_show()
{
   Ecore_List *themes;
   char *theme;
   char theme_file[PATH_MAX];
   
   win = ew_dialog_new(_("Entrance - Theme Chooser"), EW_FALSE);

   group_preview = ew_dialog_group_add(win, _("Preview"));
   img_preview = ew_image_new(320, 240);
  /* ew_image_edje_load(img_preview, "/usr/share/entrance/themes/default.edj", "Preview");*/
   ew_group_add(group_preview, img_preview);

   group_themes = ew_dialog_group_add(win, _("Themes"));
   
   list_thumbs = ew_textlist_new(NULL, 320, 140, 20, 90);
   
   /* scan for themes and add them to the list */
   themes = ecore_file_ls("/usr/share/entrance/themes");
   if(!themes || ecore_list_is_empty(themes))
     return;
   ecore_list_goto_first(themes);
   while((theme = ecore_list_next(themes)))
     {
        char *theme_no_ext;
        
        theme_no_ext = ecore_file_strip_ext(theme);
        snprintf(theme_file, sizeof(theme_file), "/usr/share/entrance/themes/%s", theme);
		ew_textlist_add(list_thumbs, theme_no_ext, theme_file, strlen(theme_file) + 1, _egui_theme_cb_selected);

        free(theme_no_ext);
     }
   ecore_list_destroy(themes);
   
   ew_group_add(group_themes, list_thumbs);

   /*Entrance_Widget group_options = ew_dialog_group_add(win, _("Options"));*/
   
   ew_dialog_close_button_add(win, _egui_theme_cb_close, NULL);
   ew_dialog_apply_button_add(win, _egui_theme_cb_apply, NULL);
   ew_dialog_ok_button_add(win, _egui_theme_cb_ok, NULL);
   
   ew_dialog_show(win);
}

static void
_egui_theme_cb_selected()
{
	char* themefile = ew_list_selected_data_get(list_thumbs);
	ew_image_edje_load(img_preview, themefile, "Preview");
}


static void
_egui_theme_cb_ok(void *object, void *data)
{
   printf("ok pressed!\n");
   ew_dialog_destroy(win);
}

static void
_egui_theme_cb_apply(void *object, void *data)
{
   char *themefile = ew_list_selected_data_get(list_thumbs);
   printf("apply pressed - %s!\n", themefile);
}

static void
_egui_theme_cb_close(void *object, void *data)
{
	ew_dialog_destroy(win);
}
