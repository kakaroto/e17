/** @file etk_widget.h */
#ifndef _ETK_WIDGET_H_
#define _ETK_WIDGET_H_

#include <Evas.h>
#include <stdarg.h>
#include "etk_object.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Widget Etk_Widget
 * @{
 */

/** @brief Gets the type of a widget */
#define ETK_WIDGET_TYPE       (etk_widget_type_get())
/** @brief Casts the object to an Etk_Widget */
#define ETK_WIDGET(obj)       (ETK_OBJECT_CAST((obj), ETK_WIDGET_TYPE, Etk_Widget))
/** @brief Check if the object is an Etk_Widget */
#define ETK_IS_WIDGET(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_WIDGET_TYPE))

/**
 * @struct Etk_Event_Mouse_In_out
 * @brief The event sent as argument with the signals "mouse_in" and "mouse_out"
 */
struct _Etk_Event_Mouse_In_Out
{
   /* Button pressed mask, Bits set to 1 are buttons currently pressed (bit 0 = mouse button 1, bit 1 = mouse button 2 etc.) */
   int buttons;
   struct {
      int x, y;
   } canvas, widget;
   Evas_Modifier *modifiers;
   Evas_Lock *locks;
   unsigned int timestamp;
};

/**
 * @struct Etk_Event_Mouse_Up_Down
 * @brief The event sent as argument with the signals "mouse_down", "mouse_up" and "mouse_clicked"
 */
struct _Etk_Event_Mouse_Up_Down
{
   /* Mouse button number that was raised (1 - 32) */
   int button;
   struct {
      int x, y;
   } canvas, widget;
   Evas_Modifier *modifiers;
   Evas_Lock *locks;
   Evas_Button_Flags flags;
   unsigned int timestamp;
};

/**
 * @struct Etk_Event_Mouse_Move
 * @brief The event sent as argument with the signal "mouse_move"
 */
struct _Etk_Event_Mouse_Move
{
   /* Button pressed mask, Bits set to 1 are buttons currently pressed (bit 0 = mouse button 1, bit 1 = mouse button 2 etc.) */
   int buttons;
   struct {
      struct {
         int x, y;
      } canvas, widget;
   } cur, prev;
   Evas_Modifier *modifiers;
   Evas_Lock *locks;
   unsigned int timestamp;
};

/**
 * @struct Etk_Event_Mouse_Wheel
 * @brief The event sent as argument with the signal "mouse_wheel"
 */
struct _Etk_Event_Mouse_Wheel
{
   /* 0 = default up/down wheel */
   int direction;
   /* ...,-2,-1 = down, 1,2,... = up */
   int z;
   struct {
      int x, y;
   } canvas, widget;
   Evas_Modifier *modifiers;
   Evas_Lock *locks;
   Evas_Button_Flags flags;
   unsigned int timestamp;
};

/**
 * @struct Etk_Event_Key_Up_Down
 * @brief The event sent as argument with the signals "key_down" and "key_up"
 */
struct _Etk_Event_Key_Up_Down
{
   char *keyname;
   Evas_Modifier *modifiers;
   Evas_Lock *locks;

   const char *key;
   const char *string;
   const char *compose;
   unsigned int timestamp;
};

/**
 * @struct Etk_Widget
 * @brief All the Etk widgets inherits from an Etk_Widget.
 */
struct _Etk_Widget
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;

   char *name;
   Etk_Toplevel_Widget *toplevel_parent;
   Etk_Widget *parent;
   void *child_properties;
   Evas_List *children;
   Evas_List *focus_order;
   
   Evas_Object *theme_object;
   int theme_min_width, theme_min_height;
   char *theme_file;
   char *theme_group;
   Etk_Widget *theme_parent;
   Evas_List *theme_children;

   Evas_Object *smart_object;
   Evas_Object *event_object;
   Evas_Object *clip;
   Evas_List *member_objects;
   Evas_List *swallowed_objects;

   int left_inset, right_inset, top_inset, bottom_inset;
   Etk_Geometry geometry;
   Etk_Geometry inner_geometry;
   /* The size requested by the user */
   Etk_Size requested_size;
   /* The result of the last call to etk_widget_size_request() */
   Etk_Size last_size_requisition;
   void (*size_request)(Etk_Widget *widget, Etk_Size *size_requisition);
   void (*size_allocate)(Etk_Widget *widget, Etk_Geometry geometry);

   void (*scroll_size_get)(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);
   void (*scroll_margins_get)(Etk_Widget *widget, Etk_Size *margin_size);
   void (*scroll)(Etk_Widget *widget, int x, int y);

   void (*show)(Etk_Widget *widget);
   void (*enter)(Etk_Widget *widget);
   void (*leave)(Etk_Widget *widget);
   void (*focus)(Etk_Widget *widget);
   void (*unfocus)(Etk_Widget *widget);
   void (*drag_drop)(Etk_Widget *widget);
   void (*drag_motion)(Etk_Widget *widget);
   void (*drag_enter)(Etk_Widget *widget);   
   void (*drag_leave)(Etk_Widget *widget);
   void (*drag_begin)(Etk_Widget *widget);   
   void (*drag_end)(Etk_Widget *widget);

   unsigned char realized : 1;
   unsigned char swallowed : 1;
   unsigned char visible : 1;
   unsigned char visibility_locked : 1;
   unsigned char repeat_mouse_events : 1;
   unsigned char pass_mouse_events : 1;
   unsigned char has_event_object : 1;
   unsigned char focusable : 1;
   unsigned char focus_on_press : 1;
   unsigned char can_pass_focus : 1;
   unsigned char use_focus_order : 1;
   unsigned char need_size_recalc : 1;
   unsigned char need_redraw : 1;
   unsigned char need_theme_min_size_recalc : 1;
   unsigned char accepts_dnd : 1;
   unsigned char dnd_source : 1;
   unsigned char dnd_dest : 1;
   unsigned char dnd_internal: 1;

   Etk_Widget *drag;
   char    **dnd_types;
   int       dnd_types_num;
};

