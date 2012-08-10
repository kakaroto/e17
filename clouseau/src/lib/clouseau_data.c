#include "clouseau_private.h"

static int clouseau_init_count = 0;

static Eet_Data_Descriptor *clouseau_object_edd = NULL;
static Eet_Data_Descriptor *clouseau_union_edd = NULL;
static Eet_Data_Descriptor *clouseau_textblock_edd = NULL;
static Eet_Data_Descriptor *clouseau_edje_edd = NULL;
static Eet_Data_Descriptor *clouseau_image_edd = NULL;
static Eet_Data_Descriptor *clouseau_text_edd = NULL;
static Eet_Data_Descriptor *clouseau_elm_edd = NULL;
static Eet_Data_Descriptor *clouseau_connect_edd = NULL;
static Eet_Data_Descriptor *clouseau_app_add_edd = NULL;
static Eet_Data_Descriptor *clouseau_data_req_edd = NULL;
static Eet_Data_Descriptor *clouseau_bmp_info_edd = NULL;
static Eet_Data_Descriptor *clouseau_shot_list_edd = NULL;
static Eet_Data_Descriptor *clouseau_tree_data_edd = NULL;
static Eet_Data_Descriptor *clouseau_tree_edd = NULL;
static Eet_Data_Descriptor *clouseau_app_closed_edd = NULL;
static Eet_Data_Descriptor *clouseau_highlight_edd = NULL;
static Eet_Data_Descriptor *clouseau_bmp_req_edd = NULL;
static Eet_Data_Descriptor *clouseau_variant_edd = NULL;
static Eet_Data_Descriptor *clouseau_protocol_edd = NULL;
static Eet_Data_Descriptor *clouseau_map_point_props_edd = NULL;

void
clouseau_lines_free(bmp_info_st *st)
{  /* Free lines asociated with a bmp */
   if (st->lx)
     evas_object_del(st->lx);

   if (st->ly)
     evas_object_del(st->ly);

   st->lx = st->ly = NULL;
}

void
clouseau_bmp_blob_free(bmp_info_st *st)
{  /* We also free all lines drawn in this bmp canvas */
   clouseau_lines_free(st);

   if (st->bmp)
     free(st->bmp);
}

static void
_clouseau_tree_item_free(Clouseau_Tree_Item *parent)
{
   Clouseau_Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     _clouseau_tree_item_free(treeit);

   clouseau_object_information_free(parent->info);
   eina_stringshare_del(parent->name);
   free(parent);
}

void
clouseau_tree_free(Eina_List *tree)
{
   Clouseau_Tree_Item *treeit;

   EINA_LIST_FREE(tree, treeit)
     _clouseau_tree_item_free(treeit);
}

static const struct {
   Clouseau_Message_Type t;
   const char *name;
} eet_mapping[] = {
   { CLOUSEAU_GUI_CLIENT_CONNECT, "GUI_CONNECT" },
   { CLOUSEAU_APP_CLIENT_CONNECT, "APP_CONNECT" },
   { CLOUSEAU_APP_ADD, "APP_ADD" },
   { CLOUSEAU_DATA_REQ, "DATA_REQ" },
   { CLOUSEAU_TREE_DATA, "TREE_DATA" },
   { CLOUSEAU_APP_CLOSED, "APP_CLOSED" },
   { CLOUSEAU_HIGHLIGHT, "HIGHLIGHT" },
   { CLOUSEAU_BMP_REQ, "BMP_REQ" },
   { CLOUSEAU_BMP_DATA, "BMP_DATA" },
   { CLOUSEAU_UNKNOWN, NULL }
};

Clouseau_Message_Type
clouseau_packet_mapping_type_get(const char *name)
{
   int i;
   if (!name)
     return CLOUSEAU_UNKNOWN;

   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (strcmp(name, eet_mapping[i].name) == 0)
       return eet_mapping[i].t;

   return CLOUSEAU_UNKNOWN;
}

static const char *
_clouseau_packet_mapping_type_str_get(Clouseau_Message_Type t)
{
   int i;
   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (t == eet_mapping[i].t)
       return eet_mapping[i].name;

   return NULL;
}

