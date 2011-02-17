MAINTAINERCLEANFILES = \
	Makefile.in ABOUT-NLS INSTALL aclocal.m4 config.guess compile \
	config.h.in config.rpath config.sub configure install-sh \
	ltconfig ltmain.sh missing mkinstalldirs \
	m4/lib-link.m4 m4/lib-prefix.m4 m4/lib-ld.m4 m4/lcmessage.m4 \
	m4/libtool.m4 m4/ulonglong.m4 \
	m4/inttypes-pri.m4 m4/progtest.m4 m4/uintmax_t.m4 m4/lt~obsolete.m4 \
	m4/stdint_h.m4 m4/intdiv0.m4 \
	m4/iconv.m4 m4/po.m4 m4/isc-posix.m4 m4/inttypes.m4 m4/ltsugar.m4 \
	m4/glibc21.m4 m4/gettext.m4 m4/ltversion.m4 \
	m4/codeset.m4 m4/inttypes_h.m4 m4/ltoptions.m4 m4/nls.m4 \
	po/Rules-quot po/en@quot.header po/insert-header.sin \
	po/quot.sed po/en@boldquot.header po/boldquot.sed \
	po/Makevars.template po/remove-potcdate.sin po/Makefile.in.in \
	stamp-h.in acconfig.h depcomp

# no idea why this is required, it should not be:
DISTCLEANFILES = po/stamp-po

SUBDIRS = src data po

EXTRA_DIST = README AUTHORS COPYING COPYING.icons ephoto.spec

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --disable-quicklaunch
