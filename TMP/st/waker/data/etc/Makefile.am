4AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

filesdir = $(sysconfdir)/X11/Xsession.d
files_DATA = \
80x-enlightenment-wakerd

EXTRA_DIST = $(files_DATA)

install-data-hook:
	 @chmod a+x $(DESTDIR)$(filesdir)/80x-enlightenment-wakerd || true
