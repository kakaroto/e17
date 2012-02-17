AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra -Wshadow \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)/$(PACKAGE)/$(MODULE_ARCH)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@ELEV8_CFLAGS@


bin_PROGRAMS = elev8

elev8_SOURCES = \
	main.cc \
	elev8_utils.cc \
	downloader.cc \
	module.cc

elev8_LDADD = @ELEV8_LIBS@ @V8_LIBS@

