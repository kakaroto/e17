## Process this file with automake to produce Makefile.in

bin_PROGRAMS = efsdsh

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile efsdsh

INCLUDES = -I$(top_srcdir)/efsd

efsdsh_SOURCES = efsdsh.c
efsdsh_LDADD = @ecore_libs@ $(top_builddir)/efsd/.libs/libefsd.so

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	if test -e filetypes.xml; then \
	  $(INSTALL_DATA) filetypes.xml $(DESTDIR)$(pkgdatadir)/filetypes.xml; \
	fi
	if test -e filetypes.dtd; then \
	  $(INSTALL_DATA) filetypes.dtd $(DESTDIR)$(pkgdatadir)/filetypes.dtd; \
	fi

EXTRA_DIST = filetypes.xml filetypes.dtd
