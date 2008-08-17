#include "exhibit_model_wobbly.h"
#include <string.h>
#include <Evas.h>
#include <Edje.h>
#include "config.h"

/* Structure of the "wobbly" model */
typedef struct Etk_Tree_Model_Wobbly
{
   Etk_Tree_Model model;
   Etk_Cache *cache;
} Etk_Tree_Model_Wobbly;

/* Data associated to the "wobbly" model */
typedef struct Etk_Tree_Model_Wobbly_Data
{
   char *filename;
   char *key;
   enum
   {
      ETK_TREE_MODEL_UNKNOWN_YET,
      ETK_TREE_MODEL_NORMAL,
      ETK_TREE_MODEL_EDJE,
      ETK_TREE_MODEL_NOT_FOUND
   } type;
} Etk_Tree_Model_Wobbly_Data;

static void _wobbly_model_free(Etk_Tree_Model *model);
static void _wobbly_cell_data_free(Etk_Tree_Model *model, void *cell_data);
static void _wobbly_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void _wobbly_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void _wobbly_objects_cache(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
static Etk_Bool _wobbly_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL], Evas *evas);
static int _wobbly_width_get(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
static void _wobbly_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _wobbly_cache_free_cb(Evas_Object *object, void *data);
static void _wobbly_cache_remove(Etk_Tree_Model *model, const char *filename, const char *key);

/* TODOC */
Etk_Tree_Model *etk_tree_model_wobbly_new(void)
{
   Etk_Tree_Model *model;
   Etk_Tree_Model_Wobbly *wobbly_model;
   
   model = calloc(1, sizeof(Etk_Tree_Model_Wobbly));
   wobbly_model = (Etk_Tree_Model_Wobbly *)model;
   
   model->cell_data_size = sizeof(Etk_Tree_Model_Wobbly_Data);
   model->model_free = _wobbly_model_free;
   model->cell_data_free = _wobbly_cell_data_free;
   model->cell_data_set = _wobbly_cell_data_set;
   model->cell_data_get = _wobbly_cell_data_get;
   model->objects_cache = _wobbly_objects_cache;
   model->render = _wobbly_render;
   model->width_get = _wobbly_width_get;
   model->cache_remove = _wobbly_cache_remove;
   
   wobbly_model->cache = etk_cache_new(50);
   etk_cache_free_callback_set(wobbly_model->cache, _wobbly_cache_free_cb, NULL);
   
   return model;
}

# if 0
/* Get the Etk_Cache object associated with the model */
Etk_Cache *etk_tree_model_wobbly_cache_get(Etk_Tree_Model *model)
{
   Etk_Tree_Model_Wobbly *wobbly_model;
   
   if (!(wobbly_model = (Etk_Tree_Model_Wobbly *)model))
     return NULL;
   
   return wobbly_model->cache;
}
#endif

/* Wobbly: model_free() */
static void _wobbly_model_free(Etk_Tree_Model *model)
{
   Etk_Tree_Model_Wobbly *wobbly_model;
   
   if (!(wobbly_model = (Etk_Tree_Model_Wobbly *)model))
      return;
   etk_cache_destroy(wobbly_model->cache);
}

/* Wobbly: cell_data_free() */
static void _wobbly_cell_data_free(Etk_Tree_Model *model, void *cell_data)
{
   Etk_Tree_Model_Wobbly_Data *wobbly_data;
   
   if (!(wobbly_data = cell_data))
      return;
   
   free(wobbly_data->filename);
   free(wobbly_data->key);
}

/* Wobbly: cell_data_set() */
static void _wobbly_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Wobbly_Data *wobbly_data;
   char *string;
   
   if (!(wobbly_data = cell_data) || !args || !model)
      return;
   
   /* Get the file and the key from the args */
   string = va_arg(*args, char *);
   if (wobbly_data->filename != string)
   {
      free(wobbly_data->filename);
      wobbly_data->filename = string ? strdup(string) : NULL;
   }
   string = va_arg(*args, char *);
   if (wobbly_data->key != string)
   {
      free(wobbly_data->key);
      wobbly_data->key = string ? strdup(string) : NULL;
   }
   
   wobbly_data->type = ETK_TREE_MODEL_UNKNOWN_YET;
}

/* Wobbly: cell_data_get() */
static void _wobbly_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Wobbly_Data *wobbly_data;
   char **string;
   
   if (!(wobbly_data = cell_data) || !args || !model)
      return;
   
   string = va_arg(*args, char **);
   if (string)
      *string = wobbly_data->filename;
   string = va_arg(*args, char **);
   if (string)
      *string = wobbly_data->key;
}

/* Wobbly: objects_cache() */
static void _wobbly_objects_cache(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL])
{
   Etk_Tree_Model_Wobbly *wobbly_model;
   Etk_Tree_Model_Wobbly_Data *wobbly_data;
   
   if (!(wobbly_model = (Etk_Tree_Model_Wobbly *)model) || !cell_objects[0])
      return;
   
   wobbly_data = cell_data;
   if (wobbly_data && (wobbly_data->type == ETK_TREE_MODEL_NORMAL || wobbly_data->type == ETK_TREE_MODEL_EDJE))
      etk_cache_add(wobbly_model->cache, cell_objects[0], wobbly_data->filename, wobbly_data->key);
   else
   {
      evas_object_del(cell_objects[0]);
      evas_object_del(cell_objects[1]);
   }
   
   cell_objects[0] = NULL;
   cell_objects[1] = NULL;
}

