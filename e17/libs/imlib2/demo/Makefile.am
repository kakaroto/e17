## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS              = -L/usr/X11R6/lib
INCLUDES             = -I/usr/X11R6/include -I$(top_srcdir)/libltdl \
                       $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders

bin_PROGRAMS         = imlib2_view
imlib2_view_SOURCES  = view.c
imlib2_view_LDADD    = @DLLDFLAGS@ $(top_builddir)/libltdl/libltdlc.la \
                       -lX11 -lXext -lttf -lImlib2

