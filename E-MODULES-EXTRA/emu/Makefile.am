ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src images po

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/images

filesdir = $(datadir)
files_DATA = e-module-emu.edj module.desktop

EXTRA_DIST = module.desktop.in \
	     EMU_PROTOCOL.txt \
	     emu_client \
	     e_modules-emu.spec.in \
	     $(wildcard *.edc)

e-module-emu.edj: Makefile module_edje.edc module_face.edc module_icon.edc
		  $(EDJE_CC) $(EDJE_FLAGS) \
		  $(top_srcdir)/src/modules/emu/module_edje.edc \
		  e-module-emu.edj

bindir = $(datadir)/bin
bin_SCRIPTS = emu_client

clean-local:
	 rm -rf e_modules-emu.spec e-module-emu.edj module.desktop *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
