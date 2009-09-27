// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"

struct pm_album
{
    //Global header (required) => unique key
    const char *file_name;
    const char *path;

    const char *name;
    long long time;

    //header
    //nothing

    //list of PM_Photo*
    Eina_List *photos;

    //extra data, not saved in an Eet file
    int header_load_is;
    int photos_load_is;
    PM_Root *root;
    Ecore_File_Monitor *monitor;
    void *user_data;
};


static void _album_eet_photos_load(PM_Album *album);
static int _album_eet_photos_save(PM_Album *album);
static void _album_monitor_cb(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path);
static Eet_Data_Descriptor * _pm_album_photos_file_name_edd_new(Eet_Data_Descriptor *edd_file_name);
static PM_Album *_pm_album_eet_photos_list_load(PM_Album *album);
static Eet_Data_Descriptor * _pm_album_photos_file_name_edd_new(Eet_Data_Descriptor *edd_file_name);

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

PM_Album *pm_album_new()
{
    return calloc(1,sizeof(PM_Album));
}

/**
 * @brief Create a copy of an album.
 */
PM_Album *pm_album_copy_new(const PM_Album *album)
{
    ASSERT_RETURN(album!=NULL);

    PM_Album *_album = pm_album_new();

    pm_album_copy(album, _album);

    //the list of photos is loaded when necessary, that's why we do not copy it
    return _album;
}

/**
 * @brief Copy an PM_Album in a second album. This method is used when a change is detected on an album already loaded. We load the changed album (album_src) and copy his fields in the loaded album (album_dest).<br>
 * A change on an album doesn't means we have a change in the photo list, the changes on a photo are managed separately. Consequently we do not copy the photo list.
 */
void pm_album_copy(const PM_Album *album_src, PM_Album *album_dest)
{
    ASSERT_RETURN_VOID(album_src!=NULL);
    ASSERT_RETURN_VOID(album_dest!=NULL);

    pm_album_name_set(album_dest, pm_album_name_get(album_src));
    pm_album_file_name_set(album_dest, pm_album_file_name_get(album_src));
    pm_album_path_set(album_dest, pm_album_path_get(album_src));
    pm_album_time_set(album_dest, pm_album_time_get(album_src));
}


void pm_album_free(PM_Album **album)
{
    PM_Photo *photo;

    ASSERT_RETURN_VOID(album!=NULL);
    ASSERT_RETURN_VOID((*album)!=NULL);
    EINA_STRINGSHARE_DEL( (*album)->name);
    EINA_STRINGSHARE_DEL( (*album)->file_name);
    EINA_STRINGSHARE_DEL( (*album)->path);

    EINA_LIST_FREE( (*album)->photos, photo)
        pm_photo_free(&photo);

    pm_album_monitor_stop(*album);

    FREE(*album);
}

void pm_album_list_print(Eina_List *l_pm)
{
    Eina_List *l;
    PM_Album *album;
    ASSERT_RETURN_VOID(l_pm!=NULL);
    EINA_LIST_FOREACH(l_pm, l, album)
    {
        printf("\n");
        pm_album_print(album);
    }
}

void pm_album_print(PM_Album *album)
{
    ASSERT_RETURN_VOID(album!=NULL);
    printf("\t## ALBUM ##\n");
    printf("\t###########\n");
    printf("Name\t:\t%s\n", album->name);
    printf("path\t:\t%s\n", album->path);
    printf("\t## PHOTO ##\n");
    pm_photo_list_print(pm_album_photos_get(album));
}

void pm_album_monitor_start(PM_Album *album)
{
    char buf[PATH_MAX];

    ASSERT_RETURN_VOID(album!=NULL);
    ASSERT_RETURN_VOID(album->monitor==NULL);

    snprintf(buf,PATH_MAX,"%s/%s", pm_album_path_get(album), pm_album_file_name_get(album));
    album->monitor = ecore_file_monitor_add(buf, _album_monitor_cb, album);
    ASSERT_RETURN_VOID(album->monitor!=NULL);
}

void pm_album_monitor_stop(PM_Album *album)
{
    ASSERT_RETURN_VOID(album!=NULL);

    if(album->monitor)
        ecore_file_monitor_del(album->monitor);
}


