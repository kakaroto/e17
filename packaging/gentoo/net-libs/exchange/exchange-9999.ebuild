# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_SVN_SUB_PROJECT="PROTO"

inherit efl

DESCRIPTION="Enlightenment way to exchange contributed stuff"
HOMEPAGE="http://www.enlightenment.org/"
SRC_URI=""

LICENSE="GPL-2"
SLOT="0"
KEYWORDS=""

IUSE=""

DEPEND="
	>=dev-libs/eina-9999
	>=dev-libs/ecore-9999[curl,evas]
	>=media-libs/edje-9999
	dev-libs/libxml2
"

RDEPEND="${DEPEND}"

src_configure() {
	export MY_ECONF="
	  ${MY_ECONF}
	  --disable-etk
	  --disable-ewl
	"

	efl_src_configure
}
