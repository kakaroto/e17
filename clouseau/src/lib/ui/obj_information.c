#include "../libclouseau.h"
typedef struct _Inf_Tree_Item Inf_Tree_Item;
struct _Inf_Tree_Item
{
   Eina_List *children;
   Evas_Object *icon;
   const char *string;
};

/* START EET stuff */
struct _extra_props_descs
{
   Eet_Data_Descriptor *elm;
   Eet_Data_Descriptor *text;
   Eet_Data_Descriptor *image;
   Eet_Data_Descriptor *edje;
   Eet_Data_Descriptor *textblock;

   Eet_Data_Descriptor *_union_unified_descriptor;
   Eet_Data_Descriptor_Class eddc;
};
typedef struct _extra_props_descs extra_props_descs;

static extra_props_descs *props_descs = NULL; /* to be used later for union */

static const char *
_props_union_type_get(const void *data, Eina_Bool  *unknow)
{  /* _union_type_get */
   const en_obj_type *u = data;
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
_props_union_type_set(const char *type, void *data, Eina_Bool unknow)
{  /* same as _union_type_set */
   en_obj_type *u = data;
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

Eet_Data_Descriptor *
elm_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, st_elm);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "type", type, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "style", style, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "scale", scale, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "has_focus",
         has_focus, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "has_focus",
         has_focus, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "is_disabled",
         is_disabled, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "is_mirrored",
         is_mirrored, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_elm, "is_mirrored_automatic",
         is_mirrored_automatic, EET_T_UCHAR);

   return d;
}

Eet_Data_Descriptor *
text_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, st_text);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_text, "font", font, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_text, "size", size, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_text, "source", source, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_text, "text", text, EET_T_STRING);

   return d;
}

Eet_Data_Descriptor *
image_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, st_image);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_image, "file", file, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_image, "key", key, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_image, "source", source, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_image, "load_err",
         load_err, EET_T_STRING);

   return d;
}

Eet_Data_Descriptor *
edje_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, st_edje);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_edje, "file", file, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_edje, "group", group, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_edje, "load_err",
         load_err, EET_T_STRING);

   return d;
}

Eet_Data_Descriptor *
textblock_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, st_textblock);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, st_textblock, "style",
         style, EET_T_STRING);

   return d;
}

Eet_Data_Descriptor *
Obj_Information_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Obj_Information);
   d = eet_data_descriptor_stream_new(&eddc);

   /* START - evas_props Struct desc */
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.name",
         evas_props.name, EET_T_STRING);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.bt",
         evas_props.bt, EET_T_STRING);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.layer",
         evas_props.layer, EET_T_SHORT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.x",
         evas_props.x, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.y",
         evas_props.y, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.w",
         evas_props.w, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.h",
         evas_props.h, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.scale",
         evas_props.scale, EET_T_DOUBLE);


   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.min_w",
         evas_props.min_w, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.min_h",
         evas_props.min_h, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.max_w",
         evas_props.max_w, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.max_h",
         evas_props.max_h, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.req_w",
         evas_props.req_w, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.req_h",
         evas_props.req_h, EET_T_INT);


   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.align_x", evas_props.align_x, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.align_y", evas_props.align_y, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.weight_x", evas_props.weight_x, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.weight_y", evas_props.weight_y, EET_T_DOUBLE);


   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.r",
         evas_props.r, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.g",
         evas_props.g, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.b",
         evas_props.b, EET_T_INT);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information, "evas_props.a",
         evas_props.a, EET_T_INT);


   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.pass_events", evas_props.pass_events, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.has_focus", evas_props.has_focus, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.is_clipper", evas_props.is_clipper, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.is_visible", evas_props.is_visible, EET_T_UCHAR);

   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Obj_Information,
         "evas_props.mode", evas_props.mode, EET_T_INT);
   /* END - evas_props Struct desc */


   if (!props_descs)  /* Free later in desc shutdown */
     {
        props_descs = calloc(1, sizeof(extra_props_descs));
        props_descs->elm = elm_desc_make();
        props_descs->text = text_desc_make();
        props_descs->image = image_desc_make();
        props_descs->edje = edje_desc_make();
        props_descs->textblock = textblock_desc_make();
     }

   /* for union */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&(props_descs->eddc),
         st_extra_props);

   props_descs->eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   props_descs->eddc.func.type_get = _props_union_type_get;
   props_descs->eddc.func.type_set = _props_union_type_set;
   props_descs->_union_unified_descriptor =
      eet_data_descriptor_file_new(&(props_descs->eddc));

   /* START handle UNION desc */
   EET_DATA_DESCRIPTOR_ADD_MAPPING(props_descs->_union_unified_descriptor,
         "CLOUSEAU_OBJ_TYPE_ELM_STR", props_descs->elm);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(props_descs->_union_unified_descriptor,
         "CLOUSEAU_OBJ_TYPE_TEXT_STR", props_descs->text);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(props_descs->_union_unified_descriptor,
         "CLOUSEAU_OBJ_TYPE_IMAGE_STR", props_descs->image);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(props_descs->_union_unified_descriptor,
         "CLOUSEAU_OBJ_TYPE_EDJE_STR", props_descs->edje);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(props_descs->_union_unified_descriptor,
         "CLOUSEAU_OBJ_TYPE_TEXTBLOCK_STR", props_descs->textblock);

   EET_DATA_DESCRIPTOR_ADD_UNION(d, st_extra_props, "u", u, type,
         props_descs->_union_unified_descriptor);
   /* END   handle UNION desc */

   return d;
}

