# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="TMP/st"
inherit efl

DESCRIPTION="Basic widget set, based on EFL with focus mobile touch-screen devices."
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Elementary"

IUSE="directfb fbcon opengl sdl X dbus xdg weather"

RDEPEND="
	>=dev-libs/ecore-9999[evas]
	>=media-libs/evas-9999[directfb?,fbcon?,opengl?,sdl?,X?,xcb?]
	>=media-libs/edje-9999
	dbus? ( >=dev-libs/e_dbus-9999 )
	xdg? ( >=dev-libs/efreet-9999 )
	weather? ( >=net-libs/libeweather )
	"
DEPEND="${RDEPEND}"
