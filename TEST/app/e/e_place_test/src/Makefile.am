## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	$(E_PLACE_TEST_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = e_place_test

e_place_test_SOURCES = \
	main.cpp      \
	searchFile.cpp      \
	searchFile.h      \
	Application.cpp      \
	Application.h      \
	TestPlaceWindow.cpp      \
	TestPlaceWindow.h      \
	TestWindow.cpp      \
	TestWindow.h      \
	TestGravityWindow.cpp      \
	TestGravityWindow.h      \
	TestStateWindow.cpp      \
	TestStateWindow.h      \
	TestDnDWindow.cpp      \
	TestDnDWindow.h      \
	TestResizeWindow.cpp      \
	TestResizeWindow.h

e_place_test_LDFLAGS = 

e_place_test_LDADD = $(E_PLACE_TEST_LIBS)

# Remove glade directory on uninstall
uninstall-local:
	-rm -r $(datadir)/e_place_test
