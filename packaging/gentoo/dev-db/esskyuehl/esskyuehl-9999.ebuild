# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="PROTO"

inherit enlightenment

DESCRIPTION="EFL DB abstraction library"
LICENSE="LGPL-2"

IUSE="mysql postgresql static-libs"

RDEPEND=">=dev-libs/ecore-1.0
	mysql? ( dev-db/mysql )
	postgresql? ( dev-db/postgresql-base )"
DEPEND="${RDEPEND}"

# there are no configure options.
