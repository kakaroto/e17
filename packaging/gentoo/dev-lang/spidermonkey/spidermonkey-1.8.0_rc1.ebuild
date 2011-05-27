# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils multilib flag-o-matic

MY_P="js-${PV}"
DESCRIPTION="Stand-alone JavaScript C library"
HOMEPAGE="http://www.mozilla.org/js/spidermonkey/"
SRC_URI="ftp://ftp.mozilla.org/pub/mozilla.org/js/${MY_P/_/-}.tar.gz"

LICENSE="NPL-1.1"
SLOT="0"
KEYWORDS="~alpha ~amd64 ~arm ~hppa ~ppc ~ppc64 ~sparc ~x86 ~x86-fbsd"
IUSE="debug static-libs threads unicode"

S="${WORKDIR}/js/src"

RDEPEND="threads? ( dev-libs/nspr )"
DEPEND="${RDEPEND}"

src_compile() {
	use unicode && append-flags "-DJS_C_STRINGS_ARE_UTF8"
	if use debug; then
		build_optimized=""
	else
		build_optimized="BUILD_OPT=1"
	fi

	if use threads; then
		emake -j1 -f Makefile.ref LIBDIR="$(get_libdir)" JS_DIST=/usr \
			$build_optimized JS_THREADSAFE=1 \
			|| die "emake with threadsafe enabled failed"
	else
		emake -j1 -f Makefile.ref LIBDIR="$(get_libdir)" JS_DIST=/usr \
			$build_optimized \
			|| die "emake without threadsafe enabled failed"
	fi
}

src_install() {
	# spidermonkey-1.8.0 provides no install, do it using mozjs name to
	# avoid clashes with other javascript libraries

	if use debug; then
		OBJDIR_TAG=_DBG
	else
		OBJDIR_TAG=_OPT
	fi

	NAME=mozjs
	GENDIR=Linux_All${OBJDIR_TAG}.OBJ

	exeinto /usr/bin
	doexe $GENDIR/js
	doexe $GENDIR/jskwgen
	doexe $GENDIR/jscpucfg

	newlib.so $GENDIR/libjs.so lib$NAME.so
	if use static-libs; then
		newlib.a $GENDIR/libjs.a lib$NAME.a
	fi

	insinto /usr/include/$NAME
	doins $GENDIR/jsautocfg.h
	doins *.h
	doins *.tbl

	dodoc ../jsd/README
	dohtml README.html
}
