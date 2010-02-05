# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_NO_NLS="1"
inherit efl

DESCRIPTION="library for handling of freedesktop.org specs (desktop/icon/theme/etc...)"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Efreet"
IUSE=""

RDEPEND="
	>=dev-libs/ecore-9999
	>=dev-libs/eina-9999"
DEPEND="${RDEPEND}"
