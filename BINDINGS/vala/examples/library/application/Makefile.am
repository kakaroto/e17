include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = \
	-I$(top_srcdir)/eflvala \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = application

application_VALASOURCES = \
	application.vala \
	$(NULL)

application_SOURCES = \
	$(application_VALASOURCES:.vala=.c) \
	$(NULL)

.vala.stamp: $(application_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --vapidir ../../../eflvala \
	            --pkg dbus-glib-1 \
	            --pkg eflvala-1.0 \
	            --save-temps \
	            --basedir $(top_srcdir) $^
	touch $@

application_LDADD = \
	$(DEPS_LIBS) \
	../../../eflvala/libeflvala.la \
	$(NULL)

EXTRA_DIST += \
	$(application_VALASOURCES) \
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
