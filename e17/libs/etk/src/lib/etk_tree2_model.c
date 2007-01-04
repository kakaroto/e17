#include "etk_tree2_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_tree2.h"
#include "etk_theme.h"
#include "etk_utils.h"

/* Data associated to the "image" model */
typedef struct Etk_Tree2_Model_Image_Data
{
   char *filename;
   char *key;
   enum
   {
      ETK_TREE2_MODEL_UNKNOWN_YET,
      ETK_TREE2_MODEL_NORMAL,
      ETK_TREE2_MODEL_EDJE,
      ETK_TREE2_MODEL_NOT_FOUND
   } type;
} Etk_Tree2_Model_Image_Data;

/* Data associated to the "progressbar" model */
typedef struct Etk_Tree2_Model_Progressbar_Data
{
   double fraction;
   char *text;
} Etk_Tree2_Model_Progressbar_Data;


/* Text model */
static void _text_cell_data_free(Etk_Tree2_Model *model, void *cell_data);
static void _text_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _text_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _text_objects_create(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas);
static void _text_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);
static int _text_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects);

/* Integer model */
static void _int_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _int_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _int_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);

/* Double model */
static void _double_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _double_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _double_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);

/* Image model */
static void _image_cell_data_free(Etk_Tree2_Model *model, void *cell_data);
static void _image_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _image_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _image_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);
static int _image_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects);

/* Checkbox model */
static void _checkbox_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _checkbox_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _checkbox_objects_create(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas);
static void _checkbox_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);
static int _checkbox_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects);
static void _checkbox_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

/* Progressbar model */
static void _progress_bar_cell_data_free(Etk_Tree2_Model *model, void *cell_data);
static void _progress_bar_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _progress_bar_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args);
static void _progress_bar_objects_create(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas);
static void _progress_bar_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);
static int _progress_bar_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects);


/**************************
 *
 * Implementation
 *
 **************************/
 
/**
 * @brief Creates a tree model that displays a text
 * @return Returns the new model
 * @note You don't need to free it with etk_tree2_model_free() if is associtated to a column.
 * It will be freed automatically when the column is destroyed
 */
Etk_Tree2_Model *etk_tree2_model_text_new(void)
{
   Etk_Tree2_Model *model;
   
   model = calloc(1, sizeof(Etk_Tree2_Model));
   
   model->cell_data_size = sizeof(char *);
   model->cell_data_free = _text_cell_data_free;
   model->cell_data_set = _text_cell_data_set;
   model->cell_data_get = _text_cell_data_get;
   model->objects_create = _text_objects_create;
   model->render = _text_render;
   model->width_get = _text_width_get;
   
   return model;
}

/**
 * @brief Creates a tree model that displays an integer value
 * @return Returns the new model
 * @note You don't need to free it with etk_tree2_model_free() if is associtated to a column.
 * It will be freed automatically when the column is destroyed
 */
Etk_Tree2_Model *etk_tree2_model_int_new(void)
{
   Etk_Tree2_Model *model;
   
   model = calloc(1, sizeof(Etk_Tree2_Model));
   
   model->cell_data_size = sizeof(int);
   model->cell_data_set = _int_cell_data_set;
   model->cell_data_get = _int_cell_data_get;
   model->objects_create = _text_objects_create;
   model->render = _int_render;
   model->width_get = _text_width_get;
   
   return model;
}

/**
 * @brief Creates a tree model that displays a float value
 * @return Returns the new model
 * @note You don't need to free it with etk_tree2_model_free() if is associtated to a column.
 * It will be freed automatically when the column is destroyed
 */
Etk_Tree2_Model *etk_tree2_model_double_new(void)
{
   Etk_Tree2_Model *model;
   
   model = calloc(1, sizeof(Etk_Tree2_Model));
   
   model->cell_data_size = sizeof(double);
   model->cell_data_set = _double_cell_data_set;
   model->cell_data_get = _double_cell_data_get;
   model->objects_create = _text_objects_create;
   model->render = _double_render;
   model->width_get = _text_width_get;
   
   return model;
}

/**
 * @brief Creates a tree model that displays an image
 * @return Returns the new model
 * @note You don't need to free it with etk_tree2_model_free() if is associtated to a column.
 * It will be freed automatically when the column is destroyed
 */
