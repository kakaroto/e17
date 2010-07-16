# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="examples"
E_CYTHON="1"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit efl

DESCRIPTION="Python bindings for Ethumb thumbnailing library"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS=""

IUSE="+dbus"

RDEPEND=">=media-libs/ethumb-9999[dbus?]"

DEPEND="${RDEPEND}"

src_configure() {
	if use dbus; then
		export ETHUMB_BUILD_CLIENT=1
	else
		export ETHUMB_BUILD_CLIENT=0
	fi

	efl_src_configure
}

src_install() {
	if use dbus; then
		export ETHUMB_BUILD_CLIENT=1
	else
		export ETHUMB_BUILD_CLIENT=0
	fi

	efl_src_install
}
