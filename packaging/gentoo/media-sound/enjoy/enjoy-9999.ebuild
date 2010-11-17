# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
inherit enlightenment

DESCRIPTION="EFL-based music player"
IUSE="asf flac id3 mp4 ogg playlist real"

RDEPEND="
	 >=media-libs/emotion-9999
	 >=media-libs/elementary-9999[thumbnails,dbus,xdg]
	 >=media-libs/lightmediascanner-0.4.1.0[asf?,flac?,id3?,mp4?,ogg?,playlist?,real?]
	 >=dev-db/sqlite-3.6
"
DEPEND="${RDEPEND}"