Etk_Tree2_Model *etk_tree2_model_image_new(void)
{
   Etk_Tree2_Model *model;
   
   model = calloc(1, sizeof(Etk_Tree2_Model));
   
   model->cell_data_size = sizeof(Etk_Tree2_Model_Image_Data);
   model->cell_data_free = _image_cell_data_free;
   model->cell_data_set = _image_cell_data_set;
   model->cell_data_get = _image_cell_data_get;
   model->render = _image_render;
   model->width_get = _image_width_get;
   
   return model;
}

/**
 * @brief Creates a tree model that displays an checkbox. Clicking on the checkbox toggles its state
 * @return Returns the new model
 * @note You don't need to free it with etk_tree2_model_free() if is associtated to a column.
 * It will be freed automatically when the column is destroyed
 */
Etk_Tree2_Model *etk_tree2_model_checkbox_new(void)
{
   Etk_Tree2_Model *model;
   
   model = calloc(1, sizeof(Etk_Tree2_Model));
   
   model->cell_data_size = sizeof(Etk_Bool);
   model->cell_data_set = _checkbox_cell_data_set;
   model->cell_data_get = _checkbox_cell_data_get;
   model->objects_create = _checkbox_objects_create;
   model->render = _checkbox_render;
   model->width_get = _checkbox_width_get;
   
   return model;
}

/**
 * @brief Creates a tree model that displays an checkbox. Clicking on the checkbox toggles its state
 * @return Returns the new model
 * @note You don't need to free it with etk_tree2_model_free() if is associtated to a column.
 * It will be freed automatically when the column is destroyed
 */
Etk_Tree2_Model *etk_tree2_model_progress_bar_new(void)
{
   Etk_Tree2_Model *model;
   
   model = calloc(1, sizeof(Etk_Tree2_Model));
   
   model->cell_data_size = sizeof(Etk_Tree2_Model_Progressbar_Data);
   model->cell_data_set = _progress_bar_cell_data_set;
   model->cell_data_get = _progress_bar_cell_data_get;
   model->cell_data_free = _progress_bar_cell_data_free;
   model->objects_create = _progress_bar_objects_create;
   model->render = _progress_bar_render;
   model->width_get = _progress_bar_width_get;
   
   return model;
}

/**
 * @brief Frees a tree model. Usually, you don't need to call it, it's called automatically
 * when the tree is destroyed
 * @param model the model to free
 */
void etk_tree2_model_free(Etk_Tree2_Model *model)
{
   if (!model)
      return;
   
   if (model->model_free)
      model->model_free(model);
   free(model);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/**************************
 * Text Model
 **************************/

/* Text: cell_data_free() */
static void _text_cell_data_free(Etk_Tree2_Model *model, void *cell_data)
{
   char **text_data;
   
   if (!(text_data = cell_data))
      return;
   free(*text_data);
}

/* Text: cell_data_set() */
static void _text_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   char **text_data;
   char *text;
   
   if (!(text_data = cell_data) || !args)
      return;
   
   text = va_arg(*args, char *);
   free(*text_data);
   *text_data = text ? strdup(text) : NULL;
}

/* Text: cell_data_get() */
static void _text_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   char **text_data;
   char **text;
   
   if (!(text_data = cell_data) || !args)
      return;
   
   text = va_arg(*args, char **);
   if (text)
      *text = *text_data;
}

/* Text: objects_create() */
static void _text_objects_create(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!cell_objects || !evas)
      return;
   
   cell_objects[0] = evas_object_text_add(evas);
   /* TODO: make the font themable... */
   evas_object_text_font_set(cell_objects[0], "Vera", 10);
   evas_object_color_set(cell_objects[0], 0, 0, 0, 255);
   evas_object_pass_events_set(cell_objects[0], 1);
}

/* Text: render() */
static void _text_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas)
{
   char **text_data;
   int th;
   
   if (!(text_data = cell_data) || !cell_objects || !cell_objects[0])
      return;
   
   evas_object_text_text_set(cell_objects[0], *text_data);
   evas_object_geometry_get(cell_objects[0], NULL, NULL, NULL, &th);
   evas_object_move(cell_objects[0], geometry.x, geometry.y + ((geometry.h - th) / 2));
   evas_object_show(cell_objects[0]);
}

/* Text: width_get() */
static int _text_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects)
{
   int w;
   
   if (!cell_objects || !cell_objects[0])
      return 0;
   
   evas_object_geometry_get(cell_objects[0], NULL, NULL, &w, NULL);
   return w;
}

