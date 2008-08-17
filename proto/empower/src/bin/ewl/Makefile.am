## Process this file with automake to produce Makefile.in 

#INCLUDES = \
#-I$(top_srcdir) -I$(top_srcdir)/src/include -I$(top_srcdir)/src/lib \
#-I$(top_builddir)/src/lib

if BUILD_SEPARATE
bin_PROGRAMS = empower_ewl

empower_ewl_SOURCES = empower.c empower_cb_ewl.c empower_gui_ewl.c \
	empower_helper_ewl.c Empower.h

empower_ewl_CFLAGS = @EWL_CFLAGS@

empower_ewl_LDADD =  $(INTLLIBS) \
		@EWL_LIBS@
		
install-exec-hook:
			rm -f $(DESTDIR)$(bindir)/empower-askpass && \
			cd $(DESTDIR)$(bindir) && \
            $(LN_S) empower_ewl empower-askpass

else
bin_PROGRAMS = empower

empower_SOURCES = empower.c empower_cb_ewl.c empower_gui_ewl.c \
					empower_helper_ewl.c

empower_CFLAGS = @EWL_CFLAGS@

empower_LDADD =  $(INTLLIBS) \
		@EWL_LIBS@
		
install-exec-hook:
			rm -f $(DESTDIR)$(bindir)/empower-askpass && \
			cd $(DESTDIR)$(bindir) && \
            $(LN_S) empower empower-askpass
endif 

uninstall-hook:
			rm -f $(DESTDIR)$(bindir)/empower-askpass

EXTRA_DIST = 

MAINTAINERCLEANFILES = Makefile.in config.h.in
