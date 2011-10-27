include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = ecore-glib

ecore_glib_VALASOURCES = \
	ecore-glib.vala \
	$(NULL)

ecore_glib_SOURCES = \
	$(ecore_glib_VALASOURCES:.vala=.c) \
	$(NULL)

.vala.stamp: $(ecore_glib_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --pkg evas \
	            --pkg ecore \
	            --pkg ecore-evas \
	            --pkg glib-2.0 \
	            --save-temps \
	            --basedir $(top_srcdir) $<
	touch $^

ecore_glib_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(ecore_glib_VALASOURCES) \
	.vala.stamp \
	$(NULL)

CLEANFILES = \
	*.c \
	*.h \
	.vala.stamp \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES = \
	Makefile.in \
	$(NULL)
