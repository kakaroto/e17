# $Id$

bindir = $(EBIN)

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Time.epplet E-Net.epplet \
E-Cpu.epplet EppletTest.epplet E-Biff.epplet E-Power.epplet Emix.epplet \
E-MemWatch.epplet E-Disk.epplet E-Areas.epplet EppletConfigTest.epplet \
E-NetFlame.epplet E-Sys.epplet E-ScreenShoot.epplet

E_Load_epplet_SOURCES = E-Load.c
E_Load_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Load_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Clock_epplet_SOURCES = E-Clock.c
E_Clock_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Clock_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

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

E_Power_epplet_SOURCES = E-Power.c
E_Power_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Power_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EppletTest_epplet_SOURCES = TestEpplet.c
EppletTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EppletConfigTest_epplet_SOURCES = ConfigTestEpplet.c
EppletConfigTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletConfigTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Biff_epplet_SOURCES = E-Biff.c mbox.c
E_Biff_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Biff_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_MemWatch_epplet_SOURCES = E-MemWatch.c
E_MemWatch_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_MemWatch_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Disk_epplet_SOURCES = E-Disk.c
E_Disk_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Disk_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Sys_epplet_SOURCES = E-Sys.c
E_Sys_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Sys_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

Emix_epplet_SOURCES = Emix.c
Emix_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
Emix_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_ScreenShoot_epplet_SOURCES = \
E-ScreenShoot.c E-ScreenShoot.h cloak.c utils.c utils.h cloak.h
E_ScreenShoot_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_ScreenShoot_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Areas_epplet_SOURCES = E-Areas.c
E_Areas_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Areas_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

INCLUDES = -I. -I$(top_srcdir)/api -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include $(X_CFLAGS)
LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

ABOUT_DOCS = \
E-Cpu.ABOUT/MAIN E-Cpu.ABOUT/bg.png E-Cpu.ABOUT/aircut3.ttf \
E-Clock.ABOUT/MAIN E-Clock.ABOUT/bg.png E-Clock.ABOUT/aircut3.ttf \
E-Power.ABOUT/MAIN E-Power.ABOUT/bg.png E-Power.ABOUT/aircut3.ttf \
E-NetFlame.ABOUT/MAIN E-NetFlame.ABOUT/bg.png E-NetFlame.ABOUT/aircut3.ttf \
Emix.ABOUT/MAIN Emix.ABOUT/bg.png Emix.ABOUT/aircut3.ttf \
E-ScreenShoot.ABOUT/MAIN E-ScreenShoot.ABOUT/bg.png \
E-ScreenShoot.ABOUT/aircut3.ttf \
E-ScreenShoot.ABOUT/E_ScreenShoot_buttons.png \
E-ScreenShoot.ABOUT/E_ScreenShoot_cloak.png


ICONS = \
E-Load.icon E-Clock.icon E-Net.icon E-Cpu.icon E-Power.icon E-Biff.icon \
Emix.icon E-NetFlame.icon E-ScreenShoot.icon

IMAGES =\
E-Clock-Image.png E-Power-Bat-0.png E-Power-Bat-10.png E-Power-Bat-20.png \
E-Power-Bat-30.png E-Power-Bat-40.png E-Power-Bat-50.png E-Power-Bat-60.png \
E-Power-Bat-70.png E-Power-Bat-80.png E-Power-Bat-90.png E-Power-Bat-100.png \
nomail.png newmail.png 7of9.png \
E-ScreenShoot_root.png E-ScreenShoot_col.png \
E-ScreenShoot_shoot.png E-ScreenShoot_win.png E-ScreenShoot_time.png \
E-ScreenShoot_minitime.png E-ScreenShoot_minitime2.png

EXTRA_DIST = ${ICONS} ${ABOUT_DOCS} ${IMAGES}

install-data-hook:
	$(mkinstalldirs) $(EROOT)/epplet_icons
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-Cpu.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-Clock.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-Power.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/Emix.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-NetFlame.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-ScreenShoot.ABOUT/
	@for i in ${ICONS}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	done
	@for i in ${IMAGES}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	done
	@for i in ${ABOUT_DOCS}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/$$i ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/$$i ; \
	done