#define FCT_NAME pm_album
#define STRUCT_TYPE PM_Album

SET(root, PM_Root *)
STRING_SET(name)
STRING_SET(file_name)
SET(time, long long)
SET(photos, Eina_List*)
SET(user_data, void *)

GET(root, PM_Root*)
GET(name, const char*)
GET(file_name, const char*)
GET(path, const char*)
GET(time, long long)
GET(user_data, void *)

#undef FCT_NAME
#undef STRUCT_TYPE

void pm_album_path_set(PM_Album *album, const char *path)
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



Eina_List * pm_album_photos_get(PM_Album *album)
{
    ASSERT_RETURN(album!=NULL);
    ALBUM_PHOTOS_LOAD(album);

    return album->photos;
}

int pm_album_photos_count_get(PM_Album *album)
{
    ASSERT_RETURN(album!=NULL);
    ALBUM_PHOTOS_LOAD(album);

    return eina_list_count(album->photos);
}

/**
 * @brief
 * @param file_name The photo file name. The string have to be in eina_stringshare
 */
PM_Photo *pm_album_photo_search_file_name(PM_Album *album, const char *file_name)
{
    Eina_List *l;
    PM_Photo *photo;
    ASSERT_RETURN(album!=NULL);
    ASSERT_RETURN(file_name!=NULL);

    pm_album_photos_get(album);

    EINA_LIST_FOREACH(album->photos, l, photo)
    {
        if(pm_photo_file_name_get(photo) == file_name)
            return photo;
    }

    return NULL;
}


void pm_album_photo_add(PM_Album *album, PM_Photo *photo)
{
    ASSERT_RETURN_VOID(album!=NULL);
    ASSERT_RETURN_VOID(photo!=NULL);

    pm_album_photos_get(album);

    pm_photo_album_set(photo, album);
    album->photos = eina_list_append(album->photos, photo);
}

void pm_album_photo_remove(PM_Album *album, PM_Photo *photo)
{
    ASSERT_RETURN_VOID(album!=NULL);
    ASSERT_RETURN_VOID(photo!=NULL);

    pm_album_photos_get(album);

    pm_photo_album_set(photo, NULL);
    album->photos = eina_list_remove(album->photos, photo);
}

int pm_album_eet_photos_save(PM_Album *album)
{
    ASSERT_RETURN(album!=NULL);

    return _album_eet_photos_save(album);
}


int pm_album_eet_global_header_save(PM_Album *album)
{
    Eet_Data_Descriptor *edd;
    Eet_File *f;
    char buf[PATH_MAX], path[PATH_MAX];
    int res;

    ASSERT_RETURN(album!=NULL);

    pm_album_eet_header_save(album);

    snprintf(path,PATH_MAX,"%s/"EET_FILE,pm_album_path_get(album));
    f = pm_file_manager_open(path);
    ASSERT_RETURN(f!=NULL);

    edd = _pm_album_global_header_edd_new();
    snprintf(buf,PATH_MAX,"/album %s",pm_album_file_name_get(album));
    res=eet_data_write(f, edd, buf, album, 0);

    eet_data_descriptor_free(edd);
    pm_file_manager_close(path);

    return res;
}

int pm_album_eet_header_save(PM_Album *album)
{
    Eet_Data_Descriptor *edd;
    Eet_File *f;
    char path[PATH_MAX];
    int res;

    ASSERT_RETURN(album!=NULL);

    snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,pm_album_path_get(album),
            pm_album_file_name_get(album));
    f = pm_file_manager_open(path);
    ASSERT_RETURN(f!=NULL);

    edd = _pm_album_header_edd_new();
    res=eet_data_write(f, edd, "header", album, 0);

    eet_data_descriptor_free(edd);
    pm_file_manager_close(path);

    return res;
}