static const char *
_clouseau_variant_type_get(const void *data, Eina_Bool *unknow EINA_UNUSED)
{
   const char * const *type = data;
   int i;

   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (strcmp(*type, eet_mapping[i].name) == 0)
       return eet_mapping[i].name;

   return NULL;
}

static Eina_Bool
_clouseau_variant_type_set(const char *type,
                           void       *data,
                           Eina_Bool   unknow EINA_UNUSED)
{
   const char **t = data;

   *t = type;
   return EINA_TRUE;
}

void
clouseau_variant_free(Variant_st *v)
{
   if (v->data)
     free(v->data);

   free(v);
}

Variant_st *
clouseau_variant_alloc(Clouseau_Message_Type t, size_t size, void *info)
{
   Variant_st *v;

   if (t == CLOUSEAU_UNKNOWN) return NULL;

   /* This will allocate variant and message struct */
   v =  malloc(sizeof(Variant_st));
   v->data = malloc(size);
   _clouseau_variant_type_set(_clouseau_packet_mapping_type_str_get(t),
                              &v->type, EINA_FALSE);
   memcpy(v->data, info, size);

   return v;
}

static void
_clouseau_connect_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, connect_st);
   clouseau_connect_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_connect_edd, connect_st,
                                 "pid", pid, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_connect_edd, connect_st,
                                 "name", name, EET_T_STRING);
}

static void
_clouseau_app_add_desc_make(void)
{  /* view field not transferred, will be loaded on user request */
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, app_info_st);
   clouseau_app_add_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_app_add_edd, app_info_st,
                                 "pid", pid, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_app_add_edd, app_info_st,
                                 "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_app_add_edd, app_info_st,
                                 "ptr", ptr, EET_T_ULONG_LONG);
}

static void
_clouseau_data_req_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, data_req_st);
   clouseau_data_req_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_data_req_edd, data_req_st,
                                 "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_data_req_edd, data_req_st,
                                 "app", app, EET_T_ULONG_LONG);
}

static void
_clouseau_bmp_info_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, bmp_info_st);
   clouseau_bmp_info_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_info_edd, bmp_info_st,
                                 "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_info_edd, bmp_info_st,
                                 "app", app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_info_edd, bmp_info_st,
                                 "object", object, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_info_edd, bmp_info_st,
                                 "ctr", ctr, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_info_edd, bmp_info_st,
                                 "w", w, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_info_edd, bmp_info_st,
                                 "h", h, EET_T_ULONG_LONG);
}

static void
_clouseau_shot_list_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, shot_list_st);
   clouseau_shot_list_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(clouseau_shot_list_edd, shot_list_st,
                                "view", view, clouseau_bmp_info_edd);
}

static void
_clouseau_tree_item_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Tree_Item);
   clouseau_tree_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(clouseau_tree_edd, Clouseau_Tree_Item,
                                "children", children, clouseau_tree_edd);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_edd, Clouseau_Tree_Item,
                                 "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_edd, Clouseau_Tree_Item,
                                 "ptr", ptr, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_SUB(clouseau_tree_edd, Clouseau_Tree_Item,
                               "info", info, clouseau_object_edd);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_edd, Clouseau_Tree_Item,
                                 "is_obj", is_obj, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_edd, Clouseau_Tree_Item,
                                 "is_clipper", is_clipper, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_edd, Clouseau_Tree_Item,
                                 "is_visible", is_visible, EET_T_UCHAR);
}

static void
_clouseau_tree_data_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, tree_data_st);
   clouseau_tree_data_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_data_edd, tree_data_st,
                                 "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_tree_data_edd, tree_data_st,
                                 "app", app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_LIST(clouseau_tree_data_edd, tree_data_st,
                                "tree", tree, clouseau_tree_edd);
}

static void
_clouseau_app_closed_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, app_closed_st);
   clouseau_app_closed_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_app_closed_edd, app_closed_st,
                                 "ptr", ptr, EET_T_ULONG_LONG);
}

