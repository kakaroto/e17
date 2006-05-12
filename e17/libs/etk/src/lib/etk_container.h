/** @file etk_container.h */
#ifndef _ETK_CONTAINER_H_
#define _ETK_CONTAINER_H_

#include <Evas.h>
#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Container Etk_Container
 * @brief An Etk_Container is a widget that can contain other widgets
 * @{
 */

/** Gets the type of a container */
#define ETK_CONTAINER_TYPE       (etk_container_type_get())
/** Casts the object to an Etk_Container */
#define ETK_CONTAINER(obj)       (ETK_OBJECT_CAST((obj), ETK_CONTAINER_TYPE, Etk_Container))
/** Checks if the object is an Etk_Container */
#define ETK_IS_CONTAINER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_CONTAINER_TYPE))

/**
 * @enum Etk_Fill_Policy_Flags
 * @brief Describes how a child fills and expands the space allocated by its parent container
 */ 
typedef enum Etk_Fill_Policy_Flags
{
   ETK_FILL_POLICY_NONE = 1 << 0,       /**< The child doesn't fill or expand */
   ETK_FILL_POLICY_HFILL = 1 << 1,      /**< The child fills all the horizontal space allocated for him */
   ETK_FILL_POLICY_VFILL = 1 << 2,      /**< The child fills all the vertical space allocated for him */
   ETK_FILL_POLICY_HEXPAND = 1 << 3,    /**< The child expands horizontally: the cell containing the child */
                                        /**< will take as much place as possible in the horizontal direction */
   ETK_FILL_POLICY_VEXPAND = 1 << 4     /**< The child expands vertically: the cell containing the child */
                                        /**< will take as much place as possible in the vertical direction */
} Etk_Fill_Policy_Flags;

/**
 * @brief @widget The structure of a container
 * @structinfo
 */
struct Etk_Container
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   void (*child_add)(Etk_Container *container, Etk_Widget *widget);
   void (*child_remove)(Etk_Container *container, Etk_Widget *widget);
   Evas_List *(*children_get)(Etk_Container *container);

   int border_width;
};

Etk_Type *etk_container_type_get();

void etk_container_add(Etk_Container *container, Etk_Widget *widget);
void etk_container_remove(Etk_Container *container, Etk_Widget *widget);

void etk_container_border_width_set(Etk_Container *container, int border_width);
int etk_container_border_width_get(Etk_Container *container);

Evas_List *etk_container_children_get(Etk_Container *container);
Etk_Bool etk_container_is_child(Etk_Container *container, Etk_Widget *widget);
void etk_container_for_each(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child));
void etk_container_for_each_data(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child, void *data), void *data);

void etk_container_child_space_fill(Etk_Widget *child, Etk_Geometry *child_space, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign);

/** @} */

#endif
