# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
inherit enlightenment

DESCRIPTION="EFL physics library"
IUSE="test"

RDEPEND="
	 >=media-libs/elementary-9999
	 >=sci-physics/bullet-2.80
"
DEPEND="${RDEPEND}"

src_configure() {
	use test || MY_ECONF="--disable-build-tests"
	enlightenment_src_configure
}
