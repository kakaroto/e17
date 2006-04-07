/** @file etk_iconbox.c */
#include "etk_iconbox.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_scrolled_view.h"
#include "etk_label.h"
#include "etk_cache.h"
#include "etk_theme.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Iconbox
 * @{
 */

#define ETK_ICONBOX_GRID_TYPE       (_etk_iconbox_grid_type_get())
#define ETK_ICONBOX_GRID(obj)       (ETK_OBJECT_CAST((obj), ETK_ICONBOX_GRID_TYPE, Etk_Iconbox_Grid))
#define ETK_IS_ICONBOX_GRID(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ICONBOX_GRID_TYPE))

typedef struct _Etk_Iconbox_Grid
{
   /* Inherit form Etk_Widget */
   Etk_Widget widget;

   Etk_Iconbox *iconbox;
   int xoffset;
   int yoffset;
   
   Evas_List *icon_objects;
   Evas_Object *clip;
   
   Evas_Object *selection_rect;
   Etk_Bool selection_started;
   int selection_orig_x;
   int selection_orig_y;
   int selection_mouse_x;
   int selection_mouse_y;
} Etk_Iconbox_Grid;

typedef struct _Etk_Iconbox_Icon_Object
{
   Evas_Object *image;
   Etk_Widget *label;
} Etk_Iconbox_Icon_Object;

enum _Etk_Tree_Signal_Id
{
   /*ETK_TREE_ROW_SELECTED_SIGNAL,
   ETK_TREE_ROW_UNSELECTED_SIGNAL,
   ETK_TREE_ROW_CLICKED_SIGNAL,
   ETK_TREE_ROW_ACTIVATED_SIGNAL,
   ETK_TREE_ROW_EXPANDED_SIGNAL,
   ETK_TREE_ROW_COLLAPSED_SIGNAL,
   ETK_TREE_ROW_MOUSE_IN_SIGNAL,
   ETK_TREE_ROW_MOUSE_OUT_SIGNAL,
   ETK_TREE_ROW_MOUSE_MOVE_SIGNAL,     
   ETK_TREE_SELECT_ALL_SIGNAL,
   ETK_TREE_UNSELECT_ALL_SIGNAL,*/
   ETK_ICONBOX_NUM_SIGNALS
};

static void _etk_iconbox_constructor(Etk_Iconbox *iconbox);
static void _etk_iconbox_destructor(Etk_Iconbox *iconbox);
//static void _etk_tree_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
//static void _etk_tree_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_iconbox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static Etk_Type *_etk_iconbox_grid_type_get();
static void _etk_iconbox_grid_constructor(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_iconbox_grid_scroll(Etk_Widget *widget, int x, int y);
static void _etk_iconbox_grid_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);
static void _etk_iconbox_grid_realize_cb(Etk_Object *object, void *data);
static void _etk_iconbox_grid_unrealize_cb(Etk_Object *object, void *data);
static void _etk_iconbox_grid_mouse_down_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_iconbox_grid_mouse_up_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_iconbox_grid_mouse_move_cb(Etk_Object *object, void *event_info, void *data);

static void _etk_iconbox_icon_object_add(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_icon_object_delete(Etk_Iconbox_Grid *grid);
static void _etk_iconbox_icon_draw(Etk_Iconbox_Icon *icon, Etk_Iconbox_Icon_Object *icon_object, Etk_Iconbox_Model *model, int x, int y);
static void _etk_iconbox_grid_selection_rect_update(Etk_Iconbox_Grid *grid);

static Etk_Signal *_etk_iconbox_signals[ETK_ICONBOX_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Iconbox
 * @return Returns the type on an Etk_Iconbox
 */
Etk_Type *etk_iconbox_type_get()
{
   static Etk_Type *iconbox_type = NULL;

   if (!iconbox_type)
   {
      iconbox_type = etk_type_new("Etk_Iconbox", ETK_WIDGET_TYPE, sizeof(Etk_Iconbox), ETK_CONSTRUCTOR(_etk_iconbox_constructor), ETK_DESTRUCTOR(_etk_iconbox_destructor));

      /*_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL] = etk_signal_new("row_selected", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL] = etk_signal_new("row_unselected", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_CLICKED_SIGNAL] = etk_signal_new("row_clicked", tree_type, -1, etk_marshaller_VOID__POINTER_POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_ACTIVATED_SIGNAL] = etk_signal_new("row_activated", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_EXPANDED_SIGNAL] = etk_signal_new("row_expaned", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_COLLAPSED_SIGNAL] = etk_signal_new("row_collapsed", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_MOUSE_IN_SIGNAL] = etk_signal_new("row_mouse_in", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_MOUSE_OUT_SIGNAL] = etk_signal_new("row_mouse_out", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_MOUSE_MOVE_SIGNAL] = etk_signal_new("row_mouse_move", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);      
      _etk_tree_signals[ETK_TREE_SELECT_ALL_SIGNAL] = etk_signal_new("select_all", tree_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_tree_signals[ETK_TREE_UNSELECT_ALL_SIGNAL] = etk_signal_new("unselect_all", tree_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);

      etk_type_property_add(tree_type, "mode", ETK_TREE_MODE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TREE_MODE_LIST));
      etk_type_property_add(tree_type, "multiple_select", ETK_TREE_MULTIPLE_SELECT_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "headers_visible", ETK_TREE_HEADERS_VISIBLE_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "row_height", ETK_TREE_ROW_HEIGHT_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(24));
      
      tree_type->property_set = _etk_tree_property_set;
      tree_type->property_get = _etk_tree_property_get;*/
   }

   return iconbox_type;
}

/**
 * @brief Creates a new iconbox
 * @return Returns the new iconbox widget
 */
Etk_Widget *etk_iconbox_new()
{
   return etk_widget_new(ETK_ICONBOX_TYPE, "theme_group", "iconbox", "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
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
   model->height = 70;
   model->xpadding = 5;
   model->ypadding = 4;
   
   model->icon_x = 26;
   model->icon_y = 0;
   model->icon_width = 48;
   model->icon_height = 48;
   
   model->label_x = 0;
   model->label_y = 50;
   model->label_width = 100;
   model->label_height = 12;
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
      iconbox->current_model = evas_list_data(evas_list_last(iconbox->models));
   if (iconbox->default_model == model)
      iconbox->default_model = NULL;
   
   free(model);
}

/**
 * @brief Sets the global geometry of the iconbox model
 * @param model an iconbox model
 * @param width the width of the modell (min = 10)
 * @param height the height of the model (min = 10)
 * @param xpadding the horizontal padding of the model (min = 0)
 * @param yapdding the vertical padding of the model (min = 0)
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
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(model->iconbox->grid), NULL);
      etk_widget_redraw_queue(model->iconbox->grid);
   }
}

/**
 * @brief Gets the global geometry of the iconbox model
 * @param model an iconbox model
 * @param width the location where to store the width of the model
 * @param height the location where to store the height of the model
 * @param xpadding the location where to store the horizontal padding of the model
 * @param ypadding the location where to store the vertical padding of the model
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
 * @param icon_x the x position of the icon image of the model (min = 0)
 * @param icon_y the y position of the icon image of the model (min = 0)
 * @param icon_width the width of the icon image of the model (min = 10)
 * @param icon_height the height of the icon image of the model (min = 10)
 * @note the x/y positions are relative to the inner top left corner of the icon model: @n
 * i.e. the icon image will be in fact put at (model->xpadding + model->icon_x, model->ypadding + model->icon_y)
 */
void etk_iconbox_model_icon_geometry_set(Etk_Iconbox_Model *model, int icon_x, int icon_y, int icon_width, int icon_height)
{
   if (!model)
      return;
   
   model->icon_x = ETK_MAX(icon_x, 0);
   model->icon_y = ETK_MAX(icon_y, 0);
   model->icon_width = ETK_MAX(icon_width, 10);
   model->icon_height = ETK_MAX(icon_height, 10);
   
   if (model->iconbox && model->iconbox->current_model == model)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(model->iconbox->grid), NULL);
      etk_widget_redraw_queue(model->iconbox->grid);
   }
}

/**
 * @brief Gets the icon geometry of the iconbox model
 * @param model an iconbox model
 * @param icon_x the location where to store x position of the icon image
 * @param icon_y the location where to store y position of the icon image
 * @param icon_width the location where to store the width of the icon image
 * @param icon_height the location where to store the height of the icon image
 */
void etk_iconbox_model_icon_geometry_get(Etk_Iconbox_Model *model, int *icon_x, int *icon_y, int *icon_width, int *icon_height)
{
   if (icon_x)
      *icon_x = model ? model->icon_x : 0;
   if (icon_y)
      *icon_y = model ? model->icon_y : 0;
   if (icon_width)
      *icon_width = model ? model->icon_width : 0;
   if (icon_height)
      *icon_height = model ? model->icon_height : 0;
}

/**
 * @brief Sets the label geometry of the iconbox model
 * @param model an iconbox model
 * @param label_x the x position of the label of the model (min = 0)
 * @param label_y the y position of the label of the model (min = 0)
 * @param label_width the width of the label of the model (min = 10)
 * @param label_height the height of the label of the model (min = 5)
 * @param xalign the horizontal alignment of the label (from 0.0 to 1.0)
 * @param yalign the vertical alignment of the label
 * @note the x/y positions are relative to the inner top left corner of the icon model: @n
 * i.e. the label will be in fact put at (model->xpadding + model->label_x, model->ypadding + model->label_y)
 */
void etk_iconbox_model_label_geometry_set(Etk_Iconbox_Model *model, int label_x, int label_y, int label_width, int label_height, float xalign, float yalign)
{
   if (!model)
      return;
   
   model->label_x = ETK_MAX(label_x, 0);
   model->label_y = ETK_MAX(label_y, 0);
   model->label_width = ETK_MAX(label_width, 10);
   model->label_height = ETK_MAX(label_height, 5);
   model->label_xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   model->label_yalign = ETK_CLAMP(yalign, 0.0, 1.0);
   
   if (model->iconbox && model->iconbox->current_model == model)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(model->iconbox->grid), NULL);
      etk_widget_redraw_queue(model->iconbox->grid);
   }
}

