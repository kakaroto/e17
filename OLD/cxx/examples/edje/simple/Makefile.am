## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/ecore/ \
	 -I$(top_srcdir)/src/evas/ \
	 -I$(top_srcdir)/src/edje/ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_edje_simple

eflpp_edje_simple_SOURCES = \
	main.cpp	

eflpp_edje_simple_LDFLAGS = @PACKAGE_LIBS@

eflpp_edje_simple_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_edje_simple_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
