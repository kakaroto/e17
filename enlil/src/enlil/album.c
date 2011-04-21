#include "enlil_private.h"

struct enlil_album
{
	const char *file_name;
	const char *path;

	const char *name;
	const char *description;
	long long _time;

	Enlil_Album_Access_Type access_type;

	Enlil_Photo_Sort photos_sort;
	//list of Enlil_Photo*
	Eina_List *photos;

	//list of Enlil_Album_Collection
	Eina_List *collections;

	struct
	{
		int id;
		int version_header;
		int version_header_net;
		int timestamp_last_update_collections;
	} netsync;

	//extra data, not saved in an Eet file
	int header_load_is;
	int photos_load_is;
	Enlil_Library *library;
	Ecore_File_Monitor *monitor;
	void *user_data;
	Enlil_Album_Free_Cb free_cb;
};

static void _sort_photos(Enlil_Album *album);
static int _sort_photos_name_cb(const void *d1, const void *d2);
static int _sort_photos_date_cb(const void *d1, const void *d2);

static void _album_eet_photos_load(Enlil_Album *album);
static int _album_eet_photos_save(Enlil_Album *album);
static void _album_monitor_cb(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path);
static Eet_Data_Descriptor * _enlil_album_photos_file_name_edd_new(Eet_Data_Descriptor *edd_file_name);
static Enlil_Album *_enlil_album_eet_photos_list_load(Enlil_Album *album);
static Eet_Data_Descriptor * _enlil_album_photos_file_name_edd_new(Eet_Data_Descriptor *edd_file_name);

#define ALBUM_HEADER_LOAD(album) \
		if(!album->header_load_is) \
		{ \
			_album_eet_header_load(album); \
			album->header_load_is = 1; \
		}

#define ALBUM_PHOTOS_LOAD(album) \
		if(!album->photos_load_is) \
		{ \
			_album_eet_photos_load(album); \
			album->photos_load_is = 1; \
		}

Enlil_Album *enlil_album_new()
{
	return calloc(1,sizeof(Enlil_Album));
}

/**
 * @brief Create a copy of an album.
 */
Enlil_Album *enlil_album_copy_new(const Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);

	Enlil_Album *_album = enlil_album_new();

	enlil_album_copy(album, _album);

	//the list of photos is loaded when necessary, that's why we do not copy it
	return _album;
}

/**
 * @brief Copy an Enlil_Album in a second album. This method is used when a change is detected on an album already loaded. We load the changed album (album_src) and copy his fields in the loaded album (album_dest).<br>
 * A change on an album doesn't means we have a change in the photo list, the changes on a photo are managed separately. Consequently we do not copy the photo list.
 */
void enlil_album_copy(const Enlil_Album *album_src, Enlil_Album *album_dest)
{
	const Eina_List *l;
	const Enlil_Album_Collection *album_col;

	ASSERT_RETURN_VOID(album_src!=NULL);
	ASSERT_RETURN_VOID(album_dest!=NULL);

	enlil_album_name_set(album_dest, enlil_album_name_get(album_src));
	enlil_album_file_name_set(album_dest, enlil_album_file_name_get(album_src));
	enlil_album_path_set(album_dest, enlil_album_path_get(album_src));
	enlil_album_description_set(album_dest, enlil_album_description_get(album_src));
	enlil_album__time_set(album_dest, enlil_album__time_get(album_src));

	EINA_LIST_FOREACH(enlil_album_collections_get(album_src), l, album_col)
	{
		Enlil_Album_Collection *_album_col = calloc(1, sizeof(Enlil_Album_Collection));
		_album_col->name = eina_stringshare_add(album_col->name);
		_album_col->album = album_dest;
		_album_col->collection = NULL;
		album_dest->collections = eina_list_append(album_dest->collections, _album_col);
	}
}


void enlil_album_free(Enlil_Album **album)
{
	Enlil_Photo *photo;
	Enlil_Album_Collection *album_col;

	ASSERT_RETURN_VOID(album!=NULL);
	ASSERT_RETURN_VOID((*album)!=NULL);

	EINA_LIST_FREE( (*album)->photos, photo)
	enlil_photo_free(&photo);

	EINA_LIST_FREE( (*album)->collections, album_col)
	{
		if((*album)->library)
			_enlil_library_collection_album_remove((*album)->library, album_col, *album);

		eina_stringshare_del(album_col->name);
		free(album_col);
	}

	if( (*album)->free_cb )
		(*album)->free_cb((*album), (*album)->user_data);

	if(enlil_album_library_get(*album))
		enlil_library_album_remove(enlil_album_library_get(*album), *album);

	EINA_STRINGSHARE_DEL( (*album)->name);
	EINA_STRINGSHARE_DEL( (*album)->file_name);
	EINA_STRINGSHARE_DEL( (*album)->path);
	EINA_STRINGSHARE_DEL( (*album)->description);

	enlil_album_monitor_stop(*album);

	FREE(*album);
}

