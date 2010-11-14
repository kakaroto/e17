# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
inherit enlightenment

DESCRIPTION="EFL-based photo app"
IUSE=""

RDEPEND="
	 >=dev-libs/eet-9999
	 >=dev-libs/efreet-9999
	 >=dev-libs/eina-9999
	 >=dev-libs/ecore-9999[evas,inotify]
	 >=dev-libs/eio-9999
	 >=media-libs/edje-9999
	 >=media-libs/evas-9999[eet,jpeg,png]
	 >=media-libs/elementary-9999[thumbnails,xdg]
	 >=media-libs/ethumb-9999
"
DEPEND="${RDEPEND}"
