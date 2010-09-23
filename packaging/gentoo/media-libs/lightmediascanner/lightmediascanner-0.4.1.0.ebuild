# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

DESCRIPTION="Lightweight media scanner."
HOMEPAGE="http://lms.garage.maemo.org/"
SRC_URI="https://garage.maemo.org/frs/download.php/8852/lightmediascanner-0.4.1.0.tar.bz2"

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~x86 ~arm"

IUSE="+jpeg +png ogg +playlist +asf +real +mp4 +id3 +flac"

RDEPEND="
	>=dev-db/sqlite-3.3
	ogg? ( media-libs/libvorbis )
	mp4? ( media-libs/libmp4v2 )
	flac? ( media-libs/flac )"

DEPEND="${RDEPEND}"

src_configure() {
	export MY_ECONF="
	  --enable-video-dummy
	  --enable-audio-dummy
	  $(use_enable jpeg)
	  $(use_enable png)
	  $(use_enable playlist m3u)
	  $(use_enable ogg)
	  $(use_enable playlist pls)
	  $(use_enable asf)
	  $(use_enable real rm)
	  $(use_enable mp4)
	  $(use_enable id3)
	  $(use_enable flac)"

	econf ${MY_ECONF}
}
