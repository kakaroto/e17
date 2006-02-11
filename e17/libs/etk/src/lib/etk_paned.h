/** @file etk_paned.h */
#ifndef _ETK_PANED_H_
#define _ETK_PANED_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Paned Etk_Paned
 * @{
 */

/** @brief Gets the type of a paned */
#define ETK_PANED_TYPE       (etk_paned_type_get())
/** @brief Casts the object to an Etk_Paned */
#define ETK_PANED(obj)       (ETK_OBJECT_CAST((obj), ETK_PANED_TYPE, Etk_Paned))
/** @brief Checks if the object is an Etk_Paned */
#define ETK_IS_PANED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_PANED_TYPE))

/** @brief Gets the type of a hpaned */
#define ETK_HPANED_TYPE       (etk_hpaned_type_get())
/** @brief Casts the object to an Etk_HPaned */
#define ETK_HPANED(obj)       (ETK_OBJECT_CAST((obj), ETK_HPANED_TYPE, Etk_HPaned))
/** @brief Checks if the object is an Etk_HPaned */
#define ETK_IS_HPANED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HPANED_TYPE))

/** @brief Gets the type of a hpaned */
#define ETK_VPANED_TYPE       (etk_vpaned_type_get())
/** @brief Casts the object to an Etk_VPaned */
#define ETK_VPANED(obj)       (ETK_OBJECT_CAST((obj), ETK_VPANED_TYPE, Etk_VPaned))
/** @brief Checks if the object is an Etk_HPaned */
#define ETK_IS_VPANED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VPANED_TYPE))

/**
 * @struct Etk_Paned
 * @brief An Etk_Paned is a container that can contain two children separated by a draggable separator
 */
struct _Etk_Paned
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
 * @struct Etk_HPaned
 * @brief An Etk_HPaned is a container that can contain horizontally two children separated by a draggable vertical separator
 */
struct _Etk_HPaned
{
   /* private: */
   /* Inherit from Etk_Paned */
   Etk_Paned paned;
};

/**
 * @struct Etk_VPaned
 * @brief An Etk_VPaned is a container that can contain vertically two children separated by a draggable horizontal separator
 */
struct _Etk_VPaned
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

void etk_paned_add1(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand);
void etk_paned_add2(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand);
Etk_Widget *etk_paned_child1_get(Etk_Paned *paned);
Etk_Widget *etk_paned_child2_get(Etk_Paned *paned);

void etk_paned_position_set(Etk_Paned *paned, int position);
int etk_paned_position_get(Etk_Paned *paned);

/** @} */

#endif
