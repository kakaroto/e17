## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS              = -L/usr/X11R6/lib
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src \
                       -I$(top_srcdir)/loaders -I$(top_srcdir)/libltdl \
                       -I/usr/X11R6/include $(X_CFLAGS) \
                       -I$(prefix)/include -I$(includedir)

if HAVE_X

bin_PROGRAMS         = imlib2_view imconvert
imconvert_SOURCES    = imconvert.c
imlib2_view_SOURCES  = view.c 
LDADD    = $(top_builddir)/src/libImlib2.la

endif

EXTRA_DIST = imconvert.c view.c