static void
_clouseau_highlight_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, highlight_st);
   clouseau_highlight_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_highlight_edd, highlight_st,
                                 "app", app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_highlight_edd, highlight_st,
                                 "object", object, EET_T_ULONG_LONG);
}

static void
_clouseau_bmp_req_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, bmp_req_st);
   clouseau_bmp_req_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_req_edd, bmp_req_st,
                                 "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_req_edd, bmp_req_st,
                                  "app", app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_req_edd, bmp_req_st,
                                 "object", object, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_bmp_req_edd, bmp_req_st,
                                 "ctr", ctr, EET_T_UINT);
}

static struct {
   Clouseau_Object_Type u;
   const char *name;
} eet_props_mapping[] = {
   /* As eet_mapping */
   { CLOUSEAU_OBJ_TYPE_OTHER, "CLOUSEAU_OBJ_TYPE_OTHER" },
   { CLOUSEAU_OBJ_TYPE_ELM, "CLOUSEAU_OBJ_TYPE_ELM" },
   { CLOUSEAU_OBJ_TYPE_TEXT, "CLOUSEAU_OBJ_TYPE_TEXT" },
   { CLOUSEAU_OBJ_TYPE_IMAGE, "CLOUSEAU_OBJ_TYPE_IMAGE" },
   { CLOUSEAU_OBJ_TYPE_EDJE, "CLOUSEAU_OBJ_TYPE_EDJE" },
   { CLOUSEAU_OBJ_TYPE_TEXTBLOCK, "CLOUSEAU_OBJ_TYPE_TEXTBLOCK" },
   { CLOUSEAU_OBJ_TYPE_UNKNOWN, NULL }
};

static const char *
_clouseau_props_union_type_get(const void *data, Eina_Bool  *unknow)
{  /* _union_type_get */
   const Clouseau_Object_Type *u = data;
   int i;

   if (unknow)
     *unknow = EINA_FALSE;

   for (i = 0; eet_props_mapping[i].name != NULL; ++i)
     if (*u == eet_props_mapping[i].u)
       return eet_props_mapping[i].name;

   if (unknow)
     *unknow = EINA_TRUE;

   return NULL;
}

static Eina_Bool
_clouseau_props_union_type_set(const char *type, void *data, Eina_Bool unknow)
{  /* same as _union_type_set */
   Clouseau_Object_Type *u = data;
   int i;

   if (unknow)
     return EINA_FALSE;

   for (i = 0; eet_props_mapping[i].name != NULL; ++i)
     if (strcmp(eet_props_mapping[i].name, type) == 0)
       {
          *u = eet_props_mapping[i].u;
          return EINA_TRUE;
       }

   return EINA_FALSE;
}

static void
_clouseau_elm_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Elm_Props);
   clouseau_elm_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "type", type, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "style", style, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "scale", scale, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "has_focus", has_focus, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "is_disabled", is_disabled, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "is_mirrored", is_mirrored, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_elm_edd, Clouseau_Elm_Props,
                                 "is_mirrored_automatic", is_mirrored_automatic, EET_T_UCHAR);
}

static void
_clouseau_text_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Evas_Text_Props);
   clouseau_text_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_text_edd, Clouseau_Evas_Text_Props,
                                 "font", font, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_text_edd, Clouseau_Evas_Text_Props,
                                 "size", size, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_text_edd, Clouseau_Evas_Text_Props,
                                 "source", source, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_text_edd, Clouseau_Evas_Text_Props,
                                 "text", text, EET_T_STRING);
}

static void
_clouseau_image_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Evas_Image_Props);
   clouseau_image_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_image_edd, Clouseau_Evas_Image_Props,
                                 "file", file, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_image_edd, Clouseau_Evas_Image_Props,
                                 "key", key, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_image_edd, Clouseau_Evas_Image_Props,
                                 "source", source, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_image_edd, Clouseau_Evas_Image_Props,
                                 "load_err", load_err, EET_T_STRING);
}

