INCLUDES = @EPHYS_CFLAGS@

lib_LTLIBRARIES = libephys.la

EPHYSHEADERS = \
	E_Phys.h \
	e_phys_world.h \
	e_phys_particle.h \
	e_phys_force.h \
	e_phys_constraint.h

libephys_la_SOURCES = \
	e_phys_constraint.c \
	e_phys_force.c \
	e_phys_particle.c \
	e_phys_world.c \
	e_phys_map.c \
	$(EPHYSHEADERS)

installed_headersdir = $(includedir)/e_phys
installed_headers_DATA = $(EPHYSHEADERS)

libephys_la_LIBADD = @EPHYS_LIBS@ -lm
