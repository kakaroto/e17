/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_iconbox.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_iconbox.h"

#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Edje.h>

#include "etk_cache.h"
#include "etk_container.h"
#include "etk_event.h"
#include "etk_label.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_scrolled_view.h"
#include "etk_theme.h"
#include "etk_utils.h"
#include "etk_string.h"

/**
 * @addtogroup Etk_Iconbox
 * @{
 */

#define ETK_ICONBOX_GRID_TYPE       (_etk_iconbox_grid_type_get())
#define ETK_ICONBOX_GRID(obj)       (ETK_OBJECT_CAST((obj), ETK_ICONBOX_GRID_TYPE, Etk_Iconbox_Grid))
#define ETK_IS_ICONBOX_GRID(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ICONBOX_GRID_TYPE))

#define ETK_ICONBOX_SCROLL_DELAY (1 / 30.0)
#define ETK_ICONBOX_MAX_SCROLL_SPEED 40.0
#define ETK_ICONBOX_MAX_SCROLL_DISTANCE 100
#define ETK_ICONBOX_SCROLL_MARGIN 15

typedef struct Etk_Iconbox_Grid
{
   /* Inherit form Etk_Widget */
   Etk_Widget widget;

   Etk_Iconbox *iconbox;

   Evas_List *icon_objects;
   Evas_Object *clip;

   Evas_Object *selection_rect;
   int selection_orig_x;
   int selection_orig_y;
   int selection_mouse_x;
   int selection_mouse_y;
   int selection_first_col;
   int selection_last_col;
   int selection_first_row;
   int selection_last_row;
   int xoffset;
   int yoffset;
   int num_cols;

   Ecore_Timer *scroll_timer;
   float hscrolling_speed;
   float vscrolling_speed;
   Etk_Bool selection_started:1;
} Etk_Iconbox_Grid;

typedef struct Etk_Iconbox_Icon_Object
{
   Evas_Object *image;
   Evas_Object *emblem;
   Etk_Widget *label;
   Etk_Bool use_edje;
} Etk_Iconbox_Icon_Object;

int ETK_ICONBOX_ICON_SELECTED_SIGNAL;
int ETK_ICONBOX_ICON_UNSELECTED_SIGNAL;
int ETK_ICONBOX_ALL_SELECTED_SIGNAL;
int ETK_ICONBOX_ALL_UNSELECTED_SIGNAL;

static void _etk_iconbox_constructor(Etk_Iconbox *iconbox);
static void _etk_iconbox_destructor(Etk_Iconbox *iconbox);
static void _etk_iconbox_size_request(Etk_Widget *widget, Etk_Size *requested_size);
static void _etk_iconbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static Etk_Type *_etk_iconbox_grid_type_get();
static void _etk_iconbox_grid_constructor(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_grid_destructor(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_iconbox_grid_scroll(Etk_Widget *widget, int x, int y);
static void _etk_iconbox_grid_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);

static Etk_Bool _etk_iconbox_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_iconbox_grid_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_iconbox_grid_unrealized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_iconbox_grid_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
static Etk_Bool _etk_iconbox_grid_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_iconbox_grid_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data);

static void _etk_iconbox_icon_object_add(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_icon_object_delete(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_icon_draw(Etk_Iconbox_Icon *icon, Etk_Iconbox_Icon_Object *icon_object, Etk_Iconbox_Model *model, int x, int y, Etk_Bool clip);
static void _etk_iconbox_grid_selection_rect_update(Etk_Iconbox_Grid *grid);
static int _etk_iconbox_grid_scroll_cb(void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Iconbox
 * @return Returns the type of an Etk_Iconbox
 */
Etk_Type *etk_iconbox_type_get(void)
{
   static Etk_Type *iconbox_type = NULL;

   if (!iconbox_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_ICONBOX_ICON_SELECTED_SIGNAL,
            "icon-selected", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_ICONBOX_ICON_UNSELECTED_SIGNAL,
            "icon-unselected", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_ICONBOX_ALL_SELECTED_SIGNAL,
            "all-selected", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_ICONBOX_ALL_UNSELECTED_SIGNAL,
            "all-unselected", etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      iconbox_type = etk_type_new("Etk_Iconbox", ETK_WIDGET_TYPE,
         sizeof(Etk_Iconbox), ETK_CONSTRUCTOR(_etk_iconbox_constructor),
         ETK_DESTRUCTOR(_etk_iconbox_destructor), signals);
   }

   return iconbox_type;
}

/**
 * @brief Creates a new iconbox
 * @return Returns the new iconbox widget
 */
Etk_Widget *etk_iconbox_new(void)
{
   return etk_widget_new(ETK_ICONBOX_TYPE, "theme-group", "iconbox",
      "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new model for the icons of the iconbox
 * @param iconbox an iconbox
 * @return Returns the new model
 */
Etk_Iconbox_Model *etk_iconbox_model_new(Etk_Iconbox *iconbox)
{
   Etk_Iconbox_Model *model;

   if (!iconbox)
      return NULL;

   model = malloc(sizeof(Etk_Iconbox_Model));
   model->iconbox = iconbox;

   model->width = 110;
   model->height = 76;
   model->xpadding = 5;
   model->ypadding = 4;

   model->icon_x = 26;
   model->icon_y = 0;
   model->icon_width = 48;
   model->icon_height = 48;
   model->emblem_width = 22;
   model->emblem_height = 22;
   model->icon_fill = ETK_FALSE;
   model->icon_keep_aspect = ETK_TRUE;

   model->label_x = 0;
   model->label_y = 50;
   model->label_width = 100;
   model->label_height = 18;
   model->label_xalign = 0.5;
   model->label_yalign = 0.0;

   iconbox->models = evas_list_append(iconbox->models, model);
   return model;
}

/**
 * @brief Frees the iconbox model
 * @param model the iconbox model to free
 * @note You don't need to do it manually, the models are automatically freed when the iconbox is destroyed
 */
void etk_iconbox_model_free(Etk_Iconbox_Model *model)
{
   Etk_Iconbox *iconbox;

   if (!model)
      return;

   iconbox = model->iconbox;
   iconbox->models = evas_list_remove(iconbox->models, model);
   if (iconbox->current_model == model)
      iconbox->current_model = evas_list_data(iconbox->models);

   free(model);
}

/**
 * @brief Sets the model used by the iconbox
 * @param iconbox an iconbox
 * @param model the model that the iconbox should use (if NULL, no icon will be displayed)
 */
void etk_iconbox_current_model_set(Etk_Iconbox *iconbox, Etk_Iconbox_Model *model)
{
   if (!iconbox || (model && model->iconbox != iconbox))
      return;

   iconbox->current_model = model;
   etk_range_increments_set(etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(iconbox->scrolled_view)),
      model->width * 0.75, model->width * 3.0);
   etk_range_increments_set(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(iconbox->scrolled_view)),
      model->height * 0.75, model->height * 3.0);

   etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(iconbox->grid));
   etk_widget_redraw_queue(iconbox->grid);
}

/**
 * @brief Gets the model currently used by the iconbox
 * @param iconbox an iconbox
 * @return Returns the model currently used by the iconbox
 */
Etk_Iconbox_Model *etk_iconbox_current_model_get(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return NULL;
   return iconbox->current_model;
}

/**
 * @brief Set the position of the icon's emblem
 * @param iconbox an iconbox
 * @param pos the position of the emblems
 */
void etk_iconbox_emblem_position_set(Etk_Iconbox *iconbox, Etk_Emblem_Position pos)
{
   if (!iconbox)
      return;
   iconbox->emblem_position = pos;
}

/**
 * @brief Get the position of the icon's emblem
 * @param iconbox an iconbox
 * @return the position of the emblems
 */
Etk_Emblem_Position etk_iconbox_emblem_position_get(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return -1;
   return iconbox->emblem_position;
}

/**
 * @brief Sets the global geometry of the iconbox model
 * @param model an iconbox model
 * @param width the width of the cells of the model (min = 10)
 * @param height the height of the cells of the model (min = 10)
 * @param xpadding the horizontal padding of the cells of the model (min = 0)
 * @param ypadding the vertical padding of the cells of the model (min = 0)
 */
void etk_iconbox_model_geometry_set(Etk_Iconbox_Model *model, int width, int height, int xpadding, int ypadding)
{
   if (!model)
      return;

   model->width = ETK_MAX(width, 10);
   model->height = ETK_MAX(height, 10);
   model->xpadding = ETK_MAX(xpadding, 0);
   model->ypadding = ETK_MAX(ypadding, 0);

   if (model->iconbox && model->iconbox->current_model == model)
   {
      etk_widget_redraw_queue(model->iconbox->grid);
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(model->iconbox->grid));
   }
}

/**
 * @brief Gets the global geometry of the iconbox model
 * @param model an iconbox model
 * @param width the location where to store the width of the cells of the model
 * @param height the location where to store the height of the cells of the model
 * @param xpadding the location where to store the horizontal padding of the cells of the model
 * @param ypadding the location where to store the vertical padding of the cells of the model
 */
void etk_iconbox_model_geometry_get(Etk_Iconbox_Model *model, int *width, int *height, int *xpadding, int *ypadding)
{
   if (width)
      *width = model ? model->width : 0;
   if (height)
      *height = model ? model->height : 0;
   if (xpadding)
      *xpadding = model ? model->xpadding : 0;
   if (ypadding)
      *ypadding = model ? model->ypadding : 0;
}

/**
 * @brief Sets the icon geometry of the iconbox model
 * @param model an iconbox model
 * @param x the x position of the icon image of the model (min = 0)
 * @param y the y position of the icon image of the model (min = 0)
 * @param width the width of the icon image of the model (min = 10)
 * @param height the height of the icon image of the model (min = 10)
 * @param fill if @a fill == ETK_TRUE, and if the icon image is
 * smaller than the icon geometry, the image will fill the geometry
 * @param keep_aspect if @a keep_aspect == ETK_TRUE, the icon image will keep its aspect ratio (no distortion)
 * @note the x/y positions are relative to the inner top left corner of the icon model: @n
 * i.e. the icon image will be in fact put at (model->xpadding + model->icon_x, model->ypadding + model->icon_y)
 */
void etk_iconbox_model_icon_geometry_set(Etk_Iconbox_Model *model, int x, int y, int width, int height, Etk_Bool fill, Etk_Bool keep_aspect)
{
   if (!model)
      return;

   model->icon_x = ETK_MAX(x, 0);
   model->icon_y = ETK_MAX(y, 0);
   model->icon_width = ETK_MAX(width, 10);
   model->icon_height = ETK_MAX(height, 10);
   model->icon_fill = fill;
   model->icon_keep_aspect = keep_aspect;

   if (model->iconbox && model->iconbox->current_model == model)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(model->iconbox->grid));
      etk_widget_redraw_queue(model->iconbox->grid);
   }
}

