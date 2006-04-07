/** @file etk_iconbox.h */
#ifndef _ETK_ICONBOX_H_
#define _ETK_ICONBOX_H_

#include "etk_widget.h"
#include <Evas.h>
#include <stdarg.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Iconbox Etk_Iconbox
 * @{
 */

/** @brief Gets the type of an iconbox */
#define ETK_ICONBOX_TYPE       (etk_iconbox_type_get())
/** @brief Casts the object to an Etk_Iconbox */
#define ETK_ICONBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_ICONBOX_TYPE, Etk_Iconbox))
/** @brief Check if the object is an Etk_Iconbobx */
#define ETK_IS_ICONBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ICONBOX_TYPE))

/**
 * @struct Etk_Iconbox_Model
 * @brief TODO
 */
struct _Etk_Iconbox_Model
{
   Etk_Iconbox *iconbox;
   
   int width;
   int height;
   int xpadding;
   int ypadding;
   
   int icon_x;
   int icon_y;
   int icon_width;
   int icon_height;
   
   int label_x;
   int label_y;
   int label_width;
   int label_height;
   float label_xalign;
   float label_yalign;
};

/**
 * @struct Etk_Iconbox_Icon
 * @brief An icon of the iconbox
 */
struct _Etk_Iconbox_Icon
{
   Etk_Iconbox *iconbox;

   Etk_Iconbox_Icon *prev;
   Etk_Iconbox_Icon *next;
   
   char *filename;
   char *edje_group;
   char *label;
   
   void *data;
   void (*data_free_cb)(void *data);
};

/**
 * @struct Etk_Iconbox
 * @brief An iconbox is a widget that can displays icons on a grid
 */
struct _Etk_Iconbox
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *scrolled_view;
   Etk_Widget *grid;

   Evas_List *models;
   Etk_Iconbox_Model *current_model;
   Etk_Iconbox_Model *default_model;
   
   int num_icons;
   Etk_Iconbox_Icon *first_icon;
   Etk_Iconbox_Icon *last_icon;

   Etk_Bool frozen;
};

Etk_Type *etk_iconbox_type_get();
Etk_Widget *etk_iconbox_new();

Etk_Iconbox_Model *etk_iconbox_model_new(Etk_Iconbox *iconbox);
void etk_iconbox_model_free(Etk_Iconbox_Model *model);

void etk_iconbox_model_geometry_set(Etk_Iconbox_Model *model, int width, int height, int xpadding, int ypadding);
void etk_iconbox_model_geometry_get(Etk_Iconbox_Model *model, int *width, int *height, int *xpadding, int *ypadding);
void etk_iconbox_model_icon_geometry_set(Etk_Iconbox_Model *model, int icon_x, int icon_y, int icon_width, int icon_height);
void etk_iconbox_model_icon_geometry_get(Etk_Iconbox_Model *model, int *icon_x, int *icon_y, int *icon_width, int *icon_height);
void etk_iconbox_model_label_geometry_set(Etk_Iconbox_Model *model, int label_x, int label_y, int label_width, int label_height, float xalign, float yalign);
void etk_iconbox_model_label_geometry_get(Etk_Iconbox_Model *model, int *label_x, int *label_y, int *label_width, int *label_height, float *xalign, float *yalign);

void etk_iconbox_current_model_set(Etk_Iconbox *iconbox, Etk_Iconbox_Model *model);
Etk_Iconbox_Model *etk_iconbox_current_model_get(Etk_Iconbox *iconbox);

void etk_iconbox_freeze(Etk_Iconbox *iconbox);
void etk_iconbox_thaw(Etk_Iconbox *iconbox);

Etk_Iconbox_Icon *etk_iconbox_append(Etk_Iconbox *iconbox, const char *filename, const char *edje_group, const char *label);
void etk_iconbox_icon_del(Etk_Iconbox_Icon *icon);
void etk_iconbox_clear(Etk_Iconbox *iconbox);
void etk_iconbox_icon_data_set(Etk_Iconbox_Icon *icon, void *data);
void etk_iconbox_icon_data_set_full(Etk_Iconbox_Icon *icon, void *data, void (*free_cb)(void *data));
void *etk_iconbox_icon_data_get(Etk_Iconbox_Icon *icon);

/** @} */

#endif
