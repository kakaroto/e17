#include "enlil_private.h"

struct enlil_tag
{
    const char *name;
    Eina_List *photos;

    void *user_data;
    Enlil_Tag_Free_Cb free_cb;
};


Enlil_Tag *enlil_tag_new()
{
    return calloc(1, sizeof(Enlil_Tag));
}

void enlil_tag_free(Enlil_Tag **tag)
{
    Enlil_Photo *photo;

    ASSERT_RETURN_VOID(tag != NULL);
    Enlil_Tag *_tag = *tag;
    ASSERT_RETURN_VOID(_tag != NULL);

    if(_tag->free_cb)
      _tag->free_cb(_tag, _tag->user_data);

    EINA_STRINGSHARE_DEL(_tag->name);
    EINA_LIST_FREE(_tag->photos, photo)
        ;
}


#define FCT_NAME enlil_tag
#define STRUCT_TYPE Enlil_Tag

STRING_SET(name)

GET(name, const char *)
GET(photos, Eina_List *)
GET(user_data, void *)

#undef FCT_NAME
#undef STRUCT_TYPE


void enlil_tag_user_data_set(Enlil_Tag *tag, void *user_data, Enlil_Tag_Free_Cb cb)
{
    ASSERT_RETURN_VOID(tag!=NULL);
    tag->user_data = user_data;
    tag->free_cb = cb;
}


void enlil_tag_photo_add(Enlil_Tag *tag, Enlil_Photo *photo)
{
   ASSERT_RETURN_VOID(tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   tag->photos = eina_list_append(tag->photos, photo);
}

void enlil_tag_photo_remove(Enlil_Tag *tag, Enlil_Photo *photo)
{
   ASSERT_RETURN_VOID(tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   tag->photos = eina_list_remove(tag->photos, photo);
}

Eet_Data_Descriptor * _enlil_tag_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Tag);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Tag, "name", name, EET_T_STRING);

    return edd;
}


