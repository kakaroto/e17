AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-Wall -Wextra \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)/$(PACKAGE)/$(MODULE_ARCH)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@EINA_CFLAGS@ \
@ECORE_CFLAGS@

bin_PROGRAMS = elev8

elev8libdir = $(libdir)/elev8
elev8lib_SCRIPTS = init.js

elev8_SOURCES = \
	main.cc \
	timer.cc \
	elev8_utils.cc

elev8_LDADD = @EINA_LIBS@ @ECORE_LIBS@ @V8_LIBS@