static void _album_monitor_cb(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
    PM_Album *album = (PM_Album*)data;
    PM_Root *pm = album->root;

    ASSERT_RETURN_VOID(pm != NULL);

    Photo_Manager_Configuration conf = pm_conf_get(pm);

    //here we only manage the changes on photos
    if(!pm_photo_is(path))
        return ;

    switch(event)
    {
        case ECORE_FILE_EVENT_CREATED_FILE:
            LOG_INFO("New photo: %s", path);
            conf.photo_new_cb(conf.data, pm, album, path);
            break;
        case ECORE_FILE_EVENT_DELETED_FILE:
            LOG_INFO("Deleted photo: %s", path);
            conf.photo_delete_cb(conf.data, pm, album, path);
            break;
        case ECORE_FILE_EVENT_MODIFIED:
            LOG_INFO("Updated photo: %s", path);
            conf.photo_update_cb(conf.data, pm, album, path);
            break;
        default: ;
    }
}
static PM_Album *_pm_album_eet_photos_list_load(PM_Album *album)
{
    Eet_Data_Descriptor *edd, *edd_photo;
    Eet_File *f;
    PM_Album *data;
    char path[PATH_MAX];

    ASSERT_RETURN(album!=NULL);

    snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,pm_album_path_get(album), pm_album_file_name_get(album));
    f = pm_file_manager_open(path);
    ASSERT_RETURN(f!=NULL);

    edd_photo = _pm_photo_file_name_edd_new();
    edd = _pm_album_photos_file_name_edd_new(edd_photo);

    data = eet_data_read(f, edd, "/photo_list");

    pm_file_manager_close(path);
    eet_data_descriptor_free(edd_photo);
    eet_data_descriptor_free(edd);

    if(data)
        data->photos_load_is  = 1;
    return data;
}

int pm_album_eet_photos_list_save(PM_Album *album)
{
    int res;
    Eet_Data_Descriptor *edd, *edd_photo;
    Eet_File *f;
    char path[PATH_MAX];

    ASSERT_RETURN(album!=NULL);

    snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,pm_album_path_get(album), pm_album_file_name_get(album));
    f = pm_file_manager_open(path);
    ASSERT_RETURN(f!=NULL);

    edd_photo = _pm_photo_file_name_edd_new();
    edd = _pm_album_photos_file_name_edd_new(edd_photo);

    res = eet_data_write(f, edd, "/photo_list", album, 0);

    pm_file_manager_close(path);
    eet_data_descriptor_free(edd_photo);
    eet_data_descriptor_free(edd);

    return res;
}

static void _album_eet_photos_load(PM_Album *album)
{
    PM_Photo *photo;
    char path[PATH_MAX];
    Eina_List *l;
    PM_Album *album_list;
    PM_Photo *photo_list;

    ASSERT_RETURN_VOID(album!=NULL);

    album->photos_load_is  = 1;

    album_list = _pm_album_eet_photos_list_load(album);
    ASSERT_RETURN_VOID(album_list!=NULL);

    snprintf(path,1024,"%s/%s/"EET_FILE,pm_album_path_get(album), pm_album_file_name_get(album));
    EINA_LIST_FOREACH(pm_album_photos_get(album_list), l, photo_list)
    {
        photo = pm_photo_eet_load(path, pm_photo_file_name_get(photo_list));
        if(photo)
            album->photos = eina_list_append(album->photos, photo);
    }
    pm_album_free(&album_list);
}

static int _album_eet_photos_save(PM_Album *album)
{
    Eina_List *l;
    PM_Photo *photo;
    char path[PATH_MAX];
    int res = 1;

    ASSERT_RETURN(album!=NULL);

    snprintf(path,PATH_MAX,"%s/%s/"EET_FILE,pm_album_path_get(album), pm_album_file_name_get(album));

    EINA_LIST_FOREACH(album->photos, l, photo)
        res = pm_photo_eet_save(photo);

    return res;
}


Eet_Data_Descriptor * _pm_album_file_name_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Album);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "path", path, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "file_name", file_name, EET_T_STRING);

    return edd;
}



static Eet_Data_Descriptor * _pm_album_photos_file_name_edd_new(Eet_Data_Descriptor *edd_file_name)
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Album);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_LIST(edd, PM_Album, "photos", photos, edd_file_name);

    return edd;
}


Eet_Data_Descriptor * _pm_album_global_header_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Album);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "name", name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "path", path, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "file_name", file_name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "time", time, EET_T_LONG_LONG);

    return edd;
}

Eet_Data_Descriptor * _pm_album_header_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Album);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "name", name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "path", path, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "file_name", file_name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Album, "time", time, EET_T_LONG_LONG);

    return edd;
}