/**
 * @brief Gets the icon geometry of the iconbox model
 * @param model an iconbox model
 * @param x the location where to store x position of the icon image
 * @param y the location where to store y position of the icon image
 * @param width the location where to store the width of the icon image
 * @param height the location where to store the height of the icon image
 * @param fill the location where to store the fill property of the model
 * (see etk_iconbox_model_icon_geometry_set())
 * @param keep_aspect the location where to store the "keep aspect ratio"
 * property of the model (see etk_iconbox_model_icon_geometry_set())
 * @see etk_iconbox_model_icon_geometry_set()
 */
void etk_iconbox_model_icon_geometry_get(Etk_Iconbox_Model *model, int *x, int *y, int *width, int *height, Etk_Bool *fill, Etk_Bool *keep_aspect)
{
   if (x)
      *x = model ? model->icon_x : 0;
   if (y)
      *y = model ? model->icon_y : 0;
   if (width)
      *width = model ? model->icon_width : 0;
   if (height)
      *height = model ? model->icon_height : 0;
   if (fill)
      *fill = model ? model->icon_fill : ETK_FALSE;
   if (keep_aspect)
      *keep_aspect = model ? model->icon_keep_aspect : ETK_TRUE;
}

/**
 * @brief Sets the label geometry of the iconbox model
 * @param model an iconbox model
 * @param x the x position of the label of the model (min = 0)
 * @param y the y position of the label of the model (min = 0)
 * @param width the width of the label of the model (min = 10)
 * @param height the height of the label of the model (min = 5)
 * @param xalign the horizontal alignment of the label (from 0.0 to 1.0)
 * @param yalign the vertical alignment of the label
 * @note the x/y positions are relative to the inner top left corner of the icon model: @n
 * i.e. the label will be in fact put at (model->xpadding + model->label_x, model->ypadding + model->label_y)
 */
void etk_iconbox_model_label_geometry_set(Etk_Iconbox_Model *model, int x, int y, int width, int height, float xalign, float yalign)
{
   if (!model)
      return;

   model->label_x = ETK_MAX(x, 0);
   model->label_y = ETK_MAX(y, 0);
   model->label_width = ETK_MAX(width, 10);
   model->label_height = ETK_MAX(height, 5);
   model->label_xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   model->label_yalign = ETK_CLAMP(yalign, 0.0, 1.0);

   if (model->iconbox && model->iconbox->current_model == model)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(model->iconbox->grid));
      etk_widget_redraw_queue(model->iconbox->grid);
   }
}

/**
 * @brief Gets the label geometry of the iconbox model
 * @param model an iconbox model
 * @param x the location where to store x position of the label
 * @param y the location where to store y position of the label
 * @param width the location where to store width of the label
 * @param height the location where to store height of the label
 * @param xalign the location where to store horizontal alignment of the label
 * @param yalign the location where to store vertical alignment of the label
 */
void etk_iconbox_model_label_geometry_get(Etk_Iconbox_Model *model, int *x, int *y, int *width, int *height, float *xalign, float *yalign)
{
   if (x)
      *x = model ? model->label_x : 0;
   if (y)
      *y = model ? model->label_y : 0;
   if (width)
      *width = model ? model->label_width : 0;
   if (height)
      *height = model ? model->label_height : 0;
   if (xalign)
      *xalign = model ? model->label_xalign : 0.0;
   if (yalign)
      *yalign = model ? model->label_yalign : 0.0;
}

/**
 * @brief Freezes the iconbox: it will not be updated until it is thawed. @n
 * This function is useful when you want to add efficiently a lot of icons
 * @param iconbox an iconbox
 */
void etk_iconbox_freeze(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return;
   iconbox->frozen = ETK_TRUE;
}

/**
 * @brief Thaws the iconbox: it will update the iconbox if it was frozen
 * @param iconbox an iconbox
 */
void etk_iconbox_thaw(Etk_Iconbox *iconbox)
{
   if (!iconbox || !iconbox->frozen)
      return;

   etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(iconbox->grid));
   etk_widget_redraw_queue(iconbox->grid);
   iconbox->frozen = ETK_FALSE;
}

/**
 * @brief Appends a new icon to the iconbox
 * @param iconbox a iconbox
 * @param filename the filename of the image to use for the icon. It can be either an image or an edje file
 * @param edje_group the edje group to use for the icon. If NULL, @a filename will be considerated as a
 * normal image file
 * @param label the label to associate to the icon
 * @return Returns the new icon
 */
Etk_Iconbox_Icon *etk_iconbox_append(Etk_Iconbox *iconbox, const char *filename, const char *edje_group, const char *label)
{
   Etk_Iconbox_Icon *icon;

   if (!iconbox)
      return NULL;

   icon = malloc(sizeof(Etk_Iconbox_Icon));
   icon->iconbox = iconbox;
   icon->prev = NULL;
   icon->next = NULL;
   icon->filename = filename ? strdup(filename) : NULL;
   icon->edje_group = edje_group ? strdup(edje_group) : NULL;
   icon->emblem_filename = NULL;
   icon->emblem_edje_group = NULL;
   icon->label = label ? strdup(label) : NULL;
   icon->data = NULL;
   icon->data_free_cb = NULL;
   icon->selected = ETK_FALSE;

   if (iconbox->last_icon)
   {
      iconbox->last_icon->next = icon;
      icon->prev = iconbox->last_icon;
   }
   iconbox->last_icon = icon;
   if (!iconbox->first_icon)
      iconbox->first_icon = icon;
   iconbox->num_icons++;

   if (!iconbox->frozen)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(iconbox->grid));
      etk_widget_redraw_queue(iconbox->grid);
   }

   return icon;
}

/**
 * @brief Removes the icon from the iconbox
 * @param icon the icon to remove
 */
