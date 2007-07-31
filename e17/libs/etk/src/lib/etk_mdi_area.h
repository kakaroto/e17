/** @file etk_mdi_area.h */
#ifndef _ETK_MDI_AREA_H_
#define _ETK_MDI_AREA_H_

#include "etk_container.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Mdi_Area Etk_Mdi_Area
 * @brief A container widget for Etk_Mdi_Window
 * @{
 */

/** Gets the type of a area */
#define ETK_MDI_AREA_TYPE    (etk_mdi_area_type_get())
/** Casts the object to an Etk_Mdi_Area */
#define ETK_MDI_AREA(obj)    (ETK_OBJECT_CAST((obj), ETK_MDI_AREA_TYPE, Etk_Mdi_Area))
/** Check if the object is an Etk_Mdi_Area */
#define ETK_IS_MDI_AREA(obj) (ETK_OBJECT_CHECK_TYPE((obj), ETK_MDI_AREA_TYPE))

/**
 * @brief A container widget for Etk_Mdi_Window
 * @structinfo
 */
struct Etk_Mdi_Area
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Evas_List *children;
   Evas_Object *clip;
};

Etk_Type   *etk_mdi_area_type_get(void);
Etk_Widget *etk_mdi_area_new(void);

void        etk_mdi_area_put(Etk_Mdi_Area *mdi_area, Etk_Widget *widget, int x, int y);
void        etk_mdi_area_move(Etk_Mdi_Area *mdi_area, Etk_Widget *widget, int x, int y);
void        etk_mdi_area_child_position_get(Etk_Mdi_Area *mdi_area, Etk_Widget *widget, int *x, int *y);

/** @} */

#endif
