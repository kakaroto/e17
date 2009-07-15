## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/ecore/ \
	 -I$(top_srcdir)/src/evas/ \
	 -I$(top_srcdir)/src/edje/ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_evas_list

eflpp_evas_list_SOURCES = \
	main.cpp	

eflpp_evas_list_LDFLAGS = @PACKAGE_LIBS@

eflpp_evas_list_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_evas_list_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