void etk_iconbox_icon_del(Etk_Iconbox_Icon *icon)
{
   Etk_Iconbox *iconbox;

   if (!icon)
      return;
   iconbox = icon->iconbox;

   if (iconbox->first_icon == icon)
      iconbox->first_icon = icon->next;
   if (iconbox->last_icon == icon)
      iconbox->last_icon = icon->prev;
   if (icon->prev)
      icon->prev->next = icon->next;
   if (icon->next)
      icon->next->prev = icon->prev;
   iconbox->num_icons--;

   free(icon->filename);
   free(icon->edje_group);
   free(icon->emblem_filename);
   free(icon->emblem_edje_group);
   free(icon->label);

   if (icon->data && icon->data_free_cb)
      icon->data_free_cb(icon->data);

   free(icon);

   if (!iconbox->frozen)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(iconbox->grid));
      etk_widget_redraw_queue(iconbox->grid);
   }
}

/**
 * @brief Removes all the icons of the iconbox
 * @param iconbox an iconbox
 */
void etk_iconbox_clear(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return;

   while (iconbox->first_icon)
      etk_iconbox_icon_del(iconbox->first_icon);

   etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(iconbox->grid));
   etk_widget_redraw_queue(iconbox->grid);
}

/**
 * @brief Gets the icon located below the position ( @a x, @a y ). The position should be relative to the canvas
 * (TODO: relative to the icobox instead?)
 * @param iconbox an iconbox
 * @param x the x position
 * @param y the y position
 * @param over_cell if @a over_cell is ETK_TRUE, the function will return the icon if (x, y) is over
 * the cell of the icon
 * @param over_icon if @a over_icon == ETK_TRUE, the function will return the icon if (x, y) is over
 * the image of the icon
 * @param over_label if @a over_label == ETK_TRUE, the function will return the icon if (x, y) is over
 * the label of the icon
 * @return Returns the icon located below the position (x, y), or NULL if none
 */
Etk_Iconbox_Icon *etk_iconbox_icon_get_at_xy(Etk_Iconbox *iconbox, int x, int y, Etk_Bool over_cell, Etk_Bool over_icon, Etk_Bool over_label)
{
   Etk_Iconbox_Grid *grid;
   Etk_Iconbox_Model *model;
   Etk_Geometry grid_geom;
   int col, row;
   int icon_id;
   Etk_Bool over = ETK_FALSE;

   if (!iconbox || !(grid = ETK_ICONBOX_GRID(iconbox->grid)) || grid->num_cols <= 0 || !(model = iconbox->current_model))
      return NULL;

   etk_widget_inner_geometry_get(iconbox->grid, &grid_geom.x, &grid_geom.y, &grid_geom.w, &grid_geom.h);
   if (x < grid_geom.x || y < grid_geom.y || x > grid_geom.x + grid_geom.w || y > grid_geom.y + grid_geom.h)
      return NULL;

   col = (x - grid_geom.x + grid->xoffset) / iconbox->current_model->width;
   row = (y - grid_geom.y + grid->yoffset) / iconbox->current_model->height;
   icon_id = row * grid->num_cols + col;
   if (col >= grid->num_cols || icon_id >= iconbox->num_icons)
      return NULL;

   if (over_cell)
      over = ETK_TRUE;
   else
   {
      if (over_icon)
      {
         int icon_x, icon_y;

         icon_x = grid_geom.x - grid->xoffset + (col * model->width) + model->xpadding + model->icon_x;
         icon_y = grid_geom.y - grid->yoffset+ (row * model->height) + model->ypadding + model->icon_y;
         if (x >= icon_x && x <= icon_x + model->icon_width && y >= icon_y && y <= icon_y + model->icon_height)
            over = ETK_TRUE;
      }
      if (over_label)
      {
         int label_x, label_y;

         label_x = grid_geom.x - grid->xoffset + (col * model->width) + model->xpadding + model->label_x;
         label_y = grid_geom.y - grid->yoffset + (row * model->height) + model->ypadding + model->label_y;
         if (x >= label_x && x <= label_x + model->label_width && y >= label_y && y <= label_y + model->label_height)
            over = ETK_TRUE;
      }
   }

   if (over)
   {
      int i;
      Etk_Iconbox_Icon *icon;

      for (i = 0, icon = iconbox->first_icon; i < icon_id && icon; i++, icon = icon->next);
      return icon;
   }
   else
      return NULL;
}

/**
 * @brief Get the first icon with the given label. If more than one icon has the
 * same label only the first is returned.
 * @param iconbox an iconbox
 * @param label the label to search for
 * @return Return the icon with the given label, or NULL if the string is not found.
 */
Etk_Iconbox_Icon *etk_iconbox_icon_get_by_label(Etk_Iconbox *iconbox, const char *label)
{
   Etk_Iconbox_Icon *icon;

   if (!iconbox || !label)
      return NULL;

   for (icon = iconbox->first_icon; icon; icon = icon->next)
      if (!strcmp(icon->label, label))
         return icon;

   return NULL;
}

/**
 * @brief Get the first icon with the given data attached. If more than one icon has the
 * same data only the first is returned.
 * @param iconbox an iconbox
 * @param data the data to search for
 * @return Return the icon with the given data attached, or NULL if the data is not found.
 */
Etk_Iconbox_Icon *etk_iconbox_icon_get_by_data(Etk_Iconbox *iconbox, void *data)
{
   Etk_Iconbox_Icon *icon;

   if (!iconbox || !data)
      return NULL;

   for (icon = iconbox->first_icon; icon; icon = icon->next)
      if (icon->data == data)
         return icon;

   return NULL;
}

/**
 * @brief Get all the selected icons.
 * @param iconbox an iconbox
 * @return Return an Evas_List of Etk_Iconbox_Icon, or NULL if none is selected.
 * You must use evas_list_free() to free the list when you don't need anymore.
 */
Evas_List *etk_iconbox_icon_get_selected(Etk_Iconbox *iconbox)
{
   Etk_Iconbox_Icon *icon;
   Evas_List *l = NULL;

   if (!iconbox)
      return NULL;

   for (icon = iconbox->first_icon; icon; icon = icon->next)
      if (icon->selected)
         l = evas_list_append(l, icon);

   return l;
}

/**
 * @brief Get the first selected icon. If more than one icon is selected only
 * the first one is returned.
 * @param iconbox an iconbox
 * @return Return the first selected icon, or NULL if none is selected.
 */
Etk_Iconbox_Icon *etk_iconbox_icon_get_first_selected(Etk_Iconbox *iconbox)
{
   Etk_Iconbox_Icon *icon;

   if (!iconbox)
      return NULL;

   for (icon = iconbox->first_icon; icon; icon = icon->next)
      if (icon->selected)
         return icon;

   return NULL;
}

/**
 * @brief Sets the file path of the icon's image
 * @param icon an icon
 * @param filename the filename of the icon's image
 * @param edje_group the edje group of the icon's image. It has to be set to NULL for a "normal" image
 */
void etk_iconbox_icon_file_set(Etk_Iconbox_Icon *icon, const char *filename, const char *edje_group)
{
   if (!icon)
      return;

   if (icon->filename != filename)
   {
      free(icon->filename);
      icon->filename = filename ? strdup(filename) : NULL;
   }
   if (icon->edje_group != edje_group)
   {
      free(icon->edje_group);
      icon->edje_group = edje_group ? strdup(edje_group) : NULL;
   }

   if (!icon->iconbox->frozen)
      etk_widget_redraw_queue(icon->iconbox->grid);
}

/**
 * @brief Gets the file used by the icon's image
 * @param icon an icon
 * @param filename a location where to store the filename of the icon's image
 * @param edje_group a location where to store the edje group of the icon's image
 */
void etk_iconbox_icon_file_get(Etk_Iconbox_Icon *icon, const char **filename, const char **edje_group)
{
   if (filename)
      *filename = icon ? icon->filename : NULL;
   if (edje_group)
      *edje_group = icon ? icon->edje_group : NULL;
}

/**
 * @brief Sets the emblem from the stock icons
 * @param icon an icon
 * @param stock_name the name of the stock image (ex."readonly", "system", "favorite", etc)
 */
