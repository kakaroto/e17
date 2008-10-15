MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @E_CFLAGS@ @ENOTIFY_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = e_mod_main.h \
		    e_mod_main.c \
		    e_mod_macro.h \
		    e_mod_types.h \
		    e_mod_box.h \
		    e_mod_box.c \
		    e_mod_popup.h \
		    e_mod_popup.c \
		    e_mod_config.c \
		    e_mod_config_item.h \
		    e_mod_config_item.c

module_la_LIBADD = @E_LIBS@ @ENOTIFY_LIBS@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	rm -rf *~
