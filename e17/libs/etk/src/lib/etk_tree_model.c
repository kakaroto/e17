#include "etk_tree_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Edje.h>
#include <Evas.h>
#include "etk_utils.h"
#include "etk_tree.h"

typedef struct _Etk_Tree_Model_Image
{
   Etk_Tree_Model model;
   Etk_Tree_Model_Image_Type image_type;
} Etk_Tree_Model_Image;

typedef struct _Etk_Tree_Model_Image_Data
{
   char *filename;
   char *edje_group;
} Etk_Tree_Model_Image_Data;

typedef struct _Etk_Tree_Model_Icon_Text
{
   Etk_Tree_Model model;
   Etk_Tree_Model_Image_Type icon_type;
   int icon_width;
} Etk_Tree_Model_Icon_Text;

typedef struct _Etk_Tree_Model_Icon_Text_Data
{
   char *filename;
   char *edje_group;
   char *text;
} Etk_Tree_Model_Icon_Text_Data;

/* Text model */
static void etk_tree_model_text_cell_data_free(Etk_Tree_Model *model, void *cell_data);
static void etk_tree_model_text_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_text_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_text_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas);
static void etk_tree_model_text_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
/* Int model */
static void etk_tree_model_int_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_int_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_int_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
/* Double model */
static void etk_tree_model_double_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_double_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_double_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
/* Image model */
static void etk_tree_model_image_cell_data_free(Etk_Tree_Model *model, void *cell_data);
static void etk_tree_model_image_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_image_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_image_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas);
static void etk_tree_model_image_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
/* Icon Text model */
static void etk_tree_model_icon_text_cell_data_free(Etk_Tree_Model *model, void *cell_data);
static void etk_tree_model_icon_text_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_icon_text_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_icon_text_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas);
static void etk_tree_model_icon_text_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
/* Checkbox model */
static void etk_tree_model_checkbox_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_checkbox_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void etk_tree_model_checkbox_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas);
static void etk_tree_model_checkbox_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
static void etk_tree_model_checkbox_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

/**
 * @brief Creates a model of column whose cells contain a text
 * @param tree a tree
 * @return Returns the new model
 * @note You don't need to free it with etk_tree_model_free() if you use it in a tree. It will be freed when the tree is destroyed
 */
Etk_Tree_Model *etk_tree_model_text_new(Etk_Tree *tree)
{
   Etk_Tree_Model *tree_model;
   
   tree_model = calloc(1, sizeof(Etk_Tree_Model));
   
   tree_model->tree = tree;
   tree_model->xalign = 0.0;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(char *);
   tree_model->cell_data_free = etk_tree_model_text_cell_data_free;
   tree_model->cell_data_set = etk_tree_model_text_cell_data_set;
   tree_model->cell_data_get = etk_tree_model_text_cell_data_get;
   tree_model->objects_create = etk_tree_model_text_objects_create;
   tree_model->render = etk_tree_model_text_render;
   
   return tree_model;
}

/**
 * @brief Creates a model of column whose cells contain an integer
 * @param tree a tree
 * @return Returns the new model
 * @note You don't need to free it with etk_tree_model_free() if you use it in a tree. It will be freed when the tree is destroyed
 */
Etk_Tree_Model *etk_tree_model_int_new(Etk_Tree *tree)
{
   Etk_Tree_Model *tree_model;
   
   tree_model = calloc(1, sizeof(Etk_Tree_Model));
   
   tree_model->tree = tree;
   tree_model->xalign = 1.0;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(int);
   tree_model->cell_data_set = etk_tree_model_int_cell_data_set;
   tree_model->cell_data_get = etk_tree_model_int_cell_data_get;
   tree_model->objects_create = etk_tree_model_text_objects_create;
   tree_model->render = etk_tree_model_int_render;
   
   return tree_model;
}

/**
 * @brief Creates a model of column whose cells contain a real
 * @param tree a tree
 * @return Returns the new model
 * @note You don't need to free it with etk_tree_model_free() if you use it in a tree. It will be freed when the tree is destroyed
 */
