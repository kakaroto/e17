# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/app-editors/enscribe/enscribe-9999.ebuild,v 1.1 2005/09/07 03:55:40 vapier Exp $

ESVN_SUB_PROJECT="PROTO"
inherit enlightenment

DESCRIPTION="PDF viewer with widgets for Evas"
IUSE="cjk poppler"

LICENSE="GPL-2 || ( LGPL-3 )"

DEPEND="poppler? ( >=app-text/poppler-0.12 )
	>=media-libs/evas-9999
	>=dev-libs/ecore-9999"
RDEPEND="${DEPEND}"

src_configure() {
	MY_ECONF="
		$(use_enable poppler)
		$(use_enable !poppler mupdf)
		"
	use poppler || MY_ECONF="${MY_ECONF} $(use_enable cjk mupdf-cjk)"

	export MY_ECONF
	enlightenment_src_configure
}
