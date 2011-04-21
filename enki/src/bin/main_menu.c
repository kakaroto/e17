#include "main.h"
#include "slideshow.h"
#include "evas_object/photo_object.h"

static Evas_Object *inwin = NULL;

static Evas_Object *libraries_list;
static Evas_Object *bt_import;
static Evas_Object *bt_slideshow;
static Evas_Object *bt_del_bg;
static Evas_Object *bt_album_new;
static Evas_Object *bt_sync;

static Elm_Gengrid_Item_Class itc_grid;

static char* _library_get(void *data, Evas_Object *obj, const char *part);
static void  _library_del(void *data, Evas_Object *obj);
static Evas_Object* _library_icon_get(void *data, Evas_Object *obj, const char *part);

static void _library_select(void *data, Evas_Object *obj, void *event_info);
static void _new_library();
static void _new_library_done_cb(void *data, Evas_Object *obj, void *event_info);

static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info);
static void _album_new_cb(void *data, Evas_Object *obj, void *event_info);
static void _import_cb(void *data, Evas_Object *obj, void *event_info);
static void _del_bg_cb(void *data, Evas_Object *obj, void *event_info);
static void _geocaching_import_cb(void *data, Evas_Object *obj, void *event_info);
static void _geocaching_import_done_cb(void *data, Evas_Object *obj, void *event_info);
static void _preferences_cb(void *data, Evas_Object *obj, void *event_info);
static void _quit_cb(void *data, Evas_Object *obj, void *event_info);
static void _sync_cb(void *data, Evas_Object *obj, void *event_info);

typedef struct
{
	Eina_Bool is_new_library;
	const char *path;
	Enlil_Photo *photo1;
	Enlil_Photo *photo2;
} Library;


void main_menu_new(Evas_Object *edje)
{
    Evas_Object *bt;

    //Libraries
    //libraries_list = edje_object_part_external_object_get(edje, "object.main_menu.list_libraries");
    libraries_list = elm_gengrid_add(edje);
    elm_gengrid_item_size_set(libraries_list, 256, 256);
    edje_object_part_swallow(edje, "object.main_menu.list_libraries", libraries_list);

    itc_grid.func.label_get = _library_get;
    itc_grid.func.del = _library_del;
    itc_grid.func.icon_get = _library_icon_get;
    itc_grid.item_style = "enki";
    //

    //tools
    bt_slideshow = edje_object_part_external_object_get(edje, "object.main_menu.bt_slideshow");
    evas_object_smart_callback_add(bt_slideshow, "clicked", _slideshow_cb, NULL);

    bt_album_new = edje_object_part_external_object_get(edje, "object.main_menu.bt_new_album");
    evas_object_smart_callback_add(bt_album_new, "clicked", _album_new_cb, NULL);

    bt_import = edje_object_part_external_object_get(edje, "object.main_menu.bt_import_photos");
    evas_object_smart_callback_add(bt_import, "clicked", _import_cb, NULL);

    bt_del_bg = edje_object_part_external_object_get(edje, "object.main_menu.bt_remove_wall");
    evas_object_smart_callback_add(bt_del_bg, "clicked", _del_bg_cb, NULL);


    bt = edje_object_part_external_object_get(edje, "object.main_menu.bt_import_gpx");
    evas_object_smart_callback_add(bt, "clicked", _geocaching_import_cb, NULL);

    bt = edje_object_part_external_object_get(edje, "object.main_menu.bt_preference");
    evas_object_smart_callback_add(bt, "clicked", _preferences_cb, NULL);

    bt_sync = edje_object_part_external_object_get(edje, "object.main_menu.bt_sync");
    evas_object_smart_callback_add(bt_sync, "clicked", _sync_cb, NULL);
    //

    //Quit
    bt = edje_object_part_external_object_get(edje, "object.main_menu.bt_close");
    evas_object_smart_callback_add(bt, "clicked", _quit_cb , NULL);
    //


    //library list
    main_menu_update_libraries_list();
}

void main_menu_loading_disable_set(Eina_Bool disabled)
{
   elm_object_disabled_set(libraries_list, disabled);
   elm_object_disabled_set(bt_import, disabled);
   elm_object_disabled_set(bt_album_new, disabled);
   elm_object_disabled_set(bt_slideshow, disabled);
   elm_object_disabled_set(bt_sync, disabled);
}

void main_menu_sync_disable_set(Eina_Bool disabled)
{
   elm_object_disabled_set(libraries_list, disabled);
}

void main_menu_nolibrary_disabled_set(Eina_Bool disabled)
{
   main_menu_loading_disable_set(disabled);
   elm_object_disabled_set(libraries_list, EINA_FALSE);
   elm_object_disabled_set(bt_del_bg, disabled);
   elm_object_disabled_set(bt_sync, disabled);
}

