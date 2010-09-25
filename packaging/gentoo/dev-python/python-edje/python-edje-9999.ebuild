# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_CYTHON="1"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit enlightenment

DESCRIPTION="Python bindings for Edje"
LICENSE="LGPL-2.1"

IUSE="examples"

RDEPEND=">=media-libs/edje-9999"

# python-evas is just required to build as it includes some useful header files
DEPEND="
	>=dev-python/python-evas-9999
	${RDEPEND}"
