/** @file etk_widget.h */
#ifndef _ETK_WIDGET_H_
#define _ETK_WIDGET_H_

#include <Evas.h>
#include "etk_object.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Widget Etk_Widget
 * @brief Etk_Widget is the base class for all the widgets of Etk
 * @{
 */

/** Gets the type of a widget */
#define ETK_WIDGET_TYPE       (etk_widget_type_get())
/** Casts the object to an Etk_Widget */
#define ETK_WIDGET(obj)       (ETK_OBJECT_CAST((obj), ETK_WIDGET_TYPE, Etk_Widget))
/** Check if the object is an Etk_Widget */
#define ETK_IS_WIDGET(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_WIDGET_TYPE))


/**
 * @brief The code corresponding to the error that occured during the last call of
 * etk_widget_swallow_widget() or etk_widget_swallow_object()
 */
typedef enum Etk_Widget_Swallow_Error
{
   ETK_SWALLOW_ERROR_NONE,                  /**< The object has been succesfully swallowed */
   ETK_SWALLOW_ERROR_INCOMPATIBLE_PARENT,   /**< The parent of the widget to swallow was not the swallower-widget */
   ETK_SWALLOW_ERROR_NOT_REALIZED,          /**< The swallower-widget was not realized */
   ETK_SWALLOW_ERROR_NO_PART                /**< The part where to swallow the object has not been found
                                             * in the theme-object of the swallower-widget */
} Etk_Widget_Swallow_Error;

/**
 * @brief @widget The base class for all the widgets of Etk
 * @structinfo
 */
struct Etk_Widget
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;

   Etk_Toplevel *toplevel_parent;
   Etk_Widget *parent;
   /* TODO: get rid of child_properties. Only Etk_Table uses this for now */
   void *child_properties;
   Evas_List *children;
   Evas_List *focus_order;

   char *theme_file;
   char *theme_group;
   char *theme_group_full;
   Etk_Widget *theme_parent;
   Evas_List *theme_children;
   Evas_Object *theme_object;

   Evas_Object *smart_object;
   Evas_Object *event_object;
   Evas_Object *content_object;
   Evas_Object *clip;
   Evas_List *member_objects;
   Evas_List *swallowed_objects;

   struct
   {
      int left, right, top, bottom;
   } inset, padding;
   Etk_Geometry geometry;
   Etk_Geometry inner_geometry;
   Etk_Size theme_min_size;
   Etk_Size requested_size;
   Etk_Size last_calced_size;
   
   struct
   {
      int r, g, b, a;
   } color;
   
   void (*size_request)(Etk_Widget *widget, Etk_Size *size_requisition);
   void (*size_allocate)(Etk_Widget *widget, Etk_Geometry geometry);

   void (*scroll_size_get)(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);
   void (*scroll_margins_get)(Etk_Widget *widget, Etk_Size *margin_size);
   void (*scroll)(Etk_Widget *widget, int x, int y);

   void (*shown_handler)(Etk_Widget *widget);
   void (*entered_handler)(Etk_Widget *widget);
   void (*left_handler)(Etk_Widget *widget);
   void (*focused_handler)(Etk_Widget *widget);
   void (*unfocused_handler)(Etk_Widget *widget);
   void (*enabled_handler)(Etk_Widget *widget);
   void (*disabled_handler)(Etk_Widget *widget);

   unsigned int realized : 1;
   unsigned int swallowed : 1;
   unsigned int visible : 1;
   unsigned int internal : 1;
   unsigned int propagate_color : 1;
   unsigned int repeat_mouse_events : 1;
   unsigned int pass_mouse_events : 1;
   unsigned int has_event_object : 1;
   unsigned int focusable : 1;
   unsigned int focus_on_click : 1;
   unsigned int disabled : 1;
   unsigned int need_size_recalc : 1;
   unsigned int need_redraw : 1;
   unsigned int need_theme_size_recalc : 1;
   unsigned int emit_theme_parent_signals : 1;
};


Etk_Type      *etk_widget_type_get(void);
Etk_Widget    *etk_widget_new(Etk_Type *widget_type, const char *first_property, ...);

void          etk_widget_show(Etk_Widget *widget);
void          etk_widget_show_all(Etk_Widget *widget);
void          etk_widget_hide(Etk_Widget *widget);
void          etk_widget_hide_all(Etk_Widget *widget);
Etk_Bool      etk_widget_is_visible(Etk_Widget *widget);
void          etk_widget_internal_set(Etk_Widget *widget, Etk_Bool internal);
Etk_Bool      etk_widget_internal_get(Etk_Widget *widget);

void          etk_widget_focus(Etk_Widget *widget);
void          etk_widget_unfocus(Etk_Widget *widget);
Etk_Bool      etk_widget_is_focused(Etk_Widget *widget);
void          etk_widget_focusable_set(Etk_Widget *widget, Etk_Bool focusable);
Etk_Bool      etk_widget_focusable_get(Etk_Widget *widget);
void          etk_widget_disabled_set(Etk_Widget *widget, Etk_Bool disabled);
Etk_Bool      etk_widget_disabled_get(Etk_Widget *widget);
void          etk_widget_enter(Etk_Widget *widget);
void          etk_widget_leave(Etk_Widget *widget);
void          etk_widget_raise(Etk_Widget *widget);
void          etk_widget_lower(Etk_Widget *widget);

