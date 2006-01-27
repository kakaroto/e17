## Process this file with automake to produce Makefile.in

SUBDIRS = src data

bin_SCRIPTS = emu_client

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

dist-hook:
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for file in data/*; do \
	    cp -pR $$file $(distdir)/data; \
	  done; \
	fi; \
	pushd $(distdir); \
	rm -f $(MAINTAINERCLEANFILES) Makefile depcomp; \
	rm -fR */Makefile */*/Makefile */*/*/Makefile; \
	rm -fR */Makefile.in */*/Makefile.in */*/*/Makefile.in; \
	rm -fR */.svn */*/.svn */*/*/.svn; \
	rm -fR src/modules/*/*.edj; \
	popd;
							  

EXTRA_DIST = m4 AUTHORS COPYING COPYING-PLAIN EMU_PROTOCOL.txt INSTALL NEWS README autogen.sh emu_client gdb.txt rebuildit xnest.sh