void
Obj_Information_desc_shutdown(void)
{
   eet_data_descriptor_free(props_descs->elm);
   eet_data_descriptor_free(props_descs->text);
   eet_data_descriptor_free(props_descs->image);
   eet_data_descriptor_free(props_descs->edje);
   eet_data_descriptor_free(props_descs->textblock);
   eet_data_descriptor_free(props_descs->_union_unified_descriptor);

   free(props_descs);
   props_descs = NULL;
}
/* END   EET stuff */





static Eina_List *information_tree = NULL;
static Evas_Object *prop_list = NULL;
static Elm_Genlist_Item_Class itc;

static void
_gl_selected(void *data __UNUSED__, Evas_Object *pobj __UNUSED__,
      void *event_info __UNUSED__)
{
   /* Currently do nothing */
   return;
}

static void
gl_exp(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   Inf_Tree_Item *parent = elm_object_item_data_get(glit);
   Inf_Tree_Item *tit;
   Eina_List *itr;

   EINA_LIST_FOREACH(parent->children, itr, tit)
     {
        Elm_Genlist_Item_Type iflag = (tit->children) ?
           ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE;
        elm_genlist_item_append(prop_list, &itc, tit, glit,
              iflag, _gl_selected, NULL);
     }
}

static void
gl_con(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
gl_exp_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
gl_con_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}

static Evas_Object *
item_icon_get(void *data __UNUSED__, Evas_Object *parent __UNUSED__,
      const char *part __UNUSED__)
{
   return NULL;
}

static char *
item_text_get(void *data, Evas_Object *obj __UNUSED__,
      const char *part __UNUSED__)
{
   Inf_Tree_Item *tit = data;
   return strdup(tit->string);
}

Evas_Object *
clouseau_obj_information_list_add(Evas_Object *parent)
{
   prop_list = elm_genlist_add(parent);
   itc.item_style = "default";
   itc.func.text_get = item_text_get;
   itc.func.content_get = item_icon_get;
   itc.func.state_get = NULL;
   itc.func.del = NULL;

   evas_object_smart_callback_add(prop_list, "expand,request", gl_exp_req,
         prop_list);
   evas_object_smart_callback_add(prop_list, "contract,request", gl_con_req,
         prop_list);
   evas_object_smart_callback_add(prop_list, "expanded", gl_exp, prop_list);
   evas_object_smart_callback_add(prop_list, "contracted", gl_con, prop_list);
   evas_object_smart_callback_add(prop_list, "selected", _gl_selected, NULL);

   return prop_list;
}

static void
_item_tree_item_free(Inf_Tree_Item *parent)
{
   Inf_Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        _item_tree_item_free(treeit);
     }

   eina_stringshare_del(parent->string);
   free(parent);
}

static void
_item_tree_free(void)
{
   Inf_Tree_Item *treeit;

   EINA_LIST_FREE(information_tree, treeit)
     {
        _item_tree_item_free(treeit);
     }
}

