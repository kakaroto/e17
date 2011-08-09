AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_FLAGS = -v

filesdir = @datadir_edj@
files_DATA = eenvaders.edj

EXTRA_DIST = eenvaders.edc

eenvaders.edj:  eenvaders.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf eenvaders.edj *~

uninstall:
	 rm -rf $(DESTDIR)@datadir_edj@/eenvaders.edj
