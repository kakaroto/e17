# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

DESCRIPTION="GTK2 theme to match the default E17 Black & White"
HOMEPAGE="http://www.gnome-look.org/content/show.php?content=95680"
SRC_URI="http://www.gnome-look.org/CONTENT/content-files/95680-B_and_W.tar.bz2"

LICENSE="as-is"
SLOT="0"
KEYWORDS="alpha amd64 arm hppa ia64 mips ppc ppc64 sh sparc x86"

IUSE=""

RDEPEND="x11-libs/gtk+"

S=${WORKDIR}/B_and_W

src_install() {
	find -name '*~' -delete
	insinto /usr/share/themes/e17-b_and_w/
	doins -r gtk-2.0 || die
}