void etk_iconbox_icon_emblem_set_from_stock(Etk_Iconbox_Icon *icon, const char *stock_name)
{
   Etk_String *key;

   if (!icon || !stock_name)
      return;

   key = etk_string_new_printf("emblems/emblem-%s_22", stock_name);
   etk_iconbox_icon_emblem_file_set(icon, etk_theme_icon_path_get(), key->string);
   etk_object_destroy(ETK_OBJECT(key));
}

/**
 * @brief Sets the file path of the emblem's image
 * @param icon an icon
 * @param filename the filename of the emblem's image
 * @param edje_group the edje group of the emblem's image. It has to be set to NULL for a "normal" image
 */
void etk_iconbox_icon_emblem_file_set(Etk_Iconbox_Icon *icon, const char *filename, const char *edje_group)
{
   if (!icon)
      return;

   if (icon->emblem_filename != filename)
   {
      free(icon->emblem_filename);
      icon->emblem_filename = filename ? strdup(filename) : NULL;
   }
   if (icon->emblem_edje_group != edje_group)
   {
      free(icon->emblem_edje_group);
      icon->emblem_edje_group = edje_group ? strdup(edje_group) : NULL;
   }

   if (!icon->iconbox->frozen)
      etk_widget_redraw_queue(icon->iconbox->grid);
}

/**
 * @brief Gets the file used by the emblem's image
 * @param icon an icon
 * @param filename a location where to store the filename of the emblem's image
 * @param edje_group a location where to store the edje group of the emblem's image
 */
void etk_iconbox_icon_emblem_file_get(Etk_Iconbox_Icon *icon, const char **filename, const char **edje_group)
{
   if (filename)
      *filename = icon ? icon->emblem_filename : NULL;
   if (edje_group)
      *edje_group = icon ? icon->emblem_edje_group : NULL;
}

/**
 * @brief Sets the label of the icon
 * @param icon an icon
 * @param label the label to set to the icon
 */
void etk_iconbox_icon_label_set(Etk_Iconbox_Icon *icon, const char *label)
{
   if (!icon)
      return;

   if (icon->label != label)
   {
      free(icon->label);
      icon->label = label ? strdup(label) : NULL;

      if (!icon->iconbox->frozen)
         etk_widget_redraw_queue(icon->iconbox->grid);
   }
}

/**
 * @brief Gets the label used for the icon image
 * @param icon an icon
 * @return Returns the label of the icon
 */
const char *etk_iconbox_icon_label_get(Etk_Iconbox_Icon *icon)
{
   if (!icon)
      return NULL;
   return icon->label;
}

/**
 * @brief Sets a value to the data member of an icon. @n
 * The data could be retrieved with @a etk_iconbox_icon_row_data_get()
 * @param icon an icon
 * @param data the data to set
 */
void etk_iconbox_icon_data_set(Etk_Iconbox_Icon *icon, void *data)
{
   etk_iconbox_icon_data_set_full(icon, data, NULL);
}

/**
 * @brief Sets a value to the data member of an icon. @n
 * The data could be retrieved with @a etk_iconbox_icon_row_data_get()
 * @param icon an icon
 * @param data the data to set
 * @param free_cb the function to call to free the data
 */
void etk_iconbox_icon_data_set_full(Etk_Iconbox_Icon *icon, void *data, void (*free_cb)(void *data))
{
   if (!icon || (icon->data == data))
      return;

   if (icon->data && icon->data_free_cb)
      icon->data_free_cb(icon->data);

   icon->data = data;
   icon->data_free_cb = free_cb;
}

/**
 * @brief Gets the value of the data member of the icon
 * @param icon an icon
 * @return Returns the value of the data member of the icon
 */
void *etk_iconbox_icon_data_get(Etk_Iconbox_Icon *icon)
{
   if (!icon)
      return NULL;
   return icon->data;
}

/**
 * @brief Selects all the icons of the iconbox
 * @param iconbox an iconbox
 */
void etk_iconbox_select_all(Etk_Iconbox *iconbox)
{
   Etk_Iconbox_Icon *icon;

   if (!iconbox)
      return;

   for (icon = iconbox->first_icon; icon; icon = icon->next)
      icon->selected = ETK_TRUE;

   etk_widget_redraw_queue(iconbox->grid);
   etk_signal_emit(ETK_ICONBOX_ALL_SELECTED_SIGNAL, ETK_OBJECT(iconbox));
}

/**
 * @brief Unselects all the icons of the iconbox
 * @param iconbox an iconbox
 */
void etk_iconbox_unselect_all(Etk_Iconbox *iconbox)
{
   Etk_Iconbox_Icon *icon;

   if (!iconbox)
      return;

   for (icon = iconbox->first_icon; icon; icon = icon->next)
      icon->selected = ETK_FALSE;

   etk_widget_redraw_queue(iconbox->grid);
   etk_signal_emit(ETK_ICONBOX_ALL_UNSELECTED_SIGNAL, ETK_OBJECT(iconbox));
}

/**
 * @brief Selects the icon
 * @param icon the icon to select
 */
void etk_iconbox_icon_select(Etk_Iconbox_Icon *icon)
{
   if (!icon)
      return;
   icon->selected = ETK_TRUE;

   if (!icon->iconbox->frozen)
      etk_widget_redraw_queue(icon->iconbox->grid);
   etk_signal_emit(ETK_ICONBOX_ICON_SELECTED_SIGNAL, ETK_OBJECT(icon->iconbox), icon);
}

/**
 * @brief Unselects the icon
 * @param icon the icon to select
 */
void etk_iconbox_icon_unselect(Etk_Iconbox_Icon *icon)
{
   if (!icon)
      return;
   icon->selected = ETK_FALSE;

   if (!icon->iconbox->frozen)
      etk_widget_redraw_queue(icon->iconbox->grid);
   etk_signal_emit(ETK_ICONBOX_ICON_UNSELECTED_SIGNAL, ETK_OBJECT(icon->iconbox), icon);
}

/**
 * @brief Gets whether the icon is selected
 * @param icon an icon
 * @return Returns ETK_TRUE if the icon is selected, ETK_FALSE otherwise
 */
Etk_Bool etk_iconbox_is_selected(Etk_Iconbox_Icon *icon)
{
   if (!icon)
      return ETK_FALSE;
   return icon->selected;
}

/**
 * @brief Gets the scrolled view of the iconbox.
 * It can be used to change the scrollbars' policy, or to get the scroll-value
 * @param iconbox an iconbox
 * @return Returns the scrolled view of the iconbox
 */
Etk_Scrolled_View *etk_iconbox_scrolled_view_get(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return NULL;
   return ETK_SCROLLED_VIEW(iconbox->scrolled_view);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/**************************
 * Iconbox
 **************************/

/* Initializes the default values of the iconbox */
static void _etk_iconbox_constructor(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return;

   iconbox->scrolled_view = etk_scrolled_view_new();
   etk_widget_parent_set(iconbox->scrolled_view, ETK_WIDGET(iconbox));
   etk_widget_theme_parent_set(iconbox->scrolled_view, ETK_WIDGET(iconbox));
   etk_widget_internal_set(iconbox->scrolled_view, ETK_TRUE);
   etk_widget_repeat_mouse_events_set(iconbox->scrolled_view, ETK_TRUE);
   etk_widget_show(iconbox->scrolled_view);

   iconbox->grid = etk_widget_new(ETK_ICONBOX_GRID_TYPE, "theme-group", "grid", "theme-parent", iconbox,
      "has-event-object", ETK_TRUE, "repeat-mouse-events", ETK_TRUE, "internal", ETK_TRUE, NULL);
   ETK_ICONBOX_GRID(iconbox->grid)->iconbox = iconbox;
   etk_container_add(ETK_CONTAINER(iconbox->scrolled_view), iconbox->grid);
   etk_widget_show(iconbox->grid);

   iconbox->models = NULL;
   iconbox->current_model = NULL;
   etk_iconbox_current_model_set(iconbox, etk_iconbox_model_new(iconbox));

   iconbox->emblem_position = ETK_ICONBOX_EMBLEM_POSITION_TL;
   iconbox->num_icons = 0;
   iconbox->first_icon = NULL;
   iconbox->last_icon = NULL;

   iconbox->frozen = ETK_FALSE;

   ETK_WIDGET(iconbox)->size_request = _etk_iconbox_size_request;
   ETK_WIDGET(iconbox)->size_allocate = _etk_iconbox_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(iconbox), ETK_CALLBACK(_etk_iconbox_realized_cb), NULL);
}

