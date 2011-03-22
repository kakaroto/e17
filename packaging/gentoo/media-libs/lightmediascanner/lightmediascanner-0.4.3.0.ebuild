# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="Lightweight media scanner"
HOMEPAGE="http://lms.garage.maemo.org/"
# ATTENTION: Garage is stupid and change the full url (number) for packages
SRC_URI="https://garage.maemo.org/frs/download.php/9439/lightmediascanner-0.4.3.0.tar.bz2"
LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS="~arm ~x86 ~amd64"

IUSE="+asf flac +id3 jpeg mp4 ogg +playlist png real static-libs"

RDEPEND="
	>=dev-db/sqlite-3.3
	ogg? ( media-libs/libvorbis )
	mp4? ( media-libs/libmp4v2 )
	flac? ( media-libs/flac )"

DEPEND="${RDEPEND}"

src_configure() {
	econf \
		--disable-static \
		--enable-video-dummy \
		--enable-audio-dummy \
		$(use_enable jpeg) \
		$(use_enable png) \
		$(use_enable playlist m3u) \
		$(use_enable ogg) \
		$(use_enable playlist pls) \
		$(use_enable asf) \
		$(use_enable real rm) \
		$(use_enable mp4) \
		$(use_enable id3) \
		$(use_enable flac)
}

src_install() {
	emake install DESTDIR="${D}" || die
	find "${D}" -name '*.la' -delete
}
