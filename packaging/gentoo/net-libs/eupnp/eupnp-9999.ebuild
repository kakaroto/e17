# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_NO_NLS="1"
ESVN_SUB_PROJECT="PROTO"

inherit efl

DESCRIPTION="Enlightenment's UPnP/DLNA framework"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-3"
SLOT="0"
KEYWORDS=""

IUSE="examples test"

RDEPEND="
	>=dev-libs/eina-9999
	dev-libs/libxml2
	>=dev-libs/ecore-9999[curl]
	examples? ( >=media-libs/elementary-9999 )"
DEPEND="
	${RDEPEND}
	test? ( dev-libs/check )"

src_configure() {
	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable test tests)
	  $(use_enable examples)
	  $(use_enable examples tools)
	  --enable-eupnp-ecore
	  --enable-eupnp-av
	"

	efl_src_configure
}
