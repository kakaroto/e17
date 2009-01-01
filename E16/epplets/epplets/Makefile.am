
edatadir = $(datadir)/e16

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Time.epplet E-Net.epplet \
E-Cpu.epplet E-Biff.epplet E-Power.epplet \
E-MemWatch.epplet E-Disk.epplet E-Areas.epplet E-Magic.epplet \
E-Toolbox.epplet E-NetFlame.epplet E-Sys.epplet E-ScreenShoot.epplet \
E-Slides.epplet E-Xss.epplet E-Mountbox.epplet E-Exec.epplet \
E-ScreenSave.epplet E-NetGraph.epplet E-MoonClock.epplet \
E-UrlWatch.epplet E-LoadMeter.epplet E-Pinger.epplet \
E-Pants.epplet E-Bandwidth.epplet E-Wireless.epplet \
@TEST_PROGS@ @MIXER_PROGS@ @ESD_PROGS@ @PLAYCD_PROGS@ @GLX_PROGS@

EXTRA_PROGRAMS = EppletTest.epplet EppletConfigTest.epplet E-Mixer.epplet \
                 Emix.epplet E-PlayCD.epplet E-OpenGL-Demo.epplet \
		 E-SD.epplet

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/api $(IMLIB2_CFLAGS) $(X_CFLAGS)
LDADD = $(top_builddir)/api/libepplet.la $(X_LIBS) $(KSTAT_LIBS)

################ Conditional Builds ################
E_Mixer_epplet_SOURCES = E-Mixer.c
E_Mixer_epplet_CPPFLAGS = $(INCLUDES) @SGI_DEFINES@
E_Mixer_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Mixer_epplet_LDFLAGS = -rpath $(libdir)
E_Mixer_epplet_LDADD = $(LDADD) @SGI_LIBS@

Emix_epplet_SOURCES = Emix.c
Emix_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
Emix_epplet_LDFLAGS = -rpath $(libdir)

E_PlayCD_epplet_SOURCES = E-PlayCD.c
E_PlayCD_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_PlayCD_epplet_LDFLAGS = -rpath $(libdir)
E_PlayCD_epplet_LDADD = $(LDADD) -lcdaudio

EppletTest_epplet_SOURCES = TestEpplet.c
EppletTest_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
EppletTest_epplet_LDFLAGS = -rpath $(libdir)

EppletConfigTest_epplet_SOURCES = ConfigTestEpplet.c
EppletConfigTest_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
EppletConfigTest_epplet_LDFLAGS = -rpath $(libdir)

E_OpenGL_Demo_epplet_SOURCES = E-OpenGL-Demo.c
E_OpenGL_Demo_epplet_DEPENDENCIES = \
	$(top_builddir)/api/libepplet.la $(top_builddir)/api/libepplet_glx.la
E_OpenGL_Demo_epplet_LDFLAGS = -rpath $(libdir)
E_OpenGL_Demo_epplet_LDADD = $(top_builddir)/api/libepplet_glx.la $(LDADD)
####################################################

E_Exec_epplet_SOURCES = E-Exec.c
E_Exec_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Exec_epplet_LDFLAGS = -rpath $(libdir)

E_SD_epplet_SOURCES = E-SD.c esdcloak.c utils.c utils.h esdcloak.h
E_SD_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_SD_epplet_LDFLAGS = -rpath $(libdir) -lesd

E_Load_epplet_SOURCES = E-Load.c
E_Load_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Load_epplet_LDFLAGS = -rpath $(libdir)
E_Load_epplet_LDADD = $(LDADD) $(GTLIBS)

E_LoadMeter_epplet_SOURCES = E-LoadMeter.c load.c
E_LoadMeter_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_LoadMeter_epplet_LDFLAGS = -rpath $(libdir)
E_LoadMeter_epplet_LDADD = $(LDADD) $(GTLIBS)

E_Clock_epplet_SOURCES = E-Clock.c
E_Clock_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Clock_epplet_LDFLAGS = -rpath $(libdir)

E_MoonClock_epplet_SOURCES = E-MoonClock.c CalcEphem.c CalcEphem.h Moon.c
E_MoonClock_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_MoonClock_epplet_LDFLAGS = -rpath $(libdir)

E_Time_epplet_SOURCES = E-Time.c
E_Time_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Time_epplet_LDFLAGS = -rpath $(libdir)

E_Magic_epplet_SOURCES = E-Magic.c
E_Magic_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Magic_epplet_LDFLAGS = -rpath $(libdir)

E_Toolbox_epplet_SOURCES = E-Toolbox.c
E_Toolbox_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Toolbox_epplet_LDFLAGS = -rpath $(libdir)

E_Areas_epplet_SOURCES = E-Areas.c 
E_Areas_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Areas_epplet_LDFLAGS = -rpath $(libdir)

