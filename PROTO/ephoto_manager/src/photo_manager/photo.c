// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"

struct pm_photo
{
    PM_Album *album;

    const char *name;
    const char *file_name;
    const char *path;
    long long time;
    const char *thumb_fdo_large;
    const char *thumb_fdo_normal;

    void *user_data;
    Photo_Manager_Photo_Free_Cb free_cb;
};

PM_Photo *pm_photo_new()
{
    return calloc(1,sizeof(PM_Photo));
}

PM_Photo *pm_photo_copy_new(PM_Photo *photo)
{
    ASSERT_RETURN(photo!=NULL);

    PM_Photo *_photo = pm_photo_new();

    pm_photo_copy(photo, _photo);
    return _photo;
}

/**
 * @brief Copy photo_src in photo_dest. This method can be used when a change occurs in a photo file. We have a photo (photo_src) which is updated and a second photo (photo_dest) which is not updated.
 */
void pm_photo_copy(PM_Photo *photo_src, PM_Photo *photo_dest)
{
    ASSERT_RETURN_VOID(photo_src!=NULL);
    ASSERT_RETURN_VOID(photo_dest!=NULL);

    pm_photo_name_set(photo_dest, pm_photo_name_get(photo_src));
    pm_photo_file_name_set(photo_dest, pm_photo_file_name_get(photo_src));
    pm_photo_path_set(photo_dest, pm_photo_path_get(photo_src));
    pm_photo_time_set(photo_dest, pm_photo_time_get(photo_src));
    pm_photo_thumb_fdo_normal_set(photo_dest, pm_photo_thumb_fdo_normal_get(photo_src));
    pm_photo_thumb_fdo_large_set(photo_dest, pm_photo_thumb_fdo_large_get(photo_src));
}

void pm_photo_free(PM_Photo **photo)
{
    ASSERT_RETURN_VOID(photo!=NULL);
    ASSERT_RETURN_VOID((*photo)!=NULL);

    if( (*photo)->free_cb )
      (*photo)->free_cb((*photo), (*photo)->user_data);

    EINA_STRINGSHARE_DEL((*photo)->name);
    EINA_STRINGSHARE_DEL((*photo)->file_name);
    EINA_STRINGSHARE_DEL((*photo)->path);

    FREE(*photo);
}

void pm_photo_list_print(Eina_List *l_photos)
{
    Eina_List *l;
    PM_Photo *photo;
    ASSERT_RETURN_VOID(l_photos!=NULL);
    EINA_LIST_FOREACH(l_photos, l, photo)
    {
        pm_photo_print(photo);
    }
}

void pm_photo_print(PM_Photo *photo)
{
    ASSERT_RETURN_VOID(photo!=NULL);
    printf("# Photo %s\n", photo->name);
    printf("\tFile name\t:\t%s\n", photo->file_name);
    printf("\tpath\t\t:\t%s\n", photo->path);
}

void pm_photo_album_set(PM_Photo *photo, PM_Album *album)
{
    ASSERT_RETURN_VOID(photo != NULL);
    photo->album = album;
}

#define FCT_NAME pm_photo
#define STRUCT_TYPE PM_Photo

STRING_SET(name)
STRING_SET(file_name)
SET(time, long long)

GET(album, PM_Album*)
GET(name, const char*)
GET(path, const char*)
GET(file_name, const char*)
GET(time, long long)
GET(user_data, void *)
GET(thumb_fdo_normal, const char*)
GET(thumb_fdo_large, const char*)

#undef FCT_NAME
#undef STRUCT_TYPE

void pm_photo_user_data_set(PM_Photo *photo, void *user_data, Photo_Manager_Photo_Free_Cb cb)
{
    ASSERT_RETURN_VOID(photo!=NULL);
    photo->user_data = user_data;
    photo->free_cb = cb;
}

void pm_photo_path_set(PM_Photo *photo, const char *path)
{
    ASSERT_RETURN_VOID(photo!=NULL);
    ASSERT_RETURN_VOID(path!=NULL);

    EINA_STRINGSHARE_DEL(photo->path);

    char *_path = strdup(path);
    int len = strlen(_path) -1;
    if(_path[len] == '/')
        _path[len] = '\0';

    photo->path = eina_stringshare_add(_path);
    FREE(_path);
}

