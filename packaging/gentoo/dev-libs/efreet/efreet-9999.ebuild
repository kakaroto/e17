# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit enlightenment

DESCRIPTION="library for handling of freedesktop.org specs (desktop/icon/theme/etc...)"
RDEPEND="
	>=dev-libs/ecore-9999
	>=dev-libs/eina-9999
	x11-misc/xdg-utils"
DEPEND="${RDEPEND}"

IUSE="static-libs"
