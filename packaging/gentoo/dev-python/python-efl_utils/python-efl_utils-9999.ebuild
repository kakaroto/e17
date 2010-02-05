# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_PYTHON="1"
E_NO_NLS="1"
E_NO_DOC="2"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit efl

DESCRIPTION="Utilities to work with EFL"
IUSE="examples"

RDEPEND="
	>=dev-python/python-evas-9999
	>=dev-python/python-ecore-9999"

DEPEND=""
