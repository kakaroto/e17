MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
@ETCH_CFLAGS@

lib_LTLIBRARIES = libetch.la

libetch_la_SOURCES = \
etch.c \
etch_time.c \
etch_animation.c \
etch_interpolator_argb.c \
etch_interpolator_uint32.c

libetch_la_LIBADD = -lm @ETCH_LIBS@

libetch_la_DEPENDENCIES =

libetch_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@
