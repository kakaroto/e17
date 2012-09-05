MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_builddir) \
@EFL_EFX_BUILD@ \
@EFX_CFLAGS@

lib_LTLIBRARIES = libefx.la
libefx_la_SOURCES = \
efx.c \
efx_bumpmapping.c \
efx_fade.c \
efx_helpers.c \
efx_move.c \
efx_pan.c \
efx_queue.c \
efx_resize.c \
efx_rotate.c \
efx_spin.c \
efx_util.c \
efx_zoom.c \
efx_private.h

libefx_la_LIBADD = @EFX_LIBS@ -lm
libefx_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@ @release_info@

include_HEADERS = Efx.h
