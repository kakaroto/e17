#include "obj_information.h"

typedef struct _Inf_Tree_Item Inf_Tree_Item;
struct _Inf_Tree_Item
{
   Eina_List *children;
   Evas_Object *icon;
   const char *string;
};

static Evas_Object *prop_list = NULL;
static Elm_Genlist_Item_Class itc;

static void
_gl_selected(void *data __UNUSED__, Evas_Object *pobj __UNUSED__,
      void *event_info)
{
}

static void
gl_exp(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
#if 0
   Elm_Genlist_Item *it = event_info;
   Evas_Object *gl = elm_genlist_item_genlist_get(it);
   Tree_Item *parent = elm_genlist_item_data_get(it);
   Tree_Item *treeit;
   Eina_List *itr;

   EINA_LIST_FOREACH(parent->children, itr, treeit)
     {
        Elm_Genlist_Item_Flags iflag = (treeit->children) ?
           ELM_GENLIST_ITEM_SUBITEMS : ELM_GENLIST_ITEM_NONE;
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
     }
#endif
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
item_icon_get(void *data, Evas_Object *parent, const char *part)
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

void
clouseau_obj_information_list_populate(Tree_Item *treeit)
{
   elm_genlist_clear(prop_list);

   if (!treeit->parent)
      return;

   Evas_Object *obj = treeit->data.obj;

   /* Populate properties list */
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
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        if (evas_object_name_get(obj))
          {
             snprintf(buf, sizeof(buf), "Name: %s",
                   evas_object_name_get(obj));
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
          }

        snprintf(buf, sizeof(buf), "Layer: %hd",
              evas_object_layer_get(obj));
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        evas_object_geometry_get(obj, &x, &y, &w, &h);
        snprintf(buf, sizeof(buf), "Position: %d %d", x, y);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
        snprintf(buf, sizeof(buf), "Size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

#if 0
        if (evas_object_clip_get(obj))
          {
             evas_object_geometry_get(
                   evas_object_clip_get(obj), &x, &y, &w, &h);
             snprintf(buf, sizeof(buf), "Clipper position: %d %d", x, y);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
             snprintf(buf, sizeof(buf), "Clipper size: %d %d", w, h);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
          }
#endif

        evas_object_size_hint_min_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Min size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        evas_object_size_hint_max_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Max size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        evas_object_size_hint_request_get(obj, &w, &h);
        snprintf(buf, sizeof(buf), "Request size: %d %d", w, h);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        evas_object_size_hint_align_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Align: %.6lg %.6lg", dx, dy);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        evas_object_size_hint_weight_get(obj, &dx, &dy);
        snprintf(buf, sizeof(buf), "Weight: %.6lg %.6lg", dx, dy);
        tit = calloc(1, sizeof(*tit));
        tit->string = eina_stringshare_add(buf);
        elm_genlist_item_append(prop_list, &itc, tit, NULL,
              ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);

        /* Handle color */
          {
             int r, g, b, a;
             evas_object_color_get(obj, &r, &g, &b, &a);
             snprintf(buf, sizeof(buf), "Color: %d %d %d %d", r, g, b, a);
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
          }

        if (evas_object_clipees_get(obj))
          {
             snprintf(buf, sizeof(buf), "Has clipees");
             tit = calloc(1, sizeof(*tit));
             tit->string = eina_stringshare_add(buf);
             elm_genlist_item_append(prop_list, &itc, tit, NULL,
                   ELM_GENLIST_ITEM_NONE, _gl_selected, NULL);
          }
     }
}

void
clouseau_obj_information_list_clear()
{
   elm_genlist_clear(prop_list);
}
