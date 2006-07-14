MAINTAINERCLEANFILES = Makefile.in mkinstalldirs

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir =  $(datadir)

files_DATA = module_icon.png \
			module.eap \
			mail.edj

EXTRA_DIST = $(files_DATA) \
			mail.edc \
			checking.png \
			newmail.png \
			VeraBd.ttf \
			autogen.sh

# the module .so file
INCLUDES = -I. \
			  -I$(top_srcdir) \
			  -I$(includedir) \
			   @e_cflags@

pkgdir                 =  $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c \
			 e_mod_config_box.h \
			 e_mod_config_box.c \
			 imap.h imap.c \
			 pop.h pop.c \
			 mdir.h mdir.c \
			 mbox.h mbox.c
			 
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/mail.edj

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf *.edj *~
