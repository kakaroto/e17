MAINTAINERCLEANFILES = Makefile.in

uninstall-local:
	rm $(bindir)/exactness 

install-exec-local:
	./write.sh $(libdir)/exactness/
	$(INSTALL) exactness $(bindir)
