#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>

static void _background_cb_selected(void);
static void _background_cb_ok(void *, void *);
static void _background_cb_apply(void *, void *);
static void _background_cb_close(void *, void *);

static char* _background_get_path(const char *);
static void _background_close(void);
static void _background_apply(void);

static Entrance_Dialog win;
static Entrance_Widget img_preview;
static Entrance_List list_thumbs;
static Entrance_Widget group_backgrounds;
static Entrance_Widget group_preview;
static Entrance_Widget group_options;

void
egui_background_dialog_show()
{
   Ecore_List *backgrounds;
   char *background;
   
   win = ew_dialog_new(_("Entrance - Background Chooser"), EW_FALSE);

   group_preview = ew_dialog_group_add(win, _("Preview"));

   img_preview = ew_image_new(320, 240);
   ew_group_add(group_preview, img_preview);

   group_backgrounds = ew_dialog_group_add(win, _("Backgrounds"));

   list_thumbs = ew_textlist_new(NULL, 320, 140, 20, 90);
   
   /* scan for backgrounds and add them to the list */
   backgrounds = ecore_file_ls("/usr/share/enlightenment/data/backgrounds");
   if(!backgrounds || ecore_list_is_empty(backgrounds))
     return;

   background = ecore_list_first(backgrounds);
   char *background_file = _background_get_path(background);
   ew_image_edje_load(img_preview, background_file, "desktop/background");
   free(background_file);
   /*FIXME: selecting the first row doesn't work - maybe we select first row while adding elements to the list:(*/
   ew_list_first_row_select(list_thumbs);

   ecore_list_goto_first(backgrounds);
   while((background = ecore_list_next(backgrounds)))
     {
        char *background_no_ext = ecore_file_strip_ext(background);
		ew_textlist_add(list_thumbs, background_no_ext, background, strlen(background) + 1, _background_cb_selected);

        free(background_no_ext);
     }
   ecore_list_destroy(backgrounds);
   
   ew_group_add(group_backgrounds, list_thumbs);

   /*Entrance_Widget group_options = ew_dialog_group_add(win, _("Options"));*/
   
   ew_dialog_close_button_add(win, _background_cb_close, NULL);
   ew_dialog_apply_button_add(win, _background_cb_apply, NULL);
   ew_dialog_ok_button_add(win, _background_cb_ok, NULL);
   
   ew_dialog_show(win);
}

static void
_background_cb_selected()
{
	char* background = ew_list_selected_data_get(list_thumbs);
	char* background_path = _background_get_path(background);
	ew_image_edje_load(img_preview, background_path, "desktop/background");

	free(background_path);
}


static void
_background_cb_ok(void *object, void *data)
{
   _background_apply();
   _background_close();
}

static void
_background_cb_apply(void *object, void *data)
{
   _background_apply();
}

static void
_background_cb_close(void *object, void *data)
{
	_background_close();
}

static char*
_background_get_path(const char *t)
{
	char *path = calloc(PATH_MAX, sizeof(*path));
	snprintf(path, PATH_MAX, "/usr/share/enlightenment/data/backgrounds/%s", t);

	return path;
}

static void 
_background_apply(void)
{
    char *background = ew_list_selected_data_get(list_thumbs);
	if(!background) 
	{
		printf("Error. Please select a background first\n");
		return;
	}

	entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR, background);
	if(!entrance_edit_save())
	{
		printf("Error setting background - %s. Please check your permissions\n", background);
	}
}

static void
_background_close(void)
{
	ew_dialog_destroy(win);
}
