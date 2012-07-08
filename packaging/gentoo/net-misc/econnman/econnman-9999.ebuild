# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="ConnMan frontend using EFL"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-3"
SLOT="0"
KEYWORDS=""
IUSE=""

RDEPEND=">=dev-python/python-elementary-9999
	>net-misc/connman-1.3"

DEPEND="
	media-libs/edje
	${RDEPEND}"
