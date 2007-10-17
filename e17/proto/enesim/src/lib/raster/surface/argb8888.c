#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"

Surface_Backend argb8888_backend = {
		.rops = {
				[ENESIM_RENDERER_BLEND] = {
						argb8888_c_color,
					},
		},
};
