# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="nls"
ESVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="${PN#e_module-}"
E_NO_DOC="1"
inherit efl

DESCRIPTION="Enlightenment's notification daemon using libnotify/notify-osd standard"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND="
	>=x11-wm/enlightenment-9999
	>=dev-libs/e_dbus-9999[libnotify]"
DEPEND="
	${RDEPEND}
	>=media-libs/edje-9999"