static void
_clouseau_edje_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Edje_Props);
   clouseau_edje_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_edje_edd, Clouseau_Edje_Props,
                                 "file", file, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_edje_edd, Clouseau_Edje_Props,
                                 "group", group, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_edje_edd, Clouseau_Edje_Props,
                                 "load_err", load_err, EET_T_STRING);
}

static void
_clouseau_textblock_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Evas_Textblock_Props);
   clouseau_textblock_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_textblock_edd, Clouseau_Evas_Textblock_Props,
                                 "style", style, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_textblock_edd, Clouseau_Evas_Textblock_Props,
                                 "text", text, EET_T_STRING);
}

static void
_clouseau_map_point_props_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc,
         Clouseau_Evas_Map_Point_Props);

   clouseau_map_point_props_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_map_point_props_edd,
         Clouseau_Evas_Map_Point_Props, "x", x, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_map_point_props_edd,
         Clouseau_Evas_Map_Point_Props, "y", y, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_map_point_props_edd,
         Clouseau_Evas_Map_Point_Props, "z", z, EET_T_INT);
}

static void
_clouseau_object_desc_make(void)
{
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Object);
   clouseau_object_edd = eet_data_descriptor_stream_new(&eddc);

   /* START - evas_props Struct desc */
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.name", evas_props.name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.bt", evas_props.bt, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.layer", evas_props.layer, EET_T_SHORT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.x", evas_props.x, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.y", evas_props.y, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.w", evas_props.w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.h", evas_props.h, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.scale", evas_props.scale, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.min_w", evas_props.min_w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.min_h", evas_props.min_h, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.max_w", evas_props.max_w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.max_h", evas_props.max_h, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.req_w", evas_props.req_w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.req_h", evas_props.req_h, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.align_x", evas_props.align_x, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.align_y", evas_props.align_y, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.weight_x", evas_props.weight_x, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.weight_y", evas_props.weight_y, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.r", evas_props.r, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.g", evas_props.g, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.b", evas_props.b, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.a", evas_props.a, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.pass_events", evas_props.pass_events, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.has_focus", evas_props.has_focus, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.is_clipper", evas_props.is_clipper, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.is_visible", evas_props.is_visible, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(clouseau_object_edd, Clouseau_Object,
                                 "evas_props.mode", evas_props.mode, EET_T_INT);

   _clouseau_elm_desc_make();
   _clouseau_text_desc_make();
   _clouseau_image_desc_make();
   _clouseau_edje_desc_make();
   _clouseau_textblock_desc_make();

   _clouseau_map_point_props_desc_make();
   EET_DATA_DESCRIPTOR_ADD_VAR_ARRAY(clouseau_object_edd, Clouseau_Object,
         "evas_props.points", evas_props.points,
         clouseau_map_point_props_edd);

   /* for union */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Clouseau_Extra_Props);
   eddc.func.type_get = _clouseau_props_union_type_get;
   eddc.func.type_set = _clouseau_props_union_type_set;
   clouseau_union_edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_union_edd,
                                   "CLOUSEAU_OBJ_TYPE_ELM", clouseau_elm_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_union_edd,
                                   "CLOUSEAU_OBJ_TYPE_TEXT",
                                   clouseau_text_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_union_edd,
                                   "CLOUSEAU_OBJ_TYPE_IMAGE",
                                   clouseau_image_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_union_edd,
                                   "CLOUSEAU_OBJ_TYPE_EDJE",
                                   clouseau_edje_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_union_edd,
                                   "CLOUSEAU_OBJ_TYPE_TEXTBLOCK",
                                   clouseau_textblock_edd);

   EET_DATA_DESCRIPTOR_ADD_UNION(clouseau_object_edd, Clouseau_Object,
                                 "u", extra_props.u,
                                 extra_props.type, clouseau_union_edd);
}

