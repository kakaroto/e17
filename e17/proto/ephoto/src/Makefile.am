## Process this file with automake to produce Makefile.in 

SUBDIRS = bin

bin_PROGRAMS = ephoto

ephoto_SOURCES = ephoto.c ephoto_callbacks.c ephoto_slideshow.c ephoto_viewer.c ephoto_database.c

ephoto_CFLAGS = @ewl_cflags@

ephoto_LDADD =  $(INTLLIBS) \
		@ewl_libs@ -lsqlite3

EXTRA_DIST = \
	     ephoto.h
