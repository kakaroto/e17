# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
E_NO_NLS="1"
inherit efl

DESCRIPTION="Enlightenment's (Ecore) integration to DBus"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/E_Dbus"

IUSE="hal connman libnotify"

RDEPEND="
	>=dev-libs/eina-9999
	>=dev-libs/ecore-9999
	sys-apps/dbus
	libnotify? ( >=media-libs/evas-9999 )"
DEPEND="${RDEPEND}"

src_configure() {
	export MY_ECONF="
	  ${MY_ECONF}
	  --disable-enm
	  --disable-build-test-gui
	  $(use_enable hal ehal)
	  $(use_enable connman econnman)
	  $(use_enable libnotify enotify)
	"
	efl_src_configure
}
