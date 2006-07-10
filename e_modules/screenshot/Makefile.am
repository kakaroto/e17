SUBDIRS = po

MAINTAINERCLEANFILES = Makefile.in mkinstalldirs

EDJE_CC = @edje_cc@
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir =  $(datadir)

files_DATA = module_icon.png module.eap screenshot.edj $(wildcard e17*.png)

EXTRA_DIST = $(files_DATA) screenshot.edc VeraBd.ttf e_modules-screenshot.spec

# the module .so file
INCLUDES = -I. \
			  -I$(top_srcdir) \
			  -I$(includedir) \
			   @e_cflags@

pkgdir                 =  $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c
			 
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

screenshot.edj: Makefile screenshot.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/screenshot.edc \
	 $(top_builddir)/screenshot.edj

clean-local:
	 rm -rf *.edj *~
