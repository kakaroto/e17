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


void
clouseau_obj_information_list_populate(Tree_Item *treeit)
{
   clouseau_obj_information_list_clear();

   if (!treeit->parent)
      return;

   Evas_Object *obj = treeit->ptr;
   Inf_Tree_Item *main_tit;

   /* Populate evas properties list */
   main_tit = calloc(1, sizeof(*main_tit));
   main_tit->string = eina_stringshare_add("Evas");
   information_tree = eina_list_append(information_tree, main_tit);

     {
        Inf_Tree_Item *tit;
        char buf[1024];
        Eina_Bool visibility;
        Evas_Coord x, y, w, h;
        double dx, dy;

        visibility = evas_object_visible_get(obj);
        snprintf(buf, sizeof(buf), "Visibility: %d", (int) visibility);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (evas_object_name_get(obj))
          {
             snprintf(buf, sizeof(buf), "Name: %s",
                   evas_object_name_get(obj));
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        snprintf(buf, sizeof(buf), "Layer: %hd",
              evas_object_layer_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        evas_object_geometry_get(obj, &x, &y, &w, &h);
        snprintf(buf, sizeof(buf), "Position: %d %d", x, y);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
        snprintf(buf, sizeof(buf), "Size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Scale: %.6lg",
              evas_object_scale_get(obj));
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

        evas_object_size_hint_min_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Min size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        evas_object_size_hint_max_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Max size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        evas_object_size_hint_request_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Request size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        evas_object_size_hint_align_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Align: %.6lg %.6lg", dx, dy);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        evas_object_size_hint_weight_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Weight: %.6lg %.6lg", dx, dy);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        evas_object_size_hint_request_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Aspect: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        /* Handle color */
          {
             int r, g, b, a;
             evas_object_color_get(obj, &r, &g, &b, &a);
             snprintf(buf, sizeof(buf), "Color: %d %d %d %d", r, g, b, a);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (evas_object_clipees_get(obj))
          {
             snprintf(buf, sizeof(buf), "Has clipees");
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }

   if (!strcmp("elm_widget", evas_object_type_get(obj)))
     {
        Inf_Tree_Item *tit;
        char buf[1024];

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Elementary");
        information_tree = eina_list_append(information_tree, main_tit);

        snprintf(buf, sizeof(buf), "Wid-Type: %s", elm_widget_type_get(obj));
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

        snprintf(buf, sizeof(buf), "Style: %s", elm_widget_style_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Scale: %.6lg",
              elm_widget_scale_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Disabled: %d",
              elm_widget_disabled_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Mirrored: %d",
              elm_widget_mirrored_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Automatic mirroring: %d",
              elm_widget_mirrored_automatic_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);
     }
   else if (!strcmp("text", evas_object_type_get(obj)))
     {
        Inf_Tree_Item *tit;
        char buf[1024];
        const char *font;
        int size;

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Text");
        information_tree = eina_list_append(information_tree, main_tit);

        evas_object_text_font_get(obj, &font, &size);
        snprintf(buf, sizeof(buf), "Font: %s", font);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Size: %d", size);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        font = evas_object_text_font_source_get(obj);
        if (font)
          {
             snprintf(buf, sizeof(buf), "Source: %s", font);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (!strcmp("image", evas_object_type_get(obj)))
     {
        Inf_Tree_Item *tit;
        char buf[1024];
        const char *file, *key;

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Image");
        information_tree = eina_list_append(information_tree, main_tit);

        evas_object_image_file_get(obj, &file, &key);

        snprintf(buf, sizeof(buf), "File name: %s", file);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (key)
          {
             snprintf(buf, sizeof(buf), "File key: %s", key);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (evas_object_image_source_get(obj))
          {
             snprintf(buf, sizeof(buf), "Source: %p",
                   evas_object_image_source_get(obj));
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }

        if (evas_object_image_load_error_get(obj) != EVAS_LOAD_ERROR_NONE)
          {
             snprintf(buf, sizeof(buf), "Load error: %s",
                   evas_load_error_str(evas_object_image_load_error_get(obj)));
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }
   else if (!strcmp("edje", evas_object_type_get(obj)))
     {
        Inf_Tree_Item *tit;
        char buf[1024];
        const char *file, *group;

        main_tit = calloc(1, sizeof(*main_tit));
        main_tit->string = eina_stringshare_add("Edje");
        information_tree = eina_list_append(information_tree, main_tit);

        edje_object_file_get(obj, &file, &group);

        snprintf(buf, sizeof(buf), "File: %s", file);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        snprintf(buf, sizeof(buf), "Group: %s", group);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        main_tit->children = eina_list_append(main_tit->children, tit);

        if (edje_object_load_error_get(obj) != EDJE_LOAD_ERROR_NONE)
          {
             snprintf(buf, sizeof(buf), "Load error: %s",
                   edje_load_error_str(edje_object_load_error_get(obj)));
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             main_tit->children = eina_list_append(main_tit->children, tit);
          }
     }

   /* Actually populate the genlist */
     {
        Eina_List *itr;
        Inf_Tree_Item *tit;

        EINA_LIST_FOREACH(information_tree, itr, tit)
          {
             Elm_Genlist_Item *git;
             git = elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_SUBITEMS, _gl_selected, NULL);
             /* Start with all the base item expanded */
             elm_genlist_item_expanded_set(git, EINA_TRUE);
          }
     }
}

void
clouseau_obj_information_list_clear()
{
   _item_tree_free();
   elm_genlist_clear(prop_list);
}
