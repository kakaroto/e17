#include "exhibit.h"

typedef struct _Etk_Tree_Model_Wobbly
{
   Etk_Tree_Model model;
   Etk_Tree_Model_Image_Type icon_type;
   int icon_width;
}
Ex_Tree_Model_Wobbly;

typedef struct _Ex_Tree_Model_Wobbly_Data
{
      char *filename;
      char *edje_group;
      char *text;
} Ex_Tree_Model_Wobbly_Data;

static void _ex_tree_model_wobbly_cell_data_free(Etk_Tree_Model *model, void *cell_data);
static void _ex_tree_model_wobbly_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void _ex_tree_model_wobbly_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void _ex_tree_model_wobbly_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas);
static void _ex_tree_model_wobbly_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
static void _ex_tree_model_wobbly_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
  
Etk_Tree_Model *_ex_tree_model_wobbly_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type icon_type)
{
   Etk_Tree_Model *tree_model;

   tree_model = calloc(1, sizeof(Ex_Tree_Model_Wobbly));

   tree_model->tree = tree;
   tree_model->xalign = 0.0;
   tree_model->yalign = 0.5;
   tree_model->cell_data_size = sizeof(Ex_Tree_Model_Wobbly_Data);
   tree_model->cell_data_free = _ex_tree_model_wobbly_cell_data_free;
   tree_model->cell_data_set = _ex_tree_model_wobbly_cell_data_set;
   tree_model->cell_data_get = _ex_tree_model_wobbly_cell_data_get;
   tree_model->objects_create = _ex_tree_model_wobbly_objects_create;
   tree_model->render = _ex_tree_model_wobbly_render;
   ((Ex_Tree_Model_Wobbly *)tree_model)->icon_type = icon_type;
   ((Ex_Tree_Model_Wobbly *)tree_model)->icon_width = -1;

   return tree_model;
}

void _ex_tree_model_wobbly_icon_width_set(Etk_Tree_Model *model, int icon_width)
{
   if (!model)
     return;

   ((Ex_Tree_Model_Wobbly *)model)->icon_width = icon_width;
   if (model->tree)
     etk_widget_redraw_queue(ETK_WIDGET(model->tree));
}

int _ex_tree_model_wobbly_icon_width_get(Etk_Tree_Model *model)
{
   if (!model)
     return -1;
   return ((Ex_Tree_Model_Wobbly *)model)->icon_width;
}

static void _ex_tree_model_wobbly_cell_data_free(Etk_Tree_Model *model, void *cell_data)
{
   Ex_Tree_Model_Wobbly_Data *icon_text_data;

   if (!(icon_text_data = cell_data))
     return;
   free(icon_text_data->filename);
   free(icon_text_data->edje_group);
   free(icon_text_data->text);
}

static void _ex_tree_model_wobbly_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Ex_Tree_Model_Wobbly_Data *icon_text_data;
   char *string;

   if (!(icon_text_data = cell_data) || !args || !model)
     return;

   free(icon_text_data->filename);
   free(icon_text_data->edje_group);
   free(icon_text_data->text);
   icon_text_data->edje_group = NULL;
   icon_text_data->filename = NULL;
   icon_text_data->text = NULL;

   switch (((Ex_Tree_Model_Wobbly *)model)->icon_type)
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

static void _ex_tree_model_wobbly_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Ex_Tree_Model_Wobbly_Data *icon_text_data;
   char **string;

   if (!(icon_text_data = cell_data) || !args)
     return;

   switch (((Ex_Tree_Model_Wobbly *)model)->icon_type)
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

static void _ex_tree_model_wobbly_objects_create(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas)
{
   if (!cell_objects || !evas || !model)
     return;

   switch (((Ex_Tree_Model_Wobbly *)model)->icon_type)
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
   
   cell_objects[2] = edje_object_add(evas);
   edje_object_file_set(cell_objects[2], PACKAGE_DATA_DIR"/gui.edj", "tree/model/wobbly");
   
   evas_object_pass_events_set(cell_objects[0], 1);
   evas_object_event_callback_add(cell_objects[0], EVAS_CALLBACK_RESIZE, _ex_tree_model_wobbly_resize_cb, NULL);
   
   cell_objects[1] = evas_object_text_add(evas);
   /* TODO: font and color theme */
   evas_object_text_font_set(cell_objects[1], "Vera", 10);
   evas_object_color_set(cell_objects[1], 0, 0, 0, 255);
   evas_object_pass_events_set(cell_objects[1], 1);
}