E_Bandwidth_epplet_SOURCES = E-Bandwidth.c net.c net.h
E_Bandwidth_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Bandwidth_epplet_LDFLAGS = -rpath $(libdir)

E_Net_epplet_SOURCES = E-Net.c net.c net.h
E_Net_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Net_epplet_LDFLAGS = -rpath $(libdir)

E_NetFlame_epplet_SOURCES = E-NetFlame.c net.c net.h
E_NetFlame_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_NetFlame_epplet_LDFLAGS = -rpath $(libdir)

E_Wireless_epplet_SOURCES = E-Wireless.c
E_Wireless_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Wireless_epplet_LDFLAGS = -rpath $(libdir)

E_Cpu_epplet_SOURCES = E-Cpu.c proc.h
E_Cpu_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Cpu_epplet_LDFLAGS = -rpath $(libdir)
E_Cpu_epplet_LDADD = $(LDADD) $(GTLIBS)

E_Power_epplet_SOURCES = E-Power.c
E_Power_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Power_epplet_LDFLAGS = -rpath $(libdir)

E_Biff_epplet_SOURCES = E-Biff.c mbox.c
E_Biff_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Biff_epplet_LDFLAGS = -rpath $(libdir)

E_MemWatch_epplet_SOURCES = E-MemWatch.c
E_MemWatch_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_MemWatch_epplet_LDFLAGS = -rpath $(libdir)
E_MemWatch_epplet_LDADD = $(LDADD) $(GTLIBS)

E_Disk_epplet_SOURCES = E-Disk.c
E_Disk_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Disk_epplet_LDFLAGS = -rpath $(libdir)

E_Sys_epplet_SOURCES = E-Sys.c
E_Sys_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Sys_epplet_LDFLAGS = -rpath $(libdir)
E_Sys_epplet_LDADD = $(LDADD) $(GTLIBS)

E_Slides_epplet_SOURCES = E-Slides.c
E_Slides_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Slides_epplet_LDFLAGS = -rpath $(libdir)

E_Xss_epplet_SOURCES = E-Xss.c
E_Xss_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Xss_epplet_LDFLAGS = -rpath $(libdir)

E_ScreenShoot_epplet_SOURCES = \
E-ScreenShoot.c cloak.c utils.c utils.h cloak.h \
E-ScreenShoot_ftp.c E-ScreenShoot_ftp.h
E_ScreenShoot_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_ScreenShoot_epplet_LDFLAGS = -rpath $(libdir)

E_NetGraph_epplet_SOURCES = E-NetGraph.c net.c net.h
E_NetGraph_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_NetGraph_epplet_LDFLAGS = -rpath $(libdir)

E_ScreenSave_epplet_SOURCES = \
E-ScreenSave.c cloak.c utils.c utils.h cloak.h
E_ScreenSave_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_ScreenSave_epplet_LDFLAGS = -rpath $(libdir)

E_UrlWatch_epplet_SOURCES = E-UrlWatch.c E-UrlWatch.h utils.c utils.h
E_UrlWatch_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_UrlWatch_epplet_LDFLAGS = -rpath $(libdir)

E_Mountbox_epplet_SOURCES = E-Mountbox.c
E_Mountbox_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Mountbox_epplet_LDFLAGS = -rpath $(libdir)

E_Pinger_epplet_SOURCES = E-Pinger.c
E_Pinger_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Pinger_epplet_LDFLAGS = -rpath $(libdir)

E_Pants_epplet_SOURCES = E-Pants.c
E_Pants_epplet_DEPENDENCIES = $(top_builddir)/api/libepplet.la
E_Pants_epplet_LDFLAGS = -rpath $(libdir)

# You can use the following command to generate this line:
# ABOUT_DOCS = `(cd epplets && ls -1d *.ABOUT/* 2>/dev/null | grep -v CVS)`
ABOUT_DOCS = E-Clock.ABOUT/MAIN \
E-Cpu.ABOUT/MAIN \
Emix.ABOUT/MAIN \
E-Mixer.ABOUT/E-Mixer-standard.png \
E-Mixer.ABOUT/E-Mixer-tall.png \
E-Mixer.ABOUT/E-Mixer-wide.png \
E-Mixer.ABOUT/MAIN \
E-MoonClock.ABOUT/MAIN \
E-Mountbox.ABOUT/dialog.png \
E-Mountbox.ABOUT/MAIN \
E-Mountbox.ABOUT/shot.png \
E-Mountbox.ABOUT/shot_unmounted.png \
E-NetFlame.ABOUT/MAIN \
E-OpenGL-Demo.ABOUT/MAIN \
E-Pants.ABOUT/MAIN \
E-Pinger.ABOUT/MAIN \
E-PlayCD.ABOUT/E-PlayCD_Controls.png \
E-PlayCD.ABOUT/EPlayCD.jpg \
E-PlayCD.ABOUT/MAIN \
E-Power.ABOUT/MAIN \
E-ScreenSave.ABOUT/MAIN \
E-ScreenShoot.ABOUT/E_ScreenShoot_buttons.png \
E-ScreenShoot.ABOUT/E_ScreenShoot_cloak.png \
E-ScreenShoot.ABOUT/MAIN \
E-SD.ABOUT/MAIN \
E-UrlWatch.ABOUT/MAIN \
E-Wireless.ABOUT/delia.jpg \
E-Wireless.ABOUT/delia.png \
E-Wireless.ABOUT/MAIN

