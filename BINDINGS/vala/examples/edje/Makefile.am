include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = edje

edje_VALASOURCES = \
	edje.vala \
	$(NULL)

edje_SOURCES = \
	$(edje_VALASOURCES:.vala=.c) \
	$(NULL)

.vala.stamp: $(edje_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --pkg edje \
	            --pkg ecore-evas \
	            --save-temps \
	            --basedir $(top_srcdir) $<
	touch $^

edje_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(edje_VALASOURCES) \
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
