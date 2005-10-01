/** @file etk_tree.c */
#include "etk_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Tree
 * @{
 */

typedef struct _Etk_Tree_Item_Object
{
   Evas_Object *text_object;
   Evas_Object *image_object;
} Etk_Tree_Item_Object;

typedef struct _Etk_Tree_Item_Objects
{
   Evas_Object *rect_bg;
   Etk_Tree_Item_Object *objects;
} Etk_Tree_Item_Objects;

static void _etk_tree_constructor(Etk_Tree *tree);
static void _etk_tree_destructor(Etk_Tree *tree);
static void _etk_tree_move_resize(Etk_Widget *widget, int x, int y, int w, int h);
static void _etk_tree_realize_cb(Etk_Object *object, void *data);
static void _etk_tree_mouse_wheel_cb(Etk_Object *object, void *event_info, void *data);

static void _etk_tree_update(Etk_Tree *tree);
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Ecore_List *rows_list, va_list args);
static Etk_Tree_Item_Objects *_etk_tree_item_objects_new(Etk_Tree *tree);
static void _etk_tree_item_objects_free(Etk_Tree_Item_Objects *item_objects, Etk_Tree *tree);
static void _etk_tree_row_free(Etk_Tree_Row *row);
static void _etk_tree_cell_clear(Etk_Tree_Cell *cell, Etk_Tree_Col_Type cell_type);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Tree
 * @return Returns the type on an Etk_Tree
 */
Etk_Type *etk_tree_type_get()
{
   static Etk_Type *tree_type = NULL;

   if (!tree_type)
   {
      tree_type = etk_type_new("Etk_Tree", ETK_WIDGET_TYPE, sizeof(Etk_Tree), ETK_CONSTRUCTOR(_etk_tree_constructor), ETK_DESTRUCTOR(_etk_tree_destructor), NULL);
   }

   return tree_type;
}

/**
 * @brief Creates a new tree
 * @return Returns the new tree widget
 */
Etk_Widget *etk_tree_new()
{
   return etk_widget_new(ETK_TREE_TYPE, "theme_group", "tree", "focusable", TRUE, NULL);
}

/**
 * @brief Creates a new column for a tree
 * @param tree a tree
 * @param title the tile of the column
 * @param type the type of the objects in the cells of the column
 * @return Returns the new column
 */
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Col_Type type)
{
   Etk_Tree_Col *new_col;

   if (!tree)
      return NULL;

   tree->columns = realloc(tree->columns, sizeof(Etk_Tree_Col *) * (tree->num_cols + 1));
   new_col = malloc(sizeof(Etk_Tree_Col));
   tree->columns[tree->num_cols] = new_col;

   new_col->id = tree->num_cols;
   new_col->tree = tree;
   if (title)
      new_col->title = strdup(title);
   else
      new_col->title = NULL;
   new_col->type = type;

   /* TODO: */
   new_col->xoffset = new_col->id * 100;
   new_col->width = 100;
   new_col->place = new_col->id;
   new_col->visible = TRUE;

   tree->num_cols++;

   return new_col;
}

/**
 * @brief Builds the tree. This function to be called after having added all the columns and before being able to add rows to the tree
 * @param tree a tree
 */
void etk_tree_build(Etk_Tree *tree)
{
   if (!tree)
      return;

   tree->built = TRUE;
}

/**
 * @brief Appends a new row to the tree
 * @param tree a tree
 * @param ... an Etk_Tree_Col * followed by the value of the cell, then again, an Etk_Tree_Col * followed by its value... terminated by NULL
 * @return Returns the new row
 */
Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...)
{
   Etk_Tree_Row *new_row;
   va_list args;

   if (!tree || !tree->built)
      return NULL;

   va_start(args, tree);
   new_row = _etk_tree_row_new_valist(tree, tree->rows, args);
   va_end(args);

   return new_row;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the tree */
static void _etk_tree_constructor(Etk_Tree *tree)
{
   if (!tree)
      return;

   tree->built = FALSE;
   tree->num_cols = 0;
   tree->columns = NULL;

   tree->rows = ecore_list_new();
   ecore_list_set_free_cb(tree->rows, ECORE_FREE_CB(_etk_tree_row_free));

   tree->item_height = 10;
   tree->items_objects = ecore_list_new();

   tree->scroll_percent = 0.0;

   ETK_WIDGET(tree)->move_resize = _etk_tree_move_resize;
   etk_signal_connect_after("realize", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_realize_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_mouse_wheel_cb), NULL);
}

