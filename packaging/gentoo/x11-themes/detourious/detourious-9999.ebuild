# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit subversion

DESCRIPTION="Detourious theme for Enlightenment and Elementary"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""
ESVN_REPO_URI="http://svn.enlightenment.org/svn/e/trunk/THEMES/detourious"
ESVN_PROJECT="enlightenment/THEMES"

LICENSE="BSD-2"
SLOT="0"
KEYWORDS="*"
IUSE=""

RDEPEND=">=x11-wm/enlightenment-9999:0.17
	|| ( >=x11-libs/elementary-9999 >=media-libs/elementary-9999 )"
DEPEND="${RDEPEND}
	>=media-libs/edje-9999
	virtual/pkgconfig"

src_compile() {
	emake || die "Could not compile theme"
}

src_install() {
	# enlightenment:
	d=`pkg-config --variable=themes enlightenment`
	if [ -z "$d" ]; then
		d="/usr/share/enlightenment/data/themes"
	fi
	dodir "$d"
	for t in detourious.edj detourious-illume.edj detourious-dark.edj; do
		chmod a+r $t
		cp $t "${D}/${d}" || die "Failed to install $t"
	done

	# elementary:
	d=`pkg-config --variable=themes elementary`
	if [ -z "$d" ]; then
		d="/usr/share/elementary/themes"
	fi
	dodir "$d"
	for t in detourious-elm.edj; do
		chmod a+r $t
		cp $t "${D}/${d}/${t/-elm/}" || die "Failed to install $t"
	done

	dodoc AUTHORS COPYING SwitzerADF.COPYING Acyl.COPYING
}
