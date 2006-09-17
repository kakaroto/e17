INCLUDES =  \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ \
-g -Werror

lib_LTLIBRARIES = libephys.la

libephys_la_SOURCES = \
	E_Phys.h \
	e_phys_constraint.c \
	e_phys_force.c \
	e_phys_particle.c \
	e_phys_world.c

libephys_la_LIBADD = @EVAS_LIBS@ @ECORE_LIBS@ -lm
