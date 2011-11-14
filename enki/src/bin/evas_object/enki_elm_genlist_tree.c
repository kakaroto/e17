#include "enki_elm_genlist_tree.h"
#include "../main.h"

static Eina_Bool is_init = EINA_FALSE;

static Elm_Genlist_Item_Class itc;
static Elm_Genlist_Item_Class itc_header;
Elm_Genlist_Item *header_library = NULL;
Elm_Genlist_Item *header_albums = NULL;
Elm_Genlist_Item *header_collections = NULL;
Elm_Genlist_Item *header_tags = NULL;
Elm_Genlist_Item *item_album_new = NULL;

#define LIBRARY 0
#define ALBUMS 3
#define COLLECTIONS 4
#define TAGS 5

#define INIT() \
         if(!is_init) \
         init(edje)

#define IFTREE(OBJ) \
         edje_object_part_external_object_get(OBJ, "object.list.tree") != NULL

static char *
_gl_header_label_get(void *data, Evas_Object *obj, const char *part)
{
   if ((int) data == LIBRARY)
      return strdup("Library");
   else if ((int) data == ALBUMS)
      return strdup("Albums");
   else if ((int) data == COLLECTIONS)
      return strdup("Collections");
   else if ((int) data == TAGS) return strdup("Tags");
}

static void
_display_all_photos_cb(void *data, Evas_Object *obj, void *event_info)
{
   photos_list_object_show_all(enlil_data->list_photo->o_list);
}

static Evas_Object *
_gl_header_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Evas_Object *icon = elm_icon_add(obj);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   if (strcmp(part, "elm.swallow.icon")) return NULL;

   if ((int) data == LIBRARY)
      elm_icon_file_set(icon, Theme, "icons/menu");
   else if ((int) data == ALBUMS)
      elm_icon_file_set(icon, Theme, "icons/albums");
   else if ((int) data == COLLECTIONS)
      elm_icon_file_set(icon, Theme, "icons/collections");
   else if ((int) data == TAGS) elm_icon_file_set(icon, Theme, "icons/tags");

   return icon;
}

static void
init(Evas_Object *edje)
{
   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");

      itc_header.item_style = "enki_menu_title";
      itc_header.func.label_get = _gl_header_label_get;
      itc_header.func.content_get = _gl_header_icon_get;
      itc_header.func.state_get = NULL;
      itc_header.func.del = NULL;

      itc.item_style = "enki_menu";
      itc.func.label_get = _gl_header_label_get;
      itc.func.content_get = _gl_header_icon_get;
      itc.func.state_get = NULL;
      itc.func.del = NULL;

      header_library = elm_genlist_item_append(tree, &itc_header,
                                               (void *) LIBRARY, NULL,
                                               ELM_GENLIST_ITEM_NONE, NULL,
                                               (void *) LIBRARY);

      header_albums = elm_genlist_item_append(tree, &itc_header,
                                              (void *) ALBUMS, NULL,
                                              ELM_GENLIST_ITEM_NONE,
                                              _display_all_photos_cb, edje);

      header_collections
               = elm_genlist_item_append(tree, &itc_header,
                                         (void *) COLLECTIONS, NULL,
                                         ELM_GENLIST_ITEM_NONE,
                                         _display_all_photos_cb, edje);

      header_tags = elm_genlist_item_append(tree, &itc_header, (void *) TAGS,
                                            NULL, ELM_GENLIST_ITEM_NONE,
                                            _display_all_photos_cb, edje);
   }
   is_init = EINA_TRUE;
}

Elm_Genlist_Item *
enki_elm_genlist_item_menu_append(Evas_Object *edje,
                                  Elm_Genlist_Item_Class *itc,
                                  const void *data, Elm_Genlist_Item *parent,
                                  Evas_Smart_Cb func, const void *func_data)
{
   INIT();

   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");
      itc->item_style = "enki_menu";

      if ((int) data == MENU_ALBUM_NEW)
      {
         parent = header_albums;
         item_album_new = elm_genlist_item_append(tree, itc, data,
                                                  header_albums,
                                                  ELM_GENLIST_ITEM_NONE, func,
                                                  func_data);
         return item_album_new;
      }
      else if ((int) data == MENU_SLIDESHOW || (int) data == MENU_IMPORT_PHOTOS
               || (int) data == MENU_DEL_BG || (int) data == MENU_WEBSYNC)
      {
         return elm_genlist_item_append(tree, itc, data, header_library,
                                        ELM_GENLIST_ITEM_NONE, func, func_data);
      }
      else
         return NULL;
   }
   else
   {
      Evas_Object *list =
               edje_object_part_external_object_get(edje, "object.list.menu");
      return elm_genlist_item_append(list, itc, data, parent,
                                     ELM_GENLIST_ITEM_NONE, func, func_data);
   }
}