void enlil_album_list_print(Eina_List *l_enlil)
{
	Eina_List *l;
	Enlil_Album *album;
	ASSERT_RETURN_VOID(l_enlil!=NULL);
	EINA_LIST_FOREACH(l_enlil, l, album)
	{
		printf("\n");
		enlil_album_print(album);
	}
}

void enlil_album_print(Enlil_Album *album)
{
	ASSERT_RETURN_VOID(album!=NULL);
	printf("\t## ALBUM ##\n");
	printf("\t###########\n");
	printf("Name\t:\t%s\n", album->name);
	printf("Path\t:\t%s\n", album->path);
	printf("\t## PHOTO ##\n");
	enlil_photo_list_print(enlil_album_photos_get(album));
}

void enlil_album_monitor_start(Enlil_Album *album)
{
	char buf[PATH_MAX];

	ASSERT_RETURN_VOID(album!=NULL);
	ASSERT_RETURN_VOID(album->monitor==NULL);

	snprintf(buf,PATH_MAX,"%s/%s", enlil_album_path_get(album), enlil_album_file_name_get(album));
	album->monitor = ecore_file_monitor_add(buf, _album_monitor_cb, album);
	ASSERT_RETURN_VOID(album->monitor!=NULL);
}

void enlil_album_monitor_stop(Enlil_Album *album)
{
	ASSERT_RETURN_VOID(album!=NULL);

	if(album->monitor)
		ecore_file_monitor_del(album->monitor);
	album->monitor = NULL;
}


#define FCT_NAME enlil_album
#define STRUCT_TYPE Enlil_Album

SET(library, Enlil_Library *)
STRING_SET(file_name)
SET(_time, long long)
SET(photos, Eina_List*)

GET(library, Enlil_Library*)
GET(name, const char*)
GET(access_type, Enlil_Album_Access_Type)
GET(file_name, const char*)
GET(path, const char*)
GET(description, const char*)
GET(_time, long long)
GET(user_data, void *)
GET(collections, const Eina_List *)
GET(photos_sort, Enlil_Photo_Sort)

#undef FCT_NAME
#undef STRUCT_TYPE

void enlil_album_name_set(Enlil_Album *album, const char *name)
{
	ASSERT_RETURN_VOID(album!=NULL);
	ASSERT_RETURN_VOID(name != NULL);

	if(name[0] == '\0' && album->name == NULL)
		return ;

	const char *new_name = eina_stringshare_add(name);

	if(album->name == new_name)
	{
		EINA_STRINGSHARE_DEL(new_name);
		return ;
	}
	EINA_STRINGSHARE_DEL(album->name);

	album->name = new_name;

	if(album->library)
		_enlil_library_album_name_changed(album->library, album);

	_enlil_album_netsync_version_header_inc(album);
}

void enlil_album_access_type_set(Enlil_Album *album, Enlil_Album_Access_Type access_type)
{
	ASSERT_RETURN_VOID(album!=NULL);

	if(album->access_type == access_type)
		return ;
	album->access_type = access_type;
	_enlil_album_netsync_version_header_inc(album);
}

void enlil_album_description_set(Enlil_Album *album, const char *desc)
{
	ASSERT_RETURN_VOID(!!album);
	ASSERT_RETURN_VOID(!!desc);

	if(desc[0] == '\0' && album->description == NULL)
			return ;

	const char *new_description = eina_stringshare_add(desc);

	if(album->description == new_description)
	{
		EINA_STRINGSHARE_DEL(new_description);
		return ;
	}
	EINA_STRINGSHARE_DEL(album->description);

	album->description = new_description;

	_enlil_album_netsync_version_header_inc(album);
}

