## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in feh.spec

LDFLAGS           = -L/usr/X11R6/lib -L/usr/lib -L/usr/local/lib
INCLUDES          = -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I.

# `ll -1d src/testimages/*` and `ll -1d src/testimages/pngs/*`
testimages = src/testimages/donna01.jpg src/testimages/donna02.jpg \
src/testimages/donna03.jpg src/testimages/donna04.jpg src/testimages/heidi.jpg \
src/testimages/nell01.jpg src/testimages/nell02.jpg src/testimages/nell03.jpg \
src/testimages/nell04.jpg src/testimages/nell05.jpg src/testimages/nell06.jpg \
src/testimages/nell07.jpg src/testimages/nell08.jpg src/testimages/nurding01.jpg \
src/testimages/nurding02.jpg src/testimages/nurding03.jpg src/testimages/nurding04.jpg \
src/testimages/nurding05.jpg src/testimages/nurding06.jpg src/testimages/nurding07.jpg \
src/testimages/nurding08.jpg src/testimages/nurding09.jpg src/testimages/nurding10.jpg \
src/testimages/nurding11.jpg src/testimages/nurding12.jpg src/testimages/nurding13.jpg \
src/testimages/nurding14.jpg src/testimages/nurding15.jpg src/testimages/nurding16.jpg \
src/testimages/spears02.jpg src/testimages/spears03.jpg \
src/testimages/spears04.jpg src/testimages/spears05.jpg src/testimages/spears06.jpg \
src/testimages/spears07.jpg src/testimages/spears08.jpg src/testimages/spears09.jpg \
src/testimages/spears10.jpg src/testimages/spears11.jpg src/testimages/spears12.jpg \
src/testimages/spears13.jpg src/testimages/spears14.jpg src/testimages/spears20.jpg \
src/testimages/spears21.jpg src/testimages/spears22.jpg src/testimages/pngs/bg.png \
src/testimages/pngs/bulb.png src/testimages/pngs/globe.png src/testimages/pngs/mail.png \
src/testimages/pngs/mush.png src/testimages/pngs/paper.png src/testimages/pngs/elogo.png

# `ll -1d src/test*`
testscripts = \
src/test01_montage_background_defaults \
src/test02_montage_background_ignore_aspect \
src/test03_multiple_images \
src/test04_montage_no_bg_restrict_width \
src/test05_single_image \
src/test06_bork_non_existant_files \
src/test07_bork_non_image_files \
src/test08_montage_pngs_on_pngs \
src/test09_bork_bg_image_too_small \
src/test10_montage_no_bg_recursive \
src/test11_montage_background_blend \
src/test12_evil_montage_create_massive_image \
src/test13_montage_pngs_on_pngs_stretch \
src/test14_index_no_bg_defaults \
src/test15_slideshow_multiple_images \
src/test16_index_no_bg_title \
src/test17_evil_many_many_multiple_windows \
src/test18_slideshow_no_progressive_loading \
src/test19_montage_background_quiet \
src/test20_stress_test_create_massive_image_save_only

man_MANS = feh.1

docs_DATA = example.fehrc
docsdir = $(prefix)/doc/feh

EXTRA_DIST = \
README AUTHORS ChangeLog TODO feh.spec feh.1 $(docs_DATA)

SUBDIRS = src

test:
	(cd src && $(MAKE) test)

testclean:
	(cd src && $(MAKE) testclean)
