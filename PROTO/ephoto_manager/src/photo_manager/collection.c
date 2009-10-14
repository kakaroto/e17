// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"

struct pm_collection
{
    const char *name;
    Eina_List *albums;

    void *user_data;
};


PM_Collection *pm_collection_new()
{
    return calloc(1, sizeof(PM_Collection));
}

void pm_collection_free(PM_Collection **col)
{
    PM_Album *album;

    ASSERT_RETURN_VOID(col != NULL);
    PM_Collection *_col = *col;
    ASSERT_RETURN_VOID(_col != NULL);

    EINA_STRINGSHARE_DEL(_col->name);
    EINA_LIST_FREE(_col->albums, album)
        ;
}


#define FCT_NAME pm_collection
#define STRUCT_TYPE PM_Collection

STRING_SET(name)
SET(user_data, void *)

GET(name, const char *)
GET(albums, Eina_List *)
GET(user_data, void *)

#undef FCT_NAME
#undef STRUCT_TYPE


void pm_collection_album_add(PM_Collection *col, PM_Album *album)
{
   ASSERT_RETURN_VOID(col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   col->albums = eina_list_append(col->albums, album);
}

void pm_collection_album_remove(PM_Collection *col, PM_Album *album)
{
   ASSERT_RETURN_VOID(col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   col->albums = eina_list_remove(col->albums, album);
}

Eet_Data_Descriptor * _pm_collection_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, PM_Collection);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, PM_Collection, "name", name, EET_T_STRING);

    return edd;
}