void pm_photo_thumb_fdo_normal_set(PM_Photo *photo,const char *thumb)
{
    ASSERT_RETURN_VOID(photo != NULL);

    photo->thumb_fdo_normal = eina_stringshare_add(thumb);

    pm_photo_eet_save(photo);
}


void pm_photo_thumb_fdo_large_set(PM_Photo *photo,const char *thumb)
{
    ASSERT_RETURN_VOID(photo != NULL);

    photo->thumb_fdo_large = eina_stringshare_add(thumb);

    pm_photo_eet_save(photo);
}

int pm_photo_is(const char *file)
{
    ASSERT_RETURN(file!=NULL);

    char *strip_ext = ecore_file_strip_ext(file);
    const char *ext = file + strlen(strip_ext);
    FREE(strip_ext);

    if(ext && (strcmp(ext,".jpeg") == 0
                || strcmp(ext, ".jpg") == 0
                || strcmp(ext, ".JPG") == 0
                || strcmp(ext, ".JPEG") == 0
                || strcmp(ext, ".png") ==0
                || strcmp(ext, ".PNG") == 0) )
        return 1;

    return 0;
}


Eet_Data_Descriptor * pm_photo_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Photo);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "name", name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "path", path, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "file_name", file_name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "time", time, EET_T_LONG_LONG);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "thumb_fdo_large", thumb_fdo_large, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "thumb_fdo_normal", thumb_fdo_normal, EET_T_STRING);

    return edd;
}

/**
 * @brief Load a photo from the file "file" or "file_path"
 * @param file An Eet file handler. If Null the file "file_path" will be open and closed
 * @param file_path An Eet file path. If "file" is NULL, the photo will be loaded from the file.
 * @param photo_key The Eet photo key.
 * @return Returns the photo or NULL if failed
 */
PM_Photo * pm_photo_eet_load(const char *eet_path, const char *key)
{
    Eet_Data_Descriptor *edd;
    Eet_File *f ;
    PM_Photo *data;
    char buf[PATH_MAX];

    ASSERT_RETURN(key!=NULL);
    ASSERT_RETURN(eet_path!=NULL);

    if(key[0]!='/')
        snprintf(buf, PATH_MAX, "/photo %s", key);
    else
        strncpy(buf, key, PATH_MAX);

    f = pm_file_manager_open(eet_path);
    ASSERT_RETURN(f!=NULL);
    edd = pm_photo_edd_new();

    data = eet_data_read(f, edd, buf);

    pm_file_manager_close(eet_path);

    eet_data_descriptor_free(edd);

    return data;
}


int pm_photo_eet_remove(const char *eet_path, const char* key)
{
    Eet_File *f;
    char buf[1024];

    ASSERT_RETURN(eet_path!=NULL);
    ASSERT_RETURN(key!=NULL);

    if(key[0]!='/')
      snprintf(buf, PATH_MAX, "/photo %s", key);
    else
      strncpy(buf, key, PATH_MAX);

    f = pm_file_manager_open(eet_path);
    ASSERT_RETURN(f!=NULL);

    eet_delete(f, buf);
    pm_file_manager_close(eet_path);

    return 1;
}


int pm_photo_eet_save(PM_Photo *photo)
{
    Eet_Data_Descriptor *edd;
    Eet_File *f;
    char buf[PATH_MAX], path[PATH_MAX];
    int res;

    ASSERT_RETURN(photo!=NULL);

    snprintf(path, PATH_MAX, "%s/"EET_FILE, pm_photo_path_get(photo));
    f = pm_file_manager_open(path);

    ASSERT_RETURN(f!=NULL);

    edd = pm_photo_edd_new();

    snprintf(buf, PATH_MAX, "/photo %s", pm_photo_file_name_get(photo));
    res=eet_data_write(f, edd, buf, photo, 0);

    pm_file_manager_close(path);
    eet_data_descriptor_free(edd);

    return res;
}


Eet_Data_Descriptor * _pm_photo_file_name_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Photo);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "path", path, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Photo, "file_name", file_name, EET_T_STRING);

    return edd;
}