/**
 * @brief Gets the label geometry of the iconbox model
 * @param model an iconbox model
 * @param label_x the location where to store x position of the label
 * @param label_y the location where to store y position of the label
 * @param label_width the location where to store width of the label
 * @param label_height the location where to store height of the label
 * @param xalign the location where to store horizontal alignment of the label
 * @param yalign the location where to store vertical alignment of the label
 */
void etk_iconbox_model_label_geometry_get(Etk_Iconbox_Model *model, int *label_x, int *label_y, int *label_width, int *label_height, float *xalign, float *yalign)
{
   if (label_x)
      *label_x = model ? model->label_x : 0;
   if (label_y)
      *label_y = model ? model->label_y : 0;
   if (label_width)
      *label_width = model ? model->label_width : 0;
   if (label_height)
      *label_height = model ? model->label_height : 0;
   if (xalign)
      *xalign = model ? model->label_xalign : 0.0;
   if (yalign)
      *yalign = model ? model->label_yalign : 0.0;
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
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(iconbox->grid), NULL);
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
 * @brief Freezes the iconbox: it will not be updated until it is thawed @n
 * This function is useful when you want to add a lot of icons quickly.
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
   
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(iconbox->grid), NULL);
   etk_widget_redraw_queue(iconbox->grid);
   iconbox->frozen = ETK_FALSE;
}

