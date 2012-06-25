MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = gen_script.sh

uninstall-local:
	rm $(bindir)/clouseau
	rm $(bindir)/clouseau_start

install-exec-local:
	$(srcdir)/gen_script.sh $(libdir)/clouseau
	$(INSTALL) clouseau_start $(bindir)
	$(INSTALL) clouseau $(bindir)

