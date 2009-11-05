MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@EKEKO_CFLAGS@

bin_PROGRAMS = ekeko_custom_value

ekeko_custom_value_DEPENDENCIES = $(top_builddir)/src/lib/libekeko.la
ekeko_custom_value_LDADD = $(top_builddir)/src/lib/libekeko.la \
@EKEKO_LIBS@

ekeko_custom_value_SOURCES = ekeko_custom_value.c