/**
 * @brief Appends a new icon to the iconbox
 * @param iconbox a iconbox
 * @param filename the filename of the image to use for the icon. It can be either an image or an edje file
 * @param edje_group the edje group to use for the icon. If NULL, @a filename will be considerated as a normal image file
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
   icon->label = label ? strdup(label) : NULL;
   icon->data = NULL;
   icon->data_free_cb = NULL;
   
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
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(iconbox->grid), NULL);
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
   
   if (iconbox->first_icon)
      iconbox->first_icon = icon->next;
   if (iconbox->last_icon)
      iconbox->last_icon = icon->prev;
   if (icon->prev)
      icon->prev->next = icon->next;
   if (icon->next)
      icon->next->prev = icon->prev;
   iconbox->num_icons--;
   
   free(icon->filename);
   free(icon->edje_group);
   free(icon->label);
   
   if (icon->data && icon->data_free_cb)
      icon->data_free_cb(icon->data);
   
   free(icon);
   
   if (!iconbox->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(iconbox->grid), NULL);
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
   
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(iconbox->grid), NULL);
   etk_widget_redraw_queue(iconbox->grid);
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
   etk_widget_visibility_locked_set(iconbox->scrolled_view, ETK_TRUE);
   etk_widget_repeat_mouse_events_set(iconbox->scrolled_view, ETK_TRUE);
   etk_widget_parent_set(iconbox->scrolled_view, ETK_WIDGET(iconbox));
   etk_widget_show(iconbox->scrolled_view);
   
   iconbox->grid = etk_widget_new(ETK_ICONBOX_GRID_TYPE, "theme_group", "grid", "theme_parent", iconbox,
      "has_event_object", ETK_TRUE, "repeat_mouse_events", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   ETK_ICONBOX_GRID(iconbox->grid)->iconbox = iconbox;
   etk_container_add(ETK_CONTAINER(iconbox->scrolled_view), iconbox->grid);
   etk_widget_show(iconbox->grid);

   iconbox->models = NULL;
   iconbox->default_model = etk_iconbox_model_new(iconbox);
   iconbox->current_model = iconbox->default_model;
   
   iconbox->num_icons = 0;
   iconbox->first_icon = NULL;
   iconbox->last_icon = NULL;
      
   iconbox->frozen = ETK_FALSE;
   
   ETK_WIDGET(iconbox)->size_allocate = _etk_iconbox_size_allocate;
   
   /*etk_signal_connect("focus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_unfocus_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_key_down_cb), NULL);*/
   
}

