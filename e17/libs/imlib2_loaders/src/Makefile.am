## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

INCLUDES             = -DXCF_DBG $(X_CFLAGS)

pkgdir		     = $(libdir)/loaders/image
pkg_LTLIBRARIES	     = xcf.la

xcf_la_SOURCES       = \
	loader_xcf.c \
	loader_xcf_pixelfuncs.c \
	image.h \
	common.h \
	color_values.h
xcf_la_LDFLAGS	     = -no-undefined -module -avoid-version
xcf_la_LIBADD	     =
