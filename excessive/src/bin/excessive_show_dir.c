#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "excessive_private.h"

static char *
_excessive_folder_item_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Excessive_File_Info *info = data;

   if (info->subdir) return strdup("..");
   return strdup(info->info.path + info->info.name_start);
}

static Evas_Object *
_excessive_folder_item_object_get(void *data, Evas_Object *obj, const char *part __UNUSED__)
{
   Excessive_File_Info *info = data;
   Evas_Object *ic;

   ic = elm_icon_add(obj);
   elm_icon_order_lookup_set(ic, ELM_ICON_LOOKUP_FDO_THEME);
   elm_icon_standard_set(ic, info->type->type);

   /* if (info->type->thumb) elm_icon_thumb_set(ic, info->info.path, NULL); */

   return ic;
}

static void
_excessive_folder_item_object_del(void *data, Evas_Object *obj __UNUSED__)
{
   /* FIXME: implement a cache of object */
   free(data);
}

static Eina_Bool
_excessive_is_directory(Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   if (info->type == EINA_FILE_DIR)
     return EINA_TRUE;
   return EINA_FALSE;
}

static const Elm_Gengrid_Item_Class _excessive_folder_class = {
  "excessive-folder",
  {
    _excessive_folder_item_label_get,
    _excessive_folder_item_object_get,
    NULL,
    _excessive_folder_item_object_del,
  }
};

const Excessive_Mapping _excessive_folder_mapping = {
  "folder", &_excessive_folder_class,
  {
    _excessive_is_directory,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  }
};

Eina_Bool
excessive_show_folder_init(void)
{
   return excessive_browse_register(&_excessive_folder_mapping);
}

Eina_Bool
excessive_show_folder_shutdown(void)
{
   return EINA_TRUE;
}
