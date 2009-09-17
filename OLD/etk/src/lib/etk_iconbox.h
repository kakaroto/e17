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

/** @file etk_iconbox.h */
#ifndef _ETK_ICONBOX_H_
#define _ETK_ICONBOX_H_

#include <stdarg.h>

#include <Evas.h>

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - Implement dnd
 * - Support "vertically-filled" iconboxes
 * - Better documentation of model settings
 * - More insertion function (prepend, insert, ...)
 * - Key navigation
 * - focus the grid, the scrolled view, ... when focused
 * - support long labels
 */

/**
 * @defgroup Etk_Iconbox Etk_Iconbox
 * @brief An Etk_Iconbox is a widget that can display icons aligned on a grid
 * @{
 */

/** Gets the type of an iconbox */
#define ETK_ICONBOX_TYPE       (etk_iconbox_type_get())
/** Casts the object to an Etk_Iconbox */
#define ETK_ICONBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_ICONBOX_TYPE, Etk_Iconbox))
/** Check if the object is an Etk_Iconbobx */
#define ETK_IS_ICONBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ICONBOX_TYPE))

extern int ETK_ICONBOX_ICON_SELECTED_SIGNAL;
extern int ETK_ICONBOX_ICON_UNSELECTED_SIGNAL;
extern int ETK_ICONBOX_ALL_SELECTED_SIGNAL;
extern int ETK_ICONBOX_ALL_UNSELECTED_SIGNAL;

/** @brief The different positions of the emblems. Default is top-left corner*/
typedef enum
{
   ETK_ICONBOX_EMBLEM_POSITION_TL,     /**< Emblem in the top-left corner */
   ETK_ICONBOX_EMBLEM_POSITION_TR,     /**< Emblem in the top-right corner */
   ETK_ICONBOX_EMBLEM_POSITION_TC,     /**< Emblem in the top at the center */
   ETK_ICONBOX_EMBLEM_POSITION_BL,     /**< Emblem in the bottom-left corner */
   ETK_ICONBOX_EMBLEM_POSITION_BR,     /**< Emblem in the bottom-right corner */
   ETK_ICONBOX_EMBLEM_POSITION_BC,     /**< Emblem in the bottom at the center */
   ETK_ICONBOX_EMBLEM_POSITION_CC,     /**< Emblem is in the middle of the icon */
   ETK_ICONBOX_EMBLEM_POSITION_CL,     /**< Emblem is in the middle of the left side */
   ETK_ICONBOX_EMBLEM_POSITION_CR,     /**< Emblem is in the middle of the right side */
   ETK_ICONBOX_EMBLEM_POSITION_FULL    /**< Emblem take the same size/position of the icon */
} Etk_Emblem_Position;

/**
 * @brief Describes the model of an iconbox: it includes the position and the size of the cell, of the icon and
 * of the label. An iconbox can have several models, and, at any moment, you can change the current model to
 * change the view
 * @structinfo
 */
struct Etk_Iconbox_Model
{
   /* private: */
   Etk_Iconbox *iconbox;

   int width;
   int height;
   int xpadding;
   int ypadding;

   int icon_x;
   int icon_y;
   int icon_width;
   int icon_height;
   int emblem_width;
   int emblem_height;

   int label_x;
   int label_y;
   int label_width;
   int label_height;
   float label_xalign;
   float label_yalign;

   Etk_Bool icon_fill:1;
   Etk_Bool icon_keep_aspect:1;
};

/**
 * @brief An icon of the iconbox
 * @structinfo
 */
struct Etk_Iconbox_Icon
{
   /* private: */
   Etk_Iconbox *iconbox;

   Etk_Iconbox_Icon *prev;
   Etk_Iconbox_Icon *next;

   char *filename;
   char *edje_group;
   char *emblem_filename;
   char *emblem_edje_group;
   char *label;

   void *data;
   void (*data_free_cb)(void *data);

   Etk_Bool was_selected:1;
   Etk_Bool selected:1;
};

/**
 * @brief @widget A widget that can display icons aligned on a grid
 * @structinfo
 */
struct Etk_Iconbox
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *scrolled_view;
   Etk_Widget *grid;

   Eina_List *models;
   Etk_Iconbox_Model *current_model;
   Etk_Emblem_Position emblem_position;

   int num_icons;
   Etk_Iconbox_Icon *first_icon;
   Etk_Iconbox_Icon *last_icon;

   Etk_Color selected_icon_color;
   Etk_Bool frozen:1;
};


Etk_Type          *etk_iconbox_type_get(void);
Etk_Widget        *etk_iconbox_new(void);

