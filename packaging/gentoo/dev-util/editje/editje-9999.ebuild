# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit enlightenment

DESCRIPTION="View and edit Edje (EDC/EDJ) files."
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Editje"

LICENSE="GPL-3"

RDEPEND="
	>=dev-python/python-evas-9999
	>=dev-python/python-ecore-9999
	>=dev-python/python-edje-9999
	>=dev-python/python-elementary-9999"

DEPEND="
	>=media-libs/edje-9999"

src_install() {
	emake DESTDIR="${D}" install || die "Install failed"
	if [ -z "$ED" ]; then
		ED="$D"
	fi
	mv "${ED}/usr/bin/editje-bin" "${ED}/usr/bin/editje" || die
	sed -i -e 's/editje-bin/editje/' \
		"${ED}/usr/share/applications/editje.desktop" || die
}
