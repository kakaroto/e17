# $Id$

bindir = $(EBIN)

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Time.epplet E-Net.epplet \
E-Cpu.epplet E-Biff.epplet E-Power.epplet @TEST_PROGS@ @MIXER_PROGS@ \
E-MemWatch.epplet E-Disk.epplet E-Areas.epplet \
E-NetFlame.epplet E-Sys.epplet E-ScreenShoot.epplet E-Slides.epplet \
E-Xss.epplet E-Mountbox.epplet E-Exec.epplet E-SD.epplet \
E-ScreenSave.epplet E-NetGraph.epplet E-MoonClock.epplet

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

E_Areas_epplet_SOURCES = E-Areas.c 
E_Areas_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Areas_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Net_epplet_SOURCES = E-Net.c
E_Net_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Net_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_NetFlame_epplet_SOURCES = E-NetFlame.c
E_NetFlame_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_NetFlame_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Cpu_epplet_SOURCES = E-Cpu.c
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

E_NetGraph_epplet_SOURCES = E-NetGraph.c
E_NetGraph_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_NetGraph_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_ScreenSave_epplet_SOURCES = \
E-ScreenSave.c E-ScreenSave.h cloak.c utils.c utils.h cloak.h
E_ScreenSave_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_ScreenSave_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Areas_epplet_SOURCES = E-Areas.c
E_Areas_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Areas_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Mountbox_epplet_SOURCES = E-Mountbox.c E-Mountbox.h
E_Mountbox_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Mountbox_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

INCLUDES = -I. -I$(top_builddir)/api -I$(top_srcdir)/api -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include -I/usr/local/include $(X_CFLAGS) @SGI_DEFINES@
LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib -L/usr/local/lib $(LIBS) $(X_LIBS)

ABOUT_DOCS = \
E-Cpu.ABOUT/MAIN E-Cpu.ABOUT/bg.png E-Cpu.ABOUT/aircut3.ttf \
E-Clock.ABOUT/MAIN E-Clock.ABOUT/bg.png E-Clock.ABOUT/aircut3.ttf \
E-Power.ABOUT/MAIN E-Power.ABOUT/bg.png E-Power.ABOUT/aircut3.ttf \
E-NetFlame.ABOUT/MAIN E-NetFlame.ABOUT/bg.png E-NetFlame.ABOUT/aircut3.ttf \
Emix.ABOUT/MAIN Emix.ABOUT/bg.png Emix.ABOUT/aircut3.ttf \
E-ScreenShoot.ABOUT/MAIN E-ScreenShoot.ABOUT/bg.png \
E-ScreenShoot.ABOUT/aircut3.ttf \
E-ScreenShoot.ABOUT/E_ScreenShoot_buttons.png \
E-ScreenShoot.ABOUT/E_ScreenShoot_cloak.png \
E-Mountbox.ABOUT/MAIN E-Mountbox.ABOUT/bg.png E-Mountbox.ABOUT/aircut3.ttf \
E-Mountbox.ABOUT/shot.png E-Mountbox.ABOUT/shot_unmounted.png \
E-Mixer.ABOUT/MAIN E-Mixer.ABOUT/aircut3.ttf E-Mixer.ABOUT/bg.png \
E-SD.ABOUT/MAIN E-SD.ABOUT/aircut3.ttf E-SD.ABOUT/bg.png \
E-ScreenSave.ABOUT/MAIN E-ScreenSave.ABOUT/bg.png \
E-ScreenSave.ABOUT/aircut3.ttf E-ScreenSave.ABOUT/MAIN \
E-ScreenSave.ABOUT/bg.png E-ScreenSave.ABOUT/aircut3.ttf

ICONS = \
E-Load.icon E-Clock.icon E-Net.icon E-Cpu.icon E-Power.icon E-Biff.icon \
Emix.icon E-NetFlame.icon E-ScreenShoot.icon E-Slides.icon E-Disk.icon \
E-MemWatch.icon E-Sys.icon E-Time.icon E-Xss.icon E-Mountbox.icon \
E-Mixer.icon E-SD.icon E-ScreenSave.icon E-MoonClock.icon

IMAGES =\
E-Clock-Image.png E-Power-Bat-0.png E-Power-Bat-10.png E-Power-Bat-20.png \
E-Power-Bat-30.png E-Power-Bat-40.png E-Power-Bat-50.png E-Power-Bat-60.png \
E-Power-Bat-70.png E-Power-Bat-80.png E-Power-Bat-90.png E-Power-Bat-100.png \
nomail.png newmail.png 7of9.png \
E-ScreenShoot_root.png E-ScreenShoot_col.png \
E-ScreenShoot_shoot.png E-ScreenShoot_win.png E-ScreenShoot_time.png \
E-ScreenShoot_minitime.png E-ScreenShoot_minitime2.png \
E-Mountbox-bg.png E-Mountbox-blockdev.png E-Mountbox-cd.png \
E-Mountbox-floppy.png E-Mountbox-zip.png \
E-SD_minitime.png E-SD_standby.png \
E-MoonClock-01.png  E-MoonClock-16.png  E-MoonClock-31.png  E-MoonClock-46.png \
E-MoonClock-02.png  E-MoonClock-17.png  E-MoonClock-32.png  E-MoonClock-47.png \
E-MoonClock-03.png  E-MoonClock-18.png  E-MoonClock-33.png  E-MoonClock-48.png \
E-MoonClock-04.png  E-MoonClock-19.png  E-MoonClock-34.png  E-MoonClock-49.png \
E-MoonClock-05.png  E-MoonClock-20.png  E-MoonClock-35.png  E-MoonClock-50.png \
E-MoonClock-06.png  E-MoonClock-21.png  E-MoonClock-36.png  E-MoonClock-51.png \
E-MoonClock-07.png  E-MoonClock-22.png  E-MoonClock-37.png  E-MoonClock-52.png \
E-MoonClock-08.png  E-MoonClock-23.png  E-MoonClock-38.png  E-MoonClock-53.png \
E-MoonClock-09.png  E-MoonClock-24.png  E-MoonClock-39.png  E-MoonClock-54.png \
E-MoonClock-10.png  E-MoonClock-25.png  E-MoonClock-40.png  E-MoonClock-55.png \
E-MoonClock-11.png  E-MoonClock-26.png  E-MoonClock-41.png  E-MoonClock-56.png \
E-MoonClock-12.png  E-MoonClock-27.png  E-MoonClock-42.png  E-MoonClock-57.png \
E-MoonClock-13.png  E-MoonClock-28.png  E-MoonClock-43.png  E-MoonClock-58.png \
E-MoonClock-14.png  E-MoonClock-29.png  E-MoonClock-44.png  E-MoonClock-59.png \
E-MoonClock-15.png  E-MoonClock-30.png  E-MoonClock-45.png  E-MoonClock-60.png


EXTRA_DIST = ${ICONS} ${ABOUT_DOCS} ${IMAGES} proc.h

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_data
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-Cpu.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-Clock.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-Power.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/Emix.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-NetFlame.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-ScreenShoot.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-ScreenSave.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-Mixer.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-SD.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-Mountbox.ABOUT/
	$(mkinstalldirs) $(DESTDIR)$(EROOT)/epplet_icons/E-MoonClock.ABOUT/
	@for i in ${ICONS}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(EROOT)/epplet_icons/ ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(EROOT)/epplet_icons/ ; \
	done
	@for i in ${IMAGES}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(EROOT)/epplet_data/ ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(EROOT)/epplet_data/ ; \
	done
	@for i in ${ABOUT_DOCS}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/$$i ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(EROOT)/epplet_icons/$$i ; \
	done
