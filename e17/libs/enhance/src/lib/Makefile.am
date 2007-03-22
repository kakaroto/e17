AUTOMAKE_OPTIONS       = 1.7 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

lib_LTLIBRARIES = libenhance.la

enhance_headers = Enhance.h

libenhance_la_INCLUDES = \
enhance_private.h \
enhance_widget.h \
$(enhance_headers)

libenhance_la_SOURCES = \
enhance.c \
enhance_widget.c \
$(libenhance_la_INCLUDES)

installed_headersdir = $(prefix)/include
installed_headers_DATA = $(enhance_headers)

libenhance_la_CFLAGS = @ENHANCE_CFLAGS@
libenhance_la_LDFLAGS = -export-dynamic
libenhance_la_LIBADD = $(INTLLIBS) @ENHANCE_LIBS@
