ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = po

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)/fonts

filesdir = $(datadir)
files_DATA = e-module-alarm.edj module.desktop alarm.edj

EXTRA_DIST = $(files_DATA) e_modules-alarm.spec \
	     alarm.edc \
	     fonts/vera.ttf \
	     images/alarm_enabled.png \
	     images/alarm_disabled.png \
	     images/alarm_snooze.png

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @E_CFLAGS@
pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c \
			 e_mod_config_alarm.c
module_la_LIBADD       = @E_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/alarm.edj

alarm.edj: Makefile alarm.edc
	   $(EDJE_CC) $(EDJE_FLAGS) \
	   $(top_srcdir)/alarm.edc \
	   $(top_builddir)/alarm.edj
	   
clean-local:
	    rm -rf alarm.edj *~
	    
