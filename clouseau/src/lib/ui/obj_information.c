#include "obj_information.h"

typedef struct _Inf_Tree_Item Inf_Tree_Item;
struct _Inf_Tree_Item
{
   Eina_List *children;
   Evas_Object *icon;
   const char *string;
};


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
   Elm_Genlist_Item *it = event_info;
   Inf_Tree_Item *parent = elm_genlist_item_data_get(it);
   Inf_Tree_Item *tit;
   Eina_List *itr;

   EINA_LIST_FOREACH(parent->children, itr, tit)
     {
        Elm_Genlist_Item_Flags iflag = (tit->children) ?
           ELM_GENLIST_ITEM_SUBITEMS : ELM_GENLIST_ITEM_NONE;
        elm_genlist_item_append(prop_list, &itc, tit, it,
              iflag, _gl_selected, NULL);
     }
}

static void
gl_con(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_subitems_clear(it);
}

static void
gl_exp_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_expanded_set(it, EINA_TRUE);
}

static void
gl_con_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_expanded_set(it, EINA_FALSE);
}

static Evas_Object *
item_icon_get(void *data __UNUSED__, Evas_Object *parent __UNUSED__,
      const char *part __UNUSED__)
{
   return NULL;
}

static char *
item_label_get(void *data, Evas_Object *obj __UNUSED__,
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
   itc.func.label_get = item_label_get;
   itc.func.icon_get = item_icon_get;
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

   if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_ELM)
     {
        eina_stringshare_del(oinfo->extra_props.elm.type);
        eina_stringshare_del(oinfo->extra_props.elm.style);
     }
   else if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_TEXT)
     {
        eina_stringshare_del(oinfo->extra_props.text.font);
        eina_stringshare_del(oinfo->extra_props.text.source);
     }
   else if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_IMAGE)
     {
        eina_stringshare_del(oinfo->extra_props.image.file);
        eina_stringshare_del(oinfo->extra_props.image.key);
        eina_stringshare_del(oinfo->extra_props.image.load_err);
     }
   else if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_EDJE)
     {
        eina_stringshare_del(oinfo->extra_props.edje.file);
        eina_stringshare_del(oinfo->extra_props.edje.group);

        eina_stringshare_del(oinfo->extra_props.edje.load_err);
     }

   free(oinfo);
}

static Obj_Information *
_obj_information_get(Tree_Item *treeit)
{
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

   oinfo->evas_props.is_clipper = !!evas_object_clipees_get(obj);

   if (!strcmp("elm_widget", evas_object_type_get(obj)))
     {
        oinfo->obj_type = CLOUSEAU_OBJ_TYPE_ELM;

        oinfo->extra_props.elm.type =
           eina_stringshare_add(elm_widget_type_get(obj));
        oinfo->extra_props.elm.style =
           eina_stringshare_add(elm_widget_style_get(obj));
        oinfo->extra_props.elm.scale = elm_widget_scale_get(obj);
        oinfo->extra_props.elm.has_focus = elm_object_focus_get(obj);
        oinfo->extra_props.elm.is_disabled = elm_widget_disabled_get(obj);
        oinfo->extra_props.elm.is_mirrored = elm_widget_mirrored_get(obj);
        oinfo->extra_props.elm.is_mirrored_automatic =
              elm_widget_mirrored_automatic_get(obj);
     }
   else if (!strcmp("text", evas_object_type_get(obj)))
     {
        const char *font;
        int size;
        oinfo->obj_type = CLOUSEAU_OBJ_TYPE_TEXT;

        evas_object_text_font_get(obj, &font, &size);
        oinfo->extra_props.text.font = eina_stringshare_add(font);
        oinfo->extra_props.text.size = size;
        oinfo->extra_props.text.source =
           eina_stringshare_add(evas_object_text_font_source_get(obj));
     }
   else if (!strcmp("image", evas_object_type_get(obj)))
     {
        const char *file, *key;
        oinfo->obj_type = CLOUSEAU_OBJ_TYPE_IMAGE;

        evas_object_image_file_get(obj, &file, &key);
        oinfo->extra_props.image.file = eina_stringshare_add(file);
        oinfo->extra_props.image.key = eina_stringshare_add(key);
        oinfo->extra_props.image.source = evas_object_image_source_get(obj);

        if (evas_object_image_load_error_get(obj) != EVAS_LOAD_ERROR_NONE)
          {
             oinfo->extra_props.image.load_err = eina_stringshare_add(
                   evas_load_error_str(evas_object_image_load_error_get(obj)));
          }
     }
   else if (!strcmp("edje", evas_object_type_get(obj)))
     {
        const char *file, *group;
        oinfo->obj_type = CLOUSEAU_OBJ_TYPE_EDJE;

        edje_object_file_get(obj, &file, &group);
        oinfo->extra_props.edje.file = eina_stringshare_add(file);
        oinfo->extra_props.edje.group = eina_stringshare_add(group);

        if (edje_object_load_error_get(obj) != EDJE_LOAD_ERROR_NONE)
          {
             oinfo->extra_props.edje.load_err = eina_stringshare_add(
                   edje_load_error_str(edje_object_load_error_get(obj)));
          }
     }
   else
     {
        oinfo->obj_type = CLOUSEAU_OBJ_TYPE_OTHER;
     }
   return oinfo;
}