/* Destroys the iconbox */
static void _etk_iconbox_destructor(Etk_Iconbox *iconbox)
{
   if (!iconbox)
      return;
   
   etk_iconbox_clear(iconbox);
   
   iconbox->default_model = NULL;
   iconbox->current_model = NULL;
   while (iconbox->models)
      etk_iconbox_model_free(iconbox->models->data);
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
      iconbox_type = etk_type_new("Etk_Iconbox_Grid", ETK_WIDGET_TYPE, sizeof(Etk_Iconbox_Grid), ETK_CONSTRUCTOR(_etk_iconbox_grid_constructor), NULL);

   return iconbox_type;
}

/* Initializes the default values of the iconbox grid */
static void _etk_iconbox_grid_constructor(Etk_Iconbox_Grid *grid)
{
   if (!grid)
      return;

   grid->xoffset = 0;
   grid->yoffset = 0;
   grid->icon_objects = NULL;
   grid->clip = NULL;
   grid->selection_rect = NULL;
   grid->selection_started = ETK_FALSE;
   grid->selection_orig_x = 0;
   grid->selection_orig_y = 0;
   grid->selection_mouse_x = 0;
   grid->selection_mouse_y = 0;
   
   ETK_WIDGET(grid)->size_allocate = _etk_iconbox_grid_size_allocate;
   ETK_WIDGET(grid)->scroll = _etk_iconbox_grid_scroll;
   ETK_WIDGET(grid)->scroll_size_get = _etk_iconbox_grid_scroll_size_get;
   etk_signal_connect("realize", ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_unrealize_cb), NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_mouse_down_cb), NULL);
   etk_signal_connect("mouse_up", ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_mouse_up_cb), NULL);
   etk_signal_connect("mouse_move", ETK_OBJECT(grid), ETK_CALLBACK(_etk_iconbox_grid_mouse_move_cb), NULL);
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
      /* TODO: Do we always need the "+ 1" ?? */
      num_rows = ETK_MAX((geometry.h + iconbox->current_model->height - 1) / iconbox->current_model->height + 1, 1);
   }
   num_visible_icons = ETK_MIN(num_cols * num_rows, iconbox->num_icons);
   num_icons_to_add = num_visible_icons - evas_list_count(grid->icon_objects);
   
   /* Cache current icons */
   for (l = grid->icon_objects; l; l = l->next)
   {
      icon_object = l->data;
      etk_widget_member_object_del(iconbox->grid, icon_object->image);
      etk_cache_edje_object_add(icon_object->image);
      icon_object->image = NULL;
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
   
   /* Move/resize the clip */
   if (iconbox->current_model && num_visible_icons > 0)
   {
      evas_object_move(grid->clip, geometry.x, geometry.y);
      evas_object_resize(grid->clip, geometry.w, geometry.h);
      evas_object_show(grid->clip);
   }
   else
      evas_object_hide(grid->clip);
   
   
   /* Draw the icons */
   if (iconbox->current_model)
   {
      first_icon_id = (grid->yoffset / iconbox->current_model->height) * num_cols +
         (grid->xoffset / iconbox->current_model->width);
      for (i = 0, icon = iconbox->first_icon; i < first_icon_id && icon; i++, icon = icon->next);
      
      l = grid->icon_objects;
      y = -(grid->yoffset % iconbox->current_model->height) + geometry.y;
      
      for (i = 0; i < num_rows; i++)
      {
         x = -(grid->xoffset % iconbox->current_model->width) + geometry.x;
            
         for (j = 0; j < num_cols && icon && l; j++, icon = icon->next, l = l->next)
         {
            icon_object = l->data;
            _etk_iconbox_icon_draw(icon, icon_object, iconbox->current_model, x, y);
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
      etk_widget_hide(icon_object->label);
   }
   
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
 * Iconbox Grid
 **************************/

/* Called when the iconbox grid is realized */
static void _etk_iconbox_grid_realize_cb(Etk_Object *object, void *data)
{
   Evas *evas;
   Etk_Iconbox_Grid *grid;
   
   if (!(grid = ETK_ICONBOX_GRID(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(grid))))
      return;
   
   grid->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(grid), grid->clip);
   
   grid->selection_rect = etk_theme_object_load_from_parent(evas, ETK_WIDGET(grid->iconbox), NULL, "selection");
   evas_object_clip_set(grid->selection_rect, grid->clip);
   etk_widget_member_object_add(ETK_WIDGET(grid), grid->selection_rect);
}

/* Called when the iconbox grid is unrealized */
static void _etk_iconbox_grid_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Iconbox_Grid *grid;
   
   if (!(grid = ETK_ICONBOX_GRID(object)))
      return;
   
   while (grid->icon_objects)
      _etk_iconbox_icon_object_delete(grid);
   grid->clip = NULL;
}

/* Called when the mouse presses the iconbox */
static void _etk_iconbox_grid_mouse_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Iconbox_Grid *grid;
   Etk_Event_Mouse_Up_Down *down_event;
   
   if (!(grid = ETK_ICONBOX_GRID(object)) || !(down_event = event_info))
      return;
   
   grid->selection_started = ETK_TRUE;
   grid->selection_orig_x = down_event->widget.x + grid->xoffset;
   grid->selection_orig_y = down_event->widget.y + grid->yoffset;
   grid->selection_mouse_x = grid->selection_orig_x;
   grid->selection_mouse_y = grid->selection_orig_y;
}

