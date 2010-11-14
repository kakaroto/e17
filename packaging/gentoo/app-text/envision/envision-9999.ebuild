# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
inherit enlightenment

DESCRIPTION="EFL-based PDF reader"
IUSE=""

RDEPEND="
	 >=dev-libs/eina-9999
	 >=dev-libs/ecore-9999[evas,inotify]
	 >=media-libs/edje-9999
	 >=media-libs/evas-9999
	 >=media-libs/elementary-9999
	 >=app-text/epdf-9999
"
DEPEND="${RDEPEND}"
