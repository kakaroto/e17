## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
         @PACKAGE_CFLAGS@ \
	 -I$(top_srcdir)/src/ecore/ \
	 -I$(top_srcdir)/src/evas/ \
	 -I$(top_srcdir)/src/edje/ \
	 -I$(top_srcdir)/src/common/ \
	 -Wall


bin_PROGRAMS = eflpp_angstrom_bootmanager

eflpp_angstrom_bootmanager_SOURCES = \
	app.cpp \
	app.h \
	calibrate.cpp \
	calibrate.h \
	main.cpp \
	Makefile.am \
	win.cpp \
	win.h

eflpp_angstrom_bootmanager_LDFLAGS = @PACKAGE_LIBS@

eflpp_angstrom_bootmanager_LDADD = $(top_builddir)/src/libeflpp.la

eflpp_angstrom_bootmanager_DEPENDENCIES = $(top_builddir)/src/libeflpp.la