void          etk_widget_repeat_mouse_events_set(Etk_Widget *widget, Etk_Bool repeat_mouse_events);
Etk_Bool      etk_widget_repeat_mouse_events_get(Etk_Widget *widget);
void          etk_widget_pass_mouse_events_set(Etk_Widget *widget, Etk_Bool pass_mouse_events);
Etk_Bool      etk_widget_pass_mouse_events_get(Etk_Widget *widget);
void          etk_widget_has_event_object_set(Etk_Widget *widget, Etk_Bool has_event_object);
Etk_Bool      etk_widget_has_event_object_get(Etk_Widget *widget);

void          etk_widget_parent_set(Etk_Widget *widget, Etk_Widget *parent);
void          etk_widget_parent_set_full(Etk_Widget *widget, Etk_Widget *parent, Etk_Bool remove_from_container);
Etk_Widget   *etk_widget_parent_get(Etk_Widget *widget);
Etk_Toplevel *etk_widget_toplevel_parent_get(Etk_Widget *widget);
Evas         *etk_widget_toplevel_evas_get(Etk_Widget *widget);

void          etk_widget_theme_set(Etk_Widget *widget, const char *theme_file, const char *theme_group);
void          etk_widget_theme_file_set(Etk_Widget *widget, const char *theme_file);
const char   *etk_widget_theme_file_get(Etk_Widget *widget);
void          etk_widget_theme_group_set(Etk_Widget *widget, const char *theme_group);
const char   *etk_widget_theme_group_get(Etk_Widget *widget);
void          etk_widget_theme_parent_set(Etk_Widget *widget, Etk_Widget *theme_parent);
Etk_Widget   *etk_widget_theme_parent_get(Etk_Widget *widget);

void          etk_widget_color_set(Etk_Widget *widget, int r, int g, int b, int a);
void          etk_widget_color_get(Etk_Widget *widget, int *r, int *g, int *b, int *a);
void          etk_widget_propagate_color_set(Etk_Widget *widget, Etk_Bool propagate_color);
Etk_Bool      etk_widget_propagate_color_get(Etk_Widget *widget);

void          etk_widget_padding_set(Etk_Widget *widget, int left, int right, int top, int bottom);
void          etk_widget_padding_get(Etk_Widget *widget, int *left, int *right, int *top, int *bottom);
void          etk_widget_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h);
void          etk_widget_inner_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h);
void          etk_widget_size_recalc_queue(Etk_Widget *widget);
void          etk_widget_redraw_queue(Etk_Widget *widget);
void          etk_widget_size_request_set(Etk_Widget *widget, int w, int h);
void          etk_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
void          etk_widget_size_request_full(Etk_Widget *widget, Etk_Size *size_requisition, Etk_Bool hidden_has_no_size);
void          etk_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

void          etk_widget_theme_signal_emit(Etk_Widget *widget, const char *signal_name, Etk_Bool size_recalc);
void          etk_widget_theme_part_text_set(Etk_Widget *widget, const char *part_name, const char *text);
int           etk_widget_theme_data_get(Etk_Widget *widget, const char *data_name, const char *format, ...);

Etk_Bool      etk_widget_member_object_add(Etk_Widget *widget, Evas_Object *object);
void          etk_widget_member_object_del(Etk_Widget *widget, Evas_Object *object);
void          etk_widget_member_object_raise(Etk_Widget *widget, Evas_Object *object);
void          etk_widget_member_object_lower(Etk_Widget *widget, Evas_Object *object);
void          etk_widget_member_object_stack_above(Etk_Widget *widget, Evas_Object *object, Evas_Object *above);
void          etk_widget_member_object_stack_below(Etk_Widget *widget, Evas_Object *object, Evas_Object *below);

Etk_Bool      etk_widget_swallow_widget(Etk_Widget *swallower, const char *part, Etk_Widget *to_swallow);
void          etk_widget_unswallow_widget(Etk_Widget *swallower, Etk_Widget *swallowed);
Etk_Bool      etk_widget_is_swallowed(Etk_Widget *widget);
Etk_Bool      etk_widget_swallow_object(Etk_Widget *swallower, const char *part, Evas_Object *object);
void          etk_widget_unswallow_object(Etk_Widget *swallower, Evas_Object *object);
Etk_Widget_Swallow_Error etk_widget_swallow_error_get(void);

void          etk_widget_clip_set(Etk_Widget *widget, Evas_Object *clip);
void          etk_widget_clip_unset(Etk_Widget *widget);
Evas_Object  *etk_widget_clip_get(Etk_Widget *widget);


/* DND - Dead API, to reimplement */
void         etk_widget_dnd_dest_set(Etk_Widget *widget, Etk_Bool on);
Etk_Bool     etk_widget_dnd_dest_get(Etk_Widget *widget);
Evas_List   *etk_widget_dnd_dest_widgets_get(void);
void         etk_widget_dnd_source_set(Etk_Widget *widget, Etk_Bool on);
Etk_Bool     etk_widget_dnd_source_get(Etk_Widget *widget);
void         etk_widget_dnd_drag_widget_set(Etk_Widget *widget, Etk_Widget *drag_widget);
Etk_Widget  *etk_widget_dnd_drag_widget_get(Etk_Widget *widget);  
void         etk_widget_dnd_drag_data_set(Etk_Widget *widget, const char **types, int num_types, void *data, int data_size);
const char **etk_widget_dnd_files_get(Etk_Widget *e, int *num_files);
void         etk_widget_dnd_types_set(Etk_Widget *widget, const char **types, int num);
const char **etk_widget_dnd_types_get(Etk_Widget *widget, int *num);
Etk_Bool     etk_widget_dnd_internal_get(Etk_Widget *widget);
void         etk_widget_dnd_internal_set(Etk_Widget *widget, Etk_Bool on);

/** @} */

#endif
