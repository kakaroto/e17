## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CXXFLAGS =\
	 -Wall


bin_PROGRAMS = ecorexx_example_calibrate

ecorexx_example_calibrate_SOURCES = \
	main.cpp \
	calibrate.cpp \
	calibrate.h

ecorexx_example_calibrate_LDADD = \
	$(ECOREXX_LIBS)\
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la
	
ecorexx_example_calibrate_DEPENDENCIES = \
	$(PACKAGE_SOURCE_DIR)/src/common/libeflxx_example_common.la

AM_CPPFLAGS = \
	$(ECOREXX_CFLAGS)

