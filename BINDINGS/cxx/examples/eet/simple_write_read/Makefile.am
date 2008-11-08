## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/eet/ \
	 -Wall


bin_PROGRAMS = write_eet read_eet

write_eet_SOURCES = \
	write_eet.cpp
write_eet_LDFLAGS = @PACKAGE_LIBS@
write_eet_LDADD = $(top_builddir)/src/libeflpp.la
write_eet_DEPENDENCIES = $(top_builddir)/src/libeflpp.la


read_eet_SOURCES = \
	read_eet.cpp
read_eet_LDFLAGS = @PACKAGE_LIBS@
read_eet_LDADD = $(top_builddir)/src/libeflpp.la
read_eet_DEPENDENCIES = $(top_builddir)/src/libeflpp.la