MAINTAINERCLEANFILES = Makefile.in
MODULE = emu

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images \
-fd $(top_srcdir)/data/fonts

# data files for the module
filesdir = $(libdir)/enlightenment/modules/$(MODULE)
files_DATA = module.eap

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
                         -I$(top_srcdir) \
                         -I$(includedir) \
                         -I$(top_srcdir)$(MODULE) \
                         -I$(top_srcdir)/src/bin \
                         -I$(top_srcdir)/src/lib \
                         -I$(top_srcdir)/src/modules \
                         @e_cflags@
pkgdir                 = $(libdir)/enlightenment/modules/$(MODULE)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
                         e_mod_main.h \
			 easy_menu.c \
			 easy_menu.h \
			 border_props.c \
			 border_props.h \
                         e_mod_config.c \
                         e_mod_config.h
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

module.eap: Makefile module_edje.edc module_face.edc module_icon.edc
	$(EDJE_CC) $(EDJE_FLAGS) module_edje.edc module.eap
	enlightenment_eapp module.eap \
	-set-name "Emu" \
	-set-generic "gadget" \
	-set-comment "Experimental generic scriptable module for E17" \
	-set-exe "enlightenment" \
	-set-icon-class "emu,gadget"

clean-local:
	 rm -rf *.eap *~
