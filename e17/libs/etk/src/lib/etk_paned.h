/** @file etk_paned.h */
#ifndef _ETK_PANED_H_
#define _ETK_PANED_H_

#include "etk_container.h"
#include "etk_types.h"

/* TODO/FIXME list:
 * - etk_paned_position_set() does not work if the paned is not realized
 * - add the ability to change the expand setting
 */

/**
 * @defgroup Etk_Paned Etk_Paned
 * @brief The Etk_Paned widget is a container that can contain two children separated by a draggable separator
 * @{
 */

/** Gets the type of a paned */
#define ETK_PANED_TYPE       (etk_paned_type_get())
/** Casts the object to an Etk_Paned */
#define ETK_PANED(obj)       (ETK_OBJECT_CAST((obj), ETK_PANED_TYPE, Etk_Paned))
/** Checks if the object is an Etk_Paned */
#define ETK_IS_PANED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_PANED_TYPE))

/** Gets the type of a hpaned */
#define ETK_HPANED_TYPE       (etk_hpaned_type_get())
/** Casts the object to an Etk_HPaned */
#define ETK_HPANED(obj)       (ETK_OBJECT_CAST((obj), ETK_HPANED_TYPE, Etk_HPaned))
/** Checks if the object is an Etk_HPaned */
#define ETK_IS_HPANED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HPANED_TYPE))

/** Gets the type of a vpaned */
#define ETK_VPANED_TYPE       (etk_vpaned_type_get())
/** Casts the object to an Etk_VPaned */
#define ETK_VPANED(obj)       (ETK_OBJECT_CAST((obj), ETK_VPANED_TYPE, Etk_VPaned))
/** Checks if the object is an Etk_VPaned */
#define ETK_IS_VPANED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VPANED_TYPE))

/**
 * @brief @widget A container that can contain two children separated by a draggable separator
 * @structinfo
 */
struct Etk_Paned
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Widget *separator;
   Etk_Widget *child1;
   Etk_Bool expand1;
   Etk_Widget *child2;
   Etk_Bool expand2;

   Etk_Bool drag;
   int drag_delta;
   int position;
};

/**
 * @brief @widget An horizontal paned
 * @structinfo
 */
struct Etk_HPaned
{
   /* private: */
   /* Inherit from Etk_Paned */
   Etk_Paned paned;
};

/**
 * @brief @widget A vertical paned
 * @structinfo
 */
struct Etk_VPaned
{
   /* private: */
   /* Inherit from Etk_Paned */
   Etk_Paned paned;
};

Etk_Type *etk_paned_type_get();

Etk_Type *etk_hpaned_type_get();
Etk_Widget *etk_hpaned_new();

Etk_Type *etk_vpaned_type_get();
Etk_Widget *etk_vpaned_new();

void etk_paned_child1_set(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand);
void etk_paned_child2_set(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand);
Etk_Widget *etk_paned_child1_get(Etk_Paned *paned);
Etk_Widget *etk_paned_child2_get(Etk_Paned *paned);

void etk_paned_position_set(Etk_Paned *paned, int position);
int etk_paned_position_get(Etk_Paned *paned);

/** @} */

#endif
