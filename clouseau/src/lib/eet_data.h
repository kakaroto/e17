#ifndef EET_DATA_H
#define EET_DATA_H
#include "libclouseau.h"
/*  Global constants  */
#define APP_ADD_ENTRY   "/clouseau/app"
#define TREE_DATA_ENTRY "/clouseau/app/tree"
#define BMP_LIST_ENTRY  "/clouseau/app/shot_list"
#define BMP_DATA_ENTRY  "/clouseau/app/screenshot"
#define BMP_FIELD "bmp"

#define PORT           (22522)
#define MAX_LINE       (1023)
#define LOCALHOST      "127.0.0.1"
#define LISTEN_IP      "0.0.0.0" /* Avail all, no mask */

/* Define packet types, used by packet encode / decode */
#define VARIANT_PACKET 0
#define BMP_RAW_DATA   1


#define DESC_ADD_BASIC(desc, type, member, eet_type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC             \
   (desc, type, #member, member, eet_type)

#define GUI_CONNECT_STR      "GUI_CONNECT"
#define APP_CONNECT_STR      "APP_CONNECT"
#define APP_ADD_STR          "APP_ADD"
#define DATA_REQ_STR         "DATA_REQ"
#define TREE_DATA_STR        "TREE_DATA"
#define APP_CLOSED_STR       "APP_CLOSED"
#define HIGHLIGHT_STR        "HIGHLIGHT"
#define BMP_REQ_STR          "BMP_REQ"
#define BMP_DATA_STR         "BMP_DATA"

enum _message_type
{  /*  Add any supported types of packets here */
   UNKNOWN = 0,
   GUI_CLIENT_CONNECT, /* client PID, name */
   APP_CLIENT_CONNECT, /* client PID, name */
   APP_ADD,            /* client PTR, name, PID fwd to GUI client */
   DATA_REQ,  /* GUI client PTR (NULL for all),APP client PTR (NULL for all) */
   TREE_DATA, /* GUI client PTR (NULL for all),APP client PTR, Tree Data */
   APP_CLOSED,         /* APP client PTR from DAEMON to GUI */
   HIGHLIGHT,          /* APP client PTR, object PTR */
   BMP_REQ,            /* APP client PTR, object PTR */
   BMP_DATA            /* bmp_info_st header + BMP raw data */
};
typedef enum _message_type message_type;

struct _Variant_Type_st
{
   const char *type;
   Eina_Bool   unknow : 1;
};
typedef struct _Variant_Type_st Variant_Type_st;

struct _Variant_st
{  /* This is used for composing message and encoding/decoding with EET */
   Variant_Type_st t;
   void *data;
};
typedef struct _Variant_st Variant_st;

struct _connect_st
{  /* This will be used for APP, GUI client connect */
   unsigned int pid;
   const char *name;
};
typedef struct _connect_st connect_st;

struct _app_info_st
{  /* This will be used to register new APP in GUI client */
   unsigned int pid;
   char *name;
   char *file;          /* Valid only if was read from file in offline mode */
   unsigned long long ptr; /* (void *) client ptr of app as saved by daemon */
   Eina_List *view;       /* Screen views view->data is (bmp_info_st *) ptr */
   unsigned int refresh_ctr;      /* Counter of how many times down refresh */
};
typedef struct _app_info_st app_info_st;

struct _data_req_st
{  /* This will be used to ask for tree data by DAEMON or GUI */
   unsigned long long gui; /* (void *) client ptr of GUI */
   unsigned long long app; /* (void *) client ptr APP    */
};
typedef struct _data_req_st data_req_st;

struct _tree_data_st
{  /* This will be used to send tree data to/from APP/DAEMON */
   unsigned long long gui; /* (void *) client ptr of GUI */
   unsigned long long app; /* (void *) client ptr APP    */
   Eina_List *tree; /* The actual (Tree_Item *) list */
};
typedef struct _tree_data_st tree_data_st;

struct _app_closed_st
{  /* This will be used to notify GUI of app closed   */
   unsigned long long ptr; /* (void *) client ptr APP */
};
typedef struct _app_closed_st app_closed_st;

struct _highlight_st
{  /* This will be used to highlight object in APP */
   unsigned long long app; /* (void *) client ptr of APP */
   unsigned long long object; /* (void *) object ptr of object to highlight */
};
typedef struct _highlight_st highlight_st;

struct _bmp_req_st
{  /* This will be used to send tree data to/from APP/DAEMON */
   unsigned long long gui; /* (void *) client ptr of GUI */
   unsigned long long app; /* (void *) client ptr APP    */
   unsigned long long object; /* (void *) object ptr of Evas */
   unsigned int ctr;       /* Reload counter to match    */
};
typedef struct _bmp_req_st bmp_req_st;

struct _bmp_info_st
{  /* This will be used to send app window Bitmap             */
   /* We are using ULONGLONG because we send this as RAW data */
   /* win, bt are NOT transferred.                            */
   unsigned long long gui;     /* (void *) client ptr of GUI  */
   unsigned long long app;     /* (void *) client ptr of APP  */
   unsigned long long object;  /* (void *) object ptr of evas */
   unsigned long long ctr;     /* Reload counter to match     */
   unsigned long long w;       /* BMP width, make  Evas_Coord */
   unsigned long long h;       /* BMP hight, make  Evas_Coord */
   Evas_Object *win;           /* Window of view if open      */
   Evas_Object *o;             /* Actuall object displays BMP */
   Evas_Object *lb_mouse;      /* Label contains mouse cords  */
   Evas_Object *lb_argb;       /* Current mouse pos argb val  */
   Evas_Object *bt;            /* Button opening win          */
   Evas_Object *lx;            /* Line on X axis              */
   Evas_Object *ly;            /* Line on Y axis              */
   void *bmp;      /* Bitmap BLOB, size (w * h * sizeof(int)) */
};
typedef struct _bmp_info_st bmp_info_st;

struct _eet_message_type_mapping
{
   message_type t;
   const char *name;
};
typedef struct _eet_message_type_mapping eet_message_type_mapping;

struct _shot_list_st
{  /* This will be used to write a shot list to eet file */
   Eina_List *view;       /* Screen views eahc is (bmp_info_st *) ptr */
};
typedef struct _shot_list_st shot_list_st;

struct _data_desc
{
   Eet_Data_Descriptor *bmp_data;
   Eet_Data_Descriptor *bmp_req;
   Eet_Data_Descriptor *bmp_info;
   Eet_Data_Descriptor *shot_list;
   Eet_Data_Descriptor *connect;
   Eet_Data_Descriptor *app_add;
   Eet_Data_Descriptor *data_req;
   Eet_Data_Descriptor *tree_data;
   Eet_Data_Descriptor *app_closed;
   Eet_Data_Descriptor *highlight;
   Eet_Data_Descriptor *tree;
   Eet_Data_Descriptor *obj_info;
   Eet_Data_Descriptor *_variant_descriptor;
   Eet_Data_Descriptor *_variant_unified_descriptor;
};
typedef struct _data_desc data_desc;

/* Function Declarations */
void lines_free(bmp_info_st *st);
void bmp_blob_free(bmp_info_st *st);

Eet_Data_Descriptor *connect_desc_make(void);
Eet_Data_Descriptor *app_add_desc_make(void);
Eet_Data_Descriptor *data_req_desc_make(void);
Eet_Data_Descriptor *bmp_req_desc_make(void);
Eet_Data_Descriptor *bmp_data_desc_make(void);
Eet_Data_Descriptor *bmp_info_desc_make(void);
Eet_Data_Descriptor *shot_list_desc_make(void);
Eet_Data_Descriptor *tree_data_desc_make(void);
Eet_Data_Descriptor *app_closed_desc_make(void);
Eet_Data_Descriptor *highlight_desc_make(void);
Eet_Data_Descriptor *tree_item_desc_make(void);

Obj_Information *obj_information_get(Tree_Item *treeit);
void obj_information_free(Obj_Information *oinfo);
void item_tree_item_free(Tree_Item *parent);
void item_tree_free(Eina_List *tree);
void _item_tree_item_string(Tree_Item *parent);
data_desc *data_descriptors_init(void);
void data_descriptors_shutdown(void);
void variant_free(Variant_st *v);
Variant_st *variant_alloc(message_type t, size_t size, void *info);
message_type packet_mapping_type_get(const char *name);
const char *packet_mapping_type_str_get(message_type t);
void *packet_compose(message_type t, void *data, int data_size, int *size, void *blob, int blob_size);
Variant_st *packet_info_get(void *data, int size);
Eina_Bool eet_info_save(const char *filename, app_info_st *a, tree_data_st *ftd, Eina_List *ck_list);
Eina_Bool eet_info_read(const char *filename, app_info_st **app, tree_data_st **ftd);

/* Highlight code, we may choose to move this to other file later */
void libclouseau_highlight(Evas_Object *obj, st_evas_props *props, bmp_info_st *view);
void libclouseau_make_lines(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);
#endif  /*  EET_DATA_H  */
