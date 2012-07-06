MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFL_CFLAGS@

EXTRA_DIST = \
	     libclouseau.h \
	     eet_data.h

pkgdir = $(libdir)/clouseau
pkg_LTLIBRARIES = libclouseau.la libclouseau_client.la

libclouseau_la_SOURCES = \
			 libclouseau.c \
			 obj_information.c \
			 eet_data.c

libclouseau_la_LDFLAGS = -module -avoid-version -rdynamic
libclouseau_la_DEPENDENCIES = $(top_builddir)/config.h
libclouseau_la_LIBADD = @EFL_LIBS@
libclouseau_la_CFLAGS = @EFL_CFLAGS@

libclouseau_client_la_SOURCES = obj_information.c \
				eet_data.c

libclouseau_client_la_DEPENDENCIES = $(top_builddir)/config.h
libclouseau_client_la_LIBADD = @EFL_LIBS@
libclouseau_client_la_CFLAGS = @EFL_CFLAGS@
libclouseau_client_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@ @release_info@