/**************************
 * Integer Model
 **************************/

/* Int: cell_data_set() */
static void _int_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   int *int_data;
   
   if (!(int_data = cell_data) || !args)
      return;
   *int_data = va_arg(*args, int);
}

/* Int: cell_data_get() */
static void _int_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   int *int_data;
   int *value;
   
   if (!(int_data = cell_data) || !args)
      return;
   
   value = va_arg(*args, int *);
   if (value)
      *value = *int_data;
}

/* Int: render() */
static void _int_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas)
{
   int *int_data;
   char string[256];
   int th;
   
   if (!(int_data = cell_data) || !cell_objects || !cell_objects[0])
      return;
   
   snprintf(string, 255, "%d", *int_data);
   evas_object_text_text_set(cell_objects[0], string);
   evas_object_geometry_get(cell_objects[0], NULL, NULL, NULL, &th);
   evas_object_move(cell_objects[0], geometry.x, geometry.y + ((geometry.h - th) / 2));
   evas_object_show(cell_objects[0]);
}

/**************************
 * Double Model
 **************************/

/* Double: cell_data_set() */
static void _double_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   double *double_data;
   
   if (!(double_data = cell_data) || !args)
      return;
   *double_data = va_arg(*args, double);
}

/* Double: cell_data_get() */
static void _double_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   double *double_data;
   double *value;
   
   if (!(double_data = cell_data) || !args)
      return;
   
   value = va_arg(*args, double *);
   if (value)
      *value = *double_data;
}

/* Double: render() */
static void _double_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas)
{
   double *double_data;
   char string[256];
   int th;
   
   if (!(double_data = cell_data) || !model || !cell_objects || !cell_objects[0])
      return;
   
   snprintf(string, 255, "%.2f", *double_data);
   evas_object_text_text_set(cell_objects[0], string);
   evas_object_geometry_get(cell_objects[0], NULL, NULL, NULL, &th);
   evas_object_move(cell_objects[0], geometry.x, geometry.y + ((geometry.h - th) / 2));
   evas_object_show(cell_objects[0]);
}

/**************************
 * Image Model
 **************************/

/* Image: cell_data_free() */
static void _image_cell_data_free(Etk_Tree2_Model *model, void *cell_data)
{
   Etk_Tree2_Model_Image_Data *image_data;
   
   if (!(image_data = cell_data))
      return;
   
   free(image_data->filename);
   free(image_data->key);
}

/* Image: cell_data_set() */
static void _image_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree2_Model_Image_Data *image_data;
   char *string;
   
   if (!(image_data = cell_data) || !args || !model)
      return;
   
   free(image_data->filename);
   free(image_data->key);
   image_data->key = NULL;
   image_data->filename = NULL;
   image_data->type = ETK_TREE2_MODEL_UNKNOWN_YET;
   
   /* Get the file and the key from the args */
   string = va_arg(*args, char *);
   image_data->filename = string ? strdup(string) : NULL;
   string = va_arg(*args, char *);
   image_data->key = string ? strdup(string) : NULL;
}

/* Image: cell_data_get() */
static void _image_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree2_Model_Image_Data *image_data;
   char **string;
   
   if (!(image_data = cell_data) || !args || !model)
      return;
   
   string = va_arg(*args, char **);
   if (string)
      *string = image_data->filename;
   string = va_arg(*args, char **);
   if (string)
      *string = image_data->key;
}

