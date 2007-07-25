#include <limits.h>
#include <string.h>
#include <Evas.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Edit.h>
#include <Entrance_Widgets.h>
#include <Entrance_Smart.h>
#include "Egui.h"

#define PREVIEW_WIDTH 320
#define PREVIEW_HEIGHT 240
#define PREVIEW_V_WIDTH 1024
#define PREVIEW_V_HEIGHT 768

static void _gd_cb_selected(void*);
static void _gd_cb_ok(void *, void *);
static void _gd_cb_apply(void *, void *);
static void _gd_cb_close(void *, void *);
static void _gd_cb_browse(void*, void*);

static void _cb_dirs(void *list_data, void *data);

static char* _gd_get_path(Egui_Graphics_Dialog, const char *);
static void _gd_close(void*);
static int _gd_apply(void*);
static char* _gd_populate_list(Egui_Graphics_Dialog);
static void _gd_load_preview(Egui_Graphics_Dialog, const char *);
static char* _gd_process_dir(Egui_Graphics_Dialog, char *dir);

static Egui_Graphics_Dialog _egui_gd_create_widgets(Egui_Graphics_Dialog egd);


Egui_Graphics_Dialog
egui_gd_new(Egui_Graphics_Dialog_Settings egds)
{
   Egui_Graphics_Dialog egd = calloc(1, sizeof(*egd));
   if(!egd)
	   return NULL;

   egd->egds = egds;
   egd = _egui_gd_create_widgets(egd);

   return egd;
}

Egui_Graphics_Dialog
_egui_gd_create_widgets(Egui_Graphics_Dialog egd)
{
	if(!egd)
		return NULL;

   egd->win = ew_dialog_new(egd->egds.dialog_title, EW_FALSE);

   egd->group_preview = ew_dialog_group_add(egd->win, _("Preview"), EW_GROUP_VERTICAL);

   egd->img_preview = ew_preview_new(PREVIEW_WIDTH, PREVIEW_HEIGHT);
   ew_group_add(egd->group_preview, egd->img_preview);

   egd->group_graphics = ew_dialog_group_add(egd->win, egd->egds.list_title, EW_GROUP_VERTICAL);

   egd->list_thumbs = ew_textlist_new(NULL, PREVIEW_WIDTH, 140, 20, 90);
   
      
   ew_group_add(egd->group_graphics, egd->list_thumbs);

   char t[PATH_MAX];
   snprintf(t, PATH_MAX, "Browse system for %ss", egd->egds.name);
   egd->group_options = ew_dialog_group_add(egd->win, t, EW_GROUP_HORIZONTAL);

   egd->browse_entry = ew_entry_new(NULL, NULL, EW_FALSE);
   ew_group_add(egd->group_options, egd->browse_entry);

   egd->browse_button = ew_button_new(_("Browse..."), _gd_cb_browse, egd);
   ew_group_add(egd->group_options, egd->browse_button);

   if(egd->egds.show_pointer_options)
   {
	   egd->group_pointer = ew_dialog_group_add(egd->win, _("Pointer Options"), EW_GROUP_HORIZONTAL);

	   egd->pointer_preview = ew_image_new(100, 50);
	   ew_group_add(egd->group_pointer, egd->pointer_preview);

	   egd->pointer_browse_button = ew_button_new(_("Browse..."), _gd_cb_browse, egd);
	   ew_group_add(egd->group_pointer, egd->pointer_browse_button);
   }

   egd->first = _gd_populate_list(egd);
   
   ew_dialog_close_button_add(egd->win, _gd_cb_close, egd);
   ew_dialog_apply_button_add(egd->win, _gd_cb_apply, egd);
   ew_dialog_ok_button_add(egd->win, _gd_cb_ok, egd);
   ew_dialog_onclose_set(egd->win, _gd_cb_close, egd);

   egd->newly_created = 1;
   return egd;
}
 
void
egui_gd_show(Egui_Graphics_Dialog egd)
{
	if(!egd)
		return;
  
   if(egd->win == NULL)
	   egd = _egui_gd_create_widgets(egd);

   ew_dialog_show(egd->win);
   _gd_load_preview(egd, egd->first);
   egd->newly_created = 0;
}

static void
_gd_cb_selected(void *data)
{
	Egui_Graphics_Dialog egd = data;
	if(!egd)
		return;

	char* graphic = ew_list_selected_data_get(egd->list_thumbs);
	_gd_load_preview(egd, graphic);
}


static void
_gd_cb_ok(void *object, void *data)
{
   if(_gd_apply(data)) 
	   _gd_close(data);
}

static void
_gd_cb_apply(void *object, void *data)
{
   _gd_apply(data);
}

static void
_gd_cb_close(void *object, void *data)
{
	_gd_close(data);
	/*TODO: neccessary?*/
}

static void
_gd_cb_browse(void *object, void* data)
{
	ew_messagebox_ok("Entrance Configuration - Notice", "Browsing for files :)", EW_MESSAGEBOX_ICON_MESSAGE);
}

