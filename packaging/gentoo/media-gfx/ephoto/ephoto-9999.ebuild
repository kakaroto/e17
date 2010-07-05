# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_URI_APPEND="ephoto"
inherit efl

DESCRIPTION="EFL-based photo app"
HOMEPAGE="http://www.enlightenment.org/"
SRC_URI=""

LICENSE="BSD"
SLOT="0"
KEYWORDS=""


RDEPEND="
	 >=dev-libs/eet-9999
	 >=dev-libs/efreet-9999
	 >=dev-libs/eina-9999[safety-checks]
	 >=dev-libs/embryo-9999
	 >=dev-libs/ecore-9999[X,evas,inotify,xim]
	 >=media-libs/edje-9999
	 >=media-libs/evas-9999[X,eet,jpeg,png,safety-checks]
	 >=media-libs/elementary-9999
	 >=media-libs/ethumb-9999
"
DEPEND="${RDEPEND}"
