MAINTAINERCLEANFILES = Makefile.in

EET_EET = @eet_eet@

EXTRA_DIST = \
	e.src \
	module.battery.src \
	module.illume.src \
	module.dropshadow.src \
	module.mixer.src \
	module.pager.src
	
filesdir = $(edatadir)/config/illume
files_DATA = \
	e.cfg \
	module.battery.cfg \
	module.illume.cfg \
	module.dropshadow.cfg \
	module.mixer.cfg \
	module.pager.cfg

%.cfg: %.src
	$(EET_EET) -e \
	$(top_builddir)/config/illume/$@ config \
	$< 1

clean-local:
	rm -rf *.cfg *~
	