Etk_Type *etk_widget_type_get();
Etk_Widget *etk_widget_new(Etk_Type *widget_type, const char *first_property, ...);

void etk_widget_name_set(Etk_Widget *widget, const char *name);
const char *etk_widget_name_get(Etk_Widget *widget);
void etk_widget_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h);
void etk_widget_inner_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h);

Etk_Toplevel_Widget *etk_widget_toplevel_parent_get(Etk_Widget *widget);
Evas *etk_widget_toplevel_evas_get(Etk_Widget *widget);
void etk_widget_parent_set(Etk_Widget *widget, Etk_Widget *parent);
void etk_widget_parent_set_full(Etk_Widget *widget, Etk_Widget *parent, Etk_Bool remove_from_container);
Etk_Widget *etk_widget_parent_get(Etk_Widget *widget);

void etk_widget_theme_file_set(Etk_Widget *widget, const char *theme_file);
const char *etk_widget_theme_file_get(Etk_Widget *widget);
void etk_widget_theme_group_set(Etk_Widget *widget, const char *theme_group);
const char *etk_widget_theme_group_get(Etk_Widget *widget);
void etk_widget_theme_parent_set(Etk_Widget *widget, Etk_Widget *theme_parent);
Etk_Widget *etk_widget_theme_parent_get(Etk_Widget *widget);

void etk_widget_has_event_object_set(Etk_Widget *widget, Etk_Bool has_event_object);
Etk_Bool etk_widget_has_event_object_get(Etk_Widget *widget);
void etk_widget_repeat_mouse_events_set(Etk_Widget *widget, Etk_Bool repeat_mouse_events);
Etk_Bool etk_widget_repeat_mouse_events_get(Etk_Widget *widget);
void etk_widget_pass_mouse_events_set(Etk_Widget *widget, Etk_Bool pass_mouse_events);
Etk_Bool etk_widget_pass_mouse_events_get(Etk_Widget *widget);
void etk_widget_key_event_propagation_stop();

