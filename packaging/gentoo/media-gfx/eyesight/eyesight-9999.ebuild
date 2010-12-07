# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI="2"
ESVN_SUB_PROJECT="PROTO"
inherit enlightenment

DESCRIPTION="EFL document viewer"

IUSE="nls poppler postscript"
DEPEND="dev-libs/ecore[evas]
	media-libs/edje
	poppler? ( app-text/poppler )
	postscript? ( app-text/libspectre )"
RDEPEND="${DEPEND}"

src_configure() {
	MY_ECONF="
		$(use_with poppler pdf-backend poppler)
		$(use_enable postscript)"
	enlightenment_src_configure
}
