# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_CYTHON="1"
E_NO_NLS="1"
E_NO_DOC="2"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit efl

DESCRIPTION="Python bindings for Ethumb thumbnailing library"
IUSE="+dbus examples"

RDEPEND=">=media-libs/ethumb-9999[dbus?]"

DEPEND="${RDEPEND}"

src_compile() {
	if use dbus; then
		export ETHUMB_BUILD_CLIENT=1
	else
		export ETHUMB_BUILD_CLIENT=0
	fi

	efl_src_compile
}

src_install() {
	if use dbus; then
		export ETHUMB_BUILD_CLIENT=1
	else
		export ETHUMB_BUILD_CLIENT=0
	fi

	efl_src_install
}