void etk_widget_show(Etk_Widget *widget);
void etk_widget_show_all(Etk_Widget *widget);
void etk_widget_hide(Etk_Widget *widget);
void etk_widget_hide_all(Etk_Widget *widget);
Etk_Bool etk_widget_is_visible(Etk_Widget *widget);
void etk_widget_visibility_locked_set(Etk_Widget *widget, Etk_Bool visibility_locked);
Etk_Bool etk_widget_visibility_locked_get(Etk_Widget *widget);

void etk_widget_raise(Etk_Widget *widget);
void etk_widget_lower(Etk_Widget *widget);

void etk_widget_size_recalc_queue(Etk_Widget *widget);
void etk_widget_redraw_queue(Etk_Widget *widget);
void etk_widget_size_request_set(Etk_Widget *widget, int w, int h);
void etk_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
void etk_widget_size_request_full(Etk_Widget *widget, Etk_Size *size_requisition, Etk_Bool hidden_has_no_size);
void etk_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

void etk_widget_enter(Etk_Widget *widget);
void etk_widget_leave(Etk_Widget *widget);
void etk_widget_focus(Etk_Widget *widget);
void etk_widget_unfocus(Etk_Widget *widget);

Etk_Bool etk_widget_swallow_widget(Etk_Widget *swallowing_widget, const char *part, Etk_Widget *widget_to_swallow);
void etk_widget_unswallow_widget(Etk_Widget *swallowing_widget, Etk_Widget *widget);
Etk_Bool etk_widget_is_swallowing_widget(Etk_Widget *widget, Etk_Widget *swallowed_widget);
Etk_Bool etk_widget_is_swallowed(Etk_Widget *widget);

Etk_Bool etk_widget_theme_object_swallow(Etk_Widget *swallowing_widget, const char *part, Evas_Object *object);
void etk_widget_theme_object_unswallow(Etk_Widget *swallowing_widget, Evas_Object *object);
Etk_Bool etk_widget_is_swallowing_object(Etk_Widget *widget, Evas_Object *object);

void etk_widget_theme_object_min_size_calc(Etk_Widget *widget, int *w, int *h);
void etk_widget_theme_object_signal_emit(Etk_Widget *widget, const char *signal_name);
void etk_widget_theme_object_part_text_set(Etk_Widget *widget, const char *part_name, const char *text);
int etk_widget_theme_object_data_get(Etk_Widget *widget, const char *data_name, const char *format, ...);

Etk_Bool etk_widget_member_object_add(Etk_Widget *widget, Evas_Object *object);
void etk_widget_member_object_del(Etk_Widget *widget, Evas_Object *object);

void etk_widget_member_object_raise(Etk_Widget *widget, Evas_Object *object);
void etk_widget_member_object_lower(Etk_Widget *widget, Evas_Object *object);
void etk_widget_member_object_stack_above(Etk_Widget *widget, Evas_Object *object, Evas_Object *above);
void etk_widget_member_object_stack_below(Etk_Widget *widget, Evas_Object *object, Evas_Object *below);

void etk_widget_clip_set(Etk_Widget *widget, Evas_Object *clip);
void etk_widget_clip_unset(Etk_Widget *widget);
Evas_Object *etk_widget_clip_get(Etk_Widget *widget);

void         etk_widget_dnd_dest_set(Etk_Widget *widget, Etk_Bool on);
Etk_Bool     etk_widget_dnd_dest_get(Etk_Widget *widget);
Evas_List   *etk_widget_dnd_dest_widgets_get();
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
  
void etk_widget_drag_drop(Etk_Widget *widget, Etk_Event_Selection_Request *event);
void etk_widget_drag_motion(Etk_Widget *widget);
void etk_widget_drag_enter(Etk_Widget *widget);
void etk_widget_drag_leave(Etk_Widget *widget);
void etk_widget_drag_begin(Etk_Widget *widget);
void etk_widget_drag_end(Etk_Widget *widget);

void etk_widget_selection_received(Etk_Widget *widget, Etk_Event_Selection_Request *event);
void etk_widget_clipboard_received(Etk_Widget *widget, Etk_Event_Selection_Request *event);

/** @} */

#endif
