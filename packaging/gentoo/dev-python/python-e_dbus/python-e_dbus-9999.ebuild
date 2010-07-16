# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_PYTHON="1"
ESVN_SUB_PROJECT="BINDINGS/python"

inherit efl

DESCRIPTION="D-Bus Python intergation for Ecore/EFL"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND="
	>=dev-libs/e_dbus-9999
	dev-python/dbus-python"

DEPEND="${RDEPEND}"
