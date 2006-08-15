#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Egui.h>

static void _gs_cb_selected(void);
static void _gs_cb_ok(void *, void *);
static void _gs_cb_apply(void *, void *);
static void _gs_cb_close(void *, void *);
static void _gs_cb_browse(void*, void*);

static char* _gs_get_path(const char *);
static void _gs_close(void);
static int _gs_apply(void);
static char* _gs_populate_list(void);
static void _gs_load_preview(const char *);

static Entrance_Dialog win;
static Entrance_Widget img_preview;
static Entrance_List list_thumbs;
static Entrance_Entry browse_entry;
static Entrance_Widget browse_button;
static Entrance_Widget group_graphics;
static Entrance_Widget group_preview;
static Entrance_Widget group_options;

static Egui_Graphics_Selector egs;

void
egui_gs_dialog_show(Egui_Graphics_Selector _egs)
{
   egs = _egs;
   
   win = ew_dialog_new(egs.dialog_title, EW_FALSE);


   group_preview = ew_dialog_group_add(win, _("Preview"), EW_GROUP_VERTICAL);

   img_preview = ew_image_new(320, 240);
   ew_group_add(group_preview, img_preview);

   group_graphics = ew_dialog_group_add(win, egs.list_title, EW_GROUP_VERTICAL);

   list_thumbs = ew_textlist_new(NULL, 320, 140, 20, 90);
   
   char *first = _gs_populate_list();
   _gs_load_preview(first);
   
   ew_group_add(group_graphics, list_thumbs);

   char t[PATH_MAX];
   snprintf(t, PATH_MAX, "Browse system for %ss", egs.name);
   group_options = ew_dialog_group_add(win, t, EW_GROUP_HORIZONTAL);

   browse_entry = ew_entry_new(NULL, NULL, EW_FALSE);
   ew_group_add(group_options, browse_entry);

   browse_button = ew_button_new(_("Browse..."), _gs_cb_browse, NULL);
   ew_group_add(group_options, browse_button);
   
   ew_dialog_close_button_add(win, _gs_cb_close, NULL);
   ew_dialog_apply_button_add(win, _gs_cb_apply, NULL);
   ew_dialog_ok_button_add(win, _gs_cb_ok, NULL);
   
   ew_dialog_show(win);
}

static void
_gs_cb_selected()
{
	char* graphic = ew_list_selected_data_get(list_thumbs);
	_gs_load_preview(graphic);
}


static void
_gs_cb_ok(void *object, void *data)
{
   if(_gs_apply()) 
	   _gs_close();
}

static void
_gs_cb_apply(void *object, void *data)
{
   _gs_apply();
}

static void
_gs_cb_close(void *object, void *data)
{
	_gs_close();
}

static void
_gs_cb_browse(void *object, void* data)
{
	ew_messagebox_ok("Entrance Configuration - Notice", "Browsing for files :)", EW_MESSAGEBOX_ICON_MESSAGE);
}

static char*
_gs_get_path(const char *t)
{
	char *path = calloc(PATH_MAX, sizeof(*path));
	snprintf(path, PATH_MAX, "%s/%s", egs.files_path, t);

	return path;
}

static int
_gs_apply(void)
{
	char msg[PATH_MAX];

    char *graphic = ew_list_selected_data_get(list_thumbs);
	if(!graphic) 
	{
		snprintf(msg, PATH_MAX, "Please select a %s first", egs.name);
		ew_messagebox_ok("Entrance Config - Error", msg, EW_MESSAGEBOX_ICON_ERROR);
		return 0;
	}

	if(egs.use_full_path)
	{
		char *full_path = _gs_get_path(graphic);
		entrance_edit_string_set(egs.entrance_edit_key, full_path);
		free(full_path);
	}
	else 
		entrance_edit_string_set(egs.entrance_edit_key, graphic);

	if(!entrance_edit_save())
	{
		snprintf(msg, PATH_MAX, "Can not set %s. Please check your permissions", egs.name);
		ew_messagebox_ok("Entrance Config - Error", msg, EW_MESSAGEBOX_ICON_ERROR);
		return 0;
	}

	return 1;
}

static void
_gs_close(void)
{
	ew_dialog_destroy(win);
}

static char*
_gs_populate_list(void)
{
   Ecore_List *glist = ecore_file_ls(egs.files_path);
   char* first;

   if(glist && !ecore_list_is_empty(glist))
   {
	   ecore_list_goto_first(glist);

	   char* graphic;
	   first = strdup(ecore_list_first(glist));

	   while((graphic = ecore_list_next(glist)))
		 {
			char *graphic_no_ext = ecore_file_strip_ext(graphic);
			ew_textlist_add(list_thumbs, graphic_no_ext, graphic, strlen(graphic) + 1, _gs_cb_selected);
			free(graphic_no_ext);
		 }

	   ecore_list_destroy(glist);
   }

   return first;
}

static void
_gs_load_preview(const char *graphic)
{
   char *file = _gs_get_path(graphic);
   ew_image_edje_load(img_preview, file, egs.preview_edje_part);
   free(file);
   /*FIXME: selecting the first row doesn't work - maybe we select first row while adding elements to the list:(*/
   /*ew_list_first_row_select(list_thumbs);*/
}
