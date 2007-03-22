INCLUDES = @EPHYS_CFLAGS@

lib_LTLIBRARIES = libephys.la

include_HEADERS = E_Phys.h

libephys_la_SOURCES = \
	E_Phys.h \
	e_phys_constraint.c \
	e_phys_force.c \
	e_phys_particle.c \
	e_phys_world.c \
	e_phys_map.c

libephys_la_LIBADD = @EPHYS_LIBS@ -lm