/* Destroys the tree */
static void _etk_tree_destructor(Etk_Tree *tree)
{
   Etk_Tree_Item_Objects *item_objects;
   int i;

   if (!tree)
      return;

   
   ecore_list_goto_first(tree->items_objects);
   while ((item_objects = ecore_list_next(tree->items_objects)))
      _etk_tree_item_objects_free(item_objects, tree);
   ecore_list_destroy(tree->items_objects);
   ecore_list_destroy(tree->rows);
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i])
      {
         free(tree->columns[i]->title);
         free(tree->columns[i]);
      }
   }
   free(tree->columns);
}

/* Moves and resizes the tree */
static void _etk_tree_move_resize(Etk_Widget *widget, int x, int y, int w, int h)
{
   Etk_Tree *tree;
   int num_visible_items;
   int num_item_to_add;
   int i;

   if (!(tree = ETK_TREE(widget)) || !(tree->built))
      return;

   num_visible_items = ceil((float)h / tree->item_height) + 1;
   if (num_visible_items < 0)
      num_visible_items = 0;
   num_item_to_add = num_visible_items - ecore_list_nodes(tree->items_objects);

   if (num_item_to_add > 0)
   {
      Etk_Tree_Item_Objects *item_objects;

      for (i = 0; i < num_item_to_add; i++)
      {
         if (!(item_objects = _etk_tree_item_objects_new(tree)))
            break;
         ecore_list_append(tree->items_objects, item_objects);
      }
   }
   else if (num_item_to_add < 0)
   {
      for (i = 0; i < -num_item_to_add; i++)
      {
         if (!ecore_list_goto_last(tree->items_objects))
            break;
         _etk_tree_item_objects_free(ecore_list_current(tree->items_objects), tree);
         ecore_list_remove(tree->items_objects);
      }
   }

   _etk_tree_update(tree);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the tree is realized */
static void _etk_tree_realize_cb(Etk_Object *object, void *data)
{
   const char *data_value;
   Etk_Tree *tree;
   Etk_Widget *tree_widget;

   if (!(tree_widget = ETK_WIDGET(object)) || !tree_widget->theme_object)
      return;
   tree = ETK_TREE(tree_widget);
   
   data_value = edje_object_data_get(tree_widget->theme_object, "separator_color");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->separator_color.r, &tree->separator_color.g, &tree->separator_color.b, &tree->separator_color.a) != 4)
   {
      tree->separator_color.r = 255;
      tree->separator_color.g = 255;
      tree->separator_color.b = 255;
      tree->separator_color.a = 0;
   }

   data_value = edje_object_data_get(tree_widget->theme_object, "row_color1");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->row_color1.r, &tree->row_color1.g, &tree->row_color1.b, &tree->row_color1.a) != 4)
   {
      tree->row_color1.r = 255;
      tree->row_color1.g = 255;
      tree->row_color1.b = 255;
      tree->row_color1.a = 0;
   }

   data_value = edje_object_data_get(tree_widget->theme_object, "row_color2");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->row_color2.r, &tree->row_color2.g, &tree->row_color2.b, &tree->row_color2.a) != 4)
   {
      tree->row_color2.r = tree->row_color1.r;
      tree->row_color2.g = tree->row_color1.g;
      tree->row_color2.b = tree->row_color1.b;
      tree->row_color2.a = tree->row_color1.a;
   }

   data_value = edje_object_data_get(tree_widget->theme_object, "item_height");
   if (!data_value || sscanf(data_value, "%d", &tree->item_height) != 1)
      tree->item_height = 10;
}

/* TODO! */
static void _etk_tree_mouse_wheel_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Tree *tree;
   Etk_Event_Mouse_Wheel *event;

   if (!(tree = ETK_TREE(object)) || !(event = event_info))
      return;

   tree->scroll_percent += event->z * 0.05;
   tree->scroll_percent = ETK_CLAMP(tree->scroll_percent, 0.0, 1.0);
   _etk_tree_update(tree);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Updates the tree */