/* Image: render() */
static void _image_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas)
{
   Etk_Tree2_Model_Image_Data *image_data;
   int image_width, image_height;
   Etk_Geometry image_geometry;
   
   if (!(image_data = cell_data) || !model || !cell_objects || !evas)
      return;

   if (!image_data->filename)
      return;
   
   /* This code is ugly... */
   while (image_data->type != ETK_TREE2_MODEL_NOT_FOUND)
   {
      if (cell_objects[0])
      {
         evas_object_del(cell_objects[0]);
         cell_objects[0] = NULL;
      }
   
      if (image_data->type == ETK_TREE2_MODEL_EDJE
         || (image_data->key && image_data->type == ETK_TREE2_MODEL_UNKNOWN_YET))
      {
         cell_objects[0] = edje_object_add(evas);
         if (edje_object_file_set(cell_objects[0], image_data->filename, image_data->key))
         {
            edje_object_size_min_get(cell_objects[0], &image_width, &image_height);
            image_data->type = ETK_TREE2_MODEL_EDJE;
            break;
         }
         else if (image_data->type == ETK_TREE2_MODEL_EDJE)
            image_data->type = ETK_TREE2_MODEL_NOT_FOUND;
         else
            image_data->type = ETK_TREE2_MODEL_NORMAL;
      }
      else
      {
         cell_objects[0] = evas_object_image_add(evas);
         evas_object_image_file_set(cell_objects[0], image_data->filename, image_data->key);
         if (!evas_object_image_load_error_get(cell_objects[0]))
         {
            evas_object_image_size_get(cell_objects[0], &image_width, &image_height);
            image_data->type = ETK_TREE2_MODEL_NORMAL;
            break;
         }
         else
            image_data->type = ETK_TREE2_MODEL_NOT_FOUND;
      }
   }
   
   /* Loading has failed... we return */
   if (image_data->type == ETK_TREE2_MODEL_NOT_FOUND)
   {
      if (cell_objects[0])
      {
         evas_object_del(cell_objects[0]);
         cell_objects[0] = NULL;
      }
      return;
   }
   
   /* TODO: humm... we shouldn't do this. It's dirty */
   evas_object_clip_set(cell_objects[0], model->col->clip);
   etk_widget_member_object_add(model->tree->grid, cell_objects[0]);
   
   
   /* The image is correctly loaded, we can now render it */
   if (image_width == 0 || image_height == 0)
   {
      image_geometry.w = geometry.h;
      image_geometry.h = geometry.h;
   }
   else
   {
      if (image_height <= geometry.h)
      {
         image_geometry.w = image_width;
         image_geometry.h = image_height;
      }
      else
      {
         image_geometry.w = geometry.h * ((float)image_width / image_height);
         image_geometry.h = geometry.h;
      }
   }
   
   image_geometry.x = geometry.x;
   image_geometry.y = geometry.y + ((geometry.h - image_geometry.h) / 2);
   if (image_data->type == ETK_TREE2_MODEL_NORMAL)
      evas_object_image_fill_set(cell_objects[0], 0, 0, image_geometry.w, image_geometry.h);
   
   evas_object_move(cell_objects[0], image_geometry.x, image_geometry.y);
   evas_object_resize(cell_objects[0], image_geometry.w, image_geometry.h);
   evas_object_show(cell_objects[0]);
}

/* Image: width_get() */
static int _image_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects)
{
   Etk_Tree2_Model_Image_Data *image_data;
   int w = 0;
   
   if (!(image_data = cell_data) || !cell_objects || !cell_objects[0])
      return 0;
   
   if (image_data->type == ETK_TREE2_MODEL_NORMAL)
      evas_object_image_size_get(cell_objects[0], &w, NULL);
   else if (image_data->type == ETK_TREE2_MODEL_EDJE)
      edje_object_size_min_get(cell_objects[0], &w, NULL);
   
   return w;
}

/**************************
 * Checkbox Model
 **************************/

/* Checkbox: cell_data_set() */
static void _checkbox_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   Etk_Bool *checked;
   
   if (!(checked = cell_data) || !args)
      return;
   *checked = va_arg(*args, int);
}

/* Checkbox: cell_data_get() */
static void _checkbox_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   Etk_Bool *checked;
   Etk_Bool *return_location;
   
   if (!(checked = cell_data) || !args)
      return;
   
   return_location = va_arg(*args, Etk_Bool *);
   if (return_location)
      *return_location = *checked;
}

/* Checkbox: objects_create() */
static void _checkbox_objects_create(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!model || !cell_objects || !evas)
      return;
   
   cell_objects[0] = edje_object_add(evas);
   etk_theme_edje_object_set_from_parent(cell_objects[0], "checkbox", ETK_WIDGET(model->tree));
   evas_object_event_callback_add(cell_objects[0], EVAS_CALLBACK_MOUSE_UP, _checkbox_clicked_cb, model);
}

/* Checkbox: render() */
static void _checkbox_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas)
{
   Etk_Bool *checked;
   int w, h;
   
   if (!(checked = cell_data) || !cell_objects || !cell_objects[0])
      return;
   
   if (*checked)
      edje_object_signal_emit(cell_objects[0], "etk,state,on", "etk");
   else
      edje_object_signal_emit(cell_objects[0], "etk,state,off", "etk");
   edje_object_message_signal_process(cell_objects[0]);
   
   evas_object_data_set(cell_objects[0], "_Etk_Tree2_Model_Checkbox::Row", row);
   edje_object_size_min_get(cell_objects[0], &w, &h);
   evas_object_move(cell_objects[0], geometry.x, geometry.y + ((geometry.h - h) / 2));
   evas_object_resize(cell_objects[0], w, h);
   evas_object_show(cell_objects[0]);
}