void main_menu_update_libraries_list()
{
    Enlil_String *string;
    Eina_List *list = enlil_library_eet_path_load();

    elm_gengrid_clear(libraries_list);

    //
    Library *lib = calloc(1, sizeof(Library));
	lib->is_new_library = EINA_TRUE;
	lib->path = eina_stringshare_add("Add a new Library");
    elm_gengrid_item_append(libraries_list, &itc_grid, lib, _library_select, NULL);
    //

    EINA_LIST_FREE(list, string)
    {
    	Library *lib = calloc(1, sizeof(Library));
    	lib->is_new_library = EINA_FALSE;
    	lib->path = eina_stringshare_add(string->string);

        lib->photo1 = enlil_photo_new();
        Enlil_Photo_Data *photo_data1 = calloc(1, sizeof(Enlil_Photo_Data));
        enlil_photo_user_data_set(lib->photo1, photo_data1, enlil_photo_data_free);
        enlil_photo_mustNotBeSaved_set(lib->photo1, EINA_TRUE);

        lib->photo2 = enlil_photo_new();
        Enlil_Photo_Data *photo_data2 = calloc(1, sizeof(Enlil_Photo_Data));
        enlil_photo_user_data_set(lib->photo2, photo_data2, enlil_photo_data_free);
        enlil_photo_mustNotBeSaved_set(lib->photo2, EINA_TRUE);

        photo_data1->library_item = elm_gengrid_item_append(libraries_list, &itc_grid, lib, _library_select, NULL);
        photo_data2->library_item = photo_data1->library_item;

        EINA_STRINGSHARE_DEL(string->string);
        FREE(string);
    }
}


static char* _library_get(void *data, Evas_Object *obj, const char *part)
{
	Library *lib = data;
	char *name = strdup(lib->path);
	char *name_old = name;

	if(strlen(name) > 40)
	{
		name = name + strlen(name) - 40;

		int i = 0;
		name--;
		while(i< 3 && name >= name_old)
		{
			*name = '.';
			name--;
			i++;
		}
		name++;
	}

	name = strdup(name);
	FREE(name_old);

	return name;
}

static void  _library_del(void *data, Evas_Object *obj)
{
	Library *lib = data;
	EINA_STRINGSHARE_DEL(lib->path);
	if(lib->photo1)
		enlil_photo_free(&(lib->photo1));
	if(lib->photo2)
		enlil_photo_free(&(lib->photo2));
	FREE(lib);
}

static Evas_Object* _library_icon_get(void *data, Evas_Object *obj, const char *part)
{
	const char *s1 = NULL, *s2 = NULL, *g1 = NULL, *g2 = NULL;;
	Library *lib = data;

	if(strcmp(part, "elm.swallow.icon"))
		return NULL;

	if(lib->is_new_library)
	{
		Evas_Object *o = elm_layout_add(obj);
		elm_layout_file_set(o, Theme, "photo_library");

		Evas_Object *icon = elm_icon_add(obj);
		elm_icon_file_set(icon, Theme, "icons/plus");
		elm_icon_fill_outside_set(icon, EINA_TRUE);
		elm_layout_content_set(o, "object.photo.front.swallow" , icon);

		icon = elm_icon_add(obj);
		elm_icon_file_set(icon, Theme, "icons/folder");
		elm_icon_fill_outside_set(icon, EINA_TRUE);
		elm_layout_content_set(o, "object.photo.back.swallow" , icon);

		return o;
	}


	Evas_Object *o = elm_layout_add(obj);
	elm_layout_file_set(o, Theme, "photo_library");

	//
	Enlil_Photo *photo1 = enlil_library_photo_get(lib->path,1);
	if(photo1)
	{
		if( enlil_photo_path_get(lib->photo1) != enlil_photo_path_get(photo1)
				|| enlil_photo_file_name_get(lib->photo1) != enlil_photo_file_name_get(photo1))
		{
			enlil_photo_path_set(lib->photo1, enlil_photo_path_get(photo1));
			enlil_photo_file_name_set(lib->photo1, enlil_photo_file_name_get(photo1));
		}
        Enlil_Photo_Data *photo_data1 = enlil_photo_user_data_get(lib->photo1);

        if(photo1 && !photo_data1->cant_create_thumb)
			s1 = enlil_thumb_photo_get(lib->photo1, Enlil_THUMB_FDO_LARGE, thumb_done_cb, thumb_error_cb, NULL);

		enlil_photo_free(&(photo1));
		//

		//
		Enlil_Photo *photo2 = enlil_library_photo_get(lib->path,2);
		if( enlil_photo_path_get(lib->photo2) != enlil_photo_path_get(photo2)
				|| enlil_photo_file_name_get(lib->photo2) != enlil_photo_file_name_get(photo2))
		{
			enlil_photo_path_set(lib->photo2, enlil_photo_path_get(photo2));
			enlil_photo_file_name_set(lib->photo2, enlil_photo_file_name_get(photo2));
		}
		Enlil_Photo_Data *photo_data2 = enlil_photo_user_data_get(lib->photo2);

		if(photo2 && !photo_data2->cant_create_thumb)
			s2 = enlil_thumb_photo_get(lib->photo2, Enlil_THUMB_FDO_LARGE, thumb_done_cb, thumb_error_cb, NULL);

		enlil_photo_free(&(photo2));
		//

		if(!s1)
		{
			s1 = Theme;
			g1 = "libraries/icon/nophoto";
		}
		if(!s2)
		{
			s2 = Theme;
			g2 = "libraries/icon/nophoto";
		}
	}
	else
	{
		s1 = Theme;
		s2 = Theme;
		g1 = "libraries/icon/nophoto";
		g2 = "libraries/icon/nophoto";
	}

	Evas_Object *icon = elm_icon_add(obj);
	elm_icon_file_set(icon, s1, g1);
	elm_icon_fill_outside_set(icon, EINA_TRUE);
	elm_layout_content_set(o, "object.photo.front.swallow" , icon);

	icon = elm_icon_add(obj);
	elm_icon_file_set(icon, s2, g2);
	elm_icon_fill_outside_set(icon, EINA_TRUE);
	elm_layout_content_set(o, "object.photo.back.swallow" , icon);


	evas_object_show(o);
	return o;
}

