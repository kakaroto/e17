MAINTAINERCLEANFILES = Makefile.in

uninstall-local:
	rm $(bindir)/clouseau
	rm $(bindir)/clouseau_start

install-exec-local:
	$(srcdir)/gen_script.sh $(libdir)/clouseau
	$(INSTALL) clouseau_start $(bindir)
	$(INSTALL) clouseau $(bindir)

