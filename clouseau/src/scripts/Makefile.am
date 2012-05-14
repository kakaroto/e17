MAINTAINERCLEANFILES = Makefile.in

uninstall-local:
	rm $(bindir)/clouseau_start

install-exec-local:
	./gen_script.sh $(libdir)/clouseau
	$(INSTALL) clouseau_start $(bindir)

