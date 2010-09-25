# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_CYTHON="1"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit enlightenment

DESCRIPTION="Python bindings for Evas"
LICENSE="LGPL-2.1"

IUSE="examples"

RDEPEND=">=media-libs/evas-9999"

DEPEND="${RDEPEND}"
