# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="PROTO"

inherit enlightenment

DESCRIPTION="Enlightenment's weather information fetching and parsing framework"
LICENSE="LGPL-2.1"

IUSE="static-libs"

RDEPEND="
	>=dev-libs/ecore-9999[curl]
	>=media-libs/edje-9999"
DEPEND="${RDEPEND}"
