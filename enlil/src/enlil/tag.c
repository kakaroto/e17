#include "enlil_private.h"

struct enlil_tag
{
   const char       *name;
   const char       *description;
   Eina_List        *photos;

   void             *user_data;
   Enlil_Tag_Free_Cb free_cb;
};

Enlil_Tag *
enlil_tag_new()
{
   return calloc(1, sizeof(Enlil_Tag));
}

void
enlil_tag_free(Enlil_Tag **tag)
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

   free(_tag);
}

Enlil_Tag *
enlil_tag_copy_new(const Enlil_Tag *tag)
{
   ASSERT_RETURN(tag != NULL);

   Enlil_Tag *_tag = enlil_tag_new();

   enlil_tag_copy(tag, _tag);

   return _tag;
}

void
enlil_tag_copy(const Enlil_Tag *tag_src,
               Enlil_Tag       *tag_dest)
{
   ASSERT_RETURN_VOID(tag_src != NULL);
   ASSERT_RETURN_VOID(tag_dest != NULL);

   enlil_tag_name_set(tag_dest, enlil_tag_name_get(tag_src));
   enlil_tag_description_set(tag_dest, enlil_tag_description_get(tag_src));
}

#define FCT_NAME    enlil_tag
#define STRUCT_TYPE Enlil_Tag

STRING_SET(name)
STRING_SET(description)

GET(name, const char *)
GET(description, const char *)
GET(photos, Eina_List *)
GET(user_data, void *)

#undef FCT_NAME
#undef STRUCT_TYPE

int
enlil_tag_photos_count_get(const Enlil_Tag *tag)
{
   ASSERT_RETURN(tag != NULL);

   return eina_list_count(tag->photos);
}

void
enlil_tag_user_data_set(Enlil_Tag        *tag,
                        void             *user_data,
                        Enlil_Tag_Free_Cb cb)
{
   ASSERT_RETURN_VOID(tag != NULL);
   tag->user_data = user_data;
   tag->free_cb = cb;
}

void
enlil_tag_photo_add(Enlil_Tag   *tag,
                    Enlil_Photo *photo)
{
   ASSERT_RETURN_VOID(tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   tag->photos = eina_list_append(tag->photos, photo);
}

void
enlil_tag_photo_remove(Enlil_Tag   *tag,
                       Enlil_Photo *photo)
{
   ASSERT_RETURN_VOID(tag != NULL);
   ASSERT_RETURN_VOID(photo != NULL);

   tag->photos = eina_list_remove(tag->photos, photo);
}

void
enlil_tag_list_print(const Eina_List *tags)
{
   const Eina_List *l;
   const Enlil_Tag *tag;

   ASSERT_RETURN_VOID(tags != NULL);

   EINA_LIST_FOREACH(tags, l, tag)
     {
        printf("\n");
        enlil_tag_print(tag);
     }
}

void
enlil_tag_print(const Enlil_Tag *tag)
{
   ASSERT_RETURN_VOID(tag != NULL);
   printf("\t##  TAG  ##\n");
   printf("\t#########\n");
   printf("Name\t:\t%s\n", tag->name);
   printf("Description\t:\t%s\n", tag->description);
   printf("\t## PHOTO ##\n");
   enlil_photo_list_print(enlil_tag_photos_get(tag));
}

Eet_Data_Descriptor *
_enlil_tag_edd_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Tag);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Tag, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Tag, "description", description, EET_T_STRING);

   return edd;
}

