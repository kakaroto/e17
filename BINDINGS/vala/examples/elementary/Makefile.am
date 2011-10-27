include $(top_srcdir)/Makefile.decl

NULL = 

AUTOMAKE_OPTIONS = subdir-objects

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = elementary-vapi

elementary_vapi_VALASOURCES = \
	abstract.vala \
	main.vala \
	background.vala \
	genlist.vala \
	hoversel.vala \
	$(NULL)

elementary_vapi_SOURCES = \
	$(elementary_vapi_VALASOURCES:.vala=.c) \
	\
	$(NULL)

.vala.stamp: $(elementary_vapi_VALASOURCES)
	$(VALAC) -C --vapidir $(top_srcdir)/vapi \
	            --pkg dbus-glib-1 \
	            --pkg eina \
	            --pkg evas \
	            --pkg ecore \
	            --pkg elementary \
	            --save-temps \
	            --basedir $(top_srcdir) $^
	touch $@

elementary_vapi_LDADD = \
	$(DEPS_LIBS) \
	-lutil \
	$(NULL)

EXTRA_DIST += \
	$(elementary_vapi_VALASOURCES) \
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
