# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="${PN#e_module-}"

inherit enlightenment

DESCRIPTION="expose-like (window scaling) module for enlightenment"



RDEPEND=">=x11-wm/enlightenment-9999"
DEPEND="${RDEPEND}"