Elm_Genlist_Item *
enki_elm_genlist_item_album_append(Evas_Object *edje,
                                   Elm_Genlist_Item_Class *itc,
                                   const void *data, Elm_Genlist_Item *parent,
                                   Evas_Smart_Cb func, const void *func_data)
{
   INIT();

   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");
      itc->item_style = "enki_menu";

      return elm_genlist_item_append(tree, itc, data, header_albums,
                                     ELM_GENLIST_ITEM_NONE, func, func_data);
   }
   else
   {
      Evas_Object *list =
               edje_object_part_external_object_get(edje, "object.list.albums");
      return elm_genlist_item_append(list, itc, data, parent,
                                     ELM_GENLIST_ITEM_NONE, func, func_data);
   }
}

Elm_Genlist_Item *
enki_elm_genlist_item_album_prepend(Evas_Object *edje,
                                    Elm_Genlist_Item_Class *itc,
                                    const void *data, Elm_Genlist_Item *parent,
                                    Evas_Smart_Cb func, const void *func_data)
{
   INIT();

   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");
      itc->item_style = "enki_menu";

      return elm_genlist_item_insert_after(tree, itc, data, header_albums,
                                           item_album_new,
                                           ELM_GENLIST_ITEM_NONE, func,
                                           func_data);
   }
   else
   {
      Evas_Object *list =
               edje_object_part_external_object_get(edje, "object.list.albums");
      return elm_genlist_item_prepend(list, itc, data, parent,
                                      ELM_GENLIST_ITEM_NONE, func, func_data);
   }
}

Elm_Genlist_Item *
enki_elm_genlist_item_album_insert_after(Evas_Object *edje,
                                         Elm_Genlist_Item_Class *itc,
                                         const void *data,
                                         Elm_Genlist_Item *parent,
                                         Elm_Genlist_Item *after,
                                         Evas_Smart_Cb func,
                                         const void *func_data)
{
   INIT();

   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");
      itc->item_style = "enki_menu";

      return elm_genlist_item_insert_after(tree, itc, data, header_albums,
                                           after, ELM_GENLIST_ITEM_NONE, func,
                                           func_data);
   }
   else
   {
      Evas_Object *list =
               edje_object_part_external_object_get(edje,
                                                    "object.list.collections");
      return elm_genlist_item_insert_after(list, itc, data, parent, after,
                                           ELM_GENLIST_ITEM_NONE, func,
                                           func_data);
   }
}

Elm_Genlist_Item *
enki_elm_genlist_item_collection_append(Evas_Object *edje,
                                        Elm_Genlist_Item_Class *itc,
                                        const void *data,
                                        Elm_Genlist_Item *parent,
                                        Evas_Smart_Cb func,
                                        const void *func_data)
{
   INIT();

   Elm_Genlist_Item_Flags flags = ELM_GENLIST_ITEM_NONE;
   if (!parent) flags = ELM_GENLIST_ITEM_SUBITEMS;

   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");
      itc->item_style = "enki_menu";

      if (!parent)
         return elm_genlist_item_append(tree, itc, data, header_collections,
                                        flags, func, func_data);
      else
         return elm_genlist_item_append(tree, itc, data, parent, flags, func,
                                        func_data);
   }
   else
   {
      Evas_Object *list =
               edje_object_part_external_object_get(edje,
                                                    "object.list.collections");

      return elm_genlist_item_append(list, itc, data, parent, flags, func,
                                     func_data);
   }
}

Elm_Genlist_Item *
enki_elm_genlist_item_tag_append(Evas_Object *edje,
                                 Elm_Genlist_Item_Class *itc, const void *data,
                                 Elm_Genlist_Item *parent, Evas_Smart_Cb func,
                                 const void *func_data)
{
   INIT();

   if (IFTREE(edje))
   {
      Evas_Object *tree =
               edje_object_part_external_object_get(edje, "object.list.tree");
      itc->item_style = "enki_menu";

      return elm_genlist_item_append(tree, itc, data, header_tags,
                                     ELM_GENLIST_ITEM_NONE, func, func_data);
   }
   else
   {
      Evas_Object *list =
               edje_object_part_external_object_get(edje, "object.list.tags");
      return elm_genlist_item_append(list, itc, data, parent,
                                     ELM_GENLIST_ITEM_NONE, func, func_data);
   }
}
