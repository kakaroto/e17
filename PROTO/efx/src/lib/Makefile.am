MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_builddir)/

EXTRA_DIST = efx_private.h

lib_LTLIBRARIES = libefx.la
libefx_la_SOURCES = \
efx.c \
efx_helpers.c \
efx_move.c \
efx_rotate.c \
efx_spin.c \
efx_zoom.c

libefx_la_LIBADD = @EFX_DEPS_LIBS@
libefx_la_LDFLAGS = -no-undefined
libefx_la_CFLAGS = @EFX_DEPS_CFLAGS@

efxdir = $(includedir)
efx_HEADERS = Efx.h