void
clouseau_data_descriptors_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   _clouseau_bmp_req_desc_make();
   _clouseau_bmp_info_desc_make();
   _clouseau_shot_list_desc_make();
   _clouseau_object_desc_make();
   _clouseau_tree_item_desc_make();
   _clouseau_connect_desc_make();
   _clouseau_app_add_desc_make();
   _clouseau_data_req_desc_make();
   _clouseau_tree_data_desc_make();
   _clouseau_app_closed_desc_make();
   _clouseau_highlight_desc_make();

   /* for variant */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Variant_st);
   clouseau_protocol_edd = eet_data_descriptor_file_new(&eddc);

   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.type_get = _clouseau_variant_type_get;
   eddc.func.type_set = _clouseau_variant_type_set;
   clouseau_variant_edd = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "GUI_CONNECT", clouseau_connect_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "APP_CONNECT", clouseau_connect_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "APP_ADD" , clouseau_app_add_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "DATA_REQ", clouseau_data_req_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "TREE_DATA", clouseau_tree_data_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "APP_CLOSED", clouseau_app_closed_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "HIGHLIGHT", clouseau_highlight_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "BMP_REQ", clouseau_bmp_req_edd);
   EET_DATA_DESCRIPTOR_ADD_MAPPING(clouseau_variant_edd,
                                   "BMP_DATA", clouseau_bmp_info_edd);
   EET_DATA_DESCRIPTOR_ADD_VARIANT(clouseau_protocol_edd, Variant_st,
                                   "data", data, type, clouseau_variant_edd);
}

void
clouseau_data_descriptors_shutdown(void)
{
   eet_data_descriptor_free(clouseau_connect_edd);
   eet_data_descriptor_free(clouseau_app_add_edd);
   eet_data_descriptor_free(clouseau_data_req_edd);
   eet_data_descriptor_free(clouseau_tree_edd);
   eet_data_descriptor_free(clouseau_app_closed_edd);
   eet_data_descriptor_free(clouseau_highlight_edd);
   eet_data_descriptor_free(clouseau_object_edd);
   eet_data_descriptor_free(clouseau_variant_edd);
   eet_data_descriptor_free(clouseau_bmp_req_edd);
   eet_data_descriptor_free(clouseau_bmp_info_edd);
   eet_data_descriptor_free(clouseau_shot_list_edd);
   eet_data_descriptor_free(clouseau_protocol_edd);
   eet_data_descriptor_free(clouseau_elm_edd);
   eet_data_descriptor_free(clouseau_text_edd);
   eet_data_descriptor_free(clouseau_image_edd);
   eet_data_descriptor_free(clouseau_edje_edd);
   eet_data_descriptor_free(clouseau_textblock_edd);
   eet_data_descriptor_free(clouseau_union_edd);
   eet_data_descriptor_free(clouseau_map_point_props_edd);
}

static void *
_host_to_net_blob_get(void *blob, int *blob_size)
{
   if (!blob)
     return blob;

   /* Add extra long if needed */
   if ((*blob_size) % sizeof(uint32_t))
     (*blob_size)++;

   void *n_blob = malloc(*blob_size);
   uint32_t *src = blob;
   uint32_t *dst = n_blob;
   int cnt = (*blob_size) / sizeof(uint32_t);
   while (cnt)
     {
        *dst = htonl(*src);
        src++;
        dst++;
        cnt--;
     }

   return n_blob;
}

static void *
_net_to_host_blob_get(void *blob, int blob_size)
{
   if (!blob)
     return blob;

   void *h_blob = malloc(blob_size);

   uint32_t *src = blob;
   uint32_t *dst = h_blob;
   int cnt = blob_size / sizeof(uint32_t);
   while (cnt)
     {
        *dst = ntohl(*src);
        src++;
        dst++;
        cnt--;
     }

   return h_blob;
}

