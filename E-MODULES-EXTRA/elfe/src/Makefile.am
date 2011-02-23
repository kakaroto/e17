MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	   -I$(top_srcdir) \
	   -I$(includedir) \
	   @E_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = main.h \
		    main.c \
                    elfe_config.c \
		    elfe_config.h \
                    allapps.c \
		    allapps.h \
		    desktop.c \
		    desktop.h \
		    gadgets.c \
		    gadgets.h \
		    utils.c \
		    utils.h \
		    winlist.c \
		    winlist.h

module_la_LIBADD = @E_LIBS@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	rm -rf *~