/* Checkbox: width_get() */
static int _checkbox_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects)
{
   int w;
   
   if (!cell_objects[0])
      return 0;
   
   edje_object_size_min_get(cell_objects[0], &w, NULL);
   return w;
}

/* Called when the checkbox is released by the mouse */
static void _checkbox_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree2_Model *model;
   Etk_Tree2_Row *row;
   Evas_Event_Mouse_Up *event;
   int ox, oy, ow, oh;
   Etk_Bool checked;
   
   if (!(model = data) || !model->col || !(event = event_info)
      || !(row = evas_object_data_get(obj, "_Etk_Tree2_Model_Checkbox::Row")))
   {
      return;
   }
   
   evas_object_geometry_get(obj, &ox, &oy, &ow, &oh);
   if (ETK_INSIDE(event->canvas.x, event->canvas.y, ox, oy, ow, oh))
   {
      /* TODO: there is a problem here if several models are combined.... */
      etk_tree2_row_fields_get(row, model->col, &checked, NULL);
      etk_tree2_row_fields_set(row, model->col, !checked, NULL);
   }
}

/**************************
 * Progressbar Model
 **************************/

/* Progressbar: cell_data_free() */
static void _progress_bar_cell_data_free(Etk_Tree2_Model *model, void *cell_data)
{
   Etk_Tree2_Model_Progressbar_Data *pbar_data;
   
   if (!(pbar_data = cell_data))
      return;
   free(pbar_data->text);
}

/* Progressbar: cell_data_set() */
static void _progress_bar_cell_data_set(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree2_Model_Progressbar_Data *pbar_data;
   char *text;
   
   if (!(pbar_data = cell_data) || !args)
      return;
   
   free(pbar_data->text);
   pbar_data->text = NULL;
   
   pbar_data->fraction = va_arg(*args, double);
   if ((text = va_arg(*args, char *)))
      pbar_data->text = strdup(text);
}

/* Progressbar: cell_data_get() */
static void _progress_bar_cell_data_get(Etk_Tree2_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree2_Model_Progressbar_Data *pbar_data;
   char **string;
   double *fraction;
   
   if (!(pbar_data = cell_data) || !args)
      return;
   
   fraction = va_arg(*args, double *);
   if (fraction)
      *fraction = pbar_data->fraction;
   
   string = va_arg(*args, char **);
   if (string)
      *string = pbar_data->text;         
}

/* Progressbar: objects_create() */
static void _progress_bar_objects_create(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!model || !cell_objects || !evas)
      return;
   
   cell_objects[0] = edje_object_add(evas);
   etk_theme_edje_object_set_from_parent(cell_objects[0], "progress_bar", ETK_WIDGET(model->tree));
}

/* Progressbar: render() */
static void _progress_bar_render(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas)
{
   Etk_Tree2_Model_Progressbar_Data *pbar_data;   
   int w, h;
   
   if (!(pbar_data = cell_data) || !cell_objects || !cell_objects[0])
      return;
   
   edje_object_part_drag_value_set(cell_objects[0], "etk.dragable.filler", 0.0, 0.0);
   edje_object_part_drag_size_set(cell_objects[0], "etk.dragable.filler", pbar_data->fraction, 0.0);
   
   edje_object_part_text_set(cell_objects[0], "etk.text.text", pbar_data->text ? pbar_data->text : "");
   
   evas_object_data_set(cell_objects[0], "_Etk_Tree2_Model_Progressbar::Row", row);
   edje_object_size_min_get(cell_objects[0], &w, &h);
   evas_object_move(cell_objects[0], geometry.x, geometry.y + ((geometry.h - h) / 2));
   evas_object_resize(cell_objects[0], w, h);
   evas_object_show(cell_objects[0]);
}

/* Progressbar: width_get() */
static int _progress_bar_width_get(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects)
{  
   int w;
   
   if (!cell_objects || !cell_objects[0])
      return 0;
   
   edje_object_size_min_get(cell_objects[0], &w, NULL);
   return w;
}
