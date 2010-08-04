#include "enlil_private.h"

struct enlil_collection
{
    const char *name;
    Eina_List *albums;

    void *user_data;
    Enlil_Collection_Free_Cb free_cb;
};


Enlil_Collection *enlil_collection_new()
{
    return calloc(1, sizeof(Enlil_Collection));
}

void enlil_collection_free(Enlil_Collection **col)
{
    Enlil_Album *album;

    ASSERT_RETURN_VOID(col != NULL);
    Enlil_Collection *_col = *col;
    ASSERT_RETURN_VOID(_col != NULL);

    if(_col->free_cb)
      _col->free_cb(_col, _col->user_data);

    EINA_STRINGSHARE_DEL(_col->name);
    EINA_LIST_FREE(_col->albums, album)
        ;
}


#define FCT_NAME enlil_collection
#define STRUCT_TYPE Enlil_Collection

STRING_SET(name)

GET(name, const char *)
GET(albums, Eina_List *)
GET(user_data, void *)

#undef FCT_NAME
#undef STRUCT_TYPE


void enlil_collection_user_data_set(Enlil_Collection *col, void *user_data, Enlil_Collection_Free_Cb cb)
{
    ASSERT_RETURN_VOID(col!=NULL);
    col->user_data = user_data;
    col->free_cb = cb;
}


void enlil_collection_album_add(Enlil_Collection *col, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   col->albums = eina_list_append(col->albums, album);
}

void enlil_collection_album_remove(Enlil_Collection *col, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(col != NULL);
   ASSERT_RETURN_VOID(album != NULL);

   col->albums = eina_list_remove(col->albums, album);
}

Eet_Data_Descriptor * _enlil_collection_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Collection);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Collection, "name", name, EET_T_STRING);

    return edd;
}


