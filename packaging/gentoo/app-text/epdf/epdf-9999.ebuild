# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="doc"
E_SVN_SUB_PROJECT="PROTO"
E_NO_VISIBILITY="1"

inherit efl

DESCRIPTION="View PDF files using EFL applications."
HOMEPAGE="http://www.enlightenment.org/"
SRC_URI=""

LICENSE="GPL-2 || ( LGPL-3 )"
SLOT="0"
KEYWORDS=""

IUSE=""

RDEPEND="
	>=app-text/poppler-0.12
	>=media-libs/evas-9999
	>=dev-libs/ecore-9999[evas]"
DEPEND="${RDEPEND}"
