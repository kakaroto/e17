MAINTAINERCLEANFILES = \
Makefile.in

if BUILD_SURFACE_ARGB8888_UNPRE
ARGB8888_UNPRE_REQ=argb8888_unpre.c
else
ARGB8888_UNPRE_REQ=
endif

if BUILD_SURFACE_RGB565_XA5
RGB565_XA5_REQ=rgb565_xa5.c
else
RGB565_XA5_REQ=
endif

if BUILD_SURFACE_RGB565_B1A3
RGB565_B1A3_REQ=rgb565_b1a3.c
else
RGB565_B1A3_REQ=
endif

BUILT_SOURCES = \
argb8888.c \
$(ARGB8888_UNPRE_REQ) \
$(RGB565_XA5_REQ) \
$(RGB565_B1A3_REQ)

CLEANFILES = \
argb8888.c \
$(ARGB8888_UNPRE_REQ) \
$(RGB565_XA5_REQ) \
$(RGB565_B1A3_REQ)

$(ARGB8888_UNPRE_REQ): Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator drawer argb8888_unpre $@
$(RGB565_XA5_REQ): Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator drawer rgb565_xa5 $@
$(RGB565_B1A3_REQ): Makefile
	$(top_builddir)/$(native_tools)/src/tool/enesim_generator drawer rgb565_b1a3 $@

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

noinst_LTLIBRARIES = libenesim_drawer.la

libenesim_drawer_la_SOURCES = \
argb8888.c \
argb8888_unpre.c
#$(BUILT_SOURCES)