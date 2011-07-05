#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>

#include <Emotion.h>

#include "emage_private.h"

static Eina_Array *_emage_mappings = NULL;
static Evas_Object **_emage_objects = NULL;

static int
_emage_file_info_cmp(const void *a, const void *b)
{
   const Emage_File_Info *fa = a;
   const Emage_File_Info *fb = b;

   if (fa->subdir) return -1;
   if (fb->subdir) return 1;

   if (fa->info.type == EINA_FILE_DIR)
     {
        if (fb->info.type != EINA_FILE_DIR)
          return 1;
     }

   return strcasecmp(fa->info.path, fb->info.path);
}

static void
_emage_file_add(Evas_Object *grid, const Eina_File_Direct_Info *info, const Emage_Mapping *type, Evas_Object *display)
{
   Emage_File_Info *ei;

   ei = malloc(sizeof (Emage_File_Info));
   if (!ei) return ;

   memcpy(&ei->info, info, sizeof (Eina_File_Direct_Info));

   ei->type = type;
   ei->subdir = EINA_FALSE;

   ei->item = elm_gengrid_item_sorted_insert(grid, ei->type->class, ei, _emage_file_info_cmp, NULL, NULL);
   if (type->func.display_file_insert)
     ei->link = type->func.display_file_insert(display, ei);
   else
     ei->link = NULL;
}

static void
_emage_grid_del(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Eio_File *file = data;

   eio_file_cancel(file);
}

static Eina_Bool
_emage_eio_filter_cb(void *data __UNUSED__, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   const Emage_Mapping *mapping;
   Eina_Array_Iterator it;
   unsigned int i;

   if (info->path[info->name_start] == '.')
     return EINA_FALSE;

   EINA_ARRAY_ITER_NEXT(_emage_mappings, i, mapping, it)
     if (mapping->func.may_play(handler, info))
       {
          eio_file_associate_direct_add(handler, "type", mapping, NULL);
	  eio_file_associate_direct_add(handler, "display", _emage_objects[i], NULL);
          return EINA_TRUE;
       }

   return EINA_FALSE;
}

static void
_emage_eio_main_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
   Evas_Object *grid = data;
   const Emage_Mapping *type;
   Evas_Object *display;

   type = eio_file_associate_find(handler, "type");
   display = eio_file_associate_find(handler, "display");

   _emage_file_add(grid, info, type, display);
}

static void
_emage_eio_done_cb(void *data, Eio_File *handler)
{
   Evas_Object *grid = data;

   evas_object_data_set(grid, "emage/eio", NULL);
   elm_gengrid_item_show(evas_object_data_get(grid, "emage/up_item"));
}

static void
_emage_eio_error_cb(void *data, Eio_File *handler, int error)
{
   Evas_Object *grid = data;

   if (evas_object_data_get(grid, "emage/eio") == handler)
     evas_object_data_set(grid, "emage/eio", NULL);
}

static void
_emage_selected(void *data __UNUSED__, Evas_Object *grid, void *event_info)
{
   Elm_Gengrid_Item *it = event_info;
   Emage_File_Info *info = elm_gengrid_item_data_get(it);
   const char *tmp;

   elm_gengrid_item_selected_set(it, EINA_FALSE);

   if (info->info.type != EINA_FILE_DIR)
     {
        const Emage_Mapping *mapping;
        Eina_Array_Iterator eit;
        unsigned int i;
        Evas_Object *search = NULL;

        EINA_ARRAY_ITER_NEXT(_emage_mappings, i, mapping, eit)
          if (mapping == info->type)
            {
               search = _emage_objects[i];
               break;
            }

        if (!search) return ;

        if (info->type->func.action)
          info->type->func.action(search, info->link);
	elm_layout_content_set(evas_object_data_get(grid, "emage/layout"), "show/content", search);
	edje_object_signal_emit(elm_layout_edje_get(evas_object_data_get(grid, "emage/layout")), "show,content", "code");
        return ;
     }

   tmp = eina_stringshare_add(info->info.path);
   emage_browse_directory(grid, tmp);
   eina_stringshare_del(tmp);
}

/*
  Assynchronously add the '..' directory because the clean up of the gengrid happen after
  the 'selected' callback and not during it.
 */
static Eina_Bool
_emage_add_up(void *data)
{
   Emage_File_Info *info;
   Evas_Object *grid = data;

   info = evas_object_data_get(grid, "emage/up");
   if (info)
     {
       info->item = elm_gengrid_item_sorted_insert(grid, info->type->class,
						   info, _emage_file_info_cmp,
						   NULL, NULL);
       if (!evas_object_data_get(grid, "emage/eio"))
         elm_gengrid_item_show(info->item);
       evas_object_data_set(grid, "emage/up_item", info->item);
     }

   evas_object_data_set(grid, "emage/up", NULL);
   evas_object_data_set(grid, "emage/up_timer", NULL);

   return ECORE_CALLBACK_CANCEL;
}

