# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="PROTO"

inherit enlightenment

DESCRIPTION="Enlightenment's UPnP/DLNA framework"
LICENSE="LGPL-3"

IUSE="examples test static-libs"

RDEPEND="
	>=dev-libs/eina-9999
	dev-libs/libxml2
	>=dev-libs/ecore-9999[curl]
	examples? ( >=media-libs/elementary-9999 )"
DEPEND="${RDEPEND}"

src_configure() {
	MY_ECONF="
	  $(use_enable test tests)
	  $(use_enable examples)
	  $(use_enable examples tools)
	  --enable-eupnp-ecore
	  --enable-eupnp-av
	"

	enlightenment_src_configure
}
