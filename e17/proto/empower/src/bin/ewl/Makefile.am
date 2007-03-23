## Process this file with automake to produce Makefile.in 

#INCLUDES = \
#-I$(top_srcdir) -I$(top_srcdir)/src/include -I$(top_srcdir)/src/lib \
#-I$(top_builddir)/src/lib

if BUILD_SEPARATE
bin_PROGRAMS = empower_ewl

empower_ewl_SOURCES = empower.c empower_cb_ewl.c empower_gui_ewl.c

empower_ewl_CFLAGS = @EWL_CFLAGS@

empower_ewl_LDADD =  $(INTLLIBS) \
		@EWL_LIBS@
else
bin_PROGRAMS = empower

empower_SOURCES = empower.c empower_cb_ewl.c empower_gui_ewl.c

empower_CFLAGS = @EWL_CFLAGS@

empower_LDADD =  $(INTLLIBS) \
		@EWL_LIBS@
endif 

EXTRA_DIST = 
		
MAINTAINERCLEANFILES = Makefile.in config.h.in
