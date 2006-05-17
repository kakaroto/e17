## $Id$

SUBDIRS = lib plugins include

AM_CFLAGS = -I./include \
            @ENLIGHTENMENT_CFLAGS@ \
            @ECORE_CFLAGS@  \
            @EDB_CFLAGS@    \
            @EVAS_CFLAGS@   \
            @EDJE_CFLAGS@   \
            @ESMART_CFLAGS@

bin_PROGRAMS = embrace

embrace_SOURCES = main.c

embrace_LDADD = $(top_builddir)/src/lib/libembrace.la \
                @ECORE_LIBS@  \
                @EDB_LIBS@    \
                @EVAS_LIBS@   \
                @EDJE_LIBS@   \
                @ESMART_LIBS@ \
                @DL_LIBS@     \
                -lesmart_container -lesmart_draggies -lesmart_trans_x11

embrace_LDFLAGS = -export-dynamic
embrace_DEPENDENCIES = $(top_builddir)/src/lib/libembrace.la

if BUILD_MODULE

MODULE = embrace

pkgdir                 = @e_module_dir@/$(MODULE)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c e_mod_main.h
module_la_LIBADD       = $(top_builddir)/src/lib/libembrace.la \
                         @ENLIGHTENMENT_LIBS@ -lm @DL_LIBS@ \
                         @ESMART_LIBS@ @EDB_LIBS@ \
                         -lesmart_container -lesmart_draggies -lesmart_trans_x11
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/src/lib/libembrace.la

endif