static void _ex_tree_model_wobbly_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects)
{
   Ex_Tree_Model_Wobbly_Data *icon_text_data;
   int model_icon_width;
   int icon_max_width, icon_max_height;
   int icon_width = 0, icon_height = 0;
   Etk_Geometry icon_geometry;
   Etk_Bool show_icon = ETK_FALSE;
   Evas_Coord tw, th;
   int icon_offset = 0;
   const char *old_filename = NULL;
   const char *old_groupname = NULL;
   
   if (!(icon_text_data = cell_data) || !model)
     return;

   switch (((Ex_Tree_Model_Wobbly *)model)->icon_type)
     {
      case ETK_TREE_FROM_FILE:
	evas_object_image_file_get(cell_objects[0], &old_filename, NULL);
	evas_object_image_file_set(cell_objects[0], icon_text_data->filename, NULL);
	if (!evas_object_image_load_error_get(cell_objects[0]))
	  {
	     evas_object_image_size_get(cell_objects[0], &icon_width, &icon_height);
	     show_icon = ETK_TRUE;
	  }
	break;
      case ETK_TREE_FROM_EDJE:
	edje_object_file_get(cell_objects[0], &old_filename, &old_groupname);
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

   model_icon_width = ((Ex_Tree_Model_Wobbly *)model)->icon_width;
   if (model_icon_width >= 0)
     icon_max_width = ((Ex_Tree_Model_Wobbly *)model)->icon_width;
   else
     icon_max_width = 10000;
   icon_max_height = geometry.h;
   if (show_icon)
     {
	int wobble = 1;
	
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

	switch (((Ex_Tree_Model_Wobbly *)model)->icon_type)
	  {
	   case ETK_TREE_FROM_FILE:	     
	     if(old_filename && !strcmp(icon_text_data->filename, old_filename))	       
	       wobble = 0;	       
	     break;
	   case ETK_TREE_FROM_EDJE:
	     if(old_filename && old_groupname && 
		!strcmp(old_filename, icon_text_data->filename) && 
		!strcmp(old_groupname,icon_text_data->edje_group))
	       wobble = 0;
	     break;
	   default:
	     wobble = 1;
	     break;
	  }
		
	evas_object_show(cell_objects[2]);
	evas_object_show(cell_objects[0]);
	
	if ((((Ex_Tree_Model_Wobbly *)model)->icon_type) == ETK_TREE_FROM_FILE)
	  {
	     evas_object_image_fill_set(cell_objects[0], 0, 0, icon_geometry.w, icon_geometry.h);
	  }
	
	evas_object_move(cell_objects[2], icon_geometry.x, icon_geometry.y);	
	evas_object_resize(cell_objects[2], icon_geometry.w, icon_geometry.h);
	edje_extern_object_aspect_set(cell_objects[0], EDJE_ASPECT_CONTROL_BOTH, icon_geometry.w, icon_geometry.h);	
	
	edje_object_part_swallow(cell_objects[2], "swallow_icon", cell_objects[0]);
	if(wobble)
	  edje_object_signal_emit(cell_objects[2], "e,action,thumb,gen", "e");	
     }
   else
     {
	evas_object_hide(cell_objects[0]);	
	evas_object_hide(cell_objects[2]);
     }
   
   evas_object_move(cell_objects[1], geometry.x + icon_offset + (geometry.w - icon_offset - tw) * model->xalign, geometry.y + (geometry.h - th) * model->yalign);
   evas_object_show(cell_objects[1]);   
}

static void
_ex_tree_model_wobbly_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   int w, h;
   
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_image_fill_set(obj, 0, 0, w, h);
}
