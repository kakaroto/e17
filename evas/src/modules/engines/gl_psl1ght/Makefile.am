
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/include \
-I$(top_srcdir)/src/modules/engines \
-I$(top_srcdir)/src/modules/engines/gl_common \
@FREETYPE_CFLAGS@ \
@PIXMAN_CFLAGS@ \
@EINA_CFLAGS@ \
@GL_EET_CFLAGS@ \
@FRIBIDI_CFLAGS@ \
@evas_engine_gl_psl1ght_cflags@

if BUILD_ENGINE_GL_PSL1GHT

GL_PSL1GHT_SOURCES = \
evas_engine.c

GL_PSL1GHT_LIBADD = @evas_engine_gl_psl1ght_libs@ $(top_builddir)/src/modules/engines/gl_common/libevas_engine_gl_common.la 


includes_HEADERS = Evas_Engine_GL_PSL1GHT.h
includesdir = $(includedir)/evas-@VMAJ@

if !EVAS_STATIC_BUILD_GL_PSL1GHT

pkgdir = $(libdir)/evas/modules/engines/psl1ght/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = $(GL_PSL1GHT_SOURCES)
module_la_LIBADD = @EINA_LIBS@ $(GL_PSL1GHT_LIBADD) $(top_builddir)/src/lib/libevas.la
module_la_LDFLAGS = -module -avoid-version
module_la_LIBTOOLFLAGS = --tag=disable-static

else

noinst_LTLIBRARIES = libevas_engine_gl_psl1ght.la

libevas_engine_gl_psl1ght_la_SOURCES = $(GL_PSL1GHT_SOURCES)
libevas_engine_gl_psl1ght_la_LIBADD = $(GL_PSL1GHT_LIBADD)

endif
endif

EXTRA_DIST = evas_engine.h
