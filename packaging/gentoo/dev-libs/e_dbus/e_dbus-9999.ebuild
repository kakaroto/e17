# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="Enlightenment's (Ecore) integration to DBus"

IUSE="bluetooth connman +hal +libnotify ofono static-libs ukit"

RDEPEND="
	>=dev-libs/eina-9999
	>=dev-libs/ecore-9999
	sys-apps/dbus
	libnotify? ( >=media-libs/evas-9999 )
	hal? ( sys-apps/hal )
	ukit? ( sys-power/upower sys-fs/udisks )
"
DEPEND="${RDEPEND}"

src_configure() {
	MY_ECONF="
	$(use_enable bluetooth ebluez)
	$(use_enable connman econnman)
	$(use_enable doc)
	$(use_enable hal ehal)
	$(use_enable libnotify enotify)
	$(use_enable ofono eofono)
	$(use_enable ukit eukit)
	"
	enlightenment_src_configure
}