Etk_Tree_Model *etk_tree_model_double_new(Etk_Tree *tree)
{
   Etk_Tree_Model *tree_model;
   
   tree_model = calloc(1, sizeof(Etk_Tree_Model));
   
   tree_model->tree = tree;
   tree_model->xalign = 1.0;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(double);
   tree_model->cell_data_set = etk_tree_model_double_cell_data_set;
   tree_model->cell_data_get = etk_tree_model_double_cell_data_get;
   tree_model->objects_create = etk_tree_model_text_objects_create;
   tree_model->render = etk_tree_model_double_render;
   
   return tree_model;
}

/**
 * @brief Creates a model of column whose cells contain an image. The image can be loaded from a file or from an edje file
 * @param tree a tree
 * @param image_type the type of the images: ETK_TREE_FROM_FILE if the images will be loaded from a file @n
 * or ETK_TREE_FROM_EDJE if the images will be loaded from an edje file
 * @return Returns the new model
 * @note You don't need to free it with etk_tree_model_free() if you use it in a tree. It will be freed when the tree is destroyed
 */
Etk_Tree_Model *etk_tree_model_image_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type image_type)
{
   Etk_Tree_Model *tree_model;
   
   tree_model = calloc(1, sizeof(Etk_Tree_Model_Image));
   
   tree_model->tree = tree;
   tree_model->xalign = 0.0;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(Etk_Tree_Model_Image_Data);
   tree_model->cell_data_free = etk_tree_model_image_cell_data_free;
   tree_model->cell_data_set = etk_tree_model_image_cell_data_set;
   tree_model->cell_data_get = etk_tree_model_image_cell_data_get;
   tree_model->objects_create = etk_tree_model_image_objects_create;
   tree_model->render = etk_tree_model_image_render;
   ((Etk_Tree_Model_Image *)tree_model)->image_type = image_type;
   
   return tree_model;
}

/**
 * @brief Creates a model of column whose cells contain an icon and a text label. @n
 * The icon can be loaded from a file or from an edje file
 * @param tree a tree
 * @param icon_type the type of the icons: ETK_TREE_FROM_FILE if the icons will be loaded from a file @n
 * or ETK_TREE_FROM_EDJE if the icons will be loaded from an edje file
 * @return Returns the new model
 * @note You don't need to free it with etk_tree_model_free() if you use it in a tree. It will be freed when the tree is destroyed
 */
Etk_Tree_Model *etk_tree_model_icon_text_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type icon_type)
{
   Etk_Tree_Model *tree_model;
   
   tree_model = calloc(1, sizeof(Etk_Tree_Model_Icon_Text));
   
   tree_model->tree = tree;
   tree_model->xalign = 0.0;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(Etk_Tree_Model_Icon_Text_Data);
   tree_model->cell_data_free = etk_tree_model_icon_text_cell_data_free;
   tree_model->cell_data_set = etk_tree_model_icon_text_cell_data_set;
   tree_model->cell_data_get = etk_tree_model_icon_text_cell_data_get;
   tree_model->objects_create = etk_tree_model_icon_text_objects_create;
   tree_model->render = etk_tree_model_icon_text_render;
   ((Etk_Tree_Model_Icon_Text *)tree_model)->icon_type = icon_type;
   ((Etk_Tree_Model_Icon_Text *)tree_model)->icon_width = -1;
   
   return tree_model;
}

/**
 * @brief Creates a model of column whose cells contain a checkbox
 * @param tree a tree
 * @return Returns the new model
 * @note You don't need to free it with etk_tree_model_free() if you use it in a tree. It will be freed when the tree is destroyed
 */
Etk_Tree_Model *etk_tree_model_checkbox_new(Etk_Tree *tree)
{
   Etk_Tree_Model *tree_model;
   
   tree_model = calloc(1, sizeof(Etk_Tree_Model));
   
   tree_model->tree = tree;
   tree_model->xalign = 0.5;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(Etk_Bool);
   tree_model->cell_data_set = etk_tree_model_checkbox_cell_data_set;
   tree_model->cell_data_get = etk_tree_model_checkbox_cell_data_get;
   tree_model->objects_create = etk_tree_model_checkbox_objects_create;
   tree_model->render = etk_tree_model_checkbox_render;
   
   return tree_model;
}

