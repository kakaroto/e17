MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ENESIM_CFLAGS@ \
@EXAMPLE_CFLAGS@

if BUILD_EXAMPLE

bin_PROGRAMS = enesim_bench

enesim_bench_DEPENDENCIES = $(top_builddir)/src/lib/libenesim.la
enesim_bench_LDADD = \
$(top_builddir)/src/lib/libenesim.la \
@ENESIM_LIBS@ \
@EXAMPLE_LIBS@

enesim_bench_SOURCES = \
enesim_bench.c \
image.c

endif
