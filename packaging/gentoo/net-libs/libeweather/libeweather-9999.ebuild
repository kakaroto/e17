# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="PROTO"

inherit efl

DESCRIPTION="Enlightenment's weather information fetching and parsing framework"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""

LICENSE="LGPL-2.1"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND="
	>=dev-libs/ecore-9999[curl]
	>=media-libs/edje-9999"
DEPEND="${RDEPEND}"
