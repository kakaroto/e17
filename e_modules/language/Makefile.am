MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

EXTRA_DIST = $(files_DATA) language.png

# data files for the module
filesdir = $(datadir)
files_DATA = \
module_icon.png \
language.edj \
language.edc

# the module .so file
INCLUDES               = -I. \
                         -I$(top_srcdir) \
                         -I$(includedir) \
                         @e_cflags@

pkgdir		       = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
                         e_mod_main.h \
                         e_mod_config.c \
                         e_mod_config.h \
			 e_mod_keybind.h \
			 e_mod_keybind.c \
			 e_mod_lang.h \
			 e_mod_lang.c
module_la_LIBADD       = @e_libs@ -lexml
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

language.edj: Makefile language.edc
	$(EDJE_CC) $(EDJE_FLAGS) language.edc language.edj

clean-local:
	rm -rf *.edj *~