static void _etk_tree_update(Etk_Tree *tree)
{
   Etk_Tree_Row *row;
   Etk_Tree_Item_Objects *item_objects;
   int invisible_height, offset, first_visible_nth, delta;
   int x, y, w, h;
   int i, j;

   if (!tree)
      return;

   x = ETK_WIDGET(tree)->inner_geometry.x;
   y = ETK_WIDGET(tree)->inner_geometry.y;
   w = ETK_WIDGET(tree)->inner_geometry.w;
   h = ETK_WIDGET(tree)->inner_geometry.h;

   invisible_height = (ecore_list_nodes(tree->rows) * tree->item_height) - h;
   if (invisible_height < 0)
      invisible_height = 0;
   offset = invisible_height * tree->scroll_percent;
   first_visible_nth = offset / tree->item_height;
   delta = offset - (first_visible_nth * tree->item_height);

   ecore_list_goto_index(tree->rows, first_visible_nth);
   ecore_list_goto_first(tree->items_objects);
   i = 0;
   while ((item_objects = ecore_list_next(tree->items_objects)))
   {
      int item_y;

      item_y = y + i * tree->item_height - delta;
      evas_object_move(item_objects->rect_bg, x, item_y);
      evas_object_resize(item_objects->rect_bg, w, tree->item_height);

      if ((row = ecore_list_current(tree->rows)))
      {
         evas_object_show(item_objects->rect_bg);
         if ((first_visible_nth + i) % 2 == 0)
            evas_object_color_set(item_objects->rect_bg, tree->row_color1.r, tree->row_color1.g, tree->row_color1.b, tree->row_color1.a);
         else
            evas_object_color_set(item_objects->rect_bg, tree->row_color2.r, tree->row_color2.g, tree->row_color2.b, tree->row_color2.a);

         for (j = 0; j < tree->num_cols; j++)
         {
            Etk_Bool show_text = FALSE, show_image = FALSE;

            if (!tree->columns[j])
               continue;

            switch (tree->columns[j]->type)
            {
               case ETK_TREE_COL_TEXT:
                  evas_object_text_text_set(item_objects->objects[j].text_object, row->cells[j].text_value);
                  show_text = TRUE;
                  break;
               case ETK_TREE_COL_INT:
               {
                  char string[256];

                  snprintf(string, 255, "%d", row->cells[j].int_value);
                  evas_object_text_text_set(item_objects->objects[j].text_object, string);
                  show_text = TRUE;
                  break;
               }
               case ETK_TREE_COL_DOUBLE:
               {
                  char string[256];

                  snprintf(string, 255, "%'.2f", row->cells[j].double_value);
                  evas_object_text_text_set(item_objects->objects[j].text_object, string);
                  show_text = TRUE;
                  break;
               }
            }

            if (show_text)
            {
               Evas_Coord ch;

               evas_object_text_char_pos_get(item_objects->objects[j].text_object, 0, NULL, NULL, NULL, &ch);
               evas_object_move(item_objects->objects[j].text_object, x + tree->columns[j]->xoffset, item_y + (tree->item_height - ch + 1) / 2);
               evas_object_resize(item_objects->objects[j].text_object, tree->columns[j]->width, tree->item_height);
               evas_object_show(item_objects->objects[j].text_object);
            }
            else
               evas_object_hide(item_objects->objects[j].text_object);
         }

         ecore_list_next(tree->rows);
         i++;
      }
      else
      {
         evas_object_hide(item_objects->rect_bg);
      }
   }
}

/* Creates a new row and add it to the rows list*/
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Ecore_List *rows_list, va_list args)
{
   Etk_Tree_Row *new_row;
   Etk_Tree_Col *col;

   if (!tree || !tree->built || !rows_list)
      return NULL;

   new_row = malloc(sizeof(Etk_Tree_Row));
   new_row->tree = tree;
   new_row->child_rows = NULL;

   new_row->cells = malloc(sizeof(Etk_Tree_Cell) * tree->num_cols);
   while ((col = va_arg(args, Etk_Tree_Col *)))
   {
      Etk_Tree_Cell *new_cell;

      new_cell = &new_row->cells[col->id];
      switch (col->type)
      {
         case ETK_TREE_COL_TEXT:
         {
            const char *text;

            text = va_arg(args, char *);
            if (text)
               new_cell->text_value = strdup(text);
            else
               new_cell->text_value = NULL;
            break;
         }
         case ETK_TREE_COL_IMAGE:
         {
            const char *image;

            image = va_arg(args, char *);
            if (image)
               new_cell->image_filename_value = strdup(image);
            else
               new_cell->image_filename_value = NULL;
            break;
         }
         case ETK_TREE_COL_TEXT_IMAGE:
         {
            const char *text;
            const char *image;

            text = va_arg(args, char *);
            image = va_arg(args, char *);

            if (text)
               new_cell->text_image_value.text = strdup(text);
            else
               new_cell->text_image_value.text = NULL;

            if (image)
               new_cell->text_image_value.image_filename = strdup(image);
            else
               new_cell->text_image_value.image_filename = NULL;

            break;
         }
         case ETK_TREE_COL_INT:
         {
            int value;

            value = va_arg(args, int);
            new_cell->int_value = value;

            break;
         }
         case ETK_TREE_COL_DOUBLE:
         {
            double value;

            value = va_arg(args, double);
            new_cell->double_value = value;

            break;
         }
         default:
            new_cell->int_value = 0;
            break;
      }
   }

   ecore_list_append(rows_list, new_row);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
   return new_row;
}