/**
 * @brief Frees a tree model. Usually, you don't need to call it, it's automatically called when the tree is destroyed
 * @param model the model to free
 */
void etk_tree_model_free(Etk_Tree_Model *model)
{
   if (!model)
      return;
   
   if (model->model_free)
      model->model_free(model);
   free(model);
}

/**
 * @brief Sets the alignement of the objects inside the cell
 * @param model a tree model
 * @param xalign the horizontal alignment of the objects inside the cell (0.0 for left, 1.0 for right)
 * @param yalign the vertical alignment of the objects inside the cell (0.0 for up, 1.0 for bottom)
 */
void etk_tree_model_alignement_set(Etk_Tree_Model *model, float xalign, float yalign)
{
   if (!model)
      return;
   
   model->xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   model->yalign = ETK_CLAMP(yalign, 0.0, 1.0);
   if (model->tree)
      etk_widget_redraw_queue(ETK_WIDGET(model->tree));
}

/**
 * @brief Gets the alignement of the objects inside the cell
 * @param model a tree model
 * @param xalign the location where to set the horizontal alignment
 * @param yalign the location where to set the horizontal alignment
 */
void etk_tree_model_alignement_get(Etk_Tree_Model *model, float *xalign, float *yalign)
{
   if (!model)
      return;
   
   if (xalign)
      *xalign = model->xalign;
   if (yalign)
      *yalign = model->yalign;
}

/**
 * @brief Sets the width of the icons of the icon/text model
 * @param model a icon/text model
 * @param icon_width the width to set for the icons of the model. -1 to let Etk compute the width for each icon (the text could not be aligned then)
 */
void etk_tree_model_icon_text_icon_width_set(Etk_Tree_Model *model, int icon_width)
{
   if (!model)
      return;
   
   ((Etk_Tree_Model_Icon_Text *)model)->icon_width = icon_width;
   if (model->tree)
      etk_widget_redraw_queue(ETK_WIDGET(model->tree));
}

/**
 * @brief Gets the width of the icons of an icon/text model
 * @param model a icon/text model
 * @return Returns the width of the icons of the model. -1 means that Etk computes the width for each icon
 */
int etk_tree_model_icon_text_icon_width_get(Etk_Tree_Model *model)
{
   if (!model)
      return -1;
   return ((Etk_Tree_Model_Icon_Text *)model)->icon_width;
}

/*---------------------
 * Text Model
 *-------------------*/
/* Text: cell_data_free */
static void etk_tree_model_text_cell_data_free(Etk_Tree_Model *model, void *cell_data)
{
   char **text_data;
   
   if (!(text_data = cell_data))
      return;
   free(*text_data);
}

/* Text: cell_data_set */
static void etk_tree_model_text_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   char **text_data;
   char *text;
   
   if (!(text_data = cell_data) || !args)
      return;
   
   text = va_arg(*args, char *);
   free(*text_data);
   *text_data = text ? strdup(text) : NULL;
}

/* Text: cell_data_get */
static void etk_tree_model_text_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   char **text_data;
   char **text;
   
   if (!(text_data = cell_data) || !args)
      return;
   
   text = va_arg(*args, char **);
   if (text)
      *text = *text_data;
}

/* Text: objects_create */
static void etk_tree_model_text_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!cell_objects || !evas)
      return;
   
   cell_objects[0] = evas_object_text_add(evas);
   /* TODO: font and color theme */
   evas_object_text_font_set(cell_objects[0], "Vera", 10);
   evas_object_color_set(cell_objects[0], 0, 0, 0, 255);
   evas_object_pass_events_set(cell_objects[0], 1);
}

/* Text: Render */
static void etk_tree_model_text_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   char **text_data;
   Evas_Coord tw, th;
   
   if (!(text_data = cell_data))
      return;
   
   evas_object_text_text_set(cell_objects[0], *text_data);
   evas_object_geometry_get(cell_objects[0], NULL, NULL, &tw, &th);
   evas_object_move(cell_objects[0], geometry.x + (geometry.w - tw) * model->xalign, geometry.y + (geometry.h - th) * model->yalign);
   evas_object_show(cell_objects[0]);
}

