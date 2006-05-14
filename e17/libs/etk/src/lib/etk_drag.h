/** @file etk_drag.h */
#ifndef _ETK_DRAG_H_
#define _ETK_DRAG_H_

#include "etk_window.h"
#include "etk_types.h"

#define ETK_DRAG_TYPE       (etk_drag_type_get())
#define ETK_DRAG(obj)       (ETK_OBJECT_CAST((obj), ETK_DRAG_TYPE, Etk_Drag))
#define ETK_IS_DRAG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_DRAG_TYPE))

struct _Etk_Drag
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;   

   Etk_Widget *widget;
   
   char         **types;
   unsigned int   num_types;
   void          *data;
   int            data_size;   
};

Etk_Type   *etk_drag_type_get();  
Etk_Widget *etk_drag_new(Etk_Widget *widget);
void        etk_drag_types_set(Etk_Drag *drag, const char **types, unsigned int num_types);
void        etk_drag_data_set(Etk_Drag *drag, void *data, int size);
void        etk_drag_begin(Etk_Drag *drag);
void        etk_drag_parent_widget_set(Etk_Drag *drag, Etk_Widget *widget);
Etk_Widget *etk_drag_parent_widget_get(Etk_Drag *drag);
  

/** @} */

#endif