Etk_Iconbox_Model *etk_iconbox_model_new(Etk_Iconbox *iconbox);
void               etk_iconbox_model_free(Etk_Iconbox_Model *model);
void               etk_iconbox_current_model_set(Etk_Iconbox *iconbox, Etk_Iconbox_Model *model);
Etk_Iconbox_Model *etk_iconbox_current_model_get(Etk_Iconbox *iconbox);
void               etk_iconbox_emblem_position_set(Etk_Iconbox *iconbox, Etk_Emblem_Position pos);
Etk_Emblem_Position etk_iconbox_emblem_position_get(Etk_Iconbox *iconbox);

void               etk_iconbox_model_geometry_set(Etk_Iconbox_Model *model, int width, int height, int xpadding, int ypadding);
void               etk_iconbox_model_geometry_get(Etk_Iconbox_Model *model, int *width, int *height, int *xpadding, int *ypadding);
void               etk_iconbox_model_icon_geometry_set(Etk_Iconbox_Model *model, int x, int y, int width, int height, Etk_Bool fill, Etk_Bool keep_aspect_ratio);
void               etk_iconbox_model_icon_geometry_get(Etk_Iconbox_Model *model, int *x, int *y, int *width, int *height, Etk_Bool *fill, Etk_Bool *keep_aspect_ratio);
void               etk_iconbox_model_label_geometry_set(Etk_Iconbox_Model *model, int x, int y, int width, int height, float xalign, float yalign);
void               etk_iconbox_model_label_geometry_get(Etk_Iconbox_Model *model, int *x, int *y, int *width, int *height, float *xalign, float *yalign);

void               etk_iconbox_freeze(Etk_Iconbox *iconbox);
void               etk_iconbox_thaw(Etk_Iconbox *iconbox);

Etk_Iconbox_Icon  *etk_iconbox_append(Etk_Iconbox *iconbox, const char *filename, const char *edje_group, const char *label);
void               etk_iconbox_icon_del(Etk_Iconbox_Icon *icon);
void               etk_iconbox_clear(Etk_Iconbox *iconbox);
Etk_Iconbox_Icon  *etk_iconbox_icon_get_at_xy(Etk_Iconbox *iconbox, int x, int y, Etk_Bool over_cell, Etk_Bool over_icon, Etk_Bool over_label);
Etk_Iconbox_Icon  *etk_iconbox_icon_get_by_label(Etk_Iconbox *iconbox, const char *label);
Etk_Iconbox_Icon  *etk_iconbox_icon_get_by_data(Etk_Iconbox *iconbox, void *data);
Eina_List         *etk_iconbox_icon_get_selected(Etk_Iconbox *iconbox);
Etk_Iconbox_Icon  *etk_iconbox_icon_get_first_selected(Etk_Iconbox *iconbox);

void               etk_iconbox_icon_file_set(Etk_Iconbox_Icon *icon, const char *filename, const char *edje_group);
void               etk_iconbox_icon_file_get(Etk_Iconbox_Icon *icon, const char **filename, const char **edje_group);
void               etk_iconbox_icon_emblem_set_from_stock(Etk_Iconbox_Icon *icon, const char *stock_name);
void               etk_iconbox_icon_emblem_file_set(Etk_Iconbox_Icon *icon, const char *filename, const char *edje_group);
void               etk_iconbox_icon_emblem_file_get(Etk_Iconbox_Icon *icon, const char **filename, const char **edje_group);
void               etk_iconbox_icon_label_set(Etk_Iconbox_Icon *icon, const char *label);
const char        *etk_iconbox_icon_label_get(Etk_Iconbox_Icon *icon);

void               etk_iconbox_icon_data_set(Etk_Iconbox_Icon *icon, void *data);
void               etk_iconbox_icon_data_set_full(Etk_Iconbox_Icon *icon, void *data, void (*free_cb)(void *data));
void              *etk_iconbox_icon_data_get(Etk_Iconbox_Icon *icon);

void               etk_iconbox_select_all(Etk_Iconbox *iconbox);
void               etk_iconbox_unselect_all(Etk_Iconbox *iconbox);
void               etk_iconbox_icon_select(Etk_Iconbox_Icon *icon);
void               etk_iconbox_icon_unselect(Etk_Iconbox_Icon *icon);
Etk_Bool           etk_iconbox_is_selected(Etk_Iconbox_Icon *icon);

Etk_Scrolled_View *etk_iconbox_scrolled_view_get(Etk_Iconbox *iconbox);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
