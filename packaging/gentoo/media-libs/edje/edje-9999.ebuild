# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="doc"

inherit efl elisp-common

DESCRIPTION="graphical layout and animation library"
HOMEPAGE="http://trac.enlightenment.org/e/wiki/Edje"
SRC_URI=

LICENSE="BSD"
SLOT="0"
KEYWORDS=""

IUSE="vim-syntax emacs experimental debug"

RDEPEND="
	dev-lang/lua
	>=dev-libs/eet-9999
	>=dev-libs/eina-9999
	>=dev-libs/embryo-9999
	>=dev-libs/ecore-9999
	>=media-libs/evas-9999[jpeg,eet,png]
	emacs? ( virtual/emacs )"

DEPEND="${RDEPEND}"

SITEFILE=50${PN}-gentoo.el

src_configure() {
	# TODO: detect FPU-less architectures and use --enable-fixed-point
	export MY_ECONF="
	  ${MY_ECONF}
	  $(use_enable !debug amalgamation)
	  $(use_enable experimental edje-program-cache)
	  $(use_enable experimental edje-calc-cache)
	  $(use_with vim-syntax vim /usr/share/vim)
	"
	efl_src_configure
}

src_install() {
	if use vim-syntax ; then
		insinto /usr/share/vim/vimfiles/syntax/
		doins data/edc.vim edc.vim
	fi

	if use emacs; then
		elisp-compile data/edje-mode.el
		elisp-install ${PN} data/edje-mode.el
		elisp-site-file-install "${FILESDIR}/${SITEFILE}"
	fi

	efl_src_install
	dodoc utils/gimp-edje-export.py
	dodoc utils/inkscape2edc
}

pkg_postinst() {
	use emacs && elisp-site-regen
	efl_pkg_postinst
}

pkg_postinst() {
	use emacs && elisp-site-regen
}
