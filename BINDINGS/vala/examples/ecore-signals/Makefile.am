include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = ecore_signals

ecore_signals_VALASOURCES = \
	ecore_signals.vala \
	$(NULL)

ecore_signals_SOURCES = \
	$(ecore_signals_VALASOURCES:.vala=.c) \
	$(NULL)

.vala.stamp: $(ecore_signals_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --pkg evas \
	            --pkg ecore \
	            --save-temps \
	            --basedir $(top_srcdir) $<
	touch $^

ecore_signals_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(ecore_signals_VALASOURCES) \
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
