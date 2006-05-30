SUBDIRS = po

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir =  $(datadir)

files_DATA = \
module_icon.png \
module.eap \
slideshow.edj

EXTRA_DIST = $(files_DATA) slideshow.edc

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

module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

slideshow.edj: Makefile slideshow.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 slideshow.edc \
	 slideshow.edj

clean-local:
	 rm -rf *.edj *~
