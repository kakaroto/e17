MAINTAINERCLEANFILES = Makefile.in

EET_EET = @eet_eet@

EXTRA_DIST = \
	e.src \
	module.battery.src \
	module.illume.src
	
filesdir = $(edatadir)/config/asu
files_DATA = \
	e.cfg \
	module.battery.cfg \
	module.illume.cfg

%.cfg: %.src
	$(EET_EET) -e \
	$(top_builddir)/config/asu/$@ config \
	$< 1

clean-local:
	rm -rf *.cfg *~
	