/* Creates the evas objects needed by a row */ 
static Etk_Tree_Item_Objects *_etk_tree_item_objects_new(Etk_Tree *tree)
{
   Etk_Tree_Item_Objects *new_item_objects;
   Evas *evas;
   int i;

   if (!tree || !tree->built || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return NULL;

   new_item_objects = malloc(sizeof(Etk_Tree_Item_Objects));
   new_item_objects->rect_bg = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(tree), new_item_objects->rect_bg);

   new_item_objects->objects = malloc(sizeof(Etk_Tree_Item_Object) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      if (!tree->columns[i])
         continue;

      switch (tree->columns[i]->type)
      {
         case ETK_TREE_COL_TEXT:
         case ETK_TREE_COL_INT:
         case ETK_TREE_COL_DOUBLE:
            new_item_objects->objects[i].text_object = evas_object_text_add(evas);
            etk_widget_member_object_add(ETK_WIDGET(tree), new_item_objects->objects[i].text_object);
            evas_object_text_font_set(new_item_objects->objects[i].text_object, "Vera", 10);
            evas_object_color_set(new_item_objects->objects[i].text_object, 0, 0, 0, 255);
            new_item_objects->objects[i].image_object = NULL;
            break;
         case ETK_TREE_COL_IMAGE:
            new_item_objects->objects[i].text_object = NULL;
            new_item_objects->objects[i].image_object = evas_object_image_add(evas);
            etk_widget_member_object_add(ETK_WIDGET(tree), new_item_objects->objects[i].image_object);
            break;
         case ETK_TREE_COL_TEXT_IMAGE:
            new_item_objects->objects[i].text_object = evas_object_text_add(evas);
            etk_widget_member_object_add(ETK_WIDGET(tree), new_item_objects->objects[i].text_object);
            evas_object_text_font_set(new_item_objects->objects[i].text_object, "Vera", 10);
            evas_object_color_set(new_item_objects->objects[i].text_object, 0, 0, 0, 255);
            new_item_objects->objects[i].image_object = evas_object_image_add(evas);
            etk_widget_member_object_add(ETK_WIDGET(tree), new_item_objects->objects[i].image_object);
            break;
         default:
            new_item_objects->objects[i].text_object = NULL;
            new_item_objects->objects[i].image_object = NULL;
            break;
      }
   }

   return new_item_objects;
}

/* Frees all the evas objects of a row */
static void _etk_tree_item_objects_free(Etk_Tree_Item_Objects *item_objects, Etk_Tree *tree)
{
   int i;

   if (!item_objects || !tree)
      return;

   if (item_objects->rect_bg)
   {
      etk_widget_member_object_del(ETK_WIDGET(tree), item_objects->rect_bg);
      evas_object_del(item_objects->rect_bg);
   }

   for (i = 0; i < tree->num_cols; i++)
   {
      if (item_objects->objects[i].text_object)
      {
         etk_widget_member_object_del(ETK_WIDGET(tree), item_objects->objects[i].text_object);
         evas_object_del(item_objects->objects[i].text_object);
      }
      if (item_objects->objects[i].image_object)
      {
         etk_widget_member_object_del(ETK_WIDGET(tree), item_objects->objects[i].image_object);
         evas_object_del(item_objects->objects[i].image_object);
      }
   }
   free(item_objects->objects);
   free(item_objects);
}

/* Frees a row */
static void _etk_tree_row_free(Etk_Tree_Row *row)
{
   if (!row)
      return;

   if (row->tree && row->cells)
   {
      int i;

      for (i = 0; i < row->tree->num_cols; i++)
      {
         if (row->tree->columns[i])
            _etk_tree_cell_clear(&row->cells[i], row->tree->columns[i]->type);
      }
      free(row->cells);
   }

   if (row->child_rows)
      ecore_list_destroy(row->child_rows);
   free(row);
}

/* Frees a cell */
static void _etk_tree_cell_clear(Etk_Tree_Cell *cell, Etk_Tree_Col_Type cell_type)
{
   if (!cell)
      return;

   switch (cell_type)
   {
      case ETK_TREE_COL_TEXT:
         free(cell->text_value);
         break;
      case ETK_TREE_COL_IMAGE:
         free(cell->image_filename_value);
         break;
      case ETK_TREE_COL_TEXT_IMAGE:
         free(cell->text_image_value.text);
         free(cell->text_image_value.image_filename);
         break;
      default:
         break;
   }
}
