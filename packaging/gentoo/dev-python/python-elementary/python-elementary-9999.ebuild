# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="examples"
E_CYTHON="1"
E_SVN_SUB_PROJECT="BINDINGS/python"

inherit efl

DESCRIPTION="Python bindigs for Elementary"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND=">=media-libs/elementary-9999"

# python-evas is just required to build as it includes some useful header files
DEPEND="
	>=dev-python/python-evas-9999
	${RDEPEND}"
