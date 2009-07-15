## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
	 -Wall


bin_PROGRAMS = ecorexx_example_timers

ecorexx_example_timers_SOURCES = \
	main.cpp	

ecorexx_example_timers_LDADD = \
	$(ECOREXX_LIBS) \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la
	
ecorexx_example_timers_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(ECOREXX_CFLAGS)

