AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(includedir) -I$(top_srcdir) -I$(top_srcdir)/src/bin @my_cflags@

pkgdir               = $(libdir)/elation

pkg_LTLIBRARIES      = elation_menu.la elation_dvd.la elation_disk.la

elation_menu_la_SOURCES      = elation_menu.c
elation_menu_la_LIBADD       = @my_libs@
elation_menu_la_LDFLAGS      = $(LDFLAGS) -no-undefined -module -avoid-version 
elation_menu_la_DEPENDENCIES = $(top_builddir)/config.h

elation_dvd_la_SOURCES      = elation_dvd.c
elation_dvd_la_LIBADD       = @my_libs@
elation_dvd_la_LDFLAGS      = $(LDFLAGS) -no-undefined -module -avoid-version 
elation_dvd_la_DEPENDENCIES = $(top_builddir)/config.h

elation_disk_la_SOURCES      = elation_disk.c
elation_disk_la_LIBADD       = @my_libs@
elation_disk_la_LDFLAGS      = $(LDFLAGS) -no-undefined -module -avoid-version 
elation_disk_la_DEPENDENCIES = $(top_builddir)/config.h
