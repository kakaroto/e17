MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I../lib \
-I/usr/include \
-I/usr/local/include

# noinst_LIBRARIES = libemenu.a
# include_HEADERS = EMenu.h

pkgdir = $(datadir)/$(MODULE_ARCH)/lib/
pkg_LTLIBRARIES = libemenu.la
# libemenu_la_LDFLAGS = -module -avoid-version
libemenu_la_SOURCES = main.c category.c
libemenu_la_DEPENDENCIES = $(top_builddir)/config.h
libemenu_la_LIBADD = @ecore_libs@
libemenu_la_CFLAGS = @ecore_cflags@