void *
packet_compose(Clouseau_Message_Type t, void *data,
               int data_size, int *size,
               void *blob, int blob_size)
{
   /* Returns packet BLOB and size in size param, NULL on failure */
   /* Packet is composed of packet type BYTE + packet data.       */
   void *p = NULL;
   void *pb = NULL;
   unsigned char p_type = VARIANT_PACKET;
   Variant_st *v;

   switch (t)
     {
      case CLOUSEAU_BMP_DATA:
        {  /* Builed Bitmap data as follows:
              First we have encoding size of bmp_info_st
              (Done Network Byte Order)
              The next to come will be the encoded bmp_info_st itself
              folloed by the Bitmap raw data.                          */

           /* First, we like to encode bmp_info_st from data  */
           int e_size;
           uint32_t enc_size;
           void *net_blob;
           char *b;

           v = clouseau_variant_alloc(t, data_size, data);
           p = eet_data_descriptor_encode(clouseau_protocol_edd, v, &e_size);
           clouseau_variant_free(v);

           /* Save encoded size in network format */
           enc_size = htonl((uint32_t) e_size);
           net_blob = _host_to_net_blob_get(blob, &blob_size);

           /* Update size to the buffer size we about to return */
           *size = (e_size + blob_size + sizeof(enc_size));
           b = malloc(*size);

           /* Copy encoded-size first, followd by encoded data */
           memcpy(b, &enc_size, sizeof(enc_size));
           memcpy(b + sizeof(enc_size), p, e_size);
           free(p);

           if (net_blob)    /* Copy BMP info */
             {
                memcpy(b + sizeof(enc_size) + e_size, net_blob, blob_size);
                free(net_blob);
             }

           p_type = BMP_RAW_DATA;
           p = b;
        }
        break;

      default:
        {
           /* All others are variant packets with EET encoding  */
           /* Variant is composed of message type + ptr to data */
           v = clouseau_variant_alloc(t, data_size, data);
           p = eet_data_descriptor_encode(clouseau_protocol_edd, v, size);
           clouseau_variant_free(v);
        }
     }

   pb = malloc((*size) + 1);
   *((unsigned char *) pb) = p_type;
   memcpy(((char *) pb) + 1, p, *size);
   *size = (*size) + 1;  /* Add space for packet type */
   free(p);

   return pb;  /* User has to free(pb) */
}

Variant_st *
packet_info_get(void *data, int size)
{
   /* user has to use variant_free() to free return struct */
   char *ch = data;
   Variant_st *v;

   switch (*ch)
     {
      case BMP_RAW_DATA:
        {
           void *bmp = NULL;
           bmp_info_st *st;
           char *b;
           uint32_t enc_size;
           int blob_size;

           b = (char *) (ch + 1);
           enc_size = ntohl(*(uint32_t *) b);
           /* blob_size is total size minus 1st byte and enc_size size */
           blob_size = size - (enc_size + 1 + sizeof(enc_size));
           if (blob_size)
             {
                /* Allocate BMP only if was included in packet */
                bmp = _net_to_host_blob_get(b + sizeof(enc_size) + enc_size,
                                            blob_size);
             }

           /* User have to free both: variant_free(rt), free(t->bmp) */
           v = eet_data_descriptor_decode(clouseau_protocol_edd,
                                          b + sizeof(enc_size), enc_size);

           st = v->data;
           st->bmp = bmp;
           return v;
        }
        break;

      default:
         return eet_data_descriptor_decode(clouseau_protocol_edd,
                                           ch + 1, size - 1);
     }
}

Eina_Bool
eet_info_save(const char *filename,
              app_info_st *a, tree_data_st *ftd, Eina_List *ck_list)
{
   Eina_List *shots = NULL;
   Eina_List *l;
   Eet_File *fp;
   Evas_Object *ck;

   fp = eet_open(filename, EET_FILE_MODE_WRITE);
   if (!fp) return EINA_FALSE;

   eet_data_write(fp, clouseau_app_add_edd, CLOUSEAU_APP_ADD_ENTRY,
                  a, EINA_TRUE);
   eet_data_write(fp, clouseau_tree_data_edd, CLOUSEAU_TREE_DATA_ENTRY,
                  ftd, EINA_TRUE);

   /* Build list of (bmp_info_st *) according to user selection    */
   EINA_LIST_FOREACH(ck_list, l , ck)
     if (elm_check_state_get(ck))
       {
          void *data;

          data = evas_object_data_get(ck, BMP_FIELD);
          if (data)
            shots = eina_list_append(shots, data);
       }

   if (shots)
     {
        /* Write list and bitmaps */
        char buf[1024];
        shot_list_st t;
        bmp_info_st *st;

        t.view = shots;
        eet_data_write(fp, clouseau_shot_list_edd, CLOUSEAU_BMP_LIST_ENTRY,
                       &t, EINA_TRUE);
        EINA_LIST_FREE(shots, st)
          {
             sprintf(buf, CLOUSEAU_BMP_DATA_ENTRY"/%llx", st->object);
             eet_data_image_write(fp, buf, st->bmp,
                                  st->w, st->h, 1, 0, 100, 0);
          }
     }

   eet_close(fp);

   return EINA_TRUE;
}

