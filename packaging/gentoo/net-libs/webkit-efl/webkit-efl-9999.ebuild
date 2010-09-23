# Copyright 1999-2010 Gentoo Foundation
#

EAPI="2"

inherit cmake-utils flag-o-matic enlightenment

DESCRIPTION="Open source web browser engine (EFL version)"
HOMEPAGE="http://trac.webkit.org/wiki/EFLWebKit"
#ESVN_SERVER="http://svn.webkit.org/repository/"
#ESVN_SUB_PROJECT="webkit"
#ESVN_URI_APPEND="trunk"
ESVN_REPO_URI="http://svn.webkit.org/repository/webkit/trunk"
ESVN_PROJECT="enlightenment/webkit-efl"
WANT_AUTOTOOLS="no"

LICENSE="LGPL-2 LGPL-2.1 BSD"
SLOT="0"
KEYWORDS=""
IUSE="-glib"

RDEPEND="
	dev-libs/libxslt
	media-libs/jpeg:0
	media-libs/libpng
	x11-libs/cairo
	glib? (
			dev-libs/glib
			net-libs/libsoup
		)
	!glib? ( net-misc/curl )
	>=dev-db/sqlite-3
	media-libs/edje
	media-libs/evas[fontconfig]
	dev-libs/ecore[X,glib?]
"

DEPEND="${RDEPEND}
	>=sys-devel/flex-2.5.33
	sys-devel/bison
	sys-devel/gettext
	dev-util/gperf
	dev-util/pkgconfig
"

src_configure() {
	[[ gcc-major-version == 4 ]] && [[ gcc-minor-version == 4 ]] && append-flags -fno-strict-aliasing

	if ! use glib ; then
		#could have done the whole cmake-utils_use_enable thing here but I'm lazy
		mycmakeargs+=" -DNETWORK_BACKEND=curl -DENABLE_GLIB_SUPPORT=OFF"
	fi
	enable_cmake-utils_src_configure
}
