AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@COMMON_CFLAGS@

bin_PROGRAMS = wakerd waker

wakerd_SOURCES = \
waker_main.c
wakerd_LDADD = @my_libs@ @COMMON_LIBS@

waker_SOURCES = \
waker_tool.c
waker_LDADD = @my_libs@ @COMMON_LIBS@
