if FSSTD
ENLIGHTENMENT_BIN=@bindir@
ENLIGHTENMENT_ROOT=$(pkgdatadir)
escriptsdatadir = @bindir@
else
escriptsdatadir = $(prefix)/enlightenment/bin
ENLIGHTENMENT_BIN=$(prefix)/enlightenment/bin
ENLIGHTENMENT_ROOT=$(prefix)/enlightenment
endif

escriptsdata_DATA =
escriptsdata_SCRIPTS = e_gen_kde_menu e_gen_menu e_gen_gnome_menu \
e_cache_clean e_cache_query enlightenment.install e_gen_menu.pl \
e_cache_clean.pl e_cache_query.pl

EXTRA_DIST = e_gen_kde_menu e_gen_menu e_gen_gnome_menu e_cache_clean \
e_cache_query enlightenment.install.in e_gen_menu.pl \
e_cache_clean.pl e_cache_query.pl
EXTRA_SCRIPTS = e_gen_kde_menu e_gen_menu e_gen_gnome_menu e_cache_clean \
e_cache_query enlightenment.install e_gen_menu.pl \
e_cache_clean.pl e_cache_query.pl

#if FSSTD
#
#else
#install-exec-hook:
#	rm -f @bindir@/enlightenment.install; \
#	$(LN_S) \
#	$(DESTDIR)$(escriptsdatadir)/enlightenment.install \
#	/enlightenment.install
#endif
