# $Id$

bindir = $(EBIN)

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Time.epplet E-Net.epplet \
E-Cpu.epplet E-Biff.epplet E-Power.epplet @TEST_PROGS@ @MIXER_PROGS@ \
E-MemWatch.epplet E-Disk.epplet E-Areas.epplet E-Magic.epplet \
E-NetFlame.epplet E-Sys.epplet E-ScreenShoot.epplet E-Slides.epplet \
E-Xss.epplet E-Mountbox.epplet E-Exec.epplet E-SD.epplet \
E-ScreenSave.epplet E-NetGraph.epplet E-MoonClock.epplet E-UrlWatch.epplet \
E-PlayCD.epplet

EXTRA_PROGRAMS = EppletTest.epplet EppletConfigTest.epplet E-Mixer.epplet Emix.epplet

################ Conditional Builds ################
E_Mixer_epplet_SOURCES = E-Mixer.c
E_Mixer_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Mixer_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)
E_Mixer_epplet_LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS) @SGI_LIBS@

Emix_epplet_SOURCES = Emix.c
Emix_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
Emix_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EppletTest_epplet_SOURCES = TestEpplet.c
EppletTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EppletConfigTest_epplet_SOURCES = ConfigTestEpplet.c
EppletConfigTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletConfigTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)
####################################################

E_Exec_epplet_SOURCES = E-Exec.c
E_Exec_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Exec_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_PlayCD_epplet_SOURCES = E-PlayCD.c
E_PlayCD_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_PlayCD_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir) -lcdaudio

E_SD_epplet_SOURCES = E-SD.c E-SD.h esdcloak.c utils.c utils.h esdcloak.h
E_SD_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_SD_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir) -lesd

E_Load_epplet_SOURCES = E-Load.c
E_Load_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Load_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)
E_Load_epplet_LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS) $(GTLIBS)

E_Clock_epplet_SOURCES = E-Clock.c
E_Clock_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Clock_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_MoonClock_epplet_SOURCES = E-MoonClock.c CalcEphem.c CalcEphem.h Moon.c
E_MoonClock_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_MoonClock_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Time_epplet_SOURCES = E-Time.c
E_Time_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Time_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Magic_epplet_SOURCES = E-Magic.c
E_Magic_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Magic_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Areas_epplet_SOURCES = E-Areas.c 
E_Areas_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Areas_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Net_epplet_SOURCES = E-Net.c net.c net.h
E_Net_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Net_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_NetFlame_epplet_SOURCES = E-NetFlame.c net.c net.h
E_NetFlame_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_NetFlame_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Cpu_epplet_SOURCES = E-Cpu.c proc.h
E_Cpu_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Cpu_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)
E_Cpu_epplet_LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS) $(GTLIBS)

E_Power_epplet_SOURCES = E-Power.c
E_Power_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Power_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Biff_epplet_SOURCES = E-Biff.c mbox.c
E_Biff_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Biff_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_MemWatch_epplet_SOURCES = E-MemWatch.c
E_MemWatch_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_MemWatch_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)
E_MemWatch_epplet_LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS) $(GTLIBS)

E_Disk_epplet_SOURCES = E-Disk.c
E_Disk_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Disk_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Sys_epplet_SOURCES = E-Sys.c
E_Sys_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Sys_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)
E_Sys_epplet_LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS) $(GTLIBS)

E_Slides_epplet_SOURCES = E-Slides.c
E_Slides_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Slides_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Xss_epplet_SOURCES = E-Xss.c
E_Xss_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Xss_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_ScreenShoot_epplet_SOURCES = \
E-ScreenShoot.c E-ScreenShoot.h cloak.c utils.c utils.h cloak.h
E_ScreenShoot_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_ScreenShoot_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_NetGraph_epplet_SOURCES = E-NetGraph.c net.c net.h
E_NetGraph_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_NetGraph_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_ScreenSave_epplet_SOURCES = \
E-ScreenSave.c E-ScreenSave.h cloak.c utils.c utils.h cloak.h
E_ScreenSave_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_ScreenSave_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_UrlWatch_epplet_SOURCES = E-UrlWatch.c E-UrlWatch.h utils.c utils.h
E_UrlWatch_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_UrlWatch_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir) $(X_LIBS)
E_UrlWatch_epplet_LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

E_Areas_epplet_SOURCES = E-Areas.c
E_Areas_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Areas_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Mountbox_epplet_SOURCES = E-Mountbox.c E-Mountbox.h
E_Mountbox_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Mountbox_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

INCLUDES = -I. -I$(top_builddir)/api -I$(top_srcdir)/api -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include -I/usr/local/include $(X_CFLAGS) @SGI_DEFINES@
LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib -L/usr/local/lib $(LIBS) $(X_LIBS)

