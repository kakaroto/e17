# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit eutils

DESCRIPTION="Fast memory pool allocator"
HOMEPAGE="http://code.google.com/p/ememoa/"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE="debug doc threads"

if [[ "${PV}" == "9999" ]]; then
	inherit subversion autotools

	SRC_URI=""
	ESVN_REPO_URI="http://ememoa.googlecode.com/svn/trunk/"
	S="${WORKDIR}/trunk"
	KEYWORDS=""
else
	KEYWORDS="~amd64 ~x86"
	# TODO: project did not release any tarball or snapshot!
fi

DEPEND="
	dev-util/pkgconfig
	doc? ( app-doc/doxygen )"

src_prepare() {
	[[ "${PV}" == "9999" ]] && eautoreconf
	epunt_cxx
	elibtoolize
}

src_configure() {
	if use debug; then
		if ! hasq nostrip $FEATURES && ! hasq splitdebug $FEATURES; then
			ewarn "Compiling with USE=debug but portage will strip binaries!"
			ewarn "Please use portage FEATURES=nostrip or splitdebug"
			ewarn "See http://www.gentoo.org/proj/en/qa/backtraces.xml"
		fi
	fi

	export MY_ECONF="
	  --disable-static
	  $(use_enable debug)
	  $(use_enable threads pthread)
	"

	if ! use doc; then
		export MY_ECONF="${MY_ECONF} DOXYGEN=/bin/true"
	fi
	econf "${MY_ECONF}" || die "econf failed"
}

src_compile() {
	emake || die "emake failed"

	if use doc; then
		(cd doc && doxygen doc.doxy) || die "doxygen doc.doxy"
		dohtml doc/doxygen_html/*
	fi
}