void
clouseau_obj_information_list_populate(Tree_Item *treeit)
{
   clouseau_obj_information_list_clear();

   if (!treeit->is_obj)
      return;

   Inf_Tree_Item *main_tit;
   Obj_Information *oinfo = _obj_information_get(treeit);

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

   if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_ELM)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Elementary");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "Wid-Type: %s", oinfo->extra_props.elm.type);
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
              oinfo->extra_props.elm.style);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Scale: %.6lg",
              oinfo->extra_props.elm.scale);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Disabled: %d",
              oinfo->extra_props.elm.is_disabled);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Has focus: %d",
              oinfo->extra_props.elm.has_focus);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Mirrored: %d",
              oinfo->extra_props.elm.is_mirrored);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Automatic mirroring: %d",
              oinfo->extra_props.elm.is_mirrored_automatic);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
     }
   else if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_TEXT)
     {
        Inf_Tree_Item *tit;
        char buf[1024];
        const char *font;

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Text");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "Font: %s", oinfo->extra_props.text.font);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Size: %d", oinfo->extra_props.text.size);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        font = oinfo->extra_props.text.source;
        if (font)
          {
             snprintf(buf, sizeof(buf), "Source: %s", font);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_IMAGE)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Image");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "File name: %s",
              oinfo->extra_props.image.file);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (oinfo->extra_props.image.key)
          {
             snprintf(buf, sizeof(buf), "File key: %s",
                   oinfo->extra_props.image.key);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (oinfo->extra_props.image.source)
          {
             snprintf(buf, sizeof(buf), "Source: %p",
                   oinfo->extra_props.image.source);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (oinfo->extra_props.image.load_err)
          {
             snprintf(buf, sizeof(buf), "Load error: %s",
                   oinfo->extra_props.image.load_err);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (oinfo->obj_type == CLOUSEAU_OBJ_TYPE_EDJE)
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Edje");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "File: %s", oinfo->extra_props.edje.file);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Group: %s", oinfo->extra_props.edje.group);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (oinfo->extra_props.edje.load_err)
          {
             snprintf(buf, sizeof(buf), "Load error: %s",
                   oinfo->extra_props.edje.load_err);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }

   /* Actually populate the genlist */
     {
        Eina_List *itr;
        Inf_Tree_Item *tit;
        Eina_Bool first_it = EINA_TRUE;

        EINA_LIST_FOREACH(information_tree, itr, tit)
          {
             Elm_Genlist_Item *git;
             git = elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_SUBITEMS, _gl_selected, NULL);
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
