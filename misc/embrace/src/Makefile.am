## $Id$

SUBDIRS = plugins

AM_CFLAGS = @ECORE_CFLAGS@  \
            @EDB_CFLAGS@    \
            @EVAS_CFLAGS@   \
            @EDJE_CFLAGS@   \
            @ESMART_CFLAGS@

noinst_LTLIBRARIES   = libembrace.la

libembrace_la_SOURCES  = embrace.c embrace.h               \
                         embrace_plugin.c embrace_plugin.h \
                         mailbox.c mailbox.h

libembrace_la_DEPENDENCIES = $(top_builddir)/config.h

bin_PROGRAMS = embrace

embrace_SOURCES = main.c

embrace_LDADD = @ECORE_LIBS@  \
                @EDB_LIBS@    \
                @EVAS_LIBS@   \
                @EDJE_LIBS@   \
                @ESMART_LIBS@ \
                @DL_LIBS@     \
                libembrace.la \
                -lesmart_container -lesmart_draggies -lesmart_trans_x11

embrace_LDFLAGS = -export-dynamic

if BUILD_MODULE

MODULE = embrace

INCLUDES               = -I. \
                         -I$(top_srcdir) \
                         -I$(includedir) \
                         @ENLIGHTENMENT_CFLAGS@
pkgdir                 = @e_module_dir@/$(MODULE)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c
module_la_LIBADD       = @ENLIGHTENMENT_LIBS@ -lm @DL_LIBS@ \
                         libembrace.la \
                         -lesmart_container -lesmart_draggies -lesmart_trans_x11
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