/*---------------------
 * Int Model
 *-------------------*/
/* Int: cell_data_set */
static void etk_tree_model_int_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   int *int_data;
   
   if (!(int_data = cell_data) || !args)
      return;
   *int_data = va_arg(*args, int);
}

/* Int: cell_data_get */
static void etk_tree_model_int_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   int *int_data;
   int *value;
   
   if (!(int_data = cell_data) || !args)
      return;
   
   value = va_arg(*args, int *);
   if (value)
      *value = *int_data;
}

/* Int: render */
static void etk_tree_model_int_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   int *int_data;
   char string[256];
   Evas_Coord tw, th;
   
   if (!(int_data = cell_data))
      return;
   
   snprintf(string, 255, "%d", *int_data);
   evas_object_text_text_set(cell_objects[0], string);
   evas_object_geometry_get(cell_objects[0], NULL, NULL, &tw, &th);
   evas_object_move(cell_objects[0], geometry.x + (geometry.w - tw) * model->xalign, geometry.y + (geometry.h - th) * model->yalign);
   evas_object_show(cell_objects[0]);
}

/*---------------------
 * Double Model
 *-------------------*/
/* Double: cell_data_set */
static void etk_tree_model_double_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   double *double_data;
   
   if (!(double_data = cell_data) || !args)
      return;
   *double_data = va_arg(*args, double);
}

/* Double: cell_data_get */
static void etk_tree_model_double_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   double *double_data;
   double *value;
   
   if (!(double_data = cell_data) || !args)
      return;
   
   value = va_arg(*args, double *);
   if (value)
      *value = *double_data;
}

/* Double: render */
static void etk_tree_model_double_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   double *double_data;
   char string[256];
   Evas_Coord tw, th;
   
   if (!(double_data = cell_data) || !model)
      return;
   
   snprintf(string, 255, "%.2f", *double_data);
   evas_object_text_text_set(cell_objects[0], string);
   evas_object_geometry_get(cell_objects[0], NULL, NULL, &tw, &th);
   evas_object_move(cell_objects[0], geometry.x + (geometry.w - tw) * model->xalign, geometry.y + (geometry.h - th) * model->yalign);
   evas_object_show(cell_objects[0]);
}

/*---------------------
 * Image Model
 *-------------------*/
/* Image: cell_data_free */
static void etk_tree_model_image_cell_data_free(Etk_Tree_Model *model, void *cell_data)
{
   Etk_Tree_Model_Image_Data *image_data;
   
   if (!(image_data = cell_data))
      return;
   free(image_data->filename);
   free(image_data->edje_group);
}

/* Image: cell_data_set */
static void etk_tree_model_image_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Image_Data *image_data;
   char *string;
   
   if (!(image_data = cell_data) || !args || !model)
      return;
   
   free(image_data->filename);
   free(image_data->edje_group);
   image_data->edje_group = NULL;
   image_data->filename = NULL;
   
   switch (((Etk_Tree_Model_Image *)model)->image_type)
   {
      case ETK_TREE_FROM_FILE:
         string = va_arg(*args, char *);
         image_data->filename = string ? strdup(string) : NULL;
         break;
      case ETK_TREE_FROM_EDJE:
         string = va_arg(*args, char *);
         image_data->filename = string ? strdup(string) : NULL;
         string = va_arg(*args, char *);
         image_data->edje_group = string ? strdup(string) : NULL;
         break;
      default:
         break;
   }
}

/* Image: cell_data_get */
static void etk_tree_model_image_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Image_Data *image_data;
   char **string;
   
   if (!(image_data = cell_data) || !args || !model)
      return;
   
   switch (((Etk_Tree_Model_Image *)model)->image_type)
   {
      case ETK_TREE_FROM_FILE:
         string = va_arg(*args, char **);
         if (string)
            *string = image_data->filename;
         break;
      case ETK_TREE_FROM_EDJE:
         string = va_arg(*args, char **);
         if (string)
            *string = image_data->filename;
         string = va_arg(*args, char **);
         if (string)
            *string = image_data->edje_group;
         break;
      default:
         break;
   }
}