static void
_obj_information_free(Obj_Information *oinfo)
{
   eina_stringshare_del(oinfo->evas_props.name);
   eina_stringshare_del(oinfo->evas_props.bt);

   if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_ELM)
     {
        eina_stringshare_del(oinfo->extra_props.u.elm.type);
        eina_stringshare_del(oinfo->extra_props.u.elm.style);
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_TEXT)
     {
        eina_stringshare_del(oinfo->extra_props.u.text.font);
        eina_stringshare_del(oinfo->extra_props.u.text.source);
        eina_stringshare_del(oinfo->extra_props.u.text.text);
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_IMAGE)
     {
        eina_stringshare_del(oinfo->extra_props.u.image.file);
        eina_stringshare_del(oinfo->extra_props.u.image.key);
        eina_stringshare_del(oinfo->extra_props.u.image.load_err);
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_EDJE)
     {
        eina_stringshare_del(oinfo->extra_props.u.edje.file);
        eina_stringshare_del(oinfo->extra_props.u.edje.group);

        eina_stringshare_del(oinfo->extra_props.u.edje.load_err);
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_TEXTBLOCK)
     {
        eina_stringshare_del(oinfo->extra_props.u.textblock.style);
     }

   free(oinfo);
}

Obj_Information *
_obj_information_get(Tree_Item *treeit)
{
   if (!treeit->is_obj)
      return NULL;

   Obj_Information *oinfo;
   Evas_Object *obj = treeit->ptr;
   oinfo = calloc(1, sizeof(*oinfo));

   oinfo->evas_props.pass_events = evas_object_pass_events_get(obj);
   oinfo->evas_props.has_focus = evas_object_focus_get(obj);
   oinfo->evas_props.is_visible = evas_object_visible_get(obj);
   oinfo->evas_props.name = eina_stringshare_add(evas_object_name_get(obj));
   oinfo->evas_props.layer = evas_object_layer_get(obj);
   evas_object_geometry_get(obj, &oinfo->evas_props.x, &oinfo->evas_props.y,
         &oinfo->evas_props.w, &oinfo->evas_props.h);
   oinfo->evas_props.scale = evas_object_scale_get(obj);

   evas_object_size_hint_min_get(obj, &oinfo->evas_props.min_w,
         &oinfo->evas_props.min_h);
   evas_object_size_hint_max_get(obj, &oinfo->evas_props.max_w,
         &oinfo->evas_props.max_h);
   evas_object_size_hint_request_get(obj, &oinfo->evas_props.req_w,
         &oinfo->evas_props.req_h);

   evas_object_size_hint_align_get(obj, &oinfo->evas_props.align_x,
         &oinfo->evas_props.align_y);
   evas_object_size_hint_weight_get(obj, &oinfo->evas_props.weight_x,
         &oinfo->evas_props.weight_y);

   evas_object_color_get(obj, &oinfo->evas_props.r, &oinfo->evas_props.g,
         &oinfo->evas_props.b, &oinfo->evas_props.a);

   oinfo->evas_props.mode = evas_object_pointer_mode_get(obj);

   oinfo->evas_props.is_clipper = !!evas_object_clipees_get(obj);
   oinfo->evas_props.bt = eina_stringshare_ref(evas_object_data_get(obj, ".clouseau.bt"));

   if (elm_widget_is(obj))
     {
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_ELM;

        oinfo->extra_props.u.elm.type =
           eina_stringshare_add(elm_widget_type_get(obj));
        oinfo->extra_props.u.elm.style =
           eina_stringshare_add(elm_widget_style_get(obj));
        oinfo->extra_props.u.elm.scale = elm_widget_scale_get(obj);
        oinfo->extra_props.u.elm.has_focus = elm_object_focus_get(obj);
        oinfo->extra_props.u.elm.is_disabled = elm_widget_disabled_get(obj);
        oinfo->extra_props.u.elm.is_mirrored = elm_widget_mirrored_get(obj);
        oinfo->extra_props.u.elm.is_mirrored_automatic =
              elm_widget_mirrored_automatic_get(obj);
     }
   else if (!strcmp("text", evas_object_type_get(obj)))
     {
        const char *font;
        int size;
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_TEXT;

        evas_object_text_font_get(obj, &font, &size);
        oinfo->extra_props.u.text.font = eina_stringshare_add(font);
        oinfo->extra_props.u.text.size = size;
        oinfo->extra_props.u.text.source =
           eina_stringshare_add(evas_object_text_font_source_get(obj));
	oinfo->extra_props.u.text.text = eina_stringshare_add(evas_object_text_text_get(obj));
     }
   else if (!strcmp("image", evas_object_type_get(obj)))
     {
        const char *file, *key;
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_IMAGE;

        evas_object_image_file_get(obj, &file, &key);
        oinfo->extra_props.u.image.file = eina_stringshare_add(file);
        oinfo->extra_props.u.image.key = eina_stringshare_add(key);
        oinfo->extra_props.u.image.source = evas_object_image_source_get(obj);

        if (evas_object_image_load_error_get(obj) != EVAS_LOAD_ERROR_NONE)
          {
             oinfo->extra_props.u.image.load_err = eina_stringshare_add(
                   evas_load_error_str(evas_object_image_load_error_get(obj)));
          }
     }
   else if (!strcmp("edje", evas_object_type_get(obj)))
     {
        const char *file, *group;
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_EDJE;

        edje_object_file_get(obj, &file, &group);
        oinfo->extra_props.u.edje.file = eina_stringshare_add(file);
        oinfo->extra_props.u.edje.group = eina_stringshare_add(group);

        if (edje_object_load_error_get(obj) != EDJE_LOAD_ERROR_NONE)
          {
             oinfo->extra_props.u.edje.load_err = eina_stringshare_add(
                   edje_load_error_str(edje_object_load_error_get(obj)));
          }
     }
   else if (!strcmp("textblock", evas_object_type_get(obj)))
     {
        const char *style;
        const Evas_Textblock_Style *ts;
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_TEXTBLOCK;

        ts = evas_object_textblock_style_get(obj);
        style = evas_textblock_style_get(ts);
        oinfo->extra_props.u.textblock.style = eina_stringshare_add(style);
     }
   else
     {
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_OTHER;
     }
   return oinfo;
}

