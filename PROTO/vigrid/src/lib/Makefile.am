
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
@EFL_VIGRID_BUILD@ \
@VIGRID_CFLAGS@

lib_LTLIBRARIES = libvigrid.la

libvigrid_la_SOURCES = \
vigrid_main.c \
vigrid_massif.c

libvigrid_la_LIBADD = @VIGRID_LIBS@
libvigrid_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@
