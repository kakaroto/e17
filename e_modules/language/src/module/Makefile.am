INCLUDES		= -I. \
			  -I$(top_srcdir) \
		  	  -I$(includedir) \
		  	  @e_cflags@ @EXML_CFLAGS@

pkgdir			= $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES		= module.la
module_la_SOURCES	= e_mod_main.c \
			  e_mod_main.h \
			  e_mod_config.c \
			  e_mod_config.h \
			  e_mod_keybindings.c \
			  e_mod_keybindings.h \
			  e_mod_lang.c \
			  e_mod_lang.h
module_la_LIBADD	= @e_libs@ @EXML_LIBS@ -lxkbfile -lX11
module_la_LDFLAGS	= -module -avoid-version
module_la_DEPENDENCIES	= $(top_builddir)/config.h

clean-local:
	rm -rf *~