static const struct {
   const char *text;
   Evas_Object_Pointer_Mode mode;
} pointer_mode[3] = {
# define POINTER_MODE(Value) { "Pointer mode : "#Value, Value }
  POINTER_MODE(EVAS_OBJECT_POINTER_MODE_AUTOGRAB),
  POINTER_MODE(EVAS_OBJECT_POINTER_MODE_NOGRAB),
  POINTER_MODE(EVAS_OBJECT_POINTER_MODE_NOGRAB_NO_REPEAT_UPDOWN)
# undef POINTER_MODE
};

void
clouseau_obj_information_list_populate(Tree_Item *treeit)
{
   unsigned int i;

   clouseau_obj_information_list_clear();

   if (!treeit->is_obj)
      return;

   Inf_Tree_Item *main_tit;
   Obj_Information *oinfo = treeit->info;//_obj_information_get(treeit);

   /* Populate evas properties list */
   main_tit = calloc(1, sizeof(*main_tit));
   main_tit->string = eina_stringshare_add("Evas");
   information_tree = eina_list_append(information_tree, main_tit);

     {
        Inf_Tree_Item *tit;
        char buf[1024];

        snprintf(buf, sizeof(buf), "Visibility: %d",
              (int) oinfo->evas_props.is_visible);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (oinfo->evas_props.name)
          {
             snprintf(buf, sizeof(buf), "Name: %s", oinfo->evas_props.name);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        snprintf(buf, sizeof(buf), "Layer: %hd", oinfo->evas_props.layer);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Position: %d %d", oinfo->evas_props.x,
              oinfo->evas_props.y);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
        snprintf(buf, sizeof(buf), "Size: %d %d", oinfo->evas_props.w,
              oinfo->evas_props.h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Scale: %.6lg", oinfo->evas_props.scale);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);


#if 0
        if (evas_object_clip_get(obj))
          {
             evas_object_geometry_get(
                   evas_object_clip_get(obj), &x, &y, &w, &h);
             snprintf(buf, sizeof(buf), "Clipper position: %d %d", x, y);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
             snprintf(buf, sizeof(buf), "Clipper size: %d %d", w, h);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
#endif

        snprintf(buf, sizeof(buf), "Min size: %d %d", oinfo->evas_props.min_w,
              oinfo->evas_props.min_h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Max size: %d %d", oinfo->evas_props.max_w,
              oinfo->evas_props.max_h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Request size: %d %d",
              oinfo->evas_props.req_w, oinfo->evas_props.req_h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Align: %.6lg %.6lg",
              oinfo->evas_props.align_x, oinfo->evas_props.align_y);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Weight: %.6lg %.6lg",
              oinfo->evas_props.weight_x, oinfo->evas_props.weight_y);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

#if 0
        evas_object_size_hint_aspect_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Aspect: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
#endif

        snprintf(buf, sizeof(buf), "Color: %d %d %d %d",
              oinfo->evas_props.r, oinfo->evas_props.g, oinfo->evas_props.b,
              oinfo->evas_props.a);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Has focus: %d",
              (int) oinfo->evas_props.has_focus);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

	for (i = 0; i < sizeof (pointer_mode) / sizeof (pointer_mode[0]); ++i)
	  if (pointer_mode[i].mode == oinfo->evas_props.mode)
	    {
               tit = calloc(1, sizeof(*tit));
               tit->string = eina_stringshare_add(pointer_mode[i].text);
               main_tit->children = eina_list_append(main_tit->children, tit);
               break;
	    }

        snprintf(buf, sizeof(buf), "Pass events: %d",
              (int) oinfo->evas_props.pass_events);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (oinfo->evas_props.is_clipper)
          {
             snprintf(buf, sizeof(buf), "Has clipees");
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }

   if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_ELM)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Elementary");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "Wid-Type: %s", oinfo->extra_props.u.elm.type);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

#if 0
        /* Extract actual data from theme? */
        snprintf(buf, sizeof(buf), "Theme: %s", elm_widget_theme_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
#endif

        snprintf(buf, sizeof(buf), "Style: %s",
              oinfo->extra_props.u.elm.style);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Scale: %.6lg",
              oinfo->extra_props.u.elm.scale);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Disabled: %d",
              oinfo->extra_props.u.elm.is_disabled);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Has focus: %d",
              oinfo->extra_props.u.elm.has_focus);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Mirrored: %d",
              oinfo->extra_props.u.elm.is_mirrored);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Automatic mirroring: %d",
              oinfo->extra_props.u.elm.is_mirrored_automatic);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_TEXT)
     {
        Inf_Tree_Item *tit;
        char buf[1024];
        const char *font;
        const char *text;

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Text");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "Font: %s", oinfo->extra_props.u.text.font);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Size: %d", oinfo->extra_props.u.text.size);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        font = oinfo->extra_props.u.text.source;
        if (font)
          {
             snprintf(buf, sizeof(buf), "Source: %s", font);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        text = oinfo->extra_props.u.text.text;
        if (text)
          {
             snprintf(buf, sizeof(buf), "Text: %s", text);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_IMAGE)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Image");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "File name: %s",
              oinfo->extra_props.u.image.file);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (oinfo->extra_props.u.image.key)
          {
             snprintf(buf, sizeof(buf), "File key: %s",
                   oinfo->extra_props.u.image.key);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (oinfo->extra_props.u.image.source)
          {
             snprintf(buf, sizeof(buf), "Source: %p",
                   oinfo->extra_props.u.image.source);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (oinfo->extra_props.u.image.load_err)
          {
             snprintf(buf, sizeof(buf), "Load error: %s",
                   oinfo->extra_props.u.image.load_err);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_EDJE)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Edje");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "File: %s", oinfo->extra_props.u.edje.file);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Group: %s", oinfo->extra_props.u.edje.group);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (oinfo->extra_props.u.edje.load_err)
          {
             snprintf(buf, sizeof(buf), "Load error: %s",
                   oinfo->extra_props.u.edje.load_err);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_TEXTBLOCK)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Textblock");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "Style: %s",
              oinfo->extra_props.u.textblock.style);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
     }

   /* Actually populate the genlist */
     {
        Eina_List *itr;
        Inf_Tree_Item *tit;
        Eina_Bool first_it = EINA_TRUE;

        EINA_LIST_FOREACH(information_tree, itr, tit)
          {
             Elm_Object_Item *git;
             git = elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_TREE, _gl_selected, NULL);
             if (first_it)
               {
                  /* Start with all the base item expanded */
                  elm_genlist_item_expanded_set(git, EINA_TRUE);
                  first_it = EINA_FALSE;
               }
          }
     }

   _obj_information_free(oinfo);
}

void
clouseau_obj_information_list_clear()
{
   _item_tree_free();
   elm_genlist_clear(prop_list);
}
