# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE=""

inherit efl

DESCRIPTION="Fast memory pool allocator"
HOMEPAGE="http://code.google.com/p/ememoa/"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE="debug threads"

ESVN_REPO_URI="http://ememoa.googlecode.com/svn/trunk/"
E_NO_DOC="1"
DEPEND="
	dev-util/pkgconfig
	doc? ( app-doc/doxygen )"

src_configure() {
	if use debug; then
		if ! hasq nostrip $FEATURES && ! hasq splitdebug $FEATURES; then
			ewarn "Compiling with USE=debug but portage will strip binaries!"
			ewarn "Please use portage FEATURES=nostrip or splitdebug"
			ewarn "See http://www.gentoo.org/proj/en/qa/backtraces.xml"
		fi
	fi

	MY_ECONF="
	  --disable-static
	  $(use_enable debug)
	  $(use_enable threads pthread)
	"

	efl_src_configure
}
