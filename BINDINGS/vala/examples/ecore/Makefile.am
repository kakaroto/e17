include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = ecore

ecore_VALASOURCES = \
	ecore.vala \
	$(NULL)

ecore_SOURCES = \
	$(ecore_VALASOURCES:.vala=.c) \
	$(NULL)

.vala.stamp: $(ecore_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --pkg evas \
	            --pkg ecore \
	            --pkg ecore-evas \
	            --save-temps \
	            --basedir $(top_srcdir) $<
	touch $^

ecore_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(ecore_VALASOURCES) \
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