/* Destroys the iconbox */
static void _etk_iconbox_destructor(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return;

   etk_iconbox_clear(iconbox);

   iconbox->current_model = NULL;
   while (iconbox->models)
      etk_iconbox_model_free(iconbox->models->data);
}

/* Calculates the ideal size of the iconbox */
static void _etk_iconbox_size_request(Etk_Widget *widget, Etk_Size *requested_size)
{
   Etk_Iconbox *iconbox;

   if (!(iconbox = ETK_ICONBOX(widget)) || !requested_size)
      return;
   etk_widget_size_request(iconbox->scrolled_view, requested_size);
}

/* Resizes the iconbox to the allocated size */
static void _etk_iconbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Iconbox *iconbox;

   if (!(iconbox = ETK_ICONBOX(widget)))
      return;
   etk_widget_size_allocate(iconbox->scrolled_view, geometry);
}

/**************************
 * Iconbox Grid
 **************************/

/* Creates the type of the iconbox grid widget */
static Etk_Type *_etk_iconbox_grid_type_get()
{
   static Etk_Type *iconbox_type = NULL;

   if (!iconbox_type)
   {
      iconbox_type = etk_type_new("Etk_Iconbox_Grid", ETK_WIDGET_TYPE,
         sizeof(Etk_Iconbox_Grid),
         ETK_CONSTRUCTOR(_etk_iconbox_grid_constructor),
         ETK_DESTRUCTOR(_etk_iconbox_grid_destructor), NULL);
   }

   return iconbox_type;
}

/* Initializes the default values of the iconbox grid */
static void _etk_iconbox_grid_constructor(Etk_Iconbox_Grid *grid)
{
   if (!grid)
      return;

   grid->xoffset = 0;
   grid->yoffset = 0;
   grid->num_cols = 0;
   grid->icon_objects = NULL;
   grid->clip = NULL;
   grid->selection_rect = NULL;
   grid->selection_started = ETK_FALSE;
   grid->scroll_timer = NULL;

   ETK_WIDGET(grid)->size_allocate = _etk_iconbox_grid_size_allocate;
   ETK_WIDGET(grid)->scroll = _etk_iconbox_grid_scroll;
   ETK_WIDGET(grid)->scroll_size_get = _etk_iconbox_grid_scroll_size_get;
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_unrealized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_mouse_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_mouse_up_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_mouse_move_cb), NULL);
}

/* Destroys the iconbox grid */
static void _etk_iconbox_grid_destructor(Etk_Iconbox_Grid *grid)
{
   if (!grid)
      return;

   if (grid->scroll_timer)
      ecore_timer_del(grid->scroll_timer);
}

/* Creates or destroys the objects of the icons according to the new size
 * of the iconbox grid, and then updates the iconbox */
static void _etk_iconbox_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Grid *grid;
   Etk_Iconbox_Icon *icon;
   Etk_Iconbox_Icon_Object *icon_object;
   Evas_List *l;
   int num_cols, num_rows;
   int num_visible_icons;
   int num_icons_to_add;
   Etk_Bool need_clip;
   int first_icon_id;
   int i, j;
   int x, y;

   if (!(grid = ETK_ICONBOX_GRID(widget)) || !(iconbox = grid->iconbox))
      return;

   if (!iconbox->current_model)
   {
      num_cols = 0;
      num_rows = 0;
   }
   else
   {
      num_cols = ETK_MAX(geometry.w / iconbox->current_model->width, 1);
      num_rows = ETK_MAX((geometry.h + iconbox->current_model->height - 1) / iconbox->current_model->height, 1);
      if (((num_rows * iconbox->current_model->height) - (grid->yoffset % iconbox->current_model->height)) < geometry.h)
         num_rows++;
   }
   num_visible_icons = ETK_MIN(num_cols * num_rows, iconbox->num_icons);
   num_icons_to_add = num_visible_icons - evas_list_count(grid->icon_objects);
   grid->num_cols = num_cols;

   /* Cache current icons */
   for (l = grid->icon_objects; l; l = l->next)
   {
      icon_object = l->data;

      if (icon_object->image)
      {
         /*etk_widget_member_object_del(iconbox->grid, icon_object->image);
         if (icon_object->use_edje)
            etk_cache_edje_object_add(icon_object->image);
         else
            etk_cache_image_object_add(icon_object->image);*/
         evas_object_del(icon_object->image);
         icon_object->image = NULL;
      }
      if (icon_object->emblem)
      {
         evas_object_del(icon_object->emblem);
         icon_object->emblem = NULL;
      }
   }

   /* Create or delete icon objects if necessary */
   if (num_icons_to_add > 0)
   {
      for (i = 0; i < num_icons_to_add; i++)
         _etk_iconbox_icon_object_add(grid);
   }
   else if (num_icons_to_add < 0)
   {
      for (i = 0; i < -num_icons_to_add; i++)
         _etk_iconbox_icon_object_delete(grid);
   }

   /* Draw the icons */
   l = grid->icon_objects;
   if (iconbox->current_model)
   {
      first_icon_id = (grid->yoffset / iconbox->current_model->height) * num_cols +
         (grid->xoffset / iconbox->current_model->width);
      for (i = 0, icon = iconbox->first_icon; i < first_icon_id && icon; i++, icon = icon->next);

      y = -(grid->yoffset % iconbox->current_model->height) + geometry.y;
      for (i = 0; i < num_rows; i++)
      {
         x = -(grid->xoffset % iconbox->current_model->width) + geometry.x;

         for (j = 0; j < num_cols && icon && l; j++, icon = icon->next, l = l->next)
         {
            icon_object = l->data;

            need_clip = ETK_FALSE;
            if (i == 0 || i == num_rows - 1 || j == 0 || j == num_cols - 1)
            {
               if (x < geometry.x || x + iconbox->current_model->width > geometry.x + geometry.w
                  || y < geometry.y || y + iconbox->current_model->height > geometry.y + geometry.h)
               {
                  need_clip = ETK_TRUE;
               }
            }
            _etk_iconbox_icon_draw(icon, icon_object, iconbox->current_model, x, y, need_clip);
            x += iconbox->current_model->width;
         }
         y += iconbox->current_model->height;
      }
   }

   /* Hide the other icon objects */
   for (; l; l = l->next)
   {
      icon_object = l->data;
      evas_object_hide(icon_object->image);
      evas_object_hide(icon_object->emblem);
      etk_widget_hide(icon_object->label);
   }

   /* Move/resize the clip */
   evas_object_move(grid->clip, geometry.x, geometry.y);
   evas_object_resize(grid->clip, geometry.w, geometry.h);

   /* Raise the selection rect */
   etk_widget_member_object_raise(ETK_WIDGET(grid), grid->selection_rect);
}

/* Scrolls the iconbox grid */
static void _etk_iconbox_grid_scroll(Etk_Widget *widget, int x, int y)
{
   Etk_Iconbox_Grid *grid;

   if (!(grid = ETK_ICONBOX_GRID(widget)))
      return;

   if (grid->selection_started)
   {
      grid->selection_mouse_x += (x - grid->xoffset);
      grid->selection_mouse_y += (y - grid->yoffset);
   }

   grid->xoffset = x;
   grid->yoffset = y;

   if (grid->selection_started)
      _etk_iconbox_grid_selection_rect_update(grid);

   etk_widget_redraw_queue(widget);
}