# ICONS = `(cd epplets && ls -1d icons/*.icon | grep -v CVS)`
ICONS = icons/E-Biff.icon icons/E-Clock.icon icons/E-Cpu.icon \
icons/E-Disk.icon icons/E-Exec.icon icons/E-Load.icon \
icons/E-MemWatch.icon icons/E-Mixer.icon \
icons/E-MoonClock.icon icons/E-Mountbox.icon icons/E-Net.icon \
icons/E-NetFlame.icon icons/E-Power.icon icons/E-SD.icon \
icons/E-ScreenSave.icon icons/E-ScreenShoot.icon icons/E-Slides.icon \
icons/E-Sys.icon icons/E-Time.icon icons/E-Xss.icon icons/Emix.icon \
icons/E-UrlWatch.icon icons/E-Magic.icon icons/E-PlayCD.icon \
icons/E-OpenGL-Demo.icon icons/E-Pinger.icon \
icons/E-Pinger.icon.bg icons/E-Pants.icon icons/E-Wireless.icon

# DATA_FILES = `(cd epplets && ls -1d *-data/* 2>/dev/null | grep -v CVS)`

DATA_FILES = E-Biff-data/7of9.png E-Biff-data/newmail.png \
E-Biff-data/nomail.png E-Clock-data/E-Clock-Image.png \
E-MoonClock-data/E-MoonClock-01.png E-MoonClock-data/E-MoonClock-02.png \
E-MoonClock-data/E-MoonClock-03.png E-MoonClock-data/E-MoonClock-04.png \
E-MoonClock-data/E-MoonClock-05.png E-MoonClock-data/E-MoonClock-06.png \
E-MoonClock-data/E-MoonClock-07.png E-MoonClock-data/E-MoonClock-08.png \
E-MoonClock-data/E-MoonClock-09.png E-MoonClock-data/E-MoonClock-10.png \
E-MoonClock-data/E-MoonClock-11.png E-MoonClock-data/E-MoonClock-12.png \
E-MoonClock-data/E-MoonClock-13.png E-MoonClock-data/E-MoonClock-14.png \
E-MoonClock-data/E-MoonClock-15.png E-MoonClock-data/E-MoonClock-16.png \
E-MoonClock-data/E-MoonClock-17.png E-MoonClock-data/E-MoonClock-18.png \
E-MoonClock-data/E-MoonClock-19.png E-MoonClock-data/E-MoonClock-20.png \
E-MoonClock-data/E-MoonClock-21.png E-MoonClock-data/E-MoonClock-22.png \
E-MoonClock-data/E-MoonClock-23.png E-MoonClock-data/E-MoonClock-24.png \
E-MoonClock-data/E-MoonClock-25.png E-MoonClock-data/E-MoonClock-26.png \
E-MoonClock-data/E-MoonClock-27.png E-MoonClock-data/E-MoonClock-28.png \
E-MoonClock-data/E-MoonClock-29.png E-MoonClock-data/E-MoonClock-30.png \
E-MoonClock-data/E-MoonClock-31.png E-MoonClock-data/E-MoonClock-32.png \
E-MoonClock-data/E-MoonClock-33.png E-MoonClock-data/E-MoonClock-34.png \
E-MoonClock-data/E-MoonClock-35.png E-MoonClock-data/E-MoonClock-36.png \
E-MoonClock-data/E-MoonClock-37.png E-MoonClock-data/E-MoonClock-38.png \
E-MoonClock-data/E-MoonClock-39.png E-MoonClock-data/E-MoonClock-40.png \
E-MoonClock-data/E-MoonClock-41.png E-MoonClock-data/E-MoonClock-42.png \
E-MoonClock-data/E-MoonClock-43.png E-MoonClock-data/E-MoonClock-44.png \
E-MoonClock-data/E-MoonClock-45.png E-MoonClock-data/E-MoonClock-46.png \
E-MoonClock-data/E-MoonClock-47.png E-MoonClock-data/E-MoonClock-48.png \
E-MoonClock-data/E-MoonClock-49.png E-MoonClock-data/E-MoonClock-50.png \
E-MoonClock-data/E-MoonClock-51.png E-MoonClock-data/E-MoonClock-52.png \
E-MoonClock-data/E-MoonClock-53.png E-MoonClock-data/E-MoonClock-54.png \
E-MoonClock-data/E-MoonClock-55.png E-MoonClock-data/E-MoonClock-56.png \
E-MoonClock-data/E-MoonClock-57.png E-MoonClock-data/E-MoonClock-58.png \
E-MoonClock-data/E-MoonClock-59.png E-MoonClock-data/E-MoonClock-60.png \
E-Mountbox-data/E-Mountbox-bg.png E-Mountbox-data/E-Mountbox-blockdev.png \
E-Mountbox-data/E-Mountbox-cd.png E-Mountbox-data/E-Mountbox-floppy.png \
E-Mountbox-data/E-Mountbox-zip.png E-Mountbox-data/E-Mountbox-jazz.png \
E-Power-data/E-Power-Bat-0.png E-Power-data/E-Power-Bat-10.png \
E-Power-data/E-Power-Bat-100.png E-Power-data/E-Power-Bat-20.png \
E-Power-data/E-Power-Bat-30.png E-Power-data/E-Power-Bat-40.png \
E-Power-data/E-Power-Bat-50.png E-Power-data/E-Power-Bat-60.png \
E-Power-data/E-Power-Bat-70.png E-Power-data/E-Power-Bat-80.png \
E-Power-data/E-Power-Bat-90.png E-SD-data/E-SD_minitime.png \
E-SD-data/E-SD_standby.png \
E-ScreenShoot-data/E-ScreenShoot_col.png \
E-ScreenShoot-data/E-ScreenShoot_minitime.png \
E-ScreenShoot-data/E-ScreenShoot_minitime2.png \
E-ScreenShoot-data/E-ScreenShoot_root.png \
E-ScreenShoot-data/E-ScreenShoot_shoot.png \
E-ScreenShoot-data/E-ScreenShoot_time.png \
E-ScreenShoot-data/E-ScreenShoot_win.png \
Emix-data/Emix.AUTHORS Emix-data/Emix.COPYING \
Emix-data/Emix.README Emix-data/Emix.TODO E-UrlWatch-data/wooeep.wav \
E-Magic-data/answers.txt E-Magic-data/8ball.png \
E-OpenGL-Demo-data/cube_texture.RGB \
E-Toolbox-data/eterm.png E-Toolbox-data/mail.png E-Toolbox-data/netscape.png \
E-Toolbox-data/ee2.png E-Toolbox-data/folder.png E-Toolbox-data/linux.png \
E-Toolbox-data/sample.cfg E-Toolbox-data/popup-sample.cfg \
E-Toolbox-data/default.cfg

