# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="nls"
E_SVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="${PN#e_module-}"

inherit efl

DESCRIPTION="Tracker search for Everything. requires tracker 0.8"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND=">=x11-wm/enlightenment-9999[everything]
	>=app-misc/tracker-0.6"
DEPEND="${RDEPEND}"