Eina_Bool
eet_info_read(const char *filename,
              app_info_st **a, tree_data_st **ftd)
{
   bmp_info_st *st;
   shot_list_st *t;
   Eet_File *fp;

   fp = eet_open(filename, EET_FILE_MODE_READ);
   if (!fp) return EINA_FALSE;

   *a = eet_data_read(fp, clouseau_app_add_edd, CLOUSEAU_APP_ADD_ENTRY);
   *ftd = eet_data_read(fp, clouseau_tree_data_edd, CLOUSEAU_TREE_DATA_ENTRY);
   t = eet_data_read(fp, clouseau_shot_list_edd, CLOUSEAU_BMP_LIST_ENTRY);

   EINA_LIST_FREE(t->view, st)
     {
        Variant_st *v;
        char buf[1024];
        int alpha;
        int compress;
        int quality;
        int lossy;

        sprintf(buf, CLOUSEAU_BMP_DATA_ENTRY"/%llx", st->object);
        st->bmp = eet_data_image_read(fp, buf,
                                      (unsigned int *) &st->w,
                                      (unsigned int *) &st->h,
                                      &alpha, &compress, &quality, &lossy);

        /* Add the bitmaps to the actuall app data struct */
        v = clouseau_variant_alloc(CLOUSEAU_BMP_DATA, sizeof(*st), st);
        (*a)->view = eina_list_append((*a)->view, v);
     }

   free(t);
   eet_close(fp);

   return EINA_TRUE;
}

/* HIGHLIGHT code. */
static Eina_Bool
_clouseau_highlight_fade(void *_rect)
{
   Evas_Object *rect = _rect;
   int r, g, b, a;
   double na;

   evas_object_color_get(rect, &r, &g, &b, &a);
   if (a < 20)
     {
        evas_object_del(rect);
        /* The del callback of the object will destroy the animator */
        return EINA_TRUE;
     }

   na = a - 5;
   r = na / a * r;
   g = na / a * g;
   b = na / a * b;
   evas_object_color_set(rect, r, g, b, na);

   return EINA_TRUE;
}

static Evas_Object *
_clouseau_verify_e_children(Evas_Object *obj, Evas_Object *ptr)
{
   /* Verify that obj still exists (can be found on evas canvas) */
   Evas_Object *child;
   Evas_Object *p = NULL;
   Eina_List *children;

   if (ptr == obj)
     return ptr;

   children = evas_object_smart_members_get(obj);
   EINA_LIST_FREE(children, child)
     {
        p = _clouseau_verify_e_children(child, ptr);
        if (p) break;
     }
   eina_list_free(children);

   return p;
}

static Evas_Object *
_clouseau_verify_e_obj(Evas_Object *obj)
{
   /* Verify that obj still exists (can be found on evas canvas) */
   Evas_Object *o;
   Eina_List *ees;
   Ecore_Evas *ee;
   Evas_Object *rt = NULL;

   ees = ecore_evas_ecore_evas_list_get();
   EINA_LIST_FREE(ees, ee)
     {
        Evas *e;
        Eina_List *objs;

        e = ecore_evas_get(ee);
        objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
                                             USHRT_MAX, USHRT_MAX,
                                             EINA_TRUE, EINA_TRUE);

        EINA_LIST_FREE(objs, o)
          {
             rt = _clouseau_verify_e_children(o, obj);
             if (rt) break;
          }
        eina_list_free(objs);

        if (rt) break;
     }
   eina_list_free(ees);

   return rt;
}