void enlil_album_photos_sort_set(Enlil_Album *album, Enlil_Photo_Sort photos_sort)
{
	ASSERT_RETURN_VOID(album!=NULL);
	album->photos_sort = photos_sort;
	enlil_album_eet_header_save(album);
	_sort_photos(album);

	//_enlil_album_netsync_verson_header_set(album, time(NULL));
}

void enlil_album_user_data_set(Enlil_Album *album, void *user_data, Enlil_Album_Free_Cb cb)
{
	ASSERT_RETURN_VOID(album!=NULL);
	album->user_data = user_data;
	album->free_cb = cb;
}

void enlil_album_path_set(Enlil_Album *album, const char *path)
{
	ASSERT_RETURN_VOID(album!=NULL);
	ASSERT_RETURN_VOID(path!=NULL);

	EINA_STRINGSHARE_DEL(album->path);

	char *_path = strdup(path);
	int len = strlen(_path) -1;
	if(_path[len] == '/')
		_path[len] = '\0';

	album->path = eina_stringshare_add(_path);
	FREE(_path);
}

int enlil_album_netsync_id_get(Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);

	return album->netsync.id;
}


int enlil_album_netsync_version_header_get(Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);

	return album->netsync.version_header;
}

int enlil_album_netsync_version_header_net_get(Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);

	return album->netsync.version_header_net;
}

void _enlil_album_netsync_version_header_inc(Enlil_Album *album)
{
	ASSERT_RETURN_VOID(album!=NULL);

	album->netsync.version_header++;

    enlil_album_eet_header_save(album);
    Enlil_Library *library = enlil_album_library_get(album);
    if(library)
 	   enlil_library_eet_albums_save(library);

    if(library && _enlil_library_album_version_header_increase_cb_get(library))
    	_enlil_library_album_version_header_increase_cb_get(library)(
    			_enlil_library_album_version_header_increase_data_get(library), album);
}

void _enlil_album_netsync_version_header_both_set(Enlil_Album *album, int version)
{
	ASSERT_RETURN_VOID(album!=NULL);

	album->netsync.version_header = version;
	album->netsync.version_header_net = version;

    enlil_album_eet_header_save(album);
    Enlil_Library *library = enlil_album_library_get(album);
    if(library)
 	   enlil_library_eet_albums_save(library);
}

int enlil_album_netsync_timestamp_last_update_collections_get(Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);

	return album->netsync.timestamp_last_update_collections;
}

void _enlil_album_netsync_timestamp_last_update_collections_set(Enlil_Album *album, int timestamp)
{
	ASSERT_RETURN_VOID(album!=NULL);

	album->netsync.timestamp_last_update_collections = timestamp;

	enlil_album_eet_header_save(album);
    Enlil_Library *library = enlil_album_library_get(album);
    if(library)
 	   enlil_library_eet_albums_save(library);
}
void _enlil_album_netsync_id_set(Enlil_Album *album, int id)
{
	ASSERT_RETURN_VOID(album != NULL);

	album->netsync.id = id;
}

Eina_List * enlil_album_photos_get(Enlil_Album *album)
		{
	ASSERT_RETURN(album!=NULL);
	ALBUM_PHOTOS_LOAD(album);

	return album->photos;
		}

int enlil_album_photos_count_get(Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);
	ALBUM_PHOTOS_LOAD(album);

	return eina_list_count(album->photos);
}

/**
 * If the album is in a library ( see enlil_album_library_set() ), this method will add the album
 * in the collection defined in the library. If the collection does not exists, it is created.
 *
 * @param album The album struct
 */
void enlil_album_collection_process(Enlil_Album *album)
{
	Eina_List *l;
	Enlil_Album_Collection *album_col;
	ASSERT_RETURN_VOID(album != NULL);
	ASSERT_RETURN_VOID(album->library != NULL);

	EINA_LIST_FOREACH(album->collections, l, album_col)
	{
		album_col->album = album;
		if(!album_col -> collection)
			_enlil_library_collection_album_add(album->library, album_col, album);
	}
}

/**
 * Add the album in a collection.
 *
 * If the album is in a library ( see enlil_album_library_set() ), this method will add the album
 * in the collection defined in the library. If the collection does not exists, it is created.
 *
 * @param album The album struct
 * @param col_name The name of the collection
 */
