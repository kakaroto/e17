# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="doc"

inherit efl

DESCRIPTION="hardware-accelerated retained canvas API"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Evas"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE="debug mmx sse altivec +threads +fontconfig bidi +cserve directfb fbcon opengl sdl X xcb +eet gif +jpeg +pnm +png svg +tiff +xpm static-modules +safety-checks"

RDEPEND="
	>=dev-libs/eina-9999
	>=media-libs/freetype-2.3.9
	fontconfig? ( media-libs/fontconfig )
	bidi? ( >=dev-libs/fribidi-0.19.1 )
	directfb? ( >=dev-libs/DirectFB-0.9.16 )
	sdl? ( media-libs/libsdl )
	X? (
		x11-libs/libX11
		x11-libs/libXext
		x11-libs/libXrender
		opengl? ( virtual/opengl )
	)
	!X? ( xcb? ( x11-libs/xcb-util ) )
	eet? ( >=dev-libs/eet-9999 )
	gif? ( media-libs/giflib )
	jpeg? ( media-libs/jpeg )
	png? ( media-libs/libpng )
	svg? (
		>=gnome-base/librsvg-2.14.0
		x11-libs/cairo
		x11-libs/libsvg-cairo
	)
	tiff? ( media-libs/tiff )"
DEPEND="${RDEPEND}"

src_configure() {
	local SAFETY_CHECKS_FLAGS="" X_FLAGS="" STATIC_FLAGS=""

	# ???: should we use 'use_enable' for these as well?
	if use safety-checks || use debug; then
		SAFETY_CHECKS_FLAGS="--enable-evas-magic-debug"
	else
		ewarn "Compiling without safety-checks is dangerous and unsupported."
		ewarn "Just use safety-checks if you really know what you are doing."
		SAFETY_CHECKS_FLAGS="--disable-evas-magic-debug"
	fi

	if use static-modules; then
		MODULE_ARGUMENT="static"
		STATIC_FLAGS="
		  --enable-static-software-generic
		  --enable-static-software-16
		"
	else
		MODULE_ARGUMENT="yes"
	fi

	if use opengl && ! use X; then
		die "Evas usage of OpenGL requires X11."
		die "Compile dev-libs/ecore with USE=X."
	fi

	if use X; then
		if use xcb; then
			ewarn "You have enabled both 'X' and 'xcb', so we will use"
			ewarn "X as it's considered the most stable for evas"
		fi
		X_FLAGS="
		  --enable-software-xlib=${MODULE_ARGUMENT}
		  --enable-xrender-x11=${MODULE_ARGUMENT}
		  --enable-software-16-x11=${MODULE_ARGUMENT}
		  --disable-software-xcb
		  --disable-xrender-xcb
		  $(use_enable opengl gl-x11 $MODULE_ARGUMENT)
		"
	elif use xcb; then
		X_FLAGS="
		  --disable-software-xlib
		  --disable-xrender-x11
		  --disable-software-16-x11
		  --enable-software-xcb=${MODULE_ARGUMENT}
		  --enable-xrender-xcb=${MODULE_ARGUMENT}
		"
	else
		X_FLAGS="
		  --disable-software-xlib
		  --disable-xrender-x11
		  --disable-software-16-x11
		  --disable-software-xcb
		  --disable-xrender-xcb
		  --disable-gl-x11
		"
	fi

	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable mmx cpu-mmx)
	  $(use_enable sse cpu-sse)
	  $(use_enable altivec cpu-altivec)
	  $(use_enable threads pthreads)
	  $(use_enable threads async-events)
	  $(use_enable threads async-preload)
	  $(use_enable threads async-render)
	  $(use_enable fontconfig)
	  $(use_enable bidi fribidi)
	  $(use_enable cserve evas-cserve)
	  --enable-cpu-c
	  --enable-scale-sample
	  --enable-scale-smooth
	  --enable-convert-yuv
	  --enable-buffer
	  --disable-software-gdi
	  --disable-software-ddraw
	  --disable-direct3d
	  --disable-quartz
	  --disable-gl-glew
	  $(use_enable directfb)
	  $(use_enable fbcon fb)
	  $(use_enable sdl software-sdl)
	  ${X_FLAGS}
	  --disable-software-16-ddraw
	  --disable-software-16-wince
	  --disable-image-loader-edb
	  $(use_enable eet image-loader-eet $MODULE_ARGUMENT)
	  $(use_enable eet font-loader-eet $MODULE_ARGUMENT)
	  $(use_enable gif image-loader-gif $MODULE_ARGUMENT)
	  $(use_enable jpeg image-loader-jpeg $MODULE_ARGUMENT)
	  $(use_enable pnm image-loader-pmaps $MODULE_ARGUMENT)
	  $(use_enable png image-loader-png $MODULE_ARGUMENT)
	  $(use_enable svg image-loader-svg $MODULE_ARGUMENT)
	  $(use_enable tiff image-loader-tiff $MODULE_ARGUMENT)
	  $(use_enable xpm image-loader-xpm $MODULE_ARGUMENT)
	  --enable-convert-8-rgb-332
	  --enable-convert-8-rgb-666
	  --enable-convert-8-rgb-232
	  --enable-convert-8-rgb-222
	  --enable-convert-8-rgb-221
	  --enable-convert-8-rgb-121
	  --enable-convert-8-rgb-111
	  --enable-convert-16-rgb-565
	  --enable-convert-16-rgb-555
	  --enable-convert-16-rgb-444
	  --enable-convert-16-rgb-rot-0
	  --enable-convert-16-rgb-rot-270
	  --enable-convert-16-rgb-rot-90
	  --enable-convert-24-rgb-888
	  --enable-convert-24-bgr-888
	  --enable-convert-32-rgb-8888
	  --enable-convert-32-rgbx-8888
	  --enable-convert-32-bgr-8888
	  --enable-convert-32-bgrx-8888
	  --enable-convert-32-rgb-rot-0
	  --enable-convert-32-rgb-rot-270
	  --enable-convert-32-rgb-rot-90
	  ${STATIC_FLAGS}
	  ${SAFETY_CHECKS_FLAGS}
	"

	efl_src_configure
}
