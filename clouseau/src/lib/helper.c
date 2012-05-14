#include "helper.h"
static data_desc *desc = NULL;

void
_item_tree_item_free(Tree_Item *parent)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        _item_tree_item_free(treeit);
     }

   eina_stringshare_del(parent->name);
   free(parent);
}

void
_item_tree_item_string(Tree_Item *parent)
{
   Tree_Item *treeit;
   Eina_List *l;

   EINA_LIST_FOREACH(parent->children, l, treeit)
     {
        _item_tree_item_string(treeit);
     }

   printf("<%s>\n", parent->name);
}

static eet_message_type_mapping eet_mapping[] = {
       { DAEMON_ACK, DAEMON_ACK_STR },
       { DAEMON_TREE_DATA, DAEMON_TREE_DATA_STR },
       { GUI_ACK, GUI_ACK_STR },
       { GUI_TREE_DATA, GUI_TREE_DATA_STR },
       { APP_ACK, APP_ACK_STR },
       { APP_TREE_DATA, APP_TREE_DATA_STR }
};

message_type
packet_mapping_type_get(const char *name)
{
   int i;
   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (strcmp(name, eet_mapping[i].name) == 0)
       return eet_mapping[i].t;

   return UNKNOWN;
}

const char *
packet_mapping_type_str_get(message_type t)
{
   int i;
   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (t == eet_mapping[i].t)
       return eet_mapping[i].name;

   return NULL;
}

const char *
_variant_type_get(const void *data, Eina_Bool  *unknow)
{
   const Variant_Type_st *type = data;
   int i;

   if (unknow)
     *unknow = type->unknow;

   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (strcmp(type->type, eet_mapping[i].name) == 0)
       return eet_mapping[i].name;

   if (unknow)
     *unknow = EINA_FALSE;

   return type->type;
} /* _variant_type_get */

Eina_Bool
_variant_type_set(const char *type,
      void       *data,
      Eina_Bool   unknow)
{
   Variant_Type_st *vt = data;

   vt->type = type;
   vt->unknow = unknow;
   return EINA_TRUE;
} /* _variant_type_set */

void
variant_free(Variant_st *v)
{
   if (v->data)
     free(v->data);

   free(v);
}

Variant_st *
variant_alloc(message_type t, size_t size, void *info)
{
   if (t != UNKNOWN)
     { /* This will allocate variant and message struct */
        Variant_st *v =  malloc(sizeof(Variant_st));
        v->data = malloc(size);
        _variant_type_set(packet_mapping_type_str_get(t), &v->t, EINA_FALSE);
        memcpy(v->data, info, size);

        return v;
     }

   return NULL;
}

Eet_Data_Descriptor *
ack_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, ack_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, ack_st, "text", text, EET_T_STRING);

   return d;
}

Eet_Data_Descriptor *
tree_item_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Tree_Item);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(d, Tree_Item, "children",
         children, d);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "name",
         name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "ptr",
         ptr, EET_T_NULL);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "is_obj",
         is_obj, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "is_clipper",
         is_clipper, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "is_visible",
         is_visible, EET_T_UCHAR);

   return d;
}

data_desc *_data_descriptors_init(void)
{
   if (desc)  /* Was allocated */
     return desc;

   desc = calloc(1, sizeof(data_desc));

   Eet_Data_Descriptor_Class eddc;
   desc->ack = ack_desc_make();
   desc->tree = tree_item_desc_make();

   /* for variant */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Variant_st);
   desc->_variant_descriptor = eet_data_descriptor_file_new(&eddc);

   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.type_get = _variant_type_get;
   eddc.func.type_set = _variant_type_set;
   desc->_variant_unified_descriptor = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        DAEMON_ACK_STR, desc->ack);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        DAEMON_TREE_DATA_STR , desc->tree);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        GUI_ACK_STR, desc->ack);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        GUI_TREE_DATA_STR, desc->tree);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        APP_ACK_STR, desc->ack);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        APP_TREE_DATA_STR, desc->tree);

   EET_DATA_DESCRIPTOR_ADD_VARIANT(desc->_variant_descriptor,
         Variant_st, "data", data, t, desc->_variant_unified_descriptor);

   return desc;
}

void _data_descriptors_shutdown(void)
{
   if (desc)
     {
        eet_data_descriptor_free(desc->ack);
        eet_data_descriptor_free(desc->tree);
        eet_data_descriptor_free(desc->_variant_descriptor );
        eet_data_descriptor_free(desc->_variant_unified_descriptor);

        free(desc);
        desc = NULL;
     }
}

void *
packet_compose(message_type t, void *data, int data_size, int *size)
{  /* Returns packet BLOB and size in size param, NULL on failure */
   data_desc *d = _data_descriptors_init();
   Variant_st *v = variant_alloc(t, data_size, data);
   void *p = eet_data_descriptor_encode(d->_variant_descriptor , v, size);
   variant_free(v);

   return p;  /* User has to free(p) */
}

Variant_st *
packet_info_get(void *data, int size)
{  /* user has to use free(return value), not variant_free() */
   data_desc *d = _data_descriptors_init();
   return eet_data_descriptor_decode(d->_variant_descriptor, data, size);
}
