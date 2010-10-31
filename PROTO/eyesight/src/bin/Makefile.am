
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/modules/img \
-I$(top_srcdir)/src/modules/pdf \
-I$(top_srcdir)/src/modules/ps \
-I$(top_srcdir)/src/modules/txt \
@EYESIGHT_CFLAGS@ \
@EYESIGHT_BIN_CFLAGS@

bin_PROGRAMS =

if BUILD_EYESIGHT

bin_PROGRAMS += eyesight

endif

eyesight_SOURCES = eyesight.c

eyesight_LDADD = @EYESIGHT_BIN_LIBS@ $(top_builddir)/src/lib/libeyesight.la
