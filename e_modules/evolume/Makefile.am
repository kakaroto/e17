MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = src data patches po

EXTRA_DIST=TODO TODO.rus COPYING COPYING-PLAIN autogen.sh m4/*.m4 evolume.spec evolume.spec.in

# data files for the module
filesdir = @e_modules@/evolume/

unload:
	enlightenment_remote -module-disable evolume $(DISPLAY)
	enlightenment_remote -module-unload evolume $(DISPLAY)

load:
	enlightenment_remote -module-load evolume $(DISPLAY)
	enlightenment_remote -module-enable evolume $(DISPLAY)

test:
	Xnest -ac :1 &
	/usr/local/e17/bin/enlightenment -display :1
	export DISPLAY=:1
	make -C . load

reload: unload load
