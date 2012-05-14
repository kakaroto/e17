#ifndef HELPER_H
#define HELPER_H
#include "libclouseau.h"
/*  Global constants  */
#define PORT           (8080)
#define MAX_LINE       (1023)
#define LOCALHOST      "127.0.0.1"

#define DESC_ADD_BASIC(desc, type, member, eet_type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC             \
   (desc, type, #member, member, eet_type)

#define DAEMON_ACK_STR         "daemon_ack"
#define DAEMON_TREE_DATA_STR   "daemon_tree_data"
#define GUI_ACK_STR            "gui_ack"
#define GUI_TREE_DATA_STR      "gui_tree_data"
#define APP_ACK_STR            "app_ack"
#define APP_TREE_DATA_STR      "app_tree_data"
#define TREE_OBJ_INFO_STR      "tree_data_obj_info"

enum _message_type
{  /*  Add any supported types of packets here */
   UNKNOWN = 0,
   DAEMON_ACK,        /* Daemon sends ack            */
   DAEMON_TREE_DATA,  /* Daemon sends tree-data      */
   GUI_ACK,           /* GUI Client sends ack        */
   GUI_TREE_DATA,     /* GUI Client sends tree-data  */
   APP_ACK,           /* App sends ack               */
   APP_TREE_DATA,     /* App tree-data               */
   TREE_OBJ_INFO      /* Just abusing this to construct tree-data-desc */
};
typedef enum _message_type message_type;

struct _eet_message_type_mapping
{
   message_type t;
   const char *name;
};
typedef struct _eet_message_type_mapping eet_message_type_mapping;

struct _Variant_Type_st
{
   const char *type;
   Eina_Bool   unknow : 1;
};
typedef struct _Variant_Type_st Variant_Type_st;

struct _Variant_st
{
   Variant_Type_st t;
   void *data;
};
typedef struct _Variant_st Variant_st;

struct _ack_st
{
   char *text;  /* send text for debug */
};
typedef struct _ack_st ack_st;

struct _data_desc
{
   Eet_Data_Descriptor *ack;
   Eet_Data_Descriptor *tree;
   Eet_Data_Descriptor *_variant_descriptor;
   Eet_Data_Descriptor *_variant_unified_descriptor;
};
typedef struct _data_desc data_desc;

/* Function Declarations */
void _item_tree_item_free(Tree_Item *parent);
void _item_tree_item_string(Tree_Item *parent);
Eet_Data_Descriptor *ack_desc_make(void);
Eet_Data_Descriptor *tree_item_desc_make(void);
data_desc *_data_descriptors_init(void);
void _data_descriptors_shutdown(void);
void variant_free(Variant_st *v);
Variant_st *variant_alloc(message_type t, size_t size, void *info);
message_type packet_mapping_type_get(const char *name);
const char *packet_mapping_type_str_get(message_type t);
void *packet_compose(message_type t, void *data, int data_size, int *size);
Variant_st *packet_info_get(void *data, int size);
#endif  /*  HELPER_H  */
