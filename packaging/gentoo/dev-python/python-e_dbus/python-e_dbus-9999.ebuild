# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
NEED_PYTHON="2.4"
E_NO_NLS="1"
E_NO_DOC="2"
E_NO_VISIBILITY="1"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit efl python distutils

DESCRIPTION="D-Bus Python intergation for Ecore/EFL"
IUSE=""

RDEPEND="
	>=dev-libs/e_dbus-9999
	dev-python/dbus-python"

DEPEND=">=dev-python/setuptools-0.6_rc9
	${RDEPEND}"

src_unpack() {
	efl_src_unpack
}

src_compile() {
	distutils_src_compile
}

src_install() {
	distutils_src_install
}
