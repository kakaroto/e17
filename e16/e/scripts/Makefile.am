if FSSTD
escriptsdatadir = @bindir@
else
escriptsdatadir = $(prefix)/enlightenment/bin
endif

escriptsdata_DATA =
escriptsdata_SCRIPTS = e_gen_kde_menu e_gen_menu e_gen_gnome_menu \
e_cache_clean e_cache_query enlightenment.install

EXTRA_DIST = e_gen_kde_menu e_gen_menu e_gen_gnome_menu e_cache_clean \
e_cache_query enlightenment.install.in
EXTRA_SCRIPTS = e_gen_kde_menu e_gen_menu e_gen_gnome_menu e_cache_clean \
e_cache_query enlightenment.install

#if FSSTD
#
#else
#install-exec-hook:
#	rm -f @bindir@/enlightenment.install; \
#	$(LN_S) \
#	$(DESTDIR)$(escriptsdatadir)/enlightenment.install \
#	/enlightenment.install
#endif