/* Image: objects_create */
static void etk_tree_model_image_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!cell_objects || !evas || !model)
      return;
   
   switch (((Etk_Tree_Model_Image *)model)->image_type)
   {
      case ETK_TREE_FROM_FILE:
         cell_objects[0] = evas_object_image_add(evas);
         break;
      case ETK_TREE_FROM_EDJE:
         cell_objects[0] = edje_object_add(evas);
         break;
      default:
         break;
   }
   evas_object_pass_events_set(cell_objects[0], 1);
}

/* Image: render */
static void etk_tree_model_image_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   Etk_Tree_Model_Image_Data *image_data;
   int image_width = 0, image_height = 0;
   Etk_Geometry image_geometry;
   Etk_Bool show_image = ETK_FALSE;
   
   if (!(image_data = cell_data) || !model)
      return;
   
   switch (((Etk_Tree_Model_Image *)model)->image_type)
   {
      case ETK_TREE_FROM_FILE:
         evas_object_image_file_set(cell_objects[0], image_data->filename, NULL);
         if (!evas_object_image_load_error_get(cell_objects[0]))
         {
            evas_object_image_size_get(cell_objects[0], &image_width, &image_height);
            show_image = ETK_TRUE;
         }
         break;
      case ETK_TREE_FROM_EDJE:
         if (edje_object_file_set(cell_objects[0], image_data->filename, image_data->edje_group))
         {
            edje_object_size_min_get(cell_objects[0], &image_width, &image_height);
            show_image = ETK_TRUE;
         }
         break;
      default:
         break;
   }
   
   if (!show_image)
      evas_object_hide(cell_objects[0]);
   else
   {
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
      image_geometry.x = geometry.x + (geometry.w - image_geometry.w) * model->xalign;
      image_geometry.y = geometry.y + (geometry.h - image_geometry.h) * model->yalign;
   
      evas_object_show(cell_objects[0]);
      if ((((Etk_Tree_Model_Image *)model)->image_type) == ETK_TREE_FROM_FILE)
         evas_object_image_fill_set(cell_objects[0], 0, 0, image_geometry.w, image_geometry.h);
      evas_object_move(cell_objects[0], image_geometry.x, image_geometry.y);
      evas_object_resize(cell_objects[0], image_geometry.w, image_geometry.h);
   }
}

/*---------------------
 * Icon Text Model
 *-------------------*/
/* Icon Text: cell_data_free */
static void etk_tree_model_icon_text_cell_data_free(Etk_Tree_Model *model, void *cell_data)
{
   Etk_Tree_Model_Icon_Text_Data *icon_text_data;
   
   if (!(icon_text_data = cell_data))
      return;
   free(icon_text_data->filename);
   free(icon_text_data->edje_group);
   free(icon_text_data->text);
}

/* Icon Text: cell_data_set */
static void etk_tree_model_icon_text_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Icon_Text_Data *icon_text_data;
   char *string;
   
   if (!(icon_text_data = cell_data) || !args || !model)
      return;
   
   free(icon_text_data->filename);
   free(icon_text_data->edje_group);
   free(icon_text_data->text);
   icon_text_data->edje_group = NULL;
   icon_text_data->filename = NULL;
   icon_text_data->text = NULL;
   
   switch (((Etk_Tree_Model_Icon_Text *)model)->icon_type)
   {
      case ETK_TREE_FROM_FILE:
         string = va_arg(*args, char *);
         icon_text_data->filename = string ? strdup(string) : NULL;
         break;
      case ETK_TREE_FROM_EDJE:
         string = va_arg(*args, char *);
         icon_text_data->filename = string ? strdup(string) : NULL;
         string = va_arg(*args, char *);
         icon_text_data->edje_group = string ? strdup(string) : NULL;
         break;
      default:
         break;
   }
   
   if ((string = va_arg(*args, char *)))
      icon_text_data->text = strdup(string);
}