void enlil_album_collection_add(Enlil_Album *album, const char *col_name)
{
	Enlil_Album_Collection *album_col;
	ASSERT_RETURN_VOID(album != NULL);
	ASSERT_RETURN_VOID(col_name != NULL);

	album_col = calloc(1, sizeof(Enlil_Album_Collection));
	album_col->name = eina_stringshare_add(col_name);
	album_col->album = album;

	album->collections = eina_list_append(album->collections, album_col);

	if(album->library)
		_enlil_library_collection_album_add(album->library, album_col, album);

	enlil_album_eet_header_save(album);
	enlil_library_eet_collections_save(album->library);
}

/**
 * Remove the album from the collection.
 *
 * If the album is in a library ( see enlil_album_library_set() ), this method will remove the album
 * from the collection defined in the library.
 *
 * @param album The album struct
 * @param album_col The album collection struct
 */
void enlil_album_collection_remove(Enlil_Album *album, Enlil_Album_Collection *album_col)
{
	ASSERT_RETURN_VOID(album != NULL);
	ASSERT_RETURN_VOID(album_col != NULL);

	album->collections = eina_list_remove(album->collections, album_col);

	if(album->library)
		_enlil_library_collection_album_remove(album->library, album_col, album);

	eina_stringshare_del(album_col->name);
	free(album_col);

	enlil_album_eet_header_save(album);
	enlil_library_eet_collections_save(album->library);
}

/**
 * @brief
 * @param file_name The photo file name. The string have to be in eina_stringshare
 */
Enlil_Photo *enlil_album_photo_search_file_name(Enlil_Album *album, const char *file_name)
{
	Eina_List *l;
	Enlil_Photo *photo;
	ASSERT_RETURN(album!=NULL);
	ASSERT_RETURN(file_name!=NULL);

	enlil_album_photos_get(album);

	EINA_LIST_FOREACH(album->photos, l, photo)
	{
		if(enlil_photo_file_name_get(photo) == file_name)
			return photo;
	}

	return NULL;
}

void enlil_album_photo_add(Enlil_Album *album, Enlil_Photo *photo)
{
	ASSERT_RETURN_VOID(album!=NULL);
	ASSERT_RETURN_VOID(photo!=NULL);

	enlil_album_photos_get(album);

	enlil_photo_album_set(photo, album);
	switch(album->photos_sort)
	{
	case ENLIL_PHOTO_SORT_NAME:
		album->photos = eina_list_sorted_insert(album->photos, _sort_photos_name_cb, photo);
		break;
	case ENLIL_PHOTO_SORT_DATE:
		album->photos = eina_list_sorted_insert(album->photos, _sort_photos_date_cb, photo);
		break;
	default :
		album->photos = eina_list_append(album->photos, photo);
	}
}

void _enlil_album_photo_name_changed(Enlil_Album *album, Enlil_Photo *photo)
{
	ASSERT_RETURN_VOID(album != NULL);
	ASSERT_RETURN_VOID(photo != NULL);

	if(album->photos_sort == ENLIL_PHOTO_SORT_NAME)
	{
		album->photos = eina_list_remove(album->photos, photo);
		enlil_album_photo_add(album, photo);
		enlil_album_eet_photos_list_save(album);
	}
}

void _enlil_album_photo_datetimeoriginal_changed(Enlil_Album *album, Enlil_Photo *photo)
{
	ASSERT_RETURN_VOID(album != NULL);
	ASSERT_RETURN_VOID(photo != NULL);

	if(album->photos_sort == ENLIL_PHOTO_SORT_DATE)
	{
		album->photos = eina_list_remove(album->photos, photo);
		enlil_album_photo_add(album, photo);
		enlil_album_eet_photos_list_save(album);
	}
}

Enlil_Photo *enlil_album_photo_prev_get(Enlil_Album *album, Enlil_Photo *photo)
{
	ASSERT_RETURN(album != NULL);
	ASSERT_RETURN(photo != NULL);

	Eina_List *l = eina_list_data_find_list(album->photos, photo);
	return eina_list_data_get(eina_list_prev(l));
}


void enlil_album_photo_remove(Enlil_Album *album, Enlil_Photo *photo)
{
	ASSERT_RETURN_VOID(album!=NULL);
	ASSERT_RETURN_VOID(photo!=NULL);

	enlil_album_photos_get(album);

	enlil_photo_album_set(photo, NULL);
	album->photos = eina_list_remove(album->photos, photo);
}

int enlil_album_eet_photos_save(Enlil_Album *album)
{
	ASSERT_RETURN(album!=NULL);

	return _album_eet_photos_save(album);
}

