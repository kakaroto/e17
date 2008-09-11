MAINTAINERCLEANFILES = \
config.h.in~ \
config.h.in \
config.h \
Makefile.in

if BUILD_SURFACE_ARGB8888_UNPRE
ARGB8888_UNPRE_REQ=$(top_srcdir)/src/include//private/surface_argb8888_unpre_core.h
else
ARGB8888_UNPRE_REQ=
endif

if BUILD_SURFACE_RGB565_XA5
RGB565_XA5_REQ=$(top_srcdir)/src/include/private/surface_rgb565_xa5_core.h
else
RGB565_XA5_REQ=
endif

if BUILD_SURFACE_RGB565_B1A3
RGB565_B1A3_REQ=$(top_srcdir)/src/include/private/surface_rgb565_b1a3_core.h
else
RGB565_B1A3_REQ=
endif


BUILT_SOURCES = \
$(top_srcdir)/src/include/private/surface_argb8888_core.h \
$(ARGB8888_UNPRE_REQ) \
$(RGB565_XA5_REQ) \
$(RGB565_B1A3_REQ)

CLEANFILES = \
$(top_srcdir)/src/include/private/surface_argb8888_core.h \
$(ARGB8888_UNPRE_REQ) \
$(RGB565_XA5_REQ) \
$(RGB565_B1A3_REQ)

$(top_srcdir)/src/include/private/surface_argb8888_core.h: Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator core argb8888 $@
$(ARGB8888_UNPRE_REQ): Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator core argb8888_unpre $@
$(RGB565_XA5_REQ): Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator core rgb565_xa5 $@
$(RGB565_B1A3_REQ): Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator core rgb565_b1a3 $@

ENESIM_UTILHEADERS = \
enesim_converter.h \
enesim_drawer.h \
enesim_main.h \
enesim_matrix.h \
enesim_rasterizer.h \
enesim_surface.h \
enesim_transformer.h \
enesim_renderer.h

installed_headersdir = $(prefix)/include/enesim
installed_headers_DATA = $(ENESIM_UTILHEADERS)
include_HEADERS = Enesim.h
