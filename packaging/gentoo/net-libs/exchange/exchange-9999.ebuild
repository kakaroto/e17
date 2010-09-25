# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="PROTO"

inherit enlightenment

DESCRIPTION="Enlightenment way to exchange contributed stuff"
LICENSE="GPL-2"

IUSE="static-libs"

DEPEND="
	>=dev-libs/eina-9999
	>=dev-libs/ecore-9999[curl,evas]
	>=media-libs/edje-9999
	dev-libs/libxml2
"

RDEPEND="${DEPEND}"

src_configure() {
	MY_ECONF="
	  --disable-etk
	  --disable-ewl
	"

	enlightenment_src_configure
}
