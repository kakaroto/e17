#include <Eina.h>
#include <Eet.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libclouseau.h"

static const char EET_ENTRY[] = "obj_tree";

Eet_Data_Descriptor *
eet_dump_tree_item_descriptor_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Tree_Item);
   Eet_Data_Descriptor *td = eet_data_descriptor_stream_new(&eddc);

#define ITEM_TREE_ADD_BASIC(member, eet_type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(td, Tree_Item, #member, member, eet_type)

   ITEM_TREE_ADD_BASIC(name, EET_T_STRING);
   ITEM_TREE_ADD_BASIC(is_obj, EET_T_UCHAR);
   ITEM_TREE_ADD_BASIC(is_clipper, EET_T_UCHAR);
   ITEM_TREE_ADD_BASIC(is_visible, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_LIST(td, Tree_Item, "children", children, td);

#undef ITEM_TREE_ADD_BASIC
   return td;
}

void
eet_dump_tree_item_descriptor_shutdown(Eet_Data_Descriptor *td)
{
   eet_data_descriptor_free(td);
}

Tree_Item *
eet_dump_tree_load(const char *filename)
{

   Eet_Data_Descriptor *td;
   Tree_Item *treeit;
   Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
   if (!ef)
     {
        fprintf(stderr, "ERROR: could not open '%s' for reading.\n", filename);
        return NULL;
     }

   td = eet_dump_tree_item_descriptor_init();
   treeit = eet_data_read(ef, td, EET_ENTRY);
   eet_dump_tree_item_descriptor_shutdown(td);

   eet_close(ef);
   return treeit;
}

Eina_Bool
eet_dump_tree_save(const Tree_Item *treeit, const char *filename)
{
   Eet_File * ef;
   Eina_Bool ret;
   Eet_Data_Descriptor *td;

   ef = eet_open(filename, EET_FILE_MODE_WRITE);
   if (!ef)
     {
        fprintf(stderr, "ERROR: could not open '%s' for writing.\n", filename);
        return EINA_FALSE;
     }

   td = eet_dump_tree_item_descriptor_init();
   ret = eet_data_write (ef, td, EET_ENTRY, treeit, EINA_TRUE);
   eet_dump_tree_item_descriptor_shutdown(td);
   eet_close(ef);

   return ret;
}
