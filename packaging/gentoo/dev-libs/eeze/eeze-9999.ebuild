# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_NO_NLS="1"
inherit efl

DESCRIPTION="Enlightenment's integration to various libraries"
HOMEPAGE="http://www.enlightenment.org"

IUSE="udev"

RDEPEND="
	>=dev-libs/ecore-9999
	udev? ( sys-fs/udev )
"
DEPEND="${RDEPEND}"

src_configure() {
	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable udev eudev)
	"
	efl_src_configure
}
