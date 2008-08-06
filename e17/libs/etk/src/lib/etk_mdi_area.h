/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
/** @file etk_mdi_area.h */
#ifndef _ETK_MDI_AREA_H_
#define _ETK_MDI_AREA_H_

#include "etk_container.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
