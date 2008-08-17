AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin @ELATION_CFLAGS@

pkgdir               = $(libdir)/elation

pkg_LTLIBRARIES      = \
elation_menu.la \
elation_dvd.la \
elation_disk.la \
elation_media.la \
elation_vcd.la \
elation_cd.la

elation_menu_la_SOURCES      = elation_menu.c
elation_menu_la_LIBADD       = @ELATION_LIBS@
elation_menu_la_LDFLAGS      = -no-undefined -module -avoid-version 
elation_menu_la_DEPENDENCIES = $(top_builddir)/config.h

elation_dvd_la_SOURCES      = elation_dvd.c
elation_dvd_la_LIBADD       = @ELATION_LIBS@
elation_dvd_la_LDFLAGS      = -no-undefined -module -avoid-version 
elation_dvd_la_DEPENDENCIES = $(top_builddir)/config.h

elation_disk_la_SOURCES      = elation_disk.c
elation_disk_la_LIBADD       = @ELATION_LIBS@
elation_disk_la_LDFLAGS      = -no-undefined -module -avoid-version 
elation_disk_la_DEPENDENCIES = $(top_builddir)/config.h

elation_media_la_SOURCES      = elation_media.c
elation_media_la_LIBADD       = @ELATION_LIBS@
elation_media_la_LDFLAGS      = -no-undefined -module -avoid-version 
elation_media_la_DEPENDENCIES = $(top_builddir)/config.h

elation_vcd_la_SOURCES      = elation_vcd.c
elation_vcd_la_LIBADD       = @ELATION_LIBS@
elation_vcd_la_LDFLAGS      = -no-undefined -module -avoid-version 
elation_vcd_la_DEPENDENCIES = $(top_builddir)/config.h

elation_cd_la_SOURCES      = elation_cd.c
elation_cd_la_LIBADD       = @ELATION_LIBS@
elation_cd_la_LDFLAGS      = -no-undefined -module -avoid-version 
elation_cd_la_DEPENDENCIES = $(top_builddir)/config.h

