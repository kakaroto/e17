# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="Simple EFL based terminal emulator"
HOMEPAGE="http://www.enlightenment.org/"

IUSE=""

RDEPEND="
	>dev-libs/ecore-1.2
	>dev-libs/eet-1.6
	>dev-libs/eina-1.2
	>media-libs/elementary-1.0
	>media-libs/evas-1.2
	>media-libs/edje-1.2"

DEPEND="${RDEPEND}"