static char*
_gd_get_path(Egui_Graphics_Dialog egd, const char *t)
{
	if(!egd)
		return NULL;

	char *path = calloc(PATH_MAX, sizeof(*path));
	snprintf(path, PATH_MAX, "%s/%s", egd->egds.files_path, t);

	return path;
}

static int
_gd_apply(void* data)
{
	char msg[PATH_MAX];
	Egui_Graphics_Dialog egd = data;
	if(!egd)
		return;

    char *graphic = ew_list_selected_data_get(egd->list_thumbs);
	if(!graphic) 
	{
		snprintf(msg, PATH_MAX, "Please select a %s first", egd->egds.name);
		ew_messagebox_ok("Entrance Config - Error", msg, EW_MESSAGEBOX_ICON_ERROR);
		return 0;
	}


	entrance_edit_string_set(egd->egds.entrance_edit_key, graphic);

	if(!entrance_edit_save())
	{
		snprintf(msg, PATH_MAX, "Can not set %s. Please check your permissions", egd->egds.name);
		ew_messagebox_ok("Entrance Config - Error", msg, EW_MESSAGEBOX_ICON_ERROR);
		return 0;
	}

	return 1;
}

static void
_gd_close(void* data)
{
	Egui_Graphics_Dialog egd = data;
	if(!egd)
		return;
	ew_dialog_destroy(egd->win);
	egd->win = NULL;
	evas_free(egd->preview_evas);
	egd->preview_evas = NULL;
	es_free(egd->preview_smart);
	egd->preview_smart = NULL;
}

static char*
_gd_process_dir(Egui_Graphics_Dialog egd, char *dir)
{
	char full_path[PATH_MAX];

   if(dir == NULL)
	   return;

   if(egd == NULL)
	   return;

   Ecore_List *glist = ecore_file_ls(dir);
   char* first;

   if(glist && !ecore_list_empty_is(glist))
   {
	   ecore_list_first_goto(glist);

	   char* graphic;
	   first = strdup(ecore_list_first(glist));


	   while((graphic = ecore_list_next(glist)))
		 {
			char *graphic_no_ext = ecore_file_strip_ext(graphic);
			if(egd->egds.use_full_path)
				snprintf(full_path, PATH_MAX, "%s/%s", dir, graphic);
			else
				snprintf(full_path, PATH_MAX, "%s", graphic);

			ew_textlist_add(egd->list_thumbs, graphic_no_ext, full_path, strlen(full_path) + 1, _gd_cb_selected, egd);
			free(graphic_no_ext);
		 }

	   ecore_list_destroy(glist);
   }

   char *res = calloc(sizeof(char), PATH_MAX);
   if (egd->egds.use_full_path)
	   snprintf(res, PATH_MAX, "%s/%s", dir, first);
   else
	   snprintf(res, PATH_MAX, "%s", first);

   return res;
}

static char*
_gd_populate_list(Egui_Graphics_Dialog egd)
{

   char* first = _gd_process_dir(egd, egd->egds.files_path);

   if (egd->egds.extra_file_paths != NULL)
   {
	   ecore_list_for_each(egd->egds.extra_file_paths, _cb_dirs, egd);
   }


   return first;
}

static void 
_cb_dirs(void *list_data, void *data)
{
	char* apath = list_data;
	Egui_Graphics_Dialog egd = data;

   _gd_process_dir(egd, apath);
}



static void
_gd_load_preview(Egui_Graphics_Dialog egd, const char *graphic)
{
	if(!egd)
		return;

   char *file = NULL;

   if(egd->egds.use_full_path)
	   file = strdup(graphic);
   else
	   file = _gd_get_path(egd, graphic);

   if(egd->preview_evas == NULL || egd->newly_created == 1) 
	   egd->preview_evas = ew_preview_evas_get(egd->img_preview, PREVIEW_WIDTH, PREVIEW_HEIGHT, PREVIEW_V_WIDTH, PREVIEW_V_HEIGHT);
 

   char *bg_path = egui_get_current_bg();
   char *theme_path = egui_get_current_theme();

   /*static Evas_Object *es  = NULL;*/
   if(egd->preview_smart == NULL || egd->newly_created == 1) 
	   egd->preview_smart = es_new(egd->preview_evas);
   /*Evas_Object *es = es_new(egd->preview_evas);*/

   if(egd->egds.keep_part == EGDS_BACKGROUND)
	   es_background_edje_set(egd->preview_smart, bg_path);
   else
	   es_background_edje_set(egd->preview_smart, file);

   if(egd->egds.keep_part == EGDS_THEME)
	   es_main_edje_set(egd->preview_smart, theme_path); 
   else 
	   es_main_edje_set(egd->preview_smart, file);

   evas_object_resize(egd->preview_smart, PREVIEW_V_WIDTH, PREVIEW_V_HEIGHT);
   evas_object_show(egd->preview_smart);

   free(bg_path);
   free(theme_path);
   free(file);


   /*FIXME: selecting the first row doesn't work - maybe we select first row while adding elements to the list:(*/
   /*ew_list_first_row_select(list_thumbs);*/
}