/* Gets the scrolling size of the icon grid */
static void _etk_iconbox_grid_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size)
{
   Etk_Iconbox *iconbox;

   if (!widget || !(iconbox = ETK_ICONBOX_GRID(widget)->iconbox) || !scroll_size)
      return;

   if (!iconbox->current_model)
   {
      scroll_size->w = 0;
      scroll_size->h = 0;
   }
   else
   {
      int num_cols;
      int num_rows;

      num_cols = ETK_MAX(scrollview_size.w / iconbox->current_model->width, 1);
      num_rows = (iconbox->num_icons + num_cols - 1) / num_cols;

      if (num_rows * iconbox->current_model->height > scrollview_size.h)
      {
         num_cols = ETK_MAX((scrollview_size.w - scrollbar_size.w) / iconbox->current_model->width, 1);
         num_rows = (iconbox->num_icons + num_cols - 1) / num_cols;
      }

      scroll_size->w = num_cols * iconbox->current_model->width;
      scroll_size->h = num_rows * iconbox->current_model->height;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/**************************
 * Iconbox
 **************************/

/* Called when the iconbox is realized */
static Etk_Bool _etk_iconbox_realized_cb(Etk_Object *object, void *data)
{
   Etk_Iconbox *iconbox;

   if (!(iconbox = ETK_ICONBOX(object)))
      return ETK_TRUE;

   if (etk_widget_theme_data_get(ETK_WIDGET(iconbox), "selected_icon_color", "%d %d %d %d",
      &iconbox->selected_icon_color.r, &iconbox->selected_icon_color.g,
      &iconbox->selected_icon_color.b, &iconbox->selected_icon_color.a) != 4)
   {
      iconbox->selected_icon_color.r = 128;
      iconbox->selected_icon_color.g = 128;
      iconbox->selected_icon_color.b = 128;
      iconbox->selected_icon_color.a = 255;
   }

   return ETK_TRUE;
}

/**************************
 * Iconbox Grid
 **************************/

/* Called when the iconbox grid is realized */
static Etk_Bool _etk_iconbox_grid_realized_cb(Etk_Object *object, void *data)
{
   Evas *evas;
   Etk_Iconbox_Grid *grid;

   if (!(grid = ETK_ICONBOX_GRID(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(grid))))
      return ETK_TRUE;

   grid->clip = evas_object_rectangle_add(evas);
   evas_object_show(grid->clip);
   etk_widget_member_object_add(ETK_WIDGET(grid), grid->clip);

   grid->selection_rect = edje_object_add(evas);
   etk_theme_edje_object_set_from_parent(grid->selection_rect, "selection", ETK_WIDGET(grid->iconbox));
   evas_object_pass_events_set(grid->selection_rect, 1);
   evas_object_clip_set(grid->selection_rect, grid->clip);
   etk_widget_member_object_add(ETK_WIDGET(grid), grid->selection_rect);

   return ETK_TRUE;
}

/* Called when the iconbox grid is unrealized */
static Etk_Bool _etk_iconbox_grid_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Iconbox_Grid *grid;

   if (!(grid = ETK_ICONBOX_GRID(object)))
      return ETK_TRUE;

   while (grid->icon_objects)
      _etk_iconbox_icon_object_delete(grid);
   grid->clip = NULL;

   return ETK_TRUE;
}

/* Called when the mouse presses the iconbox */
static Etk_Bool _etk_iconbox_grid_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Iconbox_Grid *grid;
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Icon *icon;
   Etk_Bool ctrl_pressed;

   if (!(grid = ETK_ICONBOX_GRID(object)) || !(iconbox = grid->iconbox))
      return ETK_TRUE;

   if (event->button != 1)
      return ETK_TRUE;

   ctrl_pressed = (event->modifiers & ETK_MODIFIER_CTRL);
   if ((icon = etk_iconbox_icon_get_at_xy(iconbox, event->canvas.x, event->canvas.y, ETK_FALSE, ETK_TRUE, ETK_TRUE)))
   {
      if (ctrl_pressed)
      {
         if (icon->selected)
            etk_iconbox_icon_unselect(icon);
         else
            etk_iconbox_icon_select(icon);
      }
      else
      {
         etk_iconbox_unselect_all(iconbox);
         etk_iconbox_icon_select(icon);
      }
   }
   else
   {
      grid->selection_started = ETK_TRUE;
      grid->selection_orig_x = event->widget.x + grid->xoffset;
      grid->selection_orig_y = event->widget.y + grid->yoffset;
      grid->selection_mouse_x = grid->selection_orig_x;
      grid->selection_mouse_y = grid->selection_orig_y;
      grid->selection_first_col = 0;
      grid->selection_last_col = 0;
      grid->selection_first_row = 0;
      grid->selection_last_row = 0;

      if (!ctrl_pressed)
         etk_iconbox_unselect_all(iconbox);
      for (icon = iconbox->first_icon; icon; icon = icon->next)
         icon->was_selected = icon->selected;
   }

   return ETK_TRUE;
}

/* Called when the mouse releases the iconbox */
static Etk_Bool _etk_iconbox_grid_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Iconbox_Grid *grid;

   if (!(grid = ETK_ICONBOX_GRID(object)))
      return ETK_TRUE;

   if (event->button != 1)
      return ETK_TRUE;

   if (grid->selection_started)
   {
      grid->selection_started = ETK_FALSE;
      evas_object_hide(grid->selection_rect);
   }
   if (grid->scroll_timer)
   {
      ecore_timer_del(grid->scroll_timer);
      grid->scroll_timer = NULL;
   }

   return ETK_TRUE;
}

/* Called when the mouse moves over the iconbox */
static Etk_Bool _etk_iconbox_grid_mouse_move_cb(Etk_Object *object, Etk_Event_Mouse_Move *event, void *data)
{
   Etk_Iconbox_Grid *grid;
   Etk_Bool should_scroll = ETK_FALSE;
   int x, y, w, h;

   if (!(grid = ETK_ICONBOX_GRID(object)))
      return ETK_TRUE;

   if (grid->selection_started)
   {
      grid->selection_mouse_x = event->cur.widget.x + grid->xoffset;
      grid->selection_mouse_y = event->cur.widget.y + grid->yoffset;
      _etk_iconbox_grid_selection_rect_update(grid);

      /* Scroll the grid if the mouse is outside of the grid */
      grid->hscrolling_speed = 0.0;
      grid->vscrolling_speed = 0.0;
      etk_widget_geometry_get(ETK_WIDGET(grid), &x, &y, &w, &h);
      if (event->cur.canvas.x <= x + ETK_ICONBOX_SCROLL_MARGIN)
      {
         grid->hscrolling_speed = -ETK_ICONBOX_MAX_SCROLL_SPEED / ETK_ICONBOX_MAX_SCROLL_DISTANCE *
            ETK_MIN((x + ETK_ICONBOX_SCROLL_MARGIN) - event->cur.canvas.x, ETK_ICONBOX_MAX_SCROLL_DISTANCE);
         should_scroll = ETK_TRUE;
      }
      else if (event->cur.canvas.x >= x + w - ETK_ICONBOX_SCROLL_MARGIN)
      {
         grid->hscrolling_speed = ETK_ICONBOX_MAX_SCROLL_SPEED / ETK_ICONBOX_MAX_SCROLL_DISTANCE *
            ETK_MIN(event->cur.canvas.x - (x + w - ETK_ICONBOX_SCROLL_MARGIN), ETK_ICONBOX_MAX_SCROLL_DISTANCE);
         should_scroll = ETK_TRUE;
      }
      if (event->cur.canvas.y <= y + ETK_ICONBOX_SCROLL_MARGIN)
      {
         grid->vscrolling_speed = -ETK_ICONBOX_MAX_SCROLL_SPEED / ETK_ICONBOX_MAX_SCROLL_DISTANCE *
            ETK_MIN((y + ETK_ICONBOX_SCROLL_MARGIN) - event->cur.canvas.y, ETK_ICONBOX_MAX_SCROLL_DISTANCE);
         should_scroll = ETK_TRUE;
      }
      else if (event->cur.canvas.y >= y + h - ETK_ICONBOX_SCROLL_MARGIN)
      {
         grid->vscrolling_speed = ETK_ICONBOX_MAX_SCROLL_SPEED / ETK_ICONBOX_MAX_SCROLL_DISTANCE *
            ETK_MIN(event->cur.canvas.y - (y + h - ETK_ICONBOX_SCROLL_MARGIN), ETK_ICONBOX_MAX_SCROLL_DISTANCE);
         should_scroll = ETK_TRUE;
      }

      if (should_scroll && !grid->scroll_timer)
         grid->scroll_timer = ecore_timer_add(ETK_ICONBOX_SCROLL_DELAY, _etk_iconbox_grid_scroll_cb, grid);
      else if (!should_scroll && grid->scroll_timer)
      {
         ecore_timer_del(grid->scroll_timer);
         grid->scroll_timer = NULL;
      }
   }

   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new icon object for the iconbox */
static void _etk_iconbox_icon_object_add(Etk_Iconbox_Grid *grid)
{
   Evas *evas;
   Etk_Iconbox_Icon_Object *icon_object;

   if (!grid || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(grid))))
      return;

   icon_object = malloc(sizeof(Etk_Iconbox_Icon_Object));
   icon_object->image = NULL;
   icon_object->emblem = NULL;
   icon_object->use_edje = ETK_FALSE;

   icon_object->label = etk_label_new(NULL);
   etk_widget_theme_parent_set(icon_object->label, ETK_WIDGET(grid->iconbox));
   etk_widget_parent_set(icon_object->label, ETK_WIDGET(grid));
   etk_widget_internal_set(icon_object->label, ETK_TRUE);
   etk_widget_repeat_mouse_events_set(icon_object->label, ETK_TRUE);
   etk_label_alignment_set(ETK_LABEL(icon_object->label), 0.0, 0.0);

   grid->icon_objects = evas_list_append(grid->icon_objects, icon_object);
}

