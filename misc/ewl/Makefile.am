## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS           = -L/usr/X11R6/lib -L/usr/lib -L/usr/local/lib
INCLUDES          = -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I.

docs_DATA = README AUTHORS ChangeLog TODO
docsdir = $(prefix)/doc/ewl

EXTRA_DIST = \
$(docs_DATA)

SUBDIRS = src

