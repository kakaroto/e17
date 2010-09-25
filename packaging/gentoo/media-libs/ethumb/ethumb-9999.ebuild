# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="Enlightenment's thumbnailing library"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Ethumb"
SRC_URI=""

LICENSE="LGPL-3"
SLOT="0"
KEYWORDS=""

IUSE="+dbus emotion epdf debug"

RDEPEND="
	>=dev-libs/eina-9999
	>=dev-libs/ecore-9999[evas]
	>=media-libs/edje-9999
	>=media-libs/evas-9999
	dbus? ( >=dev-libs/e_dbus-9999 )
	emotion? ( >=media-libs/emotion-9999 )
	epdf? ( >=app-text/epdf-9999 )"

DEPEND="${RDEPEND}"

src_configure() {
	local DEBUG_FLAGS=""

	if ! use debug; then
		DEBUG_FLAGS="--with-internal-maximum-log-level=2"
	fi

	MY_ECONF="
	  --with-dbus-services=/usr/share/dbus-1/services
	  $(use_enable dbus ethumbd)
	  $(use_enable emotion)
	  $(use_enable epdf)
	  ${DEBUG_FLAGS}"

	enlightenment_src_configure
}
