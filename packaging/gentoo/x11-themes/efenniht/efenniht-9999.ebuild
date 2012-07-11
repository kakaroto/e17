# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit subversion

DESCRIPTION="Efenniht theme for Enlightenment"
HOMEPAGE="http://www.enlightenment.org"
SRC_URI=""
ESVN_REPO_URI="http://svn.enlightenment.org/svn/e/trunk/THEMES/efenniht"
ESVN_PROJECT="enlightenment/THEMES"

LICENSE="BSD-2"
SLOT="0"
KEYWORDS="*"
IUSE=""

RDEPEND=">=x11-wm/enlightenment-9999:0.17"
DEPEND="${RDEPEND}
	>=media-libs/edje-9999
	virtual/pkgconfig"

src_compile() {
	emake efenniht.edj || die "Could not compile theme"
}

src_install() {
	# enlightenment:
	d=`pkg-config --variable=themes enlightenment`
	if [ -z "$d" ]; then
		d="/usr/share/enlightenment/data/themes"
	fi
	dodir "$d"
	for t in efenniht.edj; do
		chmod a+r $t
		cp $t "${D}/${d}" || die "Failed to install $t"
	done

	# elementary: Bad! it depends on out-of-tree files.
	#d=`pkg-config --variable=themes elementary`
	#if [ -z "$d" ]; then
	#	d="/usr/share/elementary/themes"
	#fi
	#dodir "$d"
	#for t in elm-efenniht.edj; do
	#	chmod a+r $t
	#	cp $t "${D}/${d}/${t/elm-/}" || die "Failed to install $t"
	#done
}
