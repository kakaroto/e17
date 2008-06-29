MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_gl_glew \
-I$(top_builddir)/src/engines/evas_gl_glew \
@ECORE_WIN32_CFLAGS@ \
@ECORE_CFLAGS@ \
@EVAS_CFLAGS@

if EWL_ENABLE_EVAS_OPENGL_GLEW

pkgdir = $(libdir)/ewl/engines

pkg_LTLIBRARIES = evas_gl_glew.la

evas_gl_glew_la_SOURCES = \
Ewl_Engine_Evas_Gl_Glew.h \
ewl_engine_evas_gl_glew.c

evas_gl_glew_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_WIN32_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
evas_gl_glew_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -module -version-info @INTERFACE_VERSION@
evas_gl_glew_la_DEPENDENCIES =

endif

