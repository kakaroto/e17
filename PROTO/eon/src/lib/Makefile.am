SUBDIRS = core layout paint shape parser script filter

MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES	= \
-I. -I../.. -I$(top_srcdir)/src/include -Wall -g \
@EON_CFLAGS@

lib_LTLIBRARIES = libeon.la

EXTRA_DIST =

libeon_la_SOURCES =

libeon_la_LIBADD = @EON_LIBS@ \
core/libeon_core.la \
paint/libeon_paint.la \
filter/libeon_filter.la \
shape/libeon_shape.la \
layout/libeon_layout.la \
parser/libeon_parser.la \
script/libeon_script.la \
$(top_builddir)/src/engines/enesim/libeon_engine_enesim.la

if BUILD_ENGINE_SDL
libeon_la_LIBADD += $(top_builddir)/src/engines/sdl/libeon_engine_sdl.la
endif

if BUILD_ENGINE_IPC
libeon_la_LIBADD += $(top_builddir)/src/engines/remote/libeon_engine_remote.la
endif

libeon_la_DEPENDENCIES = $(top_builddir)/src/engines/enesim/libeon_engine_enesim.la \
core/libeon_core.la \
paint/libeon_paint.la \
filter/libeon_filter.la \
shape/libeon_shape.la \
parser/libeon_parser.la \
script/libeon_script.la

if BUILD_ENGINE_SDL
libeon_la_DEPENDENCIES += $(top_builddir)/src/engines/sdl/libeon_engine_sdl.la
endif

if BUILD_ENGINE_IPC
libeon_la_DEPENDENCIES += $(top_builddir)/src/engines/remote/libeon_engine_remote.la
endif

libeon_la_LDFLAGS = -version-info 1:0:0
