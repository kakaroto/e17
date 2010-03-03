# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="TMP/st"
inherit efl

DESCRIPTION="Basic widget set, based on EFL with focus mobile touch-screen devices."
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Elementary"

LICENSE="LGPL-2.1"

IUSE="fbcon opengl sdl X dbus xdg weather xcb"

RDEPEND="
	>=dev-libs/ecore-9999[evas,fbcon?,opengl?,sdl?,X?,xcb?]
	>=media-libs/evas-9999[fbcon?,opengl?,sdl?,X?,xcb?]
	>=media-libs/edje-9999
	dbus? ( >=dev-libs/e_dbus-9999 )
	xdg? ( >=dev-libs/efreet-9999 )
	weather? ( >=net-libs/libeweather-9999 )
	"
DEPEND="${RDEPEND}"

src_configure() {
	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable dbus edbus)
	  $(use_enable xdg efreet)
	  $(use_enable weather eweather)
	  $(use_enable fbcon ecore-fb)
	  $(use_enable sdl ecore-sdl)
	  $(use_enable X ecore-x)
	"
	efl_src_configure
}
