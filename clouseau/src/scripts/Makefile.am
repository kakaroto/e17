MAINTAINERCLEANFILES = Makefile.in

uninstall-local:
	rm $(bindir)/clouseau

install-exec-local:
	./gen_script.sh $(libdir)/clouseau
	$(INSTALL) clouseau $(bindir)