/* Deletes an icon object of the iconbox */
static void _etk_iconbox_icon_object_delete(Etk_Iconbox_Grid *grid)
{
   Etk_Iconbox_Icon_Object *icon_object;

   if (!grid || !(icon_object = evas_list_data(grid->icon_objects)))
      return;

   /* Cache the image */
   if (icon_object->image)
   {
      /*etk_widget_member_object_del(ETK_WIDGET(grid), icon_object->image);
      if (icon_object->use_edje)
         etk_cache_edje_object_add(icon_object->image);
      else
         etk_cache_image_object_add(icon_object->image);*/
      evas_object_del(icon_object->image);
   }
   if (icon_object->emblem)
      evas_object_del(icon_object->emblem);

   etk_object_destroy(ETK_OBJECT(icon_object->label));
   free(icon_object);

   grid->icon_objects = evas_list_remove_list(grid->icon_objects, grid->icon_objects);
}

/* Draws the icon according to the icon model */
static void _etk_iconbox_icon_draw(Etk_Iconbox_Icon *icon, Etk_Iconbox_Icon_Object *icon_object, Etk_Iconbox_Model *model, int x, int y, Etk_Bool clip)
{
   Evas *evas;
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Grid *grid;
   Etk_Geometry icon_object_geometry;
   Etk_Geometry label_geometry;
   int icon_w, icon_h;
   float aspect_ratio;
   Etk_Geometry icon_geometry;
   Etk_Geometry emblem_geometry;

   if (!icon || !icon_object || !model || !(iconbox = icon->iconbox) || !(grid = ETK_ICONBOX_GRID(iconbox->grid)))
      return;
   if (!(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(iconbox))))
      return;

   icon_object_geometry.x = x + model->xpadding;
   icon_object_geometry.y = y + model->ypadding;
   icon_object_geometry.w = model->width - 2 * model->xpadding;
   icon_object_geometry.h = model->height - 2 * model->ypadding;
   
   icon_geometry.x = 0;
   icon_geometry.y = 0;
   icon_geometry.w = 0;
   icon_geometry.h = 0;

   /* Render the icon */
   if (icon->filename)
   {
      if (!icon->edje_group)
      {
         //if (!(icon_object->image = etk_cache_image_object_find(evas, icon->filename)))
         {
            icon_object->image = evas_object_image_add(evas);
            evas_object_image_file_set(icon_object->image, icon->filename, NULL);
         }
         icon_object->use_edje = ETK_FALSE;
         evas_object_image_size_get(icon_object->image, &icon_w, &icon_h);
         evas_object_pass_events_set(icon_object->image, 1);
      }
      else
      {
         //if (!(icon_object->image = etk_cache_edje_object_find(evas, icon->filename, icon->edje_group)))
         {
            if (edje_file_group_exists(icon->filename, icon->edje_group))
            {
               icon_object->image = edje_object_add(evas);
               edje_object_file_set(icon_object->image, icon->filename, icon->edje_group);
               icon_object->use_edje = ETK_TRUE;
               edje_object_size_min_get(icon_object->image, &icon_w, &icon_h);
            }
            else
            {
               icon_object->image = evas_object_image_add(evas);
               evas_object_image_file_set(icon_object->image, icon->filename, icon->edje_group);
               icon_object->use_edje = ETK_FALSE;
            }
         }
      }
      if (icon_w <= 0 || icon_h <= 0)
      {
         icon_w = model->icon_width;
         icon_h = model->icon_height;
      }
      etk_widget_member_object_add(iconbox->grid, icon_object->image);

      if (icon->selected)
      {
         evas_object_color_set(icon_object->image, iconbox->selected_icon_color.r, iconbox->selected_icon_color.g,
            iconbox->selected_icon_color.b, iconbox->selected_icon_color.a);
      }
      else
         evas_object_color_set(icon_object->image, 255, 255, 255, 255);
      if (grid->clip && clip)
         evas_object_clip_set(icon_object->image, grid->clip);

      if (model->icon_fill)
      {
         icon_geometry.w = model->icon_width;
         icon_geometry.h = model->icon_height;
      }
      else
      {
         icon_geometry.w = ETK_MIN(icon_w, model->icon_width);
         icon_geometry.h = ETK_MIN(icon_h, model->icon_height);
      }
      if (model->icon_keep_aspect)
      {
         aspect_ratio = (float)icon_w / icon_h;
         if (icon_geometry.w > icon_geometry.h * aspect_ratio)
            icon_geometry.w = icon_geometry.h * aspect_ratio;
         else
            icon_geometry.h = icon_geometry.w / aspect_ratio;
      }
      icon_geometry.x = icon_object_geometry.x + model->icon_x + (model->icon_width - icon_geometry.w) / 2;
      icon_geometry.y = icon_object_geometry.y + model->icon_y + (model->icon_height - icon_geometry.h) / 2;

      evas_object_move(icon_object->image, icon_geometry.x, icon_geometry.y);
      evas_object_resize(icon_object->image, icon_geometry.w, icon_geometry.h);
      if (!icon_object->use_edje)
         evas_object_image_fill_set(icon_object->image, 0, 0, icon_geometry.w, icon_geometry.h);
      evas_object_show(icon_object->image);
   }

   /* Render the emblem */
   if (icon->emblem_edje_group && edje_file_group_exists(icon->emblem_filename, icon->emblem_edje_group))
   {
      icon_object->emblem = edje_object_add(evas);
      edje_object_file_set(icon_object->emblem, icon->emblem_filename, icon->emblem_edje_group);
   }
   else if (icon->emblem_filename)
   {
      icon_object->emblem = evas_object_image_add(evas);
      evas_object_image_file_set(icon_object->emblem, icon->emblem_filename, icon->emblem_edje_group);
   }
   if (icon_object->emblem)
   {
      emblem_geometry.x = emblem_geometry.y = 0;
      emblem_geometry.w = model->emblem_width;
      emblem_geometry.h = model->emblem_height;
      switch (iconbox->emblem_position)
      {
         case ETK_ICONBOX_EMBLEM_POSITION_FULL:
            emblem_geometry.x = icon_geometry.x;
            emblem_geometry.y = icon_geometry.y;
            emblem_geometry.w = icon_geometry.w;
            emblem_geometry.h = icon_geometry.h;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_CC:
            emblem_geometry.x = icon_geometry.x + icon_geometry.w / 4;
            emblem_geometry.y = icon_geometry.y + icon_geometry.h / 4;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_CL:
            emblem_geometry.x = icon_geometry.x;
            emblem_geometry.y = icon_geometry.y + icon_geometry.h / 4;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_CR:
            emblem_geometry.x = icon_geometry.x + icon_geometry.w / 2;
            emblem_geometry.y = icon_geometry.y + icon_geometry.h / 4;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_BL:
            emblem_geometry.x = icon_geometry.x;
            emblem_geometry.y = icon_geometry.y + icon_geometry.h / 2;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_BR:
            emblem_geometry.x = icon_geometry.x + icon_geometry.w / 2;
            emblem_geometry.y = icon_geometry.y + icon_geometry.h / 2;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_BC:
            emblem_geometry.x = icon_geometry.x + icon_geometry.w / 4;
            emblem_geometry.y = icon_geometry.y + icon_geometry.h / 2;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_TR:
            emblem_geometry.x = icon_geometry.x + icon_geometry.w / 2;
            emblem_geometry.y = icon_geometry.y;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_TL:
            emblem_geometry.x = icon_geometry.x;
            emblem_geometry.y = icon_geometry.y;
            break;
         case ETK_ICONBOX_EMBLEM_POSITION_TC:
            emblem_geometry.x = icon_geometry.x + icon_geometry.w / 4;
            emblem_geometry.y = icon_geometry.y;
            break;
        }
      
      if (!icon->emblem_edje_group)
         evas_object_image_fill_set(icon_object->emblem, 0, 0, emblem_geometry.w, emblem_geometry.h);

      if (grid->clip && clip)
         evas_object_clip_set(icon_object->emblem, grid->clip);

      evas_object_pass_events_set(icon_object->emblem, 1);
      evas_object_move(icon_object->emblem, emblem_geometry.x, emblem_geometry.y);
      evas_object_resize(icon_object->emblem, emblem_geometry.w, emblem_geometry.h);
      evas_object_show(icon_object->emblem);
   }
   
   /* Render the label */
   label_geometry.x = icon_object_geometry.x + model->label_x;
   label_geometry.y = icon_object_geometry.y + model->label_y;
   label_geometry.w = model->label_width;
   label_geometry.h = model->label_height;

   etk_label_set(ETK_LABEL(icon_object->label), icon->label);
   if (icon->selected)
      etk_widget_theme_signal_emit(icon_object->label, "etk,state,selected", ETK_FALSE);
   else
      etk_widget_theme_signal_emit(icon_object->label, "etk,state,unselected", ETK_FALSE);

   if (clip && grid->clip)
      etk_widget_clip_set(icon_object->label, grid->clip);
   else
      etk_widget_clip_unset(icon_object->label);
   etk_widget_show(icon_object->label);
   etk_container_child_space_fill(icon_object->label, &label_geometry,
      ETK_FALSE, ETK_FALSE, model->label_xalign, model->label_yalign);
   etk_widget_size_allocate(icon_object->label, label_geometry);
}

