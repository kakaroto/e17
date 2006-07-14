MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

filesdir = $(datadir)
files_DATA = module_icon.png \
	     module.eap \
	     tclock.edj

EXTRA_DIST = $(files_DATA) e_modules-tclock.spec
CONFIG_CLEAN_FILES = e_modules-tclock.spec tclock.edj

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@
pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/tclock.edj

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@
