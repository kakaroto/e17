# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

ESVN_SUB_PROJECT="PROTO"
inherit enlightenment

DESCRIPTION="JavaScript bindings for EFL using V8 engine."
HOMEPAGE="http://www.enlightenment.org/"

IUSE=""

RDEPEND=">=dev-libs/ecore-1.7[evas,curl]
	>=dev-libs/eet-1.7
	>=dev-libs/eina-1.7
	>=dev-libs/eio-1.7
	>=dev-libs/e_dbus-1.7
	|| ( >=x11-libs/elementary-1.7 >=media-libs/elementary-1.7 )
	>=media-libs/evas-1.7
	>=media-libs/edje-1.7
	dev-lang/v8"
DEPEND="${RDEPEND}"
