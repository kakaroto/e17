include $(top_srcdir)/Makefile.decl

NULL =

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(DEPS_CFLAGS) \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

progs_ldadd = $(DEPS_LIBS)

VALAC_ARGS = \
	--basedir $(top_srcdir) \
	--vapidir $(top_srcdir)/vapi \
	--pkg eina \
	--pkg evas \
	--pkg ecore \
	--pkg edje \
	--pkg elementary \
	--pkg glib-2.0

#
# eina
#
TEST_PROGS += testeina
testeina_VALASOURCES = testeina.vala
testeina_SOURCES = testeina.c
$(testeina_SOURCES): $(testeina_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@
testeina_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testeina_VALASOURCES)

#
# evas
#
TEST_PROGS += testevas
testevas_VALASOURCES = testevas.vala
testevas_SOURCES = testevas.c
$(testevas_SOURCES): $(testevas_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@
testevas_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testevas_VALASOURCES)

#
# ecore
#
TEST_PROGS += testecore
testecore_VALASOURCES = testecore.vala
testecore_SOURCES = testecore.c
$(testecore_SOURCES): $(testecore_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@
testecore_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testecore_VALASOURCES)

#
# elementary
#
TEST_PROGS += testelementary
testelementary_VALASOURCES = testelementary.vala
testelementary_SOURCES = testelementary.c
$(testelementary_SOURCES): $(testelementary_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@
testelementary_LDADD = $(progs_ldadd)
EXTRA_DIST += $(testelementary_VALASOURCES)

CLEANFILES = \
    $(TEST_PROGS) \
    *.? \
    $(NULL)

MAINTAINERCLEANFILES = \
  Makefile.in \
  $(NULL)