/* Called when the mouse releases the iconbox */
static void _etk_iconbox_grid_mouse_up_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Iconbox_Grid *grid;
   Etk_Event_Mouse_Up_Down *up_event;
   
   if (!(grid = ETK_ICONBOX_GRID(object)) || !(up_event = event_info))
      return;
   
   grid->selection_started = ETK_FALSE;
   evas_object_hide(grid->selection_rect);
}

/* Called when the mouse moves over the iconbox */
static void _etk_iconbox_grid_mouse_move_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Iconbox_Grid *grid;
   Etk_Event_Mouse_Move *move_event;
   
   if (!(grid = ETK_ICONBOX_GRID(object)) || !(move_event = event_info))
      return;
   
   if (grid->selection_started)
   {
      grid->selection_mouse_x = move_event->cur.widget.x + grid->xoffset;
      grid->selection_mouse_y = move_event->cur.widget.y + grid->yoffset;
      _etk_iconbox_grid_selection_rect_update(grid);
   }
}


/**************************
 * Iconbox
 **************************/

/* TODO: events, focus */

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
   icon_object->label = etk_label_new(NULL);
   etk_widget_parent_set(icon_object->label, ETK_WIDGET(grid));
   if (grid->clip)
      etk_widget_clip_set(icon_object->label, grid->clip);
   
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
      etk_widget_member_object_del(ETK_WIDGET(grid), icon_object->image);
      etk_cache_image_object_add(icon_object->image);
   }
   
   etk_object_destroy(ETK_OBJECT(icon_object->label));
   free(icon_object);
   
   grid->icon_objects = evas_list_remove_list(grid->icon_objects, grid->icon_objects);
}