# You can use the following command to generate this line:
# ABOUT_DOCS = `(cd epplets && ls -1d *.ABOUT/* 2>/dev/null | grep -v CVS)`
ABOUT_DOCS = E-Clock.ABOUT/MAIN E-Cpu.ABOUT/MAIN E-Cpu.ABOUT/aircut3.ttf  \
E-Cpu.ABOUT/bg.png E-Mixer.ABOUT/MAIN E-Mixer.ABOUT/E-Mixer-standard.png \
E-Mixer.ABOUT/E-Mixer-tall.png E-Mixer.ABOUT/E-Mixer-wide.png \
E-MoonClock.ABOUT/MAIN E-Mountbox.ABOUT/MAIN E-Mountbox.ABOUT/shot.png \
E-Mountbox.ABOUT/shot_unmounted.png E-NetFlame.ABOUT/MAIN  \
E-Power.ABOUT/MAIN E-SD.ABOUT/MAIN E-ScreenSave.ABOUT/MAIN  \
E-ScreenShoot.ABOUT/E_ScreenShoot_buttons.png  \
E-ScreenShoot.ABOUT/E_ScreenShoot_cloak.png E-ScreenShoot.ABOUT/MAIN \
Emix.ABOUT/MAIN E-UrlWatch.ABOUT/MAIN

# ICONS = `(cd epplets && ls -1d icons/*.icon | grep -v CVS)`
ICONS = icons/E-Biff.icon icons/E-Clock.icon icons/E-Cpu.icon icons/E-Disk.icon \
icons/E-Load.icon icons/E-MemWatch.icon icons/E-Mixer.icon icons/E-MoonClock.icon \
icons/E-Mountbox.icon icons/E-Net.icon icons/E-NetFlame.icon icons/E-Power.icon \
icons/E-SD.icon icons/E-ScreenSave.icon icons/E-ScreenShoot.icon \
icons/E-Slides.icon icons/E-Sys.icon icons/E-Time.icon icons/E-Xss.icon \
icons/Emix.icon icons/E-UrlWatch.icon icons/E-Magic.icon

# DATA_FILES = `(cd epplets && ls -1d *-data/* 2>/dev/null | grep -v CVS)`

DATA_FILES = E-Biff-data/7of9.png E-Biff-data/newmail.png E-Biff-data/nomail.png \
E-Clock-data/E-Clock-Image.png E-MoonClock-data/E-MoonClock-01.png \
E-MoonClock-data/E-MoonClock-02.png E-MoonClock-data/E-MoonClock-03.png \
E-MoonClock-data/E-MoonClock-04.png E-MoonClock-data/E-MoonClock-05.png \
E-MoonClock-data/E-MoonClock-06.png E-MoonClock-data/E-MoonClock-07.png \
E-MoonClock-data/E-MoonClock-08.png E-MoonClock-data/E-MoonClock-09.png \
E-MoonClock-data/E-MoonClock-10.png E-MoonClock-data/E-MoonClock-11.png \
E-MoonClock-data/E-MoonClock-12.png E-MoonClock-data/E-MoonClock-13.png \
E-MoonClock-data/E-MoonClock-14.png E-MoonClock-data/E-MoonClock-15.png \
E-MoonClock-data/E-MoonClock-16.png E-MoonClock-data/E-MoonClock-17.png \
E-MoonClock-data/E-MoonClock-18.png E-MoonClock-data/E-MoonClock-19.png \
E-MoonClock-data/E-MoonClock-20.png E-MoonClock-data/E-MoonClock-21.png \
E-MoonClock-data/E-MoonClock-22.png E-MoonClock-data/E-MoonClock-23.png \
E-MoonClock-data/E-MoonClock-24.png E-MoonClock-data/E-MoonClock-25.png \
E-MoonClock-data/E-MoonClock-26.png E-MoonClock-data/E-MoonClock-27.png \
E-MoonClock-data/E-MoonClock-28.png E-MoonClock-data/E-MoonClock-29.png \
E-MoonClock-data/E-MoonClock-30.png E-MoonClock-data/E-MoonClock-31.png \
E-MoonClock-data/E-MoonClock-32.png E-MoonClock-data/E-MoonClock-33.png \
E-MoonClock-data/E-MoonClock-34.png E-MoonClock-data/E-MoonClock-35.png \
E-MoonClock-data/E-MoonClock-36.png E-MoonClock-data/E-MoonClock-37.png \
E-MoonClock-data/E-MoonClock-38.png E-MoonClock-data/E-MoonClock-39.png \
E-MoonClock-data/E-MoonClock-40.png E-MoonClock-data/E-MoonClock-41.png \
E-MoonClock-data/E-MoonClock-42.png E-MoonClock-data/E-MoonClock-43.png \
E-MoonClock-data/E-MoonClock-44.png E-MoonClock-data/E-MoonClock-45.png \
E-MoonClock-data/E-MoonClock-46.png E-MoonClock-data/E-MoonClock-47.png \
E-MoonClock-data/E-MoonClock-48.png E-MoonClock-data/E-MoonClock-49.png \
E-MoonClock-data/E-MoonClock-50.png E-MoonClock-data/E-MoonClock-51.png \
E-MoonClock-data/E-MoonClock-52.png E-MoonClock-data/E-MoonClock-53.png \
E-MoonClock-data/E-MoonClock-54.png E-MoonClock-data/E-MoonClock-55.png \
E-MoonClock-data/E-MoonClock-56.png E-MoonClock-data/E-MoonClock-57.png \
E-MoonClock-data/E-MoonClock-58.png E-MoonClock-data/E-MoonClock-59.png \
E-MoonClock-data/E-MoonClock-60.png E-Mountbox-data/E-Mountbox-bg.png \
E-Mountbox-data/E-Mountbox-blockdev.png E-Mountbox-data/E-Mountbox-cd.png \
E-Mountbox-data/E-Mountbox-floppy.png E-Mountbox-data/E-Mountbox-zip.png \
E-Mountbox-data/E-Mountbox-jazz.png \
E-Power-data/E-Power-Bat-0.png E-Power-data/E-Power-Bat-10.png \
E-Power-data/E-Power-Bat-100.png E-Power-data/E-Power-Bat-20.png \
E-Power-data/E-Power-Bat-30.png E-Power-data/E-Power-Bat-40.png \
E-Power-data/E-Power-Bat-50.png E-Power-data/E-Power-Bat-60.png \
E-Power-data/E-Power-Bat-70.png E-Power-data/E-Power-Bat-80.png \
E-Power-data/E-Power-Bat-90.png E-SD-data/E-SD_minitime.png E-SD-data/E-SD_standby.png \
E-ScreenShoot-data/E-ScreenShoot_col.png E-ScreenShoot-data/E-ScreenShoot_minitime.png \
E-ScreenShoot-data/E-ScreenShoot_minitime2.png E-ScreenShoot-data/E-ScreenShoot_root.png \
E-ScreenShoot-data/E-ScreenShoot_shoot.png E-ScreenShoot-data/E-ScreenShoot_time.png \
E-ScreenShoot-data/E-ScreenShoot_win.png Emix-data/Emix.AUTHORS Emix-data/Emix.COPYING \
Emix-data/Emix.README Emix-data/Emix.TODO E-UrlWatch-data/wooeep.wav \
E-Magic-data/answers.txt E-Magic-data/8ball.png

