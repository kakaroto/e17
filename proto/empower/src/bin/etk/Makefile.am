## Process this file with automake to produce Makefile.in 

#INCLUDES = \
#-I$(top_srcdir) -I$(top_srcdir)/src/include -I$(top_srcdir)/src/lib \
#-I$(top_builddir)/src/lib

if BUILD_SEPARATE
bin_PROGRAMS = empower_etk

empower_etk_SOURCES = empower.c \
		empower_cb_etk.c empower_gui_etk.c

empower_etk_CFLAGS = @ETK_CFLAGS@

empower_etk_LDADD =  $(INTLLIBS) \
		@ETK_LIBS@
else
bin_PROGRAMS = empower

empower_SOURCES = empower.c \
		empower_cb_etk.c empower_gui_etk.c

empower_CFLAGS = @ETK_CFLAGS@

empower_LDADD =  $(INTLLIBS) \
		@ETK_LIBS@
endif

EXTRA_DIST = 
		
MAINTAINERCLEANFILES = Makefile.in config.h.in
