## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_debug_simple

eflpp_debug_simple_SOURCES = \
	main.cpp	

eflpp_debug_simple_LDFLAGS = @PACKAGE_LIBS@

eflpp_debug_simple_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_debug_simple_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
