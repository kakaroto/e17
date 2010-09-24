# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=2

inherit enlightenment

DESCRIPTION="hardware-accelerated retained canvas API"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Evas"

IUSE="altivec bidi +cache directfb gif fbcon +fontconfig +jpeg mmx opengl +png sdl sse svg static-libs +threads tiff X xcb xpm"

RDEPEND="
	>=dev-libs/eina-9999
	>=media-libs/freetype-2.3.9
	fontconfig? ( media-libs/fontconfig )
	gif? ( media-libs/giflib )
	jpeg? ( media-libs/jpeg )
	png? ( media-libs/libpng )
	bidi? ( >=dev-libs/fribidi-0.19.1 )
	directfb? ( >=dev-libs/DirectFB-0.9.16 )
	sdl? ( media-libs/libsdl )
	tiff? ( media-libs/tiff )
	xpm? ( x11-libs/libXpm )
	X? (
		x11-libs/libX11
		x11-libs/libXext
		x11-libs/libXrender
		opengl? ( virtual/opengl )
	)
	!X? (
		xcb? (
			x11-libs/xcb-util
		) )
	eet? ( >=dev-libs/eet-9999 )
	media-libs/giflib
	media-libs/jpeg
	media-libs/libpng
	svg? (
		>=gnome-base/librsvg-2.14.0
		x11-libs/cairo
		x11-libs/libsvg-cairo
	)"
DEPEND="${RDEPEND}"

src_configure() {
	if use X ; then
		if use xcb ; then
			ewarn "You have enabled both 'X' and 'xcb', so we will use"
			ewarn "X as it's considered the most stable for evas"
		fi
		MY_ECONF+="
			--disable-software-xcb
			--disable-xrender-xcb
			$(use_enable opengl gl-x11 static)
		"
	elif use xcb ; then
		use opengl && ewarn "opengl support is not implemented with xcb"
		MY_ECONF+="
			--disable-gl-x11
			--enable-software-xcb=static
			--enable-xrender-xcb=static
		"

		use 16bpp && warn "16bpp optimized engine not available with xcb"
		use 8bpp && X_FLAGS+=" --enable-software-8-x11=${MODULE_ARGUMENT}"
	else
		MY_ECONF+="
			--disable-gl-x11
			--disable-software-xcb
			--disable-xrender-xcb
		"
	fi

	if use opengl ; then
		MY_ECONF+=" $(use_enable cache metric-cache)"
	else
		MY_ECONF+=" $(use_enable cache word-cache)"
	fi

	MY_ECONF="
		$(use_enable altivec cpu-altivec)
		$(use_enable bidi fribidi)
		$(use_enable directfb)
		$(use_enable doc)
		$(use_enable fbcon fb)
		$(use_enable fontconfig)
		$(use_enable gif image-loader-gif)
		$(use_enable jpeg image-loader-jpeg)
		$(use_enable eet image-loader-eet)
		$(use_enable mmx cpu-mmx)
		$(use_enable png image-loader-png)
		$(use_enable sdl software-sdl)
		$(use_enable sse cpu-sse)
		$(use_enable svg image-loader-svg static)
		$(use_enable tiff image-loader-tiff static)
		$(use_enable threads pthreads)
		$(use_enable threads async-events)
		$(use_enable threads async-preload)
		$(use_enable X software-xlib static)
		$(use_enable X xrender-x11 static)
		$(use_enable xpm image-loader-xpm static)
	"

	enlightenment_src_configure
}
