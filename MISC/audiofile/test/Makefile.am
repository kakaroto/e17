INCLUDES = -I. -I$(srcdir) -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	addcoffset \
	bitdepth \
	copy \
	results \
	transparency \
	miscread \
	miscwrite \
	query \
	query2 \
	testaupv \
	writeaiff \
	writenext \
	writeulaw \
	writewave \
	@TEST_BIN@

EXTRA_DIST = transparency.sh

EXTRA_PROGRAMS = irixread irixtest irixtestloop linuxtest

TESTS = writeaiff writenext writeulaw writewave transparency.sh

addcoffset_SOURCES = adddcoffset.c

copy_SOURCES = copy.c

miscread_SOURCES = miscread.c

miscwrite_SOURCES = miscwrite.c

results_SOURCES = results.c

transparency_SOURCES = transparency.c

testaupv_SOURCES = testaupv.c

bitdepth_SOURCES = bitdepth.c

writeaiff_SOURCES = writeaiff.c

irixread_SOURCES = irixread.c sgi.c
irixread_LDADD = -laudio $(top_builddir)/libaudiofile/libaudiofile.la

irixtest_SOURCES = irixtest.c sgi.c sgi.h
irixtest_LDADD = -laudio $(top_builddir)/libaudiofile/libaudiofile.la

irixtestloop_SOURCES = irixtestloop.c sgi.c
irixtestloop_LDADD = -laudio $(top_builddir)/libaudiofile/libaudiofile.la

linuxtest_SOURCES = linuxtest.c

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la
if BUILD_STATIC_LIBS
LDFLAGS=-static
endif
LIBS=

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la
