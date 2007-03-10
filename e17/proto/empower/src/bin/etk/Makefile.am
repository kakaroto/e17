## Process this file with automake to produce Makefile.in 

#INCLUDES = \
#-I$(top_srcdir) -I$(top_srcdir)/src/include -I$(top_srcdir)/src/lib \
#-I$(top_builddir)/src/lib

if BUILD_SEPARATE
bin_PROGRAMS = empower_etk

empower_etk_SOURCES = empower.c \
		empower_cb_etk.c empower_gui_etk.c

empower_etk_CFLAGS = @etk_cflags@

empower_etk_LDADD =  $(INTLLIBS) \
		@etk_libs@
else
bin_PROGRAMS = empower

empower_SOURCES = empower.c \
		empower_cb_etk.c empower_gui_etk.c

empower_CFLAGS = @etk_cflags@

empower_LDADD =  $(INTLLIBS) \
		@etk_libs@
endif

EXTRA_DIST = 
		
MAINTAINERCLEANFILES = Makefile.in config.h.in