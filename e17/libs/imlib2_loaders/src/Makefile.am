## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

INCLUDES             = -I.. $(X_CFLAGS) -W -Wall

pkgdir		     = $(libdir)/loaders/image

if EDB_LOADER
EDB_L               = db.la
endif

pkg_LTLIBRARIES      = xcf.la $(EDB_L)

xcf_la_SOURCES       = \
	loader_xcf.c \
	loader_xcf_pixelfuncs.c \
	image.h \
	common.h \
	color_values.h
xcf_la_LDFLAGS	     = -no-undefined -module -avoid-version
xcf_la_LIBADD	     =

db_la_SOURCES       = \
	loader_db.c \
    common.h \
    image.h
db_la_LDFLAGS	     = -no-undefined -module -avoid-version
db_la_LIBADD	     = -ledb
