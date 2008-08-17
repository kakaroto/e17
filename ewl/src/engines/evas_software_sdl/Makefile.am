MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_software_sdl \
-I$(top_builddir)/src/engines/evas_software_sdl \
@EVAS_CFLAGS@ \
@ECORE_SDL_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_SOFTWARE_SDL

pkg_LTLIBRARIES = evas_software_sdl.la

evas_software_sdl_la_SOURCES = \
Ewl_Engine_Evas_Software_SDL.h \
ewl_engine_evas_software_sdl.c

evas_software_sdl_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_SDL_LIBS@ @EVAS_LIBS@
evas_software_sdl_la_LDFLAGS = -module @lt_no_undefined@ -version-info @INTERFACE_VERSION@
evas_software_sdl_la_DEPENDENCIES =

endif

