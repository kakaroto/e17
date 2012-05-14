#ifndef _LIBCLOUSEAU_H
#define _LIBCLOUSEAU_H
#include <Eina.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Elementary.h>

#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <elm_widget.h>

//#include "config.h"

#ifdef HAVE___ATTRIBUTE__
# define __UNUSED__ __attribute__((unused))
#else
# define __UNUSED__
#endif



#define CLOUSEAU_OBJ_TYPE_OTHER_STR     "CLOUSEAU_OBJ_TYPE_OTHER"
#define CLOUSEAU_OBJ_TYPE_ELM_STR       "CLOUSEAU_OBJ_TYPE_ELM"
#define CLOUSEAU_OBJ_TYPE_TEXT_STR      "CLOUSEAU_OBJ_TYPE_TEXT"
#define CLOUSEAU_OBJ_TYPE_IMAGE_STR     "CLOUSEAU_OBJ_TYPE_IMAGE"
#define CLOUSEAU_OBJ_TYPE_EDJE_STR      "CLOUSEAU_OBJ_TYPE_EDJE"
#define CLOUSEAU_OBJ_TYPE_TEXTBLOCK_STR "CLOUSEAU_OBJ_TYPE_TEXTBLOCK"

enum _en_obj_type
{
   CLOUSEAU_OBJ_TYPE_OTHER,
   CLOUSEAU_OBJ_TYPE_ELM,
   CLOUSEAU_OBJ_TYPE_TEXT,
   CLOUSEAU_OBJ_TYPE_IMAGE,
   CLOUSEAU_OBJ_TYPE_EDJE,
   CLOUSEAU_OBJ_TYPE_TEXTBLOCK,
};
typedef enum _en_obj_type en_obj_type;

struct _eet_extra_props_mapping
{
   en_obj_type u;
   const char *name;
};
typedef struct _eet_extra_props_mapping eet_extra_props_mapping;

static eet_extra_props_mapping eet_props_mapping[] =
{    /* As eet_mapping */
     { CLOUSEAU_OBJ_TYPE_OTHER, CLOUSEAU_OBJ_TYPE_OTHER_STR },
     { CLOUSEAU_OBJ_TYPE_ELM, CLOUSEAU_OBJ_TYPE_ELM_STR },
     { CLOUSEAU_OBJ_TYPE_TEXT, CLOUSEAU_OBJ_TYPE_TEXT_STR },
     { CLOUSEAU_OBJ_TYPE_IMAGE, CLOUSEAU_OBJ_TYPE_IMAGE_STR },
     { CLOUSEAU_OBJ_TYPE_EDJE, CLOUSEAU_OBJ_TYPE_EDJE_STR },
     { CLOUSEAU_OBJ_TYPE_TEXTBLOCK, CLOUSEAU_OBJ_TYPE_TEXTBLOCK_STR }
};

struct _st_evas_props
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
};
typedef struct _st_evas_props st_evas_props;

struct _st_elm
{
   const char *type;
   const char *style;
   double scale;
   Eina_Bool has_focus;
   Eina_Bool is_disabled;
   Eina_Bool is_mirrored;
   Eina_Bool is_mirrored_automatic;
};
typedef struct _st_elm st_elm;

struct _st_text
{
   const char *font;
   int size;
   const char *source;
   const char *text;
};
typedef struct _st_text st_text;

struct _st_image
{
   const char *file, *key;
   void *source;
   const char *load_err;
};
typedef struct _st_image st_image;

struct _st_edje
{
   const char *file, *group;
   const char *load_err;
};
typedef struct _st_edje st_edje;

struct _st_textblock
{
   const char *style;
};
typedef struct _st_textblock st_textblock;

union _un_extra_props
{
   st_elm elm;
   st_text text;
   st_image image;
   st_edje edje;
   st_textblock textblock;
};
typedef union _un_extra_props un_extra_props;

struct _st_extra_props
{  /* as Example_Union */
   en_obj_type type;
   un_extra_props u;
};
typedef struct _st_extra_props st_extra_props;

struct _Obj_Information
{
   st_evas_props evas_props;
   st_extra_props extra_props;
};
typedef struct _Obj_Information Obj_Information;

struct _Tree_Item
{
   Eina_List *children;
   const char *name;
   void *ptr;  /* Just a ptr, we keep the value but not accessing mem */
   Obj_Information *info; /* Obj_Information pointer */
   Eina_Bool is_obj;
   Eina_Bool is_clipper;
   Eina_Bool is_visible;
};
typedef struct _Tree_Item Tree_Item;

Obj_Information * _obj_information_get(Tree_Item *treeit);
Eet_Data_Descriptor *Obj_Information_desc_make(void);
void Obj_Information_desc_shutdown(void);
Evas_Object *clouseau_obj_information_list_add(Evas_Object *parent);
void clouseau_obj_information_list_populate(Tree_Item *treeit);
void clouseau_obj_information_list_clear();
#endif
