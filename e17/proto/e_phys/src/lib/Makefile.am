INCLUDES =  \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@

lib_LTLIBRARIES = libephys.la

libephys_la_SOURCES = \
	E_Phys.h \
	e_phys_constraint.c \
	e_phys_force.c \
	e_phys_particle.c \
	e_phys_world.c

libephys__la_LDADD = @EVAS_LIBS@ @ECORE_LIBS@
libephys_la_LIBADD = -lm
