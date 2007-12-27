MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST =

INCLUDES	= \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/util \
@EDATA_CFLAGS@ \
@PNG_CFLAGS@

bin_PROGRAMS = enesim_test

enesim_test_DEPENDENCIES = $(top_builddir)/src/lib/libenesim.la

enesim_test_SOURCES = \
png.c \
surface.c \
enesim_test.c

enesim_test_LDADD = \
$(top_builddir)/src/lib/libenesim.la \
@EDATA_LIBS@ \
@PNG_LIBS@
