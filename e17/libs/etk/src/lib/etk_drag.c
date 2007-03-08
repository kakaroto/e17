/** @file etk_drag.c */
#include "etk_drag.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

#include "etk_widget.h"
#include "etk_window.h"
#include "etk_engine.h"

/**
 * @addtogroup Etk_Drag
 * @{
 */

/**
 * @internal
 * @brief Gets the type of an Etk_Drag
 * @return Returns the type on an Etk_Drag
 */
Etk_Type *etk_drag_type_get()
{
   static Etk_Type *drag_type = NULL;
   
   if(!drag_type)
   {
      drag_type = etk_type_new("Etk_Drag", ETK_WINDOW_TYPE, sizeof(Etk_Drag), NULL, NULL);
   }
   
   return drag_type;
}

/**
 * @brief Create a new drag widget
 * @param widget the parent widget of this drag
 * @return Returns the new drag widget
 */
Etk_Widget *etk_drag_new(Etk_Widget *widget)
{
   return etk_widget_new(ETK_DRAG_TYPE, NULL);
}

void etk_drag_types_set(Etk_Drag *drag, const char **types, unsigned int num_types)
{
}

void etk_drag_data_set(Etk_Drag *drag, void *data, int size)
{
}

void etk_drag_begin(Etk_Drag *drag)
{
}

/**
 * @brief Sets the parent widget of a drag
 * @param drag a drag
 * @param widget a widget
 */
void etk_drag_parent_widget_set(Etk_Drag *drag, Etk_Widget *widget)
{
}

/**
 * @brief Gets the parent widget of a drag
 * @param drag a drag
 * @return Returns the parent widget of the drag
 */
Etk_Widget *etk_drag_parent_widget_get(Etk_Drag *drag)
{
   return NULL;
}

/** @} */
