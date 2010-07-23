# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE=""

inherit efl

DESCRIPTION="Enlightenment's integration to IO"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND=">=dev-libs/ecore-9999"
DEPEND="${RDEPEND}"

src_configure() {
	efl_src_configure
}