int enlil_album_eet_header_save(Enlil_Album *album)
{
	Eet_Data_Descriptor *edd, *edd_collection;
	Eet_File *f;
	char path[PATH_MAX];
	int res;

	ASSERT_RETURN(album!=NULL);

	snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,enlil_album_path_get(album),
			enlil_album_file_name_get(album));

	f = enlil_file_manager_open(path);
	ASSERT_RETURN(f!=NULL);

	edd_collection =  _enlil_album_collection_edd_new();
	edd = _enlil_album_header_edd_new(edd_collection);

	res=eet_data_write(f, edd, "header", album, 0);

	eet_data_descriptor_free(edd_collection);
	eet_data_descriptor_free(edd);
	enlil_file_manager_close(path);

	return res;
}

static void _sort_photos(Enlil_Album *album)
{
	ASSERT_RETURN_VOID(album != NULL);
	enlil_album_photos_get(album);

	switch(album->photos_sort)
	{
	case ENLIL_PHOTO_SORT_NAME:
		album->photos = eina_list_sort(album->photos, eina_list_count(album->photos), _sort_photos_name_cb);
		break;
	case ENLIL_PHOTO_SORT_DATE:
		album->photos = eina_list_sort(album->photos, eina_list_count(album->photos), _sort_photos_date_cb);
		break;
	default : ;
	}
}

static int _sort_photos_name_cb(const void *d1, const void *d2)
{
	const Enlil_Photo *photo1 = d1;
	const Enlil_Photo *photo2 = d2;

	if(!enlil_photo_name_get(photo1)) return 1;
	if(!enlil_photo_name_get(photo2)) return -1;

	return strcmp(enlil_photo_name_get(photo1), enlil_photo_name_get(photo2));
}

static int _sort_photos_date_cb(const void *d1, const void *d2)
{
	const Enlil_Photo *photo1 = d1;
	const Enlil_Photo *photo2 = d2;
	const char *date1, *date2;

	date1 = _enlil_photo_exif_datetimeoriginal_get(photo1);
	date2 = _enlil_photo_exif_datetimeoriginal_get(photo2);

	if(!date1) return 1;
	if(!date2) return -1;

	return strcmp(date1, date2);
}

static void _album_monitor_cb(void *data, __UNUSED__ Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
	Enlil_Album *album = (Enlil_Album*)data;
	Enlil_Library *enlil = album->library;

	ASSERT_RETURN_VOID(enlil != NULL);

	Enlil_Configuration conf = enlil_conf_get(enlil);

	//here we only manage the changes on photos
	if(!enlil_photo_is(path) && !enlil_video_is(path))
		return ;

	switch(event)
	{
	case ECORE_FILE_EVENT_CREATED_FILE:
		LOG_INFO("New photo: %s", path);
		conf.monitor.photo_new_cb(conf.data, enlil, album, path);
		break;
	case ECORE_FILE_EVENT_DELETED_FILE:
		LOG_INFO("Deleted photo: %s", path);
		conf.monitor.photo_delete_cb(conf.data, enlil, album, path);
		break;
	case ECORE_FILE_EVENT_MODIFIED:
		LOG_INFO("Updated photo: %s", path);
		conf.monitor.photo_update_cb(conf.data, enlil, album, path);
		break;
	default: ;
	}
}
static Enlil_Album *_enlil_album_eet_photos_list_load(Enlil_Album *album)
{
	Eet_Data_Descriptor *edd, *edd_photo;
	Eet_File *f;
	Enlil_Album *data;
	char path[PATH_MAX];

	ASSERT_RETURN(album!=NULL);

	snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,enlil_album_path_get(album), enlil_album_file_name_get(album));
	f = enlil_file_manager_open(path);
	ASSERT_RETURN(f!=NULL);

	edd_photo = _enlil_photo_file_name_edd_new();
	edd = _enlil_album_photos_file_name_edd_new(edd_photo);

	data = eet_data_read(f, edd, "/photo_list");

	enlil_file_manager_close(path);
	eet_data_descriptor_free(edd_photo);
	eet_data_descriptor_free(edd);

	if(data)
		data->photos_load_is  = 1;
	return data;
}

