#ifndef _LIBCLOUSEAU_H
#define _LIBCLOUSEAU_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Elementary.h>

#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <elm_widget.h>

typedef struct _Clouseau_Evas_Props Clouseau_Evas_Props;
typedef struct _Clouseau_Evas_Text_Props Clouseau_Evas_Text_Props;
typedef struct _Clouseau_Evas_Image_Props Clouseau_Evas_Image_Props;
typedef struct _Clouseau_Elm_Props Clouseau_Elm_Props;
typedef struct _Clouseau_Edje_Props Clouseau_Edje_Props;
typedef struct _Clouseau_Evas_Map_Point_Props Clouseau_Evas_Map_Point_Props;

typedef struct _Clouseau_Extra_Props Clouseau_Extra_Props;
typedef struct _Clouseau_Object Clouseau_Object;
typedef struct _Clouseau_Bitmap Clouseau_Bitmap;

typedef struct _Clouseau_Tree_Item Clouseau_Tree_Item;

/* The color of the highlight */
enum {
   HIGHLIGHT_R = 255,
   HIGHLIGHT_G = 128,
   HIGHLIGHT_B = 128,
   HIGHLIGHT_A = 255,

   /* How much padding around the highlight box.
    * Currently we don't want any. */
   PADDING = 0,
};

typedef enum
{
   CLOUSEAU_OBJ_TYPE_UNKNOWN,
   CLOUSEAU_OBJ_TYPE_OTHER,
   CLOUSEAU_OBJ_TYPE_ELM,
   CLOUSEAU_OBJ_TYPE_TEXT,
   CLOUSEAU_OBJ_TYPE_IMAGE,
   CLOUSEAU_OBJ_TYPE_EDJE,
   CLOUSEAU_OBJ_TYPE_TEXTBLOCK
} Clouseau_Object_Type;

struct _Clouseau_Evas_Map_Point_Props
{
   Evas_Coord x, y, z;
};

struct _Clouseau_Evas_Props
{
   const char *name;
   const char *bt;
   short layer;
   Evas_Coord x, y, w, h;
   double scale;
   Evas_Coord min_w, min_h, max_w, max_h, req_w, req_h;
   double align_x, align_y;
   double weight_x, weight_y;
   int r, g, b, a;
   Eina_Bool pass_events;
   Eina_Bool has_focus;
   Eina_Bool is_clipper;
   Eina_Bool is_visible;
   Evas_Object_Pointer_Mode mode;
   Clouseau_Evas_Map_Point_Props *points;
   int points_count;
};

struct _Clouseau_Evas_Text_Props
{
   const char *font;
   int size;
   const char *source;
   const char *text;
};

struct _Clouseau_Evas_Image_Props
{
   const char *file, *key;
   void *source;
   const char *load_err;
};

struct _Clouseau_Evas_Textblock_Props
{
   const char *style;
   const char *text;
};
typedef struct _Clouseau_Evas_Textblock_Props Clouseau_Evas_Textblock_Props;

struct _Clouseau_Edje_Props
{
   const char *file, *group;
   const char *load_err;
};

struct _Clouseau_Elm_Props
{
   const char *type;
   const char *style;
   double scale;
   Eina_Bool has_focus;
   Eina_Bool is_disabled;
   Eina_Bool is_mirrored;
   Eina_Bool is_mirrored_automatic;
};

struct _Clouseau_Extra_Props
{  /* as Example_Union */
   Clouseau_Object_Type type;
   union {
      Clouseau_Elm_Props elm;
      Clouseau_Evas_Text_Props text;
      Clouseau_Evas_Image_Props image;
      Clouseau_Edje_Props edje;
      Clouseau_Evas_Textblock_Props textblock;
   } u;
};

struct _Clouseau_Object
{
   Clouseau_Evas_Props evas_props;
   Clouseau_Extra_Props extra_props;
};

struct _Clouseau_Bitmap
{
   unsigned char  *bmp;
   int bmp_count; /* is (w * h), for EET_DATA_DESCRIPTOR_ADD_BASIC_VAR_ARRAY */
   Evas_Coord w;
   Evas_Coord h;
};

struct _Clouseau_Tree_Item
{
   Eina_List *children;
   const char *name;
   unsigned long long ptr;      /* Just a ptr, we keep the value but not accessing mem */
   Clouseau_Object *info;
   Eina_Bool is_obj;
   Eina_Bool is_clipper;
   Eina_Bool is_visible;
};

Eet_Data_Descriptor *clouseau_object_desc_make(void);
void clouseau_object_desc_shutdown(void);
void clouseau_object_information_list_clear(void);

/* Public */
EAPI Evas_Object *clouseau_object_information_list_add(Evas_Object *parent);
EAPI void clouseau_object_information_list_populate(Clouseau_Tree_Item *treeit, Evas_Object *lb);
#endif
