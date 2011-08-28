#include <Eina.h>
#include <Eet.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libclouseau.h"

static const char EET_ENTRY[] = "obj_tree";

static Eet_Data_Descriptor * _Tree_Item_descriptor;
static Eet_Data_Descriptor * _Obj_Information_descriptor;

void
eet_dump_tree_item_descriptor_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Tree_Item);
   _Tree_Item_descriptor = eet_data_descriptor_stream_new(&eddc);

   //EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Obj_Information);
//   _Obj_Information_descriptor = eet_data_descriptor_stream_new(&eddc);
//
#define ITEM_TREE_ADD_BASIC(member, eet_type)\
   EET_DATA_DESCRIPTOR_ADD_BASIC\
      (_Tree_Item_descriptor,     Tree_Item,    # member, member, eet_type)
#define OBJ_INFORMATION_ADD_BASIC(member, eet_type)\
   EET_DATA_DESCRIPTOR_ADD_BASIC\
      (_Obj_Information_descriptor, Obj_Information, # member, member, eet_type)

   ITEM_TREE_ADD_BASIC(name, EET_T_STRING);
   /* FIXME: So fragile, should handle it properly!!! */
   ITEM_TREE_ADD_BASIC(ptr, EET_T_UINT);
   ITEM_TREE_ADD_BASIC(is_obj, EET_T_UCHAR);
   ITEM_TREE_ADD_BASIC(is_clipper, EET_T_UCHAR);
   ITEM_TREE_ADD_BASIC(is_visible, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_LIST
      (_Tree_Item_descriptor, Tree_Item, "children", children,
       _Tree_Item_descriptor);

#undef ITEM_TREE_ADD_BASIC
#undef OBJ_INFORMATION_ADD_BASIC
}

void
eet_dump_tree_item_descriptor_shutdown(void)
{
   eet_data_descriptor_free(_Obj_Information_descriptor);
   eet_data_descriptor_free(_Tree_Item_descriptor);
}

Tree_Item *
eet_dump_tree_item_load(const char *filename)
{
   Tree_Item *treeit;
   Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
   if (!ef)
     {
        fprintf(stderr, "ERROR: could not open '%s' for reading.\n", filename);
        return NULL;
     }

   treeit = eet_data_read(ef, _Tree_Item_descriptor, EET_ENTRY);
   if (!treeit)
      goto end;

end:
   eet_close(ef);
   return treeit;
}

Eina_Bool
eet_dump_tree_item_save(const Tree_Item *treeit, const char *filename)
{
   Eet_File * ef;
   Eina_Bool ret;

   ef = eet_open(filename, EET_FILE_MODE_WRITE);
   if (!ef)
     {
        fprintf(stderr, "ERROR: could not open '%s' for writing.\n", filename);
        return EINA_FALSE;
     }

   ret = eet_data_write
         (ef, _Tree_Item_descriptor, EET_ENTRY, treeit, EINA_TRUE);
   eet_close(ef);

   return ret;
}

