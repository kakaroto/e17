MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ENESIM_CFLAGS@ \
@EXAMPLE_CFLAGS@

if EFL_ENABLE_BENCHMARK

bin_PROGRAMS = enesim_bench

enesim_bench_DEPENDENCIES = $(top_builddir)/src/lib/libenesim.la
enesim_bench_LDADD = \
$(top_builddir)/src/lib/libenesim.la \
@ENESIM_LIBS@ \
@EXAMPLE_LIBS@

enesim_bench_SOURCES = \
enesim_bench.c \
enesim_bench_samples.c \
enesim_bench_compositor.c \
enesim_bench_rasterizer.c \
enesim_bench_renderer.c \
enesim_bench_checker.c \
enesim_bench_stripes.c \
enesim_bench_hswitch.c \
enesim_bench_dispmap.c \
enesim_bench_raddist.c \
enesim_bench_shape.c \
enesim_bench_circle.c \
enesim_bench_ellipse.c \
enesim_bench_rectangle.c \
image.c

endif
