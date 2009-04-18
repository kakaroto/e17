MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
@EMAGE_CFLAGS@

lib_LTLIBRARIES = libemage.la

libemage_la_SOURCES = \
emage.c
## add every .c module that is going to be built-in

libemage_la_LIBADD = \
@EMAGE_LIBS@

libemage_la_DEPENDENCIES =

libemage_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@
