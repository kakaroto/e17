# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_NO_DISABLE_STATIC="1"

inherit efl

DESCRIPTION="Edje viewer based on Elementary"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Edje_Viewer"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE=""

DEPEND=">=media-libs/elementary-9999"

RDEPEND="${DEPEND}"
