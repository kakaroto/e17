# $Id$

bindir = $(EBIN)

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Time.epplet E-Net.epplet E-Cpu.epplet EppletTest.epplet E-Biff.epplet

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

EppletTest_epplet_SOURCES = TestEpplet.c
EppletTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Biff_epplet_SOURCES = E-Biff.c mbox.c
E_Biff_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Biff_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

INCLUDES = -I. -I$(top_srcdir)/api -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include $(X_CFLAGS)
LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

ABOUT_DOCS = E-Cpu.ABOUT/MAIN E-Cpu.ABOUT/bg.png E-Cpu.ABOUT/aircut3.ttf \
E-Clock.ABOUT/MAIN E-Clock.ABOUT/bg.png E-Clock.ABOUT/aircut3.ttf
ICONS = E-Load.icon E-Clock.icon E-Net.icon E-Cpu.icon E-Biff.icon
IMAGES = E-Clock-Image.png nomail.png newmail.png

EXTRA_DIST = ${ICONS} ${ABOUT_DOCS} ${IMAGES}



install-data-hook:
	$(mkinstalldirs) $(EROOT)/epplet_icons
	@for i in ${ICONS}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	done
	@for i in ${IMAGES}; do \
	  echo $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	  $(INSTALL_DATA) $(srcdir)/$$i $(EROOT)/epplet_icons/ ; \
	done
	@for i in ${ABOUT_DOCS}; do \
	  echo $(INSTALL_DATA) -D $(srcdir)/$$i $(EROOT)/epplet_icons/$$i ; \
	  $(INSTALL_DATA) -D $(srcdir)/$$i $(EROOT)/epplet_icons/$$i ; \
	done
