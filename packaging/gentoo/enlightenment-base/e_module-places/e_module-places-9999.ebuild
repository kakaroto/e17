# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="nls"
E_SVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="${PN#e_module-}"
inherit efl

DESCRIPTION="Places module for E17"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="GPL-3"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND=">=x11-wm/enlightenment-9999"
DEPEND="${RDEPEND}"
