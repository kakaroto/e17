/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
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
#ifndef ENESIM_H_
#define ENESIM_H_

/**
 * @mainpage Enesim
 * @image html enesim.png
 * @section intro Introduction
 * Enesim is a direct rendering graphics library, in the sense that it does not
 * have a state. The implementation is software based only, but it has a very
 * flexible design: all the steps of the rendering process have been abstracted
 * allowing applications to only use the functionality for the drawing
 * operations they may need, not forcing on how or what to draw.
 * @image html raster.png
 * Features:
 *  - Multiple Rasterizers, with and without anti-alias.
 *  - Multiple Scanline types.
 *  - Point and Span based Drawers.
 *  - Multiple raster operations.
 *  - Multiple Surface formats, even multiple plane surface formats.
 *  - Projective and affine Transformers.
 * 
 * @file
 * @brief Enesim API
 * @defgroup Enesim_Group API
 * @{
 * 
 * In Enesim every intermediate surface format is argb8888_pre, so all surface
 * modes should know how to convert to/from argb8888_pre
 * 
 * To add a new surface format you should follow the next steps:
 * 1. Add a new entry to Enesim_Surface_Format enumaration in enesim_surface.h
 *    the format is TODO
 * 2. Create a new ENESIM_SURFACE_FORMAT(name, [yes | no]) entry into
 *    configure.in 
 * 3. Create a data struct for that format in enesim_surface.h, every plane 
 *    should be named as planeX, in case the pixel length is smaller than the 
 *    data type and contiguous you should also create a planeX_pixel variable. 
 * 4. Add your new data type to the Enesim_Surface_Data union, it should be of
 *    the same name as the format type
 * 5. Go to enesim_generator.c and follow the instructions on the beginning of
 *    the file
 * 6. Add a rule to generate the core headers in src/include/Makefile.am
 * 7. Add a rule to generate the drawer source files in src/lib/drawer/Makefile.am
 * 8. Add the drawer to the array of drawers in enesim_drawer.c, with the
 *    conditional building of course
 * 9. Add your includes in enesim_private.h in a similar way
 * 
 */

#include "Eina.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "enesim_main.h"
#include "enesim_surface.h"
#include "enesim_drawer.h"
#include "enesim_matrix.h"
#include "enesim_renderer.h"
#include "enesim_converter.h"
#include "enesim_rasterizer.h"
#include "enesim_transformer.h"

#ifdef __cplusplus
}
#endif
	
/** @} */
#endif
