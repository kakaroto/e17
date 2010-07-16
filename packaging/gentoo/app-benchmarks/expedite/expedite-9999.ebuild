# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit efl

DESCRIPTION="Comprehensive test and benchmark suite for Evas"
HOMEPAGE="http://www.enlightenment.org/"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE="directfb fbcon opengl sdl X xcb"

RDEPEND="
	>=dev-libs/eina-9999
	>=media-libs/evas-9999[directfb?,fbcon?,opengl?,sdl?,X?,xcb?]
	opengl? ( virtual/opengl )
	directfb? ( >=dev-libs/DirectFB-0.9.16 )
	sdl? ( media-libs/libsdl )
	X? (
		x11-libs/libX11
		x11-libs/libXext
		x11-libs/libXrender
	)
	!X? ( xcb? ( x11-libs/xcb-util ) )"
DEPEND="${RDEPEND}"

src_configure() {
	local x_or_xcb=""

	if ! use directfb && ! use fbcon && ! use sdl && ! use X && ! use xcb; then
		die "Expedite needs at least one output system to be useful!"
		die "Compile app-benchmarks/expedite with USE=directfb, fbcon, sdl, X or xcb"
	fi

	if use X; then
		x_or_xcb="X"
	elif use xcb; then
		x_or_xcb="xcb"
	fi

	if use opengl && [[ -z "$x_or_xcb" ]]; then
		die "Expedite usage of OpenGL requires X11."
		die "Compile app-benchmarks/expedite with USE=X or xcb."
	fi

	export MY_ECONF="
	  ${MY_ECONF}
	  --disable-software-gdi
	  --disable-software-ddraw
	  --disable-direct3d
	  --disable-opengl-glew
	  --disable-quartz
	  --disable-software-16-ddraw
	  --disable-software-16-wince
	  $(use_enable directfb)
	  $(use_enable fbcon fb)
	  $(use_enable opengl opengl-x11)
	  $(use_enable sdl software-sdl)
	  $(use_enable X software-x11)
	  $(use_enable X software-16-x11)
	  $(use_enable X xrender-x11)
	  $(use_enable xcb xrender-xcb)
	"
	efl_src_configure
}