/* Wobbly: render() */
static Etk_Bool _wobbly_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL], Evas *evas)
{
   Etk_Tree_Model_Wobbly *wobbly_model;
   Etk_Tree_Model_Wobbly_Data *wobbly_data;
   Etk_Bool object_created = ETK_FALSE;
   char *ext;
   
   if (!(wobbly_model = (Etk_Tree_Model_Wobbly *)model) || !(wobbly_data = cell_data) || !evas)
      return ETK_FALSE;

   if (!wobbly_data->filename || wobbly_data->type == ETK_TREE_MODEL_NOT_FOUND)
      return ETK_FALSE;
   
   /* If we don't know yet what the image's type is, we "guess" it */
   if (wobbly_data->type == ETK_TREE_MODEL_UNKNOWN_YET)
   {
      if (!wobbly_data->key)
         wobbly_data->type = ETK_TREE_MODEL_NORMAL;
      else if ((ext = strrchr(wobbly_data->filename, '.')) && strcasecmp(ext + 1, "eet") == 0)
         wobbly_data->type = ETK_TREE_MODEL_NORMAL;
      else
         wobbly_data->type = ETK_TREE_MODEL_EDJE;
   }
   
   /* If the object is not already in the cache, we load it */
   if (!(cell_objects[0] = etk_cache_find(wobbly_model->cache, wobbly_data->filename, wobbly_data->key)))
   {
      cell_objects[0] = edje_object_add(evas);
      edje_object_file_set(cell_objects[0], PACKAGE_DATA_DIR "/gui.edj", "tree/model/wobbly");
      edje_object_signal_emit(cell_objects[0], "e,action,thumb,gen", "e");
      evas_object_pass_events_set(cell_objects[0], 1);	
      
      if (wobbly_data->type == ETK_TREE_MODEL_NORMAL)
      {
         cell_objects[1] = evas_object_image_add(evas);
         evas_object_show(cell_objects[1]);
         evas_object_event_callback_add(cell_objects[1], EVAS_CALLBACK_RESIZE, _wobbly_resize_cb, NULL);
         
         evas_object_image_file_set(cell_objects[1], wobbly_data->filename, wobbly_data->key);
         if (!evas_object_image_load_error_get(cell_objects[1]))
            object_created = ETK_TRUE;
         else
            wobbly_data->type = ETK_TREE_MODEL_NOT_FOUND;
      }
      /* If it's not a normal image file, then it's an Edje file... */
      else
      {
         cell_objects[1] = edje_object_add(evas);
         if (edje_object_file_set(cell_objects[1], wobbly_data->filename, wobbly_data->key))
            object_created = ETK_TRUE;
         else
            wobbly_data->type = ETK_TREE_MODEL_NOT_FOUND;
      }
      
      edje_object_part_swallow(cell_objects[0], "swallow_icon", cell_objects[1]);
   }
   else
      cell_objects[1] = edje_object_part_swallow_get(cell_objects[0], "swallow_icon");
   
   /* If loading the image has failed, we destroy the objects and return */
   if (wobbly_data->type == ETK_TREE_MODEL_NOT_FOUND)
   {
      evas_object_del(cell_objects[0]);
      evas_object_del(cell_objects[1]);
      cell_objects[0] = NULL;
      cell_objects[1] = NULL;
      
      return ETK_FALSE;
   }
   
   /* Loading was successful, we render the objects */
   evas_object_move(cell_objects[0], geometry.x, geometry.y);
   evas_object_resize(cell_objects[0], geometry.h, geometry.h);
   evas_object_show(cell_objects[0]);
   
   return object_created;
}

/* Wobbly: width_get() */
static int _wobbly_width_get(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL])
{
   Etk_Tree_Model_Wobbly *wobbly_model;
   int w;
   
   if (!(wobbly_model = (Etk_Tree_Model_Wobbly *)model) || !cell_objects[0])
      return 0;
   
   evas_object_geometry_get(cell_objects[0], NULL, NULL, &w, NULL);
   return w;
}

/* Wobbly: called when the image object is resized */
static void _wobbly_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int w, h;
   
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_image_fill_set(obj, 0, 0, w, h);
}

/* Wobbly: called when an image is deleted from the cache */
static void _wobbly_cache_free_cb(Evas_Object *object, void *data)
{
   Evas_Object *image;
   
   if (!object)
      return;
   
   /* When the edje-object is deleted, we also delete the image object */
   if ((image = edje_object_part_swallow_get(object, "swallow_icon")))
      evas_object_del(image);
}

/* Wobbly: delete a certain evas object from the model's cache */
static void _wobbly_cache_remove(Etk_Tree_Model *model, const char *filename, const char *key)
{
   Etk_Tree_Model_Wobbly *wobbly_model;
      
   if (!(wobbly_model = (Etk_Tree_Model_Wobbly *)model))
     return;
   
   etk_cache_remove(wobbly_model->cache, etk_cache_find(wobbly_model->cache, filename, key));
}
