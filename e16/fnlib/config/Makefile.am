EXTRA_DIST = fnrc.in
sysconf_DATA = fnrc

fnrc: fnrc.in
	sed -e 's,\@fontdatadir\@,$(fontdatadir),g' \
		< $(srcdir)/fnrc.in > fnrc.tmp \
	&& mv fnrc.tmp fnrc
