# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="${PN#e_module-}"

inherit enlightenment

DESCRIPTION="Enlightenment's notification daemon using libnotify/notify-osd standard"



RDEPEND="
	>=x11-wm/enlightenment-9999
	>=dev-libs/e_dbus-9999[libnotify]"
DEPEND="
	${RDEPEND}
	>=media-libs/edje-9999"