EXTRA_DIST = ${ICONS} ${ABOUT_DOCS} ${DATA_FILES}

NEED_FONT = E-Clock E-Mixer E-MoonClock E-Mountbox E-NetFlame E-Power E-SD E-ScreenSave E-ScreenShoot Emix E-UrlWatch
NEED_BG = E-Clock E-Mixer E-MoonClock E-Mountbox E-NetFlame E-Power E-SD E-ScreenSave E-ScreenShoot Emix E-UrlWatch
BG_SRC = E-Cpu.ABOUT/bg.png
FONT_SRC = E-Cpu.ABOUT/aircut3.ttf

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_data
	@for i in `echo $(bin_PROGRAMS) | sed 's/\.epplet//g'`; do \
	  if test -d $${i}-data ; then \
	    $(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_data/$$i ; \
	    data=`echo $$i-data/* | sed s#$$i-data/CVS##`;\
	    echo $(INSTALL_DATA) $$data $(DESTDIR)$(EROOT)/epplet_data/$$i/ ; \
	    $(INSTALL_DATA) $$data $(DESTDIR)$(EROOT)/epplet_data/$$i/ ; \
	  fi ; \
	  if test -d $${i}.ABOUT ; then \
	    $(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT ; \
	    ABOUT=`echo $$i.ABOUT/* | sed s#$$i.ABOUT/CVS##`;\
	    echo $(INSTALL_DATA) $$ABOUT $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT/ ; \
	    $(INSTALL_DATA) $$ABOUT $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT/ ; \
	  fi ; \
	  if test -f icons/$${i}.icon ; then \
	    echo $(INSTALL_DATA) $(srcdir)/icons/$${i}.icon $(DESTDIR)$(EROOT)/epplet_icons/ ; \
	    $(INSTALL_DATA) $(srcdir)/icons/$${i}.icon $(DESTDIR)$(EROOT)/epplet_icons/ ; \
	  fi ; \
	done
	@for i in $(NEED_BG) ; do \
	  echo $(INSTALL_DATA) $(srcdir)/$(BG_SRC) $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT/ ; \
	  $(INSTALL_DATA) $(srcdir)/$(BG_SRC) $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT/ ; \
	done
	@for i in $(NEED_FONT) ; do \
	  echo $(INSTALL_DATA) $(srcdir)/$(FONT_SRC) $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT/ ; \
	  $(INSTALL_DATA) $(srcdir)/$(FONT_SRC) $(DESTDIR)$(EROOT)/epplet_data/$$i/$${i}.ABOUT/ ; \
	done
