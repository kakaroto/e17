# $Id$

bindir = $(EBIN)

bin_PROGRAMS = E-Load.epplet E-Clock.epplet E-Net.epplet E-Cpu.epplet EppletTest.epplet

E_Load_epplet_SOURCES = E-Load.c
E_Load_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Load_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Clock_epplet_SOURCES = E-Clock.c
E_Clock_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Clock_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Net_epplet_SOURCES = E-Net.c
E_Net_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Net_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

E_Cpu_epplet_SOURCES = E-Cpu.c
E_Cpu_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
E_Cpu_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

EppletTest_epplet_SOURCES = TestEpplet.c
EppletTest_epplet_DEPENDENCIES = $(top_srcdir)/api/libepplet.la
EppletTest_epplet_LDFLAGS = -rpath $(libdir):$(pkglibdir)

INCLUDES = -I. -I$(top_srcdir)/api -I$(top_srcdir) -I.. -I$(includedir) -I$(prefix)/include $(X_CFLAGS)
LDADD = $(top_builddir)/api/libepplet.la -L$(libdir) -L$(prefix)/lib $(LIBS) $(X_LIBS)

EXTRA_DIST = E-Load.icon E-Clock.icon E-Net.icon E-Cpu.icon EppletTest.icon

install-data-hook:
	$(mkinstalldirs) $(EROOT)/epplet_icons
	@for i in $(srcdir)/*.icon ; do \
	  $(INSTALL_DATA) $$i $(EROOT)/epplet_icons/ ; \
	done
