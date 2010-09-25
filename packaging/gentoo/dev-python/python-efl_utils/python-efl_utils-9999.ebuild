# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_PYTHON="1"
ESVN_SUB_PROJECT="BINDINGS/python"
WANT_AUTOTOOLS="no"
inherit enlightenment distutils

DESCRIPTION="Utilities to work with EFL"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS=""

IUSE="examples"

RDEPEND="
	>=dev-python/python-evas-9999
	>=dev-python/python-ecore-9999"

src_configure() {
	:
}

src_compile() {
	distutils_src_compile
	if [[ -x ./gendoc ]]; then
		./gendoc || efl_die "gendoc failed"
	fi
}

src_install() {
	distutils_src_install
	if use examples; then
		insinto /usr/share/doc/${PF}
		doins -r examples
	fi
}