/* Draws the selection rectangle and select the icons under the selection rect */
static void _etk_iconbox_grid_selection_rect_update(Etk_Iconbox_Grid *grid)
{
   int x, y;
   Etk_Geometry rect_geometry;
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Model *model;

   if (!grid || !(iconbox = grid->iconbox))
      return;

   rect_geometry.x = ETK_MIN(grid->selection_orig_x, grid->selection_mouse_x);
   rect_geometry.y = ETK_MIN(grid->selection_orig_y, grid->selection_mouse_y);
   rect_geometry.w = abs(grid->selection_orig_x - grid->selection_mouse_x);
   rect_geometry.h = abs(grid->selection_orig_y - grid->selection_mouse_y);

   /* Select the icons */
   if ((model = iconbox->current_model))
   {
      Etk_Iconbox_Icon *icon;
      int first_col, last_col;
      int first_row, last_row;
      int i, j, k;
      int icon_id;
      Etk_Bool need_redraw = ETK_FALSE;

      first_col = (rect_geometry.x + (model->width - model->icon_x - model->icon_width)) / model->width;
      last_col = (rect_geometry.x + rect_geometry.w - model->icon_x) / model->width;
      first_row = (rect_geometry.y + (model->height - model->icon_y - model->icon_height)) / model->height;
      last_row = (rect_geometry.y + rect_geometry.h - model->icon_y) / model->height;

      first_col = ETK_CLAMP(first_col, 0, grid->num_cols - 1);
      last_col = ETK_CLAMP(last_col, 0, grid->num_cols - 1);
      first_row = ETK_MAX(first_row, 0);
      last_row = ETK_MAX(last_row, 0);

      icon = iconbox->first_icon;
      icon_id = 0;
      for (i = ETK_MIN(grid->selection_first_row, first_row); i <=  ETK_MAX(grid->selection_last_row, last_row); i++)
      {
         for (j = ETK_MIN(grid->selection_first_col, first_col); j <=  ETK_MAX(grid->selection_last_col, last_col); j++)
         {
            for (k = icon_id; k < (i * grid->num_cols + j) && icon; k++)
            {
               icon = icon->next;
               icon_id++;
            }
            if (!icon)
               break;

            if (i >= first_row && i <= last_row && j >= first_col && j <= last_col)
               icon->selected = !icon->was_selected;
            else
               icon->selected = icon->was_selected;
            need_redraw = ETK_TRUE;
         }
      }

      grid->selection_first_col = first_col;
      grid->selection_last_col = last_col;
      grid->selection_first_row = first_row;
      grid->selection_last_row = last_row;

      if (need_redraw)
         etk_widget_redraw_queue(ETK_WIDGET(grid));
   }

   /* Draws the selection rect */
   etk_widget_inner_geometry_get(ETK_WIDGET(grid), &x, &y, NULL, NULL);
   evas_object_move(grid->selection_rect, x + rect_geometry.x - grid->xoffset, y + rect_geometry.y - grid->yoffset);
   evas_object_resize(grid->selection_rect, rect_geometry.w, rect_geometry.h);
   evas_object_show(grid->selection_rect);
   etk_widget_member_object_raise(ETK_WIDGET(grid), grid->selection_rect);
}

/* Scrolls the grid when the selection rect is being dragged and the mouse is outside the edges of the grid */
static int _etk_iconbox_grid_scroll_cb(void *data)
{
   Etk_Iconbox_Grid *grid;
   Etk_Iconbox *iconbox;
   Etk_Range *hrange, *vrange;

   if (!(grid = ETK_ICONBOX_GRID(data)) || !(iconbox = grid->iconbox))
      return 1;

   hrange = etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(iconbox->scrolled_view));
   vrange = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(iconbox->scrolled_view));

   etk_range_value_set(hrange, etk_range_value_get(hrange) + grid->hscrolling_speed);
   etk_range_value_set(vrange, etk_range_value_get(vrange) + grid->vscrolling_speed);

   return 1;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Iconbox
 *
 * @image html widgets/iconbox.png
 * The iconbox is based on a notion of "model". The icon view is describes by a model which is fully customizable:
 * the model controls the position and the size of the cell, the icon image and the label of the iconbox's icons. @n
 * A default model is created when the iconbox is created, but you can add your own with etk_iconbox_model_new(),
 * and enable it with etk_iconbox_current_model_set(). @n @n
 * The model's settings can be changed with
 * etk_iconbox_model_geometry_set(), etk_iconbox_model_icon_geometry_set() and etk_iconbox_model_label_geometry_set(). @n @n
 * Once the model is defined, you can start adding and removing icons to the iconbox with etk_iconbox_append(),
 * etk_iconbox_icon_del() and etk_iconbox_clear(). @n
 * You can add an emblem to the icons using etk_iconbox_icon_emblem_file_set() or etk_iconbox_icon_emblem_set_from_stock().
 * And you can change the position of the emblems with etk_iconbox_emblem_position_set() @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Iconbox
 *
 * \par Signals:
 * @signal_name "icon-selected": Emitted when an icon of the iconbox has been selected.
 * If several icons have been selected at the same time (with etk_iconbox_select_all(), or with
 * the selection rectangle), the "icon-selected" signal will only be emitted once, on the last selected icon.
 * @signal_cb Etk_Bool callback(Etk_Iconbox *iconbox, Etk_Iconbox_Icon *icon, void *data)
 * @signal_arg iconbox: the iconbox connected to the callback
 * @signal_arg icon: the icon which has been selected
 * @signal_data
 * \par
 * @signal_name "icon-unselected": Emitted when an icon of the iconbox has been unselected.
 * If several icons have been unselected at the same time (with etk_iconbox_unselect_all(), or with
 * the selection rectangle), the "icon-unselected" signal will only be emitted once, on the last unselected icon.
 * @signal_cb Etk_Bool callback(Etk_Iconbox *iconbox, Etk_Iconbox_Icon *icon, void *data)
 * @signal_arg iconbox: the iconbox connected to the callback
 * @signal_arg icon: the icon which has been unselected
 * @signal_data
 * \par
 * @signal_name "all-selected": Emitted when all the icons of the iconbox have been selected with etk_iconbox_select_all()
 * @signal_cb Etk_Bool callback(Etk_Iconbox *iconbox, void *data)
 * @signal_arg iconbox: the iconbox connected to the callback
 * @signal_data
 * \par
 * @signal_name "all-unselected": Emitted when all the icons of the iconbox have been unselected with etk_iconbox_select_all()
 * @signal_cb Etk_Bool callback(Etk_Iconbox *iconbox, void *data)
 * @signal_arg iconbox: the iconbox connected to the callback
 * @signal_data
 */
