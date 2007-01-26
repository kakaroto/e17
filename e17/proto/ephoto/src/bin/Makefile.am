## Process this file with automake to produce Makefile.in 

bin_PROGRAMS = ephoto_test

ephoto_test_SOURCES = ephoto.c \
		      ephoto_browsing.c \
		      ephoto_database.c \
		      ephoto_gui.c \
		      ephoto_imaging.c \
		      ephoto_nls.c 
		     
ephoto_test_CFLAGS = @ewl_cflags@

ephoto_test_LDADD =  @ewl_libs@ -lsqlite3

EXTRA_DIST = \
	     ephoto.h
