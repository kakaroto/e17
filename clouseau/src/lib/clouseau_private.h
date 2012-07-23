#ifndef EET_DATA_H
#define EET_DATA_H
#include "Clouseau.h"
/*  Global constants  */
#define BMP_FIELD "bmp"

#define PORT           (22522)
#define MAX_LINE       (1023)
#define LOCALHOST      "127.0.0.1"
#define LISTEN_IP      "0.0.0.0" /* Avail all, no mask */

/* Define packet types, used by packet encode / decode */
#define VARIANT_PACKET 0
#define BMP_RAW_DATA   1

enum _Clouseau_Message_Type
{  /*  Add any supported types of packets here */
   CLOUSEAU_UNKNOWN = 0,
   CLOUSEAU_GUI_CLIENT_CONNECT = 1, /* client PID, name */
   CLOUSEAU_APP_CLIENT_CONNECT = 2, /* client PID, name */
   CLOUSEAU_APP_ADD = 3,   /* client PTR, name, PID fwd to GUI client */
   CLOUSEAU_DATA_REQ = 4,  /* GUI client PTR (NULL for all),APP client PTR (NULL for all) */
   CLOUSEAU_TREE_DATA = 5, /* GUI client PTR (NULL for all),APP client PTR, Tree Data */
   CLOUSEAU_APP_CLOSED = 6,/* APP client PTR from DAEMON to GUI */
   CLOUSEAU_HIGHLIGHT = 7, /* APP client PTR, object PTR */
   CLOUSEAU_BMP_REQ = 8,   /* APP client PTR, object PTR */
   CLOUSEAU_BMP_DATA = 9   /* bmp_info_st header + BMP raw data */
};
typedef enum _Clouseau_Message_Type Clouseau_Message_Type;

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

   /* All the following fields are NOT transferred in EET msg */
   Evas_Object *win;           /* Window of view if open      */
   Evas_Object *scr;           /* Scroller holds view         */
   Evas_Object *o;             /* Actuall object displays BMP */
   double zoom_val;            /* Current zoom value          */
   Evas_Object *lb_mouse;      /* Label contains mouse cords  */
   Evas_Object *lb_rgba;       /* Current mouse pos rgba val  */
   Evas_Object *bt;            /* Button opening win          */
   Evas_Object *lx;            /* Line on X axis              */
   Evas_Object *ly;            /* Line on Y axis              */
   void *bmp;      /* Bitmap BLOB, size (w * h * sizeof(int)) */
};
typedef struct _bmp_info_st bmp_info_st;

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
Clouseau_Object *obj_information_get(Clouseau_Tree_Item *treeit);
void obj_information_free(Clouseau_Object *oinfo);
void item_tree_item_free(Clouseau_Tree_Item *parent);
void _item_tree_item_string(Clouseau_Tree_Item *parent);
data_desc *data_descriptors_init(void);
void data_descriptors_shutdown(void);
void *packet_compose(Clouseau_Message_Type t, void *data, int data_size, int *size, void *blob, int blob_size);
Variant_st *packet_info_get(void *data, int size);
Eina_Bool eet_info_save(const char *filename, app_info_st *a, tree_data_st *ftd, Eina_List *ck_list);
Eina_Bool eet_info_read(const char *filename, app_info_st **app, tree_data_st **ftd);

/* Highlight code, we may choose to move this to other file later */
void libclouseau_make_lines(bmp_info_st *st, Evas_Coord xx, Evas_Coord yy);
void libclouseau_lines_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info);

/* Private function */
#define CLOUSEAU_APP_ADD_ENTRY   "clouseau/app"
#define CLOUSEAU_TREE_DATA_ENTRY "clouseau/app/tree"
#define CLOUSEAU_BMP_LIST_ENTRY  "clouseau/app/shot_list"
#define CLOUSEAU_BMP_DATA_ENTRY  "clouseau/app/screenshot"

void clouseau_data_descriptors_init(void);
void clouseau_data_descriptors_shutdown(void);

/* Exported function */
int clouseau_init(void);
int clouseau_shutdown(void);

Variant_st *clouseau_variant_alloc(Clouseau_Message_Type t, size_t size, void *info);
void clouseau_variant_free(Variant_st *v);
void clouseau_tree_free(Eina_List *tree);
void clouseau_bmp_blob_free(bmp_info_st *st);
void clouseau_lines_free(bmp_info_st *st);

void clouseau_object_highlight(Evas_Object *obj, Clouseau_Evas_Props *props, bmp_info_st *view);

Clouseau_Message_Type clouseau_packet_mapping_type_get(const char *name);

#endif  /*  EET_DATA_H  */
