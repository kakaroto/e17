#ifndef _OBJ_INFORMATION_H
#define _OBJ_INFORMATION_H

#include "../libclouseau.h"
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

struct _eet_object_type_mapping
{
   en_obj_type u;
   const char *name;
};
typedef struct _eet_object_type_mapping eet_object_type_mapping;


Evas_Object *clouseau_obj_information_list_add(Evas_Object *parent);
void clouseau_obj_information_list_populate(Tree_Item *treeit);
void clouseau_obj_information_list_clear();
#endif
