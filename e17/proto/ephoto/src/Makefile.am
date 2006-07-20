## Process this file with automake to produce Makefile.in 

INCLUDES = \
-I$(top_srcdir) -I$(top_srcdir)/src/include -I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib

bin_PROGRAMS = ephoto

ephoto_SOURCES = ephoto.c ephoto_callbacks.c

ephoto_CFLAGS = @ewl_cflags@

ephoto_LDADD =  $(INTLLIBS) \
		@ewl_libs@ -lsqlite3

EXTRA_DIST = \
	     ephoto.h
