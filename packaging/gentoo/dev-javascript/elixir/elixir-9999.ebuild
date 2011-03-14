# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="BINDINGS/javascript"

inherit enlightenment

DESCRIPTION="JavaScript bindigs for EFL components"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE="debug +zlib sdl +sqlite +network +filesystem eio emotion cmp static +static-modules"

RDEPEND="
	=dev-lang/spidermonkey-1.8.0_rc1
	static? (
		=dev-libs/eet-9999[static-libs]
		=media-libs/evas-9999[eet,static-libs]
		=dev-libs/ecore-9999[evas,static-libs]
		=media-libs/edje-9999[static-libs]
		dev-libs/libgcrypt[static-libs]
		eio? ( =dev-libs/eio-9999[static-libs] )
		emotion? ( =media-libs/emotion-9999[static-libs] )
	)
	!static? (
		=dev-libs/eet-9999
		=media-libs/evas-9999[eet]
		=dev-libs/ecore-9999[evas]
		=media-libs/edje-9999
		dev-libs/libgcrypt
		eio? ( =dev-libs/eio-9999 )
		emotion? ( =media-libs/emotion-9999 )
	)
	zlib? ( sys-libs/zlib )
	sdl? ( media-libs/libsdl media-libs/sdl-mixer )
	sqlite? ( >=dev-db/sqlite-3 )
	"
DEPEND="${RDEPEND}"

src_configure() {
	MY_ECONF="
		--enable-gcrypt --disable-tomcrypt
		--enable-eet
		--enable-evas
		--enable-ecore
		--disable-ecore-config
		--enable-ecore-evas
		--disable-ecore-ipc
		$(use_enable filesystem ecore-file)
		$(use_enable network ecore-con)
		$(use_enable network ecore-download)
		$(use_enable network remote)
		--enable-edje
		--enable-let
		--enable-text
		$(use_enable debug)
		$(use_enable zlib)
		$(use_enable sdl)
		$(use_enable sdl mix)
		$(use_enable emotion)
		$(use_enable sqlite sqlite3)
		$(use_enable eio)
		$(use_enable filesystem file)
		$(use_enable filesystem dir)
		$(use_enable cmp)
		$(use_enable static static-build)
	"
	if use static-modules; then
		MY_ECONF="${MY_ECONF}
			--enable-static-elixir
			--enable-static-eet
			--enable-static-evas
			--enable-static-ecore
			--enable-static-ecore-evas
			$(use_enable filesystem static-ecore-file)
			$(use_enable network static-ecore-con)
			$(use_enable network static-ecore-download)
			$(use_enable network static-remote)
			--enable-static-edje
			--enable-static-let
			--enable-static-text
			$(use_enable sdl static-sdl)
			$(use_enable sdl static-mix)
			$(use_enable emotion static-emotion)
			$(use_enable sqlite static-sqlite3)
			$(use_enable eio static-eio)
			$(use_enable filesystem static-file)
			$(use_enable filesystem static-dir)
			$(use_enable cmp static-cmp)
		"
	fi

	enlightenment_src_configure
}
