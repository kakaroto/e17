# $Id$

bindir = $(EBIN)

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Time.epplet E-Net.epplet \
E-Cpu.epplet EppletTest.epplet E-Biff.epplet E-Power.epplet EMix.epplet

E_Load_epplet_SOURCES = E-Load.c
E_Load_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Load_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Clock_epplet_SOURCES = E-Clock.c
E_Clock_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Clock_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Time_epplet_SOURCES = E-Time.c
E_Time_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Time_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Net_epplet_SOURCES = E-Net.c
E_Net_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Net_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Cpu_epplet_SOURCES = E-Cpu.c
E_Cpu_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Cpu_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Power_epplet_SOURCES = E-Power.c
E_Power_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Power_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EppletTest_epplet_SOURCES = TestEpplet.c
EppletTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Biff_epplet_SOURCES = E-Biff.c mbox.c
E_Biff_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Biff_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EMix_epplet_SOURCES = Emix.c
EMix_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EMix_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)


INCLUDES = -I. -I$(top_srcdir)/api -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include $(X_CFLAGS)
LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

ABOUT_DOCS = \
E-Cpu.ABOUT/MAIN E-Cpu.ABOUT/bg.png E-Cpu.ABOUT/aircut3.ttf \
E-Clock.ABOUT/MAIN E-Clock.ABOUT/bg.png E-Clock.ABOUT/aircut3.ttf \
E-Power.ABOUT/MAIN E-Power.ABOUT/bg.png E-Power.ABOUT/aircut3.ttf \
Emix.ABOUT/MAIN Emix.ABOUT/bg.png Emix.ABOUT/aircut3.ttf \

ICONS = \
E-Load.icon E-Clock.icon E-Net.icon E-Cpu.icon E-Power.icon E-Biff.icon \
Emix.icon

IMAGES =\
E-Clock-Image.png E-Power-Bat-0.png E-Power-Bat-10.png E-Power-Bat-20.png \
E-Power-Bat-30.png E-Power-Bat-40.png E-Power-Bat-50.png E-Power-Bat-60.png \
E-Power-Bat-70.png E-Power-Bat-80.png E-Power-Bat-90.png E-Power-Bat-100.png \
nomail.png newmail.png

EXTRA_DIST = ${ICONS} ${ABOUT_DOCS} ${IMAGES}

install-data-hook:
	$(mkinstalldirs) $(EROOT)/epplet_icons
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-Cpu.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-Clock.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/E-Power.ABOUT/
	$(mkinstalldirs) $(EROOT)/epplet_icons/EMix.ABOUT/
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
