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
 */

#include "Eina.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/* core headers */
#include "enesim_main.h"
#include "enesim_format.h"
#include "enesim_surface.h"
/* util headers */
#include "enesim_matrix.h"
#include "enesim_curve.h"
/* old raster operations */
#include "enesim_rasterizer.h"
#include "enesim_context.h"
#include "enesim_primitives.h"

#include "enesim_cpu.h"
/* new raster operations */
#include "enesim_operator.h"
#include "enesim_drawer.h"
#include "enesim_reflector.h"
#include "enesim_repeater.h"
#include "enesim_rotator.h"
#include "enesim_scaler.h"
#include "enesim_transformer.h"
#include "enesim_converter.h"

#ifdef __cplusplus
}
#endif

/** @} */
#endif