static void _library_select(void *data, Evas_Object *obj, void *event_info)
{
	Library *lib = elm_gengrid_item_data_get(elm_gengrid_selected_item_get(obj));

	if(lib->is_new_library)
	{
		_new_library();
	}
	else
	{
		library_set(lib->path);
		select_list_photo();
	}

	//unselect items
    elm_gengrid_item_selected_set(elm_gengrid_selected_item_get(obj), EINA_FALSE);
	//
}

static void _new_library()
{
   Evas_Object *fs, *vbox;

   //create inwin & file selector
   inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin);

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);
   elm_win_inwin_content_set(inwin, vbox);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_folder_only_set(fs, 1);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _new_library_done_cb, inwin);
}

static void _new_library_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;
   if (selected)
     {
	   LOG_INFO("Create new library: %s\n", elm_fileselector_selected_get(obj));
	if(!ecore_file_exists(selected))
	  ecore_file_mkdir(selected);
	if(ecore_file_is_dir(selected))
        {
            library_set(selected);
            select_list_photo();
            main_menu_update_libraries_list();
        }
     }

   evas_object_del(data);
}

static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Photo *photo = NULL;
   PL_Child_Item *item = NULL;

   Eina_List *items = photos_list_object_selected_get(enlil_data->list_photo->o_list);

   if(items)
     {
	item = eina_list_data_get(items);
	photo = photos_list_object_item_data_get(item);
     }

   slideshow_clear();
   slideshow_library_add(enlil_data->library, photo);
   slideshow_show();
}

static void _album_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   inwin_album_new_new(NULL, NULL);
}

static void _import_cb(void *data, Evas_Object *obj, void *event_info)
{
   import_new(enlil_data->win->win);
}

static void _del_bg_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   enlil_win_bg_set(enlil_data->win, NULL);

   s.string = NULL;
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME" background", enlil_library_path_get(enlil_data->library));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
}


static void _geocaching_import_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *fs, *vbox;

   inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin);

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);
   elm_win_inwin_content_set(inwin, vbox);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs,  getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _geocaching_import_done_cb, NULL);
}

static void _geocaching_import_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;

   if (selected)
     {
	char *ext = strchr(selected, '.');
	if(ext && !strcmp(ext, ".gpx"))
	  {
	     enlil_geocaching_import(selected, geocaching_done_cb, NULL);
	  }
     }
   evas_object_del(inwin);
}


static void _preferences_cb(void *data, Evas_Object *obj, void *event_info)
{
	inwin_preferences_new();
}

static void _sync_cb(void *data, Evas_Object *obj, void *event_info)
{
	//Enlil_Flickr_Job *job =
	enlil_netsync_job_sync_albums_append(enlil_data->library, netsync_album_new_cb,
			netsync_album_notinnetsync_cb, netsync_album_notuptodate_cb, netsync_album_netsyncnotuptodate_cb,
			netsync_album_uptodate_cb, flickr_error_cb, enlil_data);
}

static void _quit_cb(void *data, Evas_Object *obj, void *event_info)
{
   close_cb(NULL, NULL, NULL);
}


