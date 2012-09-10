#include "Clouseau.h"
typedef struct _Inf_Clouseau_Tree_Item Inf_Clouseau_Tree_Item;
struct _Inf_Clouseau_Tree_Item
{
   Eina_List *children;
   Evas_Object *icon;
   const char *string;
};

static Eina_List *information_tree = NULL;
static Evas_Object *prop_list = NULL;
static Elm_Genlist_Item_Class itc;

static void
_gl_selected(void *data EINA_UNUSED, Evas_Object *pobj EINA_UNUSED,
      void *event_info EINA_UNUSED)
{
   /* Currently do nothing */
   return;
}

static void
gl_exp(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   Inf_Clouseau_Tree_Item *parent = elm_object_item_data_get(glit);
   Inf_Clouseau_Tree_Item *tit;
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
gl_con(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
gl_exp_req(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
gl_con_req(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}

static Evas_Object *
item_icon_get(void *data EINA_UNUSED, Evas_Object *parent EINA_UNUSED,
      const char *part EINA_UNUSED)
{
   return NULL;
}

static char *
item_text_get(void *data, Evas_Object *obj EINA_UNUSED,
      const char *part EINA_UNUSED)
{
   Inf_Clouseau_Tree_Item *tit = data;
   return strdup(tit->string);
}

EAPI Evas_Object *
clouseau_object_information_list_add(Evas_Object *parent)
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
_clouseau_item_tree_item_free(Inf_Clouseau_Tree_Item *parent)
{
   Inf_Clouseau_Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        _clouseau_item_tree_item_free(treeit);
     }

   eina_stringshare_del(parent->string);
   free(parent);
}

static void
_clouseau_item_tree_free(void)
{
   Inf_Clouseau_Tree_Item *treeit;

   EINA_LIST_FREE(information_tree, treeit)
     {
        _clouseau_item_tree_item_free(treeit);
     }
}

EAPI void
clouseau_object_information_free(Clouseau_Object *oinfo)
{
   if (!oinfo)
     return;

   eina_stringshare_del(oinfo->evas_props.name);
   eina_stringshare_del(oinfo->evas_props.bt);

   if (oinfo->evas_props.points)
     free(oinfo->evas_props.points);

   switch (oinfo->extra_props.type)
     {
      case CLOUSEAU_OBJ_TYPE_ELM:
         eina_stringshare_del(oinfo->extra_props.u.elm.type);
         eina_stringshare_del(oinfo->extra_props.u.elm.style);
         break;
      case CLOUSEAU_OBJ_TYPE_TEXT:
         eina_stringshare_del(oinfo->extra_props.u.text.font);
         eina_stringshare_del(oinfo->extra_props.u.text.source);
         eina_stringshare_del(oinfo->extra_props.u.text.text);
         break;
      case CLOUSEAU_OBJ_TYPE_IMAGE:
         eina_stringshare_del(oinfo->extra_props.u.image.file);
         eina_stringshare_del(oinfo->extra_props.u.image.key);
         eina_stringshare_del(oinfo->extra_props.u.image.load_err);
         break;
      case CLOUSEAU_OBJ_TYPE_EDJE:
         eina_stringshare_del(oinfo->extra_props.u.edje.file);
         eina_stringshare_del(oinfo->extra_props.u.edje.group);

         eina_stringshare_del(oinfo->extra_props.u.edje.load_err);
         break;
      case CLOUSEAU_OBJ_TYPE_TEXTBLOCK:
         eina_stringshare_del(oinfo->extra_props.u.textblock.style);
         eina_stringshare_del(oinfo->extra_props.u.textblock.text);
         break;
      case CLOUSEAU_OBJ_TYPE_UNKNOWN:
      case CLOUSEAU_OBJ_TYPE_OTHER:
         break;
     }

   free(oinfo);
}

EAPI Clouseau_Object *
clouseau_object_information_get(Clouseau_Tree_Item *treeit)
{
   Clouseau_Object *oinfo;
   Evas_Object *obj = (void*) (uintptr_t) treeit->ptr;
   const Evas_Map *map = NULL;

   if (!treeit->is_obj)
     return NULL;

   oinfo = calloc(1, sizeof(Clouseau_Object));

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

   map = evas_object_map_get(obj);
   if (map)
     {  /* Save map coords count info if object has map */
        oinfo->evas_props.points_count = evas_map_count_get(map);
        if (oinfo->evas_props.points_count)
          {
             int i;
             Clouseau_Evas_Map_Point_Props *p;
             oinfo->evas_props.points = calloc(oinfo->evas_props.points_count,
                   sizeof(Clouseau_Evas_Map_Point_Props));

             for(i = 0 ; i < oinfo->evas_props.points_count; i++)
               {
                  p = &oinfo->evas_props.points[i];
                  evas_map_point_coord_get(map, i, &p->x, &p->y, &p->z);
               }
          }
     }

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
        const char *text;
        char shorttext[48];
        const Evas_Textblock_Style *ts;
        oinfo->extra_props.type = CLOUSEAU_OBJ_TYPE_TEXTBLOCK;

        ts = evas_object_textblock_style_get(obj);
        style = evas_textblock_style_get(ts);
        text = evas_object_textblock_text_markup_get(obj);
        strncpy(shorttext, text, 38);
        if (shorttext[37])
          strcpy(shorttext + 37, "\xe2\x80\xa6"); /* HORIZONTAL ELLIPSIS */

        oinfo->extra_props.u.textblock.style = eina_stringshare_add(style);
        oinfo->extra_props.u.textblock.text = eina_stringshare_add(shorttext);
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
# define POINTER_MODE(Value) { #Value, Value }
  POINTER_MODE(EVAS_OBJECT_POINTER_MODE_AUTOGRAB),
  POINTER_MODE(EVAS_OBJECT_POINTER_MODE_NOGRAB),
  POINTER_MODE(EVAS_OBJECT_POINTER_MODE_NOGRAB_NO_REPEAT_UPDOWN)
# undef POINTER_MODE
};

static void
_clouseau_information_buffer_to_tree(Inf_Clouseau_Tree_Item *parent, const char *buffer)
{
   Inf_Clouseau_Tree_Item *item;

   item = calloc(1, sizeof (Inf_Clouseau_Tree_Item));
   if (!item) return ;
   item->string = eina_stringshare_add(buffer);

   parent->children = eina_list_append(parent->children, item);
}

static void
_clouseau_information_string_to_tree(Inf_Clouseau_Tree_Item *parent, const char *name, const char *value)
{
   char *buffer;
   int length;

   if (!value) return ;

   length = strlen(name) + 5;
   length += strlen(value);
   buffer = alloca(length);

   snprintf(buffer, length, "%s: '%s'", name, value);
   _clouseau_information_buffer_to_tree(parent, buffer);
}

static void
_clouseau_information_geometry_to_tree(Inf_Clouseau_Tree_Item *parent, const char *name, int xw, int yh)
{
   char *buffer;
   int length;

   length = strlen(name) + 4 + 20;
   buffer = alloca(length);

   snprintf(buffer, length, "%s: %d %d", name, xw, yh);

   _clouseau_information_buffer_to_tree(parent, buffer);
}

static void
_clouseau_information_bool_to_tree(Inf_Clouseau_Tree_Item *parent, const char *name, Eina_Bool value)
{
   _clouseau_information_string_to_tree(parent, name, value ? "EINA_TRUE" : "EINA_FALSE");
}

static void
_clouseau_information_double_to_tree(Inf_Clouseau_Tree_Item *parent, const char *name, double d)
{
   char *buffer;
   int length;

   length = strlen(name) + 3 + 64;
   buffer = alloca(length);

   snprintf(buffer, length, "%s: %.6lg", name, d);

   _clouseau_information_buffer_to_tree(parent, buffer);
}

static void
_clouseau_information_hint_to_tree(Inf_Clouseau_Tree_Item *parent, const char *name, double xw, double yh)
{
   char *buffer;
   int length;

   length = strlen(name) + 3 + 128;
   buffer = alloca(length);

   snprintf(buffer, length, "%s: %.6lg %.61g", name, xw, yh);

   _clouseau_information_buffer_to_tree(parent, buffer);
}

static const struct {
   Clouseau_Object_Type type;
   const char *name;
} _clouseau_types[] = {
  { CLOUSEAU_OBJ_TYPE_ELM, "Elementary" },
  { CLOUSEAU_OBJ_TYPE_TEXT, "Text" },
  { CLOUSEAU_OBJ_TYPE_IMAGE, "Image" },
  { CLOUSEAU_OBJ_TYPE_EDJE, "Edje" },
  { CLOUSEAU_OBJ_TYPE_TEXTBLOCK, "Textblock" }
};

static const char *
_clouseau_type_to_string(Clouseau_Object_Type type)
{
   unsigned int i;

   for (i = 0; i < sizeof (_clouseau_types) / sizeof (_clouseau_types[0]); ++i)
     if (_clouseau_types[i].type == type)
       return eina_stringshare_add(_clouseau_types[i].name);

   return NULL;
}

static Inf_Clouseau_Tree_Item *
_clouseau_type_to_parent(Eina_List **main, const char *name)
{
   Inf_Clouseau_Tree_Item *item;

   if (!name) return NULL;

   item = calloc(1, sizeof(Inf_Clouseau_Tree_Item));
   item->string = eina_stringshare_add(name);

   *main = eina_list_append(*main, item);

   return item;
}

EAPI void
clouseau_object_information_list_populate(Clouseau_Tree_Item *treeit, Evas_Object *lb)
{
   Inf_Clouseau_Tree_Item *main_tit;
   Clouseau_Object *oinfo;
   char buf[1024];
   unsigned int i;

   clouseau_object_information_list_clear();

   if (!treeit->is_obj)
      return;

   oinfo = treeit->info;

   /* Populate evas properties list */
   main_tit = _clouseau_type_to_parent(&information_tree, "Evas");

   _clouseau_information_bool_to_tree(main_tit, "Visibility",
                                      oinfo->evas_props.is_visible);
   _clouseau_information_string_to_tree(main_tit, "Name", oinfo->evas_props.name);

   snprintf(buf, sizeof(buf), "Layer: %hd", oinfo->evas_props.layer);
   _clouseau_information_buffer_to_tree(main_tit, buf);

   _clouseau_information_geometry_to_tree(main_tit, "Position",
                                          oinfo->evas_props.x,
                                          oinfo->evas_props.y);
   _clouseau_information_geometry_to_tree(main_tit, "Size",
                                          oinfo->evas_props.w,
                                          oinfo->evas_props.h);
   _clouseau_information_double_to_tree(main_tit, "Scale", oinfo->evas_props.scale);

#if 0
   if (evas_object_clip_get(obj))
     {
        evas_object_geometry_get(evas_object_clip_get(obj), &x, &y, &w, &h);
        _clouseau_information_geometry_to_tree(main_tit, "Clipper position", x, y);
        _clouseau_information_geometry_to_tree(main_tit, "Clipper size", w, h);
     }
#endif

   _clouseau_information_geometry_to_tree(main_tit, "Min size",
                                          oinfo->evas_props.min_w,
                                          oinfo->evas_props.min_h);
   _clouseau_information_geometry_to_tree(main_tit, "Max size",
                                          oinfo->evas_props.max_w,
                                          oinfo->evas_props.max_h);
   _clouseau_information_geometry_to_tree(main_tit, "Request size",
                                          oinfo->evas_props.req_w,
                                          oinfo->evas_props.req_h);
   _clouseau_information_hint_to_tree(main_tit, "Align",
                                      oinfo->evas_props.align_x,
                                      oinfo->evas_props.align_y);
   _clouseau_information_hint_to_tree(main_tit, "Weight",
                                      oinfo->evas_props.weight_x,
                                      oinfo->evas_props.weight_y);

#if 0
   evas_object_size_hint_aspect_get(obj, &w, &h);
   _clouseau_information_geometry_to_tree(main_tit, "Aspect", w, h);
#endif

   snprintf(buf, sizeof(buf), "Color: %d %d %d %d",
            oinfo->evas_props.r, oinfo->evas_props.g, oinfo->evas_props.b,
            oinfo->evas_props.a);
   _clouseau_information_buffer_to_tree(main_tit, buf);

   _clouseau_information_bool_to_tree(main_tit, "Has focus",
                                      oinfo->evas_props.has_focus);

   for (i = 0; i < sizeof (pointer_mode) / sizeof (pointer_mode[0]); ++i)
     if (pointer_mode[i].mode == oinfo->evas_props.mode)
       {
          _clouseau_information_string_to_tree(main_tit, "Pointer mode",
                                               pointer_mode[i].text);
          break;
       }

   _clouseau_information_bool_to_tree(main_tit, "Pass events",
                                      oinfo->evas_props.pass_events);
   _clouseau_information_bool_to_tree(main_tit, "Has clipees",
                                      oinfo->evas_props.is_clipper);

   if (oinfo->evas_props.points_count)
     {
        main_tit = _clouseau_type_to_parent(&main_tit->children, "Evas Map");
        Clouseau_Evas_Map_Point_Props *p;
        for(i = 0 ; (int) i < oinfo->evas_props.points_count; i++)
          {
             p = &oinfo->evas_props.points[i];

             snprintf(buf, sizeof(buf), "Coords: %d %d %d", p->x, p->y, p->z);
             _clouseau_information_buffer_to_tree(main_tit, buf);
          }
     }

   main_tit = _clouseau_type_to_parent(&information_tree,
                                       _clouseau_type_to_string(oinfo->extra_props.type));
   if (main_tit)
     {
        if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_ELM)
          {
             _clouseau_information_string_to_tree(main_tit, "Wid-Type", oinfo->extra_props.u.elm.type);
#if 0
             /* Extract actual data from theme? */
             _clouseau_information_string_to_tree(main_tit, "Theme", elm_widget_theme_get(obj));
#endif
             _clouseau_information_string_to_tree(main_tit, "Style", oinfo->extra_props.u.elm.style);
             _clouseau_information_double_to_tree(main_tit, "Scale",
                                                  oinfo->extra_props.u.elm.scale);
             _clouseau_information_bool_to_tree(main_tit, "Disabled",
                                                oinfo->extra_props.u.elm.is_disabled);
             _clouseau_information_bool_to_tree(main_tit, "Has focus",
                                                oinfo->extra_props.u.elm.has_focus);
             _clouseau_information_bool_to_tree(main_tit, "Mirrored",
                                                oinfo->extra_props.u.elm.is_mirrored);
             _clouseau_information_bool_to_tree(main_tit, "Automatic mirroring",
                                                oinfo->extra_props.u.elm.is_mirrored_automatic);
          }
        else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_TEXT)
          {
             _clouseau_information_string_to_tree(main_tit, "Font", oinfo->extra_props.u.text.font);

             snprintf(buf, sizeof(buf), "Size: %d", oinfo->extra_props.u.text.size);
             _clouseau_information_buffer_to_tree(main_tit, buf);

             _clouseau_information_string_to_tree(main_tit, "Source", oinfo->extra_props.u.text.source);
             _clouseau_information_string_to_tree(main_tit, "Text", oinfo->extra_props.u.text.text);
          }
        else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_IMAGE)
          {
             _clouseau_information_string_to_tree(main_tit, "Filename", oinfo->extra_props.u.image.file);
             _clouseau_information_string_to_tree(main_tit, "File key", oinfo->extra_props.u.image.key);
             snprintf(buf, sizeof(buf), "Source: %p", oinfo->extra_props.u.image.source);
             _clouseau_information_buffer_to_tree(main_tit, buf);

             _clouseau_information_string_to_tree(main_tit, "Load error", oinfo->extra_props.u.image.load_err);
          }
        else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_EDJE)
          {
             _clouseau_information_string_to_tree(main_tit, "File", oinfo->extra_props.u.edje.file);
             _clouseau_information_string_to_tree(main_tit, "Group", oinfo->extra_props.u.edje.group);
             _clouseau_information_string_to_tree(main_tit, "Load error", oinfo->extra_props.u.edje.load_err);
          }
        else if (oinfo->extra_props.type == CLOUSEAU_OBJ_TYPE_TEXTBLOCK)
          {
             _clouseau_information_string_to_tree(main_tit, "Style", oinfo->extra_props.u.textblock.style);
             _clouseau_information_string_to_tree(main_tit, "Text", oinfo->extra_props.u.textblock.text);
          }
     }

   /* Update backtrace text */
   if (oinfo->evas_props.bt)
     {  /* Build backtrace label */
        char *k = malloc(strlen("Creation backtrace:\n\n") +
              strlen(oinfo->evas_props.bt) + 1);

        sprintf(k, "Creation backtrace:\n\n%s", oinfo->evas_props.bt);
        char *p = elm_entry_utf8_to_markup(k);
        elm_object_text_set(lb, p);
        free(p);
        free(k);
     }
   else
     elm_object_text_set(lb, NULL);


   /* Actually populate the genlist */
   {
      Eina_List *itr;
      Inf_Clouseau_Tree_Item *tit;
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
}

EAPI void
clouseau_object_information_list_clear(void)
{
   _clouseau_item_tree_free();
   elm_genlist_clear(prop_list);
}
