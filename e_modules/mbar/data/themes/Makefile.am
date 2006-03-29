MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
fonts/VeraBd.ttf \
mbar.edj \
mbar.edc

EXTRA_DIST = $(files_DATA) $(wildcard images/*.png)

mbar.edj: Makefile mbar.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 mbar.edc \
	 mbar.edj

clean-local:
	rm -f *.edj
