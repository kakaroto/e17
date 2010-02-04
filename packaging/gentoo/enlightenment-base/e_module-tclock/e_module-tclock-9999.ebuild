# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

ESVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="${PN#e_module-}"
E_NO_DOC="1"
inherit efl

DESCRIPTION="Simple TEXT Clock module for e17"

RDEPEND=">=x11-wm/enlightenment-9999"
DEPEND="
	${RDEPEND}
	>=media-libs/edje-9999"
