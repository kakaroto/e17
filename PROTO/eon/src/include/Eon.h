/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
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
#ifndef _EON_H
#define _EON_H

#include "Eina.h"
#include "Ekeko.h"
#include "Enesim.h"
#include "Ecore.h"
#include "Etch.h"

typedef struct _Eon_Document Eon_Document;
typedef struct _Eon_Canvas Eon_Canvas;
typedef struct _Eon_Animation Eon_Animation;
typedef struct _Eon_Animation_Key Eon_Animation_Key;
typedef struct _Eon_Animation_Basic Eon_Animation_Basic;
typedef struct _Eon_Animation_Matrix Eon_Animation_Matrix;
typedef struct _Eon_External Eon_External;
typedef struct _Eon_Script Eon_Script;
typedef struct _Eon_Style Eon_Style;
typedef struct _Eon_Style_Applier Eon_Style_Applier;
typedef struct _Eon_Trigger_Object Eon_Trigger_Object; // FIXME called object by now until we remove the trigger value
typedef struct _Eon_Setter Eon_Setter;
/* paint objects */
typedef struct _Eon_Paint Eon_Paint;
typedef struct _Eon_Paint_Square Eon_Paint_Square;
typedef struct _Eon_Compound Eon_Compound;
typedef struct _Eon_Compound_Layer Eon_Compound_Layer;
typedef struct _Eon_Image Eon_Image;
typedef struct _Eon_Transition Eon_Transition;
typedef struct _Eon_Hswitch Eon_Hswitch;
typedef struct _Eon_Fade Eon_Fade;
typedef struct _Eon_Checker Eon_Checker;
typedef struct _Eon_Stripes Eon_Stripes;
typedef struct _Eon_Grid Eon_Grid;
typedef struct _Eon_Buffer Eon_Buffer;
/* shapes */
typedef struct _Eon_Shape Eon_Shape;
typedef struct _Eon_Shape_Square Eon_Shape_Square;
typedef struct _Eon_Rect Eon_Rect;
typedef struct _Eon_Circle Eon_Circle;
typedef struct _Eon_Polygon Eon_Polygon;
typedef struct _Eon_Text Eon_Text;
/* filter effects */
typedef struct _Eon_Filter Eon_Filter;
typedef struct _Eon_Displace Eon_Displace;

/* TODO use this generic names */
typedef struct _Eon_Engine Eon_Engine;
/* TODO remove this */
typedef void Eon_Context;
typedef void Eon_Surface;

EAPI int eon_init(void);
EAPI void eon_loop(void);
EAPI int eon_shutdown(void);

#include "eon_engine.h"

#include "eon_coord.h"
#include "eon_color.h"
#include "eon_value.h"
#include "eon_document.h"
#include "eon_canvas.h"
#include "eon_animation.h"
#include "eon_anim_key.h"
#include "eon_anim_basic.h"
#include "eon_anim_matrix.h"
#include "eon_external.h"
#include "eon_script.h"
#include "eon_style.h"
#include "eon_style_applier.h"
#include "eon_setter.h"
#include "eon_trigger.h"

#include "eon_paint.h"
#include "eon_paint_square.h"
#include "eon_compound.h"
#include "eon_compound_layer.h"
#include "eon_image.h"
#include "eon_transition.h"
#include "eon_hswitch.h"
#include "eon_fade.h"
#include "eon_checker.h"
#include "eon_stripes.h"
#include "eon_grid.h"
#include "eon_buffer.h"

#include "eon_shape.h"
#include "eon_shape_square.h"
#include "eon_text.h"
#include "eon_rect.h"
#include "eon_polygon.h"
#include "eon_circle.h"

#include "eon_filter.h"
#include "eon_displace.h"

#include "eon_widget.h"
#include "eon_button.h"
#include "eon_label.h"


#endif
