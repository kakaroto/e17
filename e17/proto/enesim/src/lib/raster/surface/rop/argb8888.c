#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"

Enesim_Surface_Func argb8888_funcs[ENESIM_RENDERER_ROPS] = {
	[ENESIM_RENDERER_BLEND] = {
		argb8888_c_color,
	},
};