/* Icon Text: cell_data_get */
static void etk_tree_model_icon_text_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Icon_Text_Data *icon_text_data;
   char **string;
   
   if (!(icon_text_data = cell_data) || !args)
      return;
   
   switch (((Etk_Tree_Model_Icon_Text *)model)->icon_type)
   {
      case ETK_TREE_FROM_FILE:
         string = va_arg(*args, char **);
         if (string)
            *string = icon_text_data->filename;
         break;
      case ETK_TREE_FROM_EDJE:
         string = va_arg(*args, char **);
         if (string)
            *string = icon_text_data->filename;
         string = va_arg(*args, char **);
         if (string)
            *string = icon_text_data->edje_group;
         break;
      default:
         break;
   }
   
   string = va_arg(*args, char **);
   if (string)
      *string = icon_text_data->text;
}

/* Icon Text: objects_create */
static void etk_tree_model_icon_text_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!cell_objects || !evas || !model)
      return;
   
   switch (((Etk_Tree_Model_Icon_Text *)model)->icon_type)
   {
      case ETK_TREE_FROM_FILE:
         cell_objects[0] = evas_object_image_add(evas);
         break;
      case ETK_TREE_FROM_EDJE:
         cell_objects[0] = edje_object_add(evas);
         break;
      default:
         break;
   }
   evas_object_pass_events_set(cell_objects[0], 1);
   
   cell_objects[1] = evas_object_text_add(evas);
   /* TODO: font and color theme */
   evas_object_text_font_set(cell_objects[1], "Vera", 10);
   evas_object_color_set(cell_objects[1], 0, 0, 0, 255);
   evas_object_pass_events_set(cell_objects[1], 1);
}

/* Icon Text: render */
static void etk_tree_model_icon_text_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   Etk_Tree_Model_Icon_Text_Data *icon_text_data;
   int model_icon_width;
   int icon_max_width, icon_max_height;
   int icon_width = 0, icon_height = 0;
   Etk_Geometry icon_geometry;
   Etk_Bool show_icon = ETK_FALSE;
   Evas_Coord tw, th;
   int icon_offset = 0;
   
   if (!(icon_text_data = cell_data) || !model)
      return;
   
   switch (((Etk_Tree_Model_Icon_Text *)model)->icon_type)
   {
      case ETK_TREE_FROM_FILE:
         evas_object_image_file_set(cell_objects[0], icon_text_data->filename, NULL);
         if (!evas_object_image_load_error_get(cell_objects[0]))
         {
            evas_object_image_size_get(cell_objects[0], &icon_width, &icon_height);
            show_icon = ETK_TRUE;
         }
         break;
      case ETK_TREE_FROM_EDJE:
         if (edje_object_file_set(cell_objects[0], icon_text_data->filename, icon_text_data->edje_group))
         {
            edje_object_size_min_get(cell_objects[0], &icon_width, &icon_height);
            show_icon = ETK_TRUE;
         }
         break;
      default:
         break;
   }
   evas_object_text_text_set(cell_objects[1], icon_text_data->text);
   evas_object_geometry_get(cell_objects[1], NULL, NULL, &tw, &th);
   
   model_icon_width = ((Etk_Tree_Model_Icon_Text *)model)->icon_width;
   if (model_icon_width >= 0)
      icon_max_width = ((Etk_Tree_Model_Icon_Text *)model)->icon_width;
   else
      icon_max_width = 10000;
   icon_max_height = geometry.h;
   
   if (show_icon)
   {
      if (icon_width == 0 || icon_height == 0)
      {
         icon_geometry.w = icon_max_height;
         icon_geometry.h = icon_max_height;
      }
      else if (icon_width <= icon_max_width && icon_height <= icon_max_height)
      {
         icon_geometry.w = icon_width;
         icon_geometry.h = icon_height;
      }
      else
      {
         if (((float)icon_height / icon_width) * icon_max_width <= icon_max_height)
         {
            icon_geometry.w = icon_max_width;
            icon_geometry.h = ((float)icon_height / icon_width) * icon_max_width;
         }
         else
         {
            icon_geometry.w = ((float)icon_width / icon_height) * icon_max_height;
            icon_geometry.h = icon_max_height;
         }
      }
      
      icon_offset = ((model_icon_width >= 0) ? model_icon_width : icon_geometry.w) + 8;
      icon_geometry.x = geometry.x + ((geometry.w - icon_offset - tw) * model->xalign);
      if (model_icon_width >= 0)
         icon_geometry.x += (model_icon_width - icon_geometry.w) / 2;
      icon_geometry.y = geometry.y + (geometry.h - icon_geometry.h) * model->yalign;
      
      evas_object_show(cell_objects[0]);
      if ((((Etk_Tree_Model_Icon_Text *)model)->icon_type) == ETK_TREE_FROM_FILE)
         evas_object_image_fill_set(cell_objects[0], 0, 0, icon_geometry.w, icon_geometry.h);
      evas_object_move(cell_objects[0], icon_geometry.x, icon_geometry.y);
      evas_object_resize(cell_objects[0], icon_geometry.w, icon_geometry.h);
   }
   else
      evas_object_hide(cell_objects[0]);

   evas_object_move(cell_objects[1], geometry.x + icon_offset + (geometry.w - icon_offset - tw) * model->xalign, geometry.y + (geometry.h - th) * model->yalign);
   evas_object_show(cell_objects[1]);
}

