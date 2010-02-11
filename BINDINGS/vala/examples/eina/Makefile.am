include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = eina

eina_VALASOURCES = \
	eina.vala \
	$(NULL)

eina_SOURCES = \
	$(eina_VALASOURCES:.vala=.c) \
	$(NULL)

.vala.stamp: $(eina_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --pkg eina \
	            --save-temps \
	            --basedir $(top_srcdir) $<
	touch $^

eina_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(eina_VALASOURCES) \
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