static void
_clouseau_highlight_del(void *data,
                        EINA_UNUSED Evas *e,
                        EINA_UNUSED Evas_Object *obj,
                        EINA_UNUSED void *event_info)
{  /* Delete timer for this rect */
   ecore_animator_del(data);
}

void
clouseau_object_highlight(Evas_Object *obj, Clouseau_Evas_Props *props, bmp_info_st *view)
{
   Ecore_Animator *t;
   Evas_Object *r;
   int x, y, wd, ht;
   Evas *e = NULL;

   if (props)
     {
        /* When working offline grab info from struct */
        Evas_Coord x_bmp, y_bmp;

        evas_object_geometry_get(view->o, &x_bmp, &y_bmp, NULL, NULL);
        x =  (view->zoom_val * props->x) + x_bmp;
        y =  (view->zoom_val * props->y) + y_bmp;
        wd = (view->zoom_val * props->w);
        ht = (view->zoom_val * props->h);

        e = evas_object_evas_get(view->win);
     }
   else
     {
        /* Check validity of object when working online */
        if (!_clouseau_verify_e_obj(obj))
          {
             printf("<%s> Evas Object not found <%p> (probably deleted)\n",
                   __func__, obj);
             return;
          }

        evas_object_geometry_get(obj, &x, &y, &wd, &ht);

        /* Take evas from object if working online */
        e = evas_object_evas_get(obj);
        if (!e) return;
     }

   /* Continue and do the Highlight */
   r = evas_object_rectangle_add(e);
   evas_object_move(r, x - PADDING, y - PADDING);
   evas_object_resize(r, wd + (2 * PADDING), ht + (2 * PADDING));
   evas_object_color_set(r,
                         HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B, HIGHLIGHT_A);
   evas_object_show(r);

   /* Add Timer for fade and a callback to delete timer on obj del */
   t = ecore_animator_add(_clouseau_highlight_fade, r);
   evas_object_event_callback_add(r, EVAS_CALLBACK_DEL,
                                  _clouseau_highlight_del, t);
/* Print backtrace info, saved for future ref
   tmp = evas_object_data_get(obj, ".clouseau.bt");
   fprintf(stderr, "Creation backtrace :\n%s*******\n", tmp); */
}

void
libclouseau_make_lines(bmp_info_st *st, Evas_Coord xx, Evas_Coord yy)
{
   /* and no, we are NOT talking about WHITE lines */
   Evas_Coord x_rgn, y_rgn, w_rgn, h_rgn;

   clouseau_lines_free(st);

   elm_scroller_region_get(st->scr, &x_rgn, &y_rgn, &w_rgn, &h_rgn);

   st->lx = evas_object_line_add(evas_object_evas_get(st->o));
   st->ly = evas_object_line_add(evas_object_evas_get(st->o));
   evas_object_repeat_events_set(st->lx, EINA_TRUE);
   evas_object_repeat_events_set(st->ly, EINA_TRUE);

   evas_object_line_xy_set(st->lx, 0, yy, w_rgn, yy);
   evas_object_line_xy_set(st->ly, xx, 0, xx, h_rgn);

   evas_object_color_set(st->lx, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_color_set(st->ly, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_show(st->lx);
   evas_object_show(st->ly);
}

void
libclouseau_lines_cb(void *data,
      Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
      void *event_info)
{
   if (((Evas_Event_Mouse_Down *) event_info)->button == 1)
     return; /* Draw line only if not left mouse button */

   libclouseau_make_lines(data, 
         (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.x),
         (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.y));
}

int
clouseau_init(void)
{
   if (clouseau_init_count++ != 0)
     return clouseau_init_count;

   eina_init();
   eet_init();
   ecore_init();

   clouseau_data_descriptors_init();

   return clouseau_init_count;
}

int
clouseau_shutdown(void)
{
   if (--clouseau_init_count != 0)
     return clouseau_init_count;

   clouseau_data_descriptors_shutdown();

   ecore_shutdown();
   eet_shutdown();
   eina_shutdown();

   return clouseau_init_count;
}
