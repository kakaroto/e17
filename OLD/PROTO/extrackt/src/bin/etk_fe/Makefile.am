## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.7 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in 

INCLUDES = 	-I$(top_srcdir)/src/bin\
		@ETK_CFLAGS@ @ENHANCE_CFLAGS@ @my_includes@

noinst_LIBRARIES = libetk_fe.a

libetk_fe_a_SOURCES = gui.c \
		      gui.h 