EXTRA_DIST = ${ICONS} ${ABOUT_DOCS} ${DATA_FILES}

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(edatadir)/epplet_icons
	$(mkinstalldirs) $(DESTDIR)$(edatadir)/epplet_data
	@for i in `echo $(bin_PROGRAMS) | sed 's/\.epplet//g'`; do \
	  if test -d $${i}-data ; then \
	    echo $(mkinstalldirs) $(DESTDIR)$(edatadir)/epplet_data/$$i ; \
	    $(mkinstalldirs) $(DESTDIR)$(edatadir)/epplet_data/$$i ; \
	    data=`echo $$i-data/*`;\
	    echo $(INSTALL_DATA) $$data $(DESTDIR)$(edatadir)/epplet_data/$$i/ ; \
	    $(INSTALL_DATA) $$data $(DESTDIR)$(edatadir)/epplet_data/$$i/ ; \
	  fi ; \
	  if test -d $${i}.ABOUT ; then \
	    echo $(mkinstalldirs) $(DESTDIR)$(edatadir)/epplet_data/$$i/ABOUT ; \
	    $(mkinstalldirs) $(DESTDIR)$(edatadir)/epplet_data/$$i/ABOUT ; \
	    ABOUT=`echo $$i.ABOUT/*`;\
	    echo $(INSTALL_DATA) $$ABOUT $(DESTDIR)$(edatadir)/epplet_data/$$i/ABOUT/ ; \
	    $(INSTALL_DATA) $$ABOUT $(DESTDIR)$(edatadir)/epplet_data/$$i/ABOUT/ ; \
	  fi ; \
	  if test -f icons/$${i}.icon ; then \
	    echo $(INSTALL_DATA) $(srcdir)/icons/$${i}.icon $(DESTDIR)$(edatadir)/epplet_icons/ ; \
	    $(INSTALL_DATA) $(srcdir)/icons/$${i}.icon $(DESTDIR)$(edatadir)/epplet_icons/ ; \
	  fi ; \
	done
