MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
mount.edj \
mount.edc

EXTRA_DIST = $(files_DATA)

mount.edj: Makefile mount.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 mount.edc \
	 mount.edj

clean-local:
	rm -f *.edj
