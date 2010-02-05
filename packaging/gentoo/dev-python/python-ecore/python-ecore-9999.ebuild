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

DESCRIPTION="Python bindings for Ecore"
IUSE="+evas +X +xscreensaver examples"

RDEPEND=">=dev-libs/ecore-9999[evas?,X?,xscreensaver?]"

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
	if use evas; then
		export ECORE_BUILD_EVAS=1
	else
		export ECORE_BUILD_EVAS=0
	fi

	if use X; then
		export ECORE_BUILD_X=1

		if use xscreensaver; then
			export ECORE_BUILD_XSCREENSAVER=1
		else
			export ECORE_BUILD_XSCREENSAVER=0
		fi
	else
		export ECORE_BUILD_X=0

		if use xscreensaver; then
			ewarn "USE=xscreensaver has no meaning without X use flag!"
		fi
	fi

	distutils_src_compile
}

src_install() {
	if use evas; then
		export ECORE_BUILD_EVAS=1
	else
		export ECORE_BUILD_EVAS=0
	fi

	if use evas; then
		export ECORE_BUILD_EVAS=1
	else
		export ECORE_BUILD_EVAS=0
	fi

	if use X; then
		export ECORE_BUILD_X=1

		if use xscreensaver; then
			export ECORE_BUILD_XSCREENSAVER=1
		else
			export ECORE_BUILD_XSCREENSAVER=0
		fi
	else
		export ECORE_BUILD_X=0
	fi

	distutils_src_install

	if use examples; then
		insinto /usr/share/doc/${PF}
		doins -r examples

		find "${D}/usr/share/doc/${PF}" '(' -name CVS -o -name .svn -o -name .git ')' -type d -exec rm -rf '{}' \; 2>/dev/null
	fi
}