/*---------------------
 * Checkbox Model
 *-------------------*/

/* Checkbox: cell_data_set */
static void etk_tree_model_checkbox_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Bool *checked;
   
   if (!(checked = cell_data) || !args)
      return;
   *checked = va_arg(*args, int);
}

/* Checkbox: cell_data_get */
static void etk_tree_model_checkbox_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Bool *checked;
   Etk_Bool *return_location;
   
   if (!(checked = cell_data) || !args)
      return;
   
   return_location = va_arg(*args, Etk_Bool *);
   if (return_location)
      *return_location = *checked;
}

/* TODO */
#include "etk_theme.h"

/* Checkbox: objects_create */
static void etk_tree_model_checkbox_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!cell_objects || !evas)
      return;
   
   /* TODO */
   cell_objects[0] = edje_object_add(evas);
   edje_object_file_set(cell_objects[0], etk_theme_widget_theme_get(), "tree_checkbox");
   evas_object_event_callback_add(cell_objects[0], EVAS_CALLBACK_MOUSE_UP, etk_tree_model_checkbox_clicked_cb, model);
}

/* Checkbox: Render */
static void etk_tree_model_checkbox_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   Etk_Bool *checked;
   Evas_Coord w, h;
   
   if (!(checked = cell_data))
      return;
   
   if (*checked)
      edje_object_signal_emit(cell_objects[0], "activate", "");
   else
      edje_object_signal_emit(cell_objects[0], "deactivate", "");
   
   evas_object_data_set(cell_objects[0], "_Etk_Tree_Model_Checkbox::Row", row);
   edje_object_size_min_get(cell_objects[0], &w, &h);
   evas_object_move(cell_objects[0], geometry.x + (geometry.w - w) * model->xalign, geometry.y + (geometry.h - h) * model->yalign);
   evas_object_resize(cell_objects[0], w, h);
   evas_object_show(cell_objects[0]);
}

/* Called when the checkbox is released by the mouse */
static void etk_tree_model_checkbox_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Model *model;
   Etk_Tree_Row *row;
   Evas_Event_Mouse_Up *event;
   Evas_Coord ox, oy, ow, oh;
   Etk_Bool checked;
   
   if (!(model = data) || !model->col || !(event = event_info) || !(row = evas_object_data_get(obj, "_Etk_Tree_Model_Checkbox::Row")))
      return;
   
   evas_object_geometry_get(obj, &ox, &oy, &ow, &oh);
   if (event->canvas.x >= ox && event->canvas.x <= ox + ow && event->canvas.y >= oy && event->canvas.y <= oy + oh)
   {
      etk_tree_row_fields_get(row, model->col, &checked, NULL);
      etk_tree_row_fields_set(row, model->col, !checked, NULL);
   }
}