int enlil_album_eet_photos_list_save(Enlil_Album *album)
{
	int res;
	Eet_Data_Descriptor *edd, *edd_photo;
	Eet_File *f;
	char path[PATH_MAX];

	ASSERT_RETURN(album!=NULL);

	snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,enlil_album_path_get(album), enlil_album_file_name_get(album));
	f = enlil_file_manager_open(path);
	ASSERT_RETURN(f!=NULL);

	edd_photo = _enlil_photo_file_name_edd_new();
	edd = _enlil_album_photos_file_name_edd_new(edd_photo);

	res = eet_data_write(f, edd, "/photo_list", album, 0);

	enlil_file_manager_close(path);
	eet_data_descriptor_free(edd_photo);
	eet_data_descriptor_free(edd);

	return res;
}

static void _album_eet_photos_load(Enlil_Album *album)
{
	Enlil_Photo *photo;
	char path[PATH_MAX];
	Eina_List *l;
	Enlil_Album *album_list;
	Enlil_Photo *photo_list;

	ASSERT_RETURN_VOID(album!=NULL);

	album->photos_load_is  = 1;

	album_list = _enlil_album_eet_photos_list_load(album);
	ASSERT_RETURN_VOID(album_list!=NULL);

	snprintf(path,1024,"%s/%s/"EET_FILE,enlil_album_path_get(album), enlil_album_file_name_get(album));
	EINA_LIST_FOREACH(enlil_album_photos_get(album_list), l, photo_list)
	{
		photo = enlil_photo_eet_load(path, enlil_photo_file_name_get(photo_list));
		if(photo)
			album->photos = eina_list_append(album->photos, photo);
	}
	enlil_album_free(&album_list);
}

static int _album_eet_photos_save(Enlil_Album *album)
{
	Eina_List *l;
	Enlil_Photo *photo;
	char path[PATH_MAX];
	int res = 1;

	ASSERT_RETURN(album!=NULL);

	snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,enlil_album_path_get(album), enlil_album_file_name_get(album));

	EINA_LIST_FOREACH(album->photos, l, photo)
	res = enlil_photo_eet_save(photo);

	return res;
}


Eet_Data_Descriptor * _enlil_album_file_name_edd_new()
		{
	Eet_Data_Descriptor *edd;
	Eet_Data_Descriptor_Class eddc;

	EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Album);
	eddc.func.str_direct_alloc = NULL;
	eddc.func.str_direct_free = NULL;

	edd = eet_data_descriptor_file_new(&eddc);

	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "path", path, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "file_name", file_name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "name", name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.version_header", netsync.version_header, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.version_header_net", netsync.version_header_net, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.timestamp_last_update_collections", netsync.timestamp_last_update_collections, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.id", netsync.id, EET_T_INT);

	return edd;
		}



static Eet_Data_Descriptor * _enlil_album_photos_file_name_edd_new(Eet_Data_Descriptor *edd_file_name)
		{
	Eet_Data_Descriptor *edd;
	Eet_Data_Descriptor_Class eddc;

	EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Album);
	eddc.func.str_direct_alloc = NULL;
	eddc.func.str_direct_free = NULL;

	edd = eet_data_descriptor_file_new(&eddc);

	EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Album, "photos", photos, edd_file_name);

	return edd;
		}


Eet_Data_Descriptor * _enlil_album_header_edd_new(Eet_Data_Descriptor *edd_collection)
		{
	Eet_Data_Descriptor *edd;
	Eet_Data_Descriptor_Class eddc;

	ASSERT_RETURN(edd_collection != NULL);

	EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Album);
	eddc.func.str_direct_alloc = NULL;
	eddc.func.str_direct_free = NULL;

	edd = eet_data_descriptor_file_new(&eddc);

	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "name", name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "path", path, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "file_name", file_name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "description", description, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "access_type", access_type, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "_time", _time, EET_T_LONG_LONG);
	EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Album, "collections", collections, edd_collection);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "photos_sort", photos_sort, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.id", netsync.id, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.version_header", netsync.version_header, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.version_header_net", netsync.version_header_net, EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album, "netsync.timestamp_last_update_collections", netsync.timestamp_last_update_collections, EET_T_INT);

	return edd;
		}

Eet_Data_Descriptor * _enlil_album_collection_edd_new()
		{
	Eet_Data_Descriptor *edd;
	Eet_Data_Descriptor_Class eddc;

	EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Album_Collection);
	eddc.func.str_direct_alloc = NULL;
	eddc.func.str_direct_free = NULL;

	edd = eet_data_descriptor_file_new(&eddc);

	EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Album_Collection, "name", name, EET_T_STRING);

	return edd;
		}

