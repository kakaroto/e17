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

DESCRIPTION="Python bindings for Edje"
IUSE="examples"

RDEPEND=">=media-libs/edje-9999"

# python-evas is just required to build as it includes some useful header files
DEPEND="
	>=dev-python/python-evas-9999
	>=dev-python/setuptools-0.6_rc9
	>=dev-python/pyrex-0.9.8.5
	>=dev-python/cython-0.12
	${RDEPEND}"

src_unpack() {
	efl_src_unpack
}

src_compile() {
	distutils_src_compile
}

src_install() {
	distutils_src_install

	if use examples; then
		insinto /usr/share/doc/${PF}
		doins -r examples

		find "${D}/usr/share/doc/${PF}" '(' -name CVS -o -name .svn -o -name .git ')' -type d -exec rm -rf '{}' \; 2>/dev/null
	fi
}
