# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_NO_NLS="1"
E_NO_DOC="1"
E_NO_VISIBILITY="1"
E_NO_DISABLE_STATIC="1"
inherit efl

DESCRIPTION="View and edit Edje (EDC/EDJ) files."
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Editje"
LICENSE="GPL-3"
IUSE=""

RDEPEND="
	>=dev-python/python-evas-9999
	>=dev-python/python-ecore-9999
	>=dev-python/python-edje-9999
	>=dev-python/python-elementary-9999"

DEPEND="
	>=media-libs/edje-9999"