void
emage_browse_directory(Evas_Object *grid, const char *path)
{
   const Emage_Mapping *mapping;
   Eina_Array_Iterator it;
   unsigned int i;
   Emage_File_Info *info;
   Eio_File *file;
   const char *current = NULL;

   if (!grid) return ;

   current = evas_object_data_get(grid, "emage/current");
   if (current && !strcmp(current, path))
       return ;

   file = evas_object_data_get(grid, "emage/eio");
   if (file)
     {
        evas_object_event_callback_del_full(grid, EVAS_CALLBACK_DEL, _emage_grid_del, file);
        evas_object_data_set(grid, "emage/eio", NULL);
     }

   EINA_ARRAY_ITER_NEXT(_emage_mappings, i, mapping, it)
     if (mapping->func.display_clear)
       mapping->func.display_clear(_emage_objects[i]);

   evas_object_data_set(grid, "emage/up_item", NULL);
   elm_gengrid_clear(grid);

   file = eio_file_stat_ls(path,
                           _emage_eio_filter_cb,
                           _emage_eio_main_cb,
                           _emage_eio_done_cb,
                           _emage_eio_error_cb,
                           grid);
   evas_object_data_set(grid, "emage/eio", file);

   if (file)
     {
        evas_object_smart_callback_del(grid, "selected", _emage_selected);
        evas_object_smart_callback_add(grid, "selected", _emage_selected, NULL);
     }

   current = eina_stringshare_add(path);

   info = evas_object_data_get(grid, "emage/up");
   if (!info) info = malloc(sizeof (Emage_File_Info));
   if (info)
     {
        const char *idx = memrchr(current, '/',  eina_stringshare_strlen(current) - 1);

	if (idx)
	  {
	    memcpy(info->info.path, current, eina_stringshare_strlen(current));
	    info->info.path_length = idx - current;
	    info->info.name_start = eina_stringshare_strlen(current) - info->info.path_length + 1;
	    info->info.name_length = eina_stringshare_strlen(current) - info->info.name_start;
	    info->info.type = EINA_FILE_DIR;
	    info->info.path[info->info.path_length + 1] = '\0';

	    info->type = &_emage_folder_mapping;
	    info->subdir = EINA_TRUE;

	    evas_object_data_set(grid, "emage/up", info);

	    if (!evas_object_data_get(grid, "emage/up_timer"))
	      evas_object_data_set(grid, "emage/up_timer", ecore_idler_add(_emage_add_up, grid));
	  }
	else
	  {
             if (evas_object_data_get(grid, "emage/up_timer"))
               ecore_idler_del(evas_object_data_get(grid, "emage/up_timer"));
             evas_object_data_set(grid, "emage/up_timer", NULL);
	  }
     }

   path = eina_stringshare_add(path);
   eina_stringshare_del(evas_object_data_get(grid, "emage/current"));
   evas_object_data_set(grid, "emage/current", path);

   elm_object_text_part_set(evas_object_data_get(grid, "emage/layout"), "path", path);
}

Eina_Bool
emage_browse_register(const Emage_Mapping *mapping)
{
   if (!mapping) return EINA_FALSE;
   if (!mapping->func.may_play) return EINA_FALSE;
   return eina_array_push(_emage_mappings, mapping);
}

#define PLUGIN(Name)                            \
  { emage_show_##Name##_init, emage_show_##Name##_shutdown }

static const struct {
   Eina_Bool (*init)(void);
   Eina_Bool (*shutdown)(void);
} _emage_plugins[] = {
  PLUGIN(emotion),
  PLUGIN(folder),
  PLUGIN(image),
  { NULL, NULL }
};

Eina_Bool
emage_browse_load(Evas_Object *layout)
{
   const Emage_Mapping *mapping;
   Eina_Array_Iterator it;
   unsigned int i;

   _emage_objects = calloc(eina_array_count_get(_emage_mappings), sizeof (Evas_Object *));
   if (!_emage_objects)
     return EINA_FALSE;

   EINA_ARRAY_ITER_NEXT(_emage_mappings, i, mapping, it)
     {
        if (mapping->func.display_object)
          _emage_objects[i] = mapping->func.display_object(layout);
     }

   return EINA_TRUE;
}

Eina_Bool
emage_browse_init(void)
{
   unsigned int i;

   _emage_mappings = eina_array_new(8);

   for (i = 0; _emage_plugins[i].init != NULL; ++i)
     _emage_plugins[i].init();

   return EINA_TRUE;
}

Eina_Bool
emage_browse_shutdown(void)
{
   unsigned int i;

   for (i = 0; _emage_plugins[i].shutdown != NULL; ++i)
     _emage_plugins[i].shutdown();

   return EINA_TRUE;
}
