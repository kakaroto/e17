CFLAGS_X      = @x_cflags@
CFLAGS_EXTRA  = -I$(includedir) -I$(top_srcdir) -I$(top_srcdir)/src -I/usr/local/include

CFLAGS_ALL    = $(CFLAGS_X) $(CFLAGS_EXTRA)

SUBDIRS       = fnt img style

INCLUDES = $(CFLAGS_ALL) \
           -DLIBDIR=\"$(libdir)\" \
	   -DBINDIR=\"$(bindir)\" \
	   -DDATADIR=\"$(datadir)\"
								     
bin_PROGRAMS = etox_test

etox_test_SOURCES = etox_test.c

etox_test_LDADD = $(top_builddir)/src/libetox.la