/* Draws the icon according to the icon model */
static void _etk_iconbox_icon_draw(Etk_Iconbox_Icon *icon, Etk_Iconbox_Icon_Object *icon_object, Etk_Iconbox_Model *model, int x, int y)
{
   Evas *evas;
   Etk_Iconbox *iconbox;
   Etk_Iconbox_Grid *grid;
   Etk_Geometry icon_object_geometry;
   Etk_Geometry label_geometry;
   
   if (!icon || !icon_object || !model || !(iconbox = icon->iconbox) || !(grid = ETK_ICONBOX_GRID(iconbox->grid)))
      return;
   if (!(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(iconbox))))
      return;
   
   icon_object_geometry.x = x + model->xpadding;
   icon_object_geometry.y = y + model->ypadding;
   icon_object_geometry.w = model->width - 2 * model->xpadding;
   icon_object_geometry.h = model->height - 2 * model->ypadding;
   
   if (icon->filename)
   {
      if (!icon->edje_group)
      {
         if (!(icon_object->image = etk_cache_image_object_find(evas, icon->filename)))
         {
            icon_object->image = evas_object_image_add(evas);
            evas_object_image_file_set(icon_object->image, icon->filename, NULL);
         }
      }
      else
      {
         if (!(icon_object->image = etk_cache_edje_object_find(evas, icon->filename, icon->edje_group)))
         {
            icon_object->image = edje_object_add(evas);
            edje_object_file_set(icon_object->image, icon->filename, icon->edje_group);
         }
      }
      
      if (icon_object->image)
      {
         etk_widget_member_object_add(iconbox->grid, icon_object->image);
         if (grid->clip)
            evas_object_clip_set(icon_object->image, grid->clip);
         evas_object_show(icon_object->image);
         evas_object_move(icon_object->image, icon_object_geometry.x + model->icon_x, icon_object_geometry.y + model->icon_y);
         evas_object_resize(icon_object->image, model->icon_width, model->icon_height);
         evas_object_image_fill_set(icon_object->image, 0, 0, model->icon_width, model->icon_height);
      }
   }
   
   label_geometry.x = icon_object_geometry.x + model->label_x;
   label_geometry.y = icon_object_geometry.y + model->label_y;
   label_geometry.w = model->label_width;
   label_geometry.h = model->label_height;
   etk_label_alignment_set(ETK_LABEL(icon_object->label), model->label_xalign, model->label_yalign);
   etk_label_set(ETK_LABEL(icon_object->label), icon->label);
   etk_widget_show(icon_object->label);
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
      int first_col, last_col;
      int first_row, last_row;
      
      first_col = (rect_geometry.x + model->icon_x) / model->width;
      last_col = (rect_geometry.x + rect_geometry.w - model->icon_x) / model->width;
      first_row = (rect_geometry.y + model->icon_y) / model->height;
      last_row = (rect_geometry.y + rect_geometry.h - model->icon_y) / model->height;
      printf("%d %d / %d %d\n", first_col, last_col, first_row, last_row);
   };
   
   /* Draws the selection rect */
   etk_widget_inner_geometry_get(ETK_WIDGET(grid), &x, &y, NULL, NULL);
   evas_object_move(grid->selection_rect, x + rect_geometry.x - grid->xoffset, y + rect_geometry.y - grid->yoffset);
   evas_object_resize(grid->selection_rect, rect_geometry.w, rect_geometry.h);
   evas_object_show(grid->selection_rect);
   etk_widget_member_object_raise(ETK_WIDGET(grid), grid->selection_rect);
}
