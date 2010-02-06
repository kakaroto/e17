# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
ESVN_URI_APPEND="e"
inherit efl

DESCRIPTION="Enlightenment DR17 window manager"
HOMEPAGE="http://www.enlightenment.org/"

SLOT="0.17"
IUSE="exchange pam alsa tracker pm-utils debug xinerama xscreensaver opengl connman"

# TODO: pm-utils changes /etc/enlightenment/sysactions.conf
# TODO: patch to not require -i-really-know-what-i-am-doing-and-accept-full-responsibility-for-it
# TODO: patch to make e17 "e_alert()" inform how to compile with debug in gentoo
RDEPEND="
	 exchange? ( >=net-libs/exchange-9999 )
	 pam? ( sys-libs/pam )
	 alsa? ( media-libs/alsa-lib )
	 tracker? ( app-misc/tracker )
	 pm-utils? ( sys-power/pm-utils )
	 >=dev-libs/eet-9999
	 >=dev-libs/efreet-9999
	 >=dev-libs/eina-9999[safety-checks]
	 >=dev-libs/embryo-9999
	 >=dev-libs/ecore-9999[X,evas,opengl?,xinerama?,xscreensaver?,inotify,xim]
	 >=dev-libs/e_dbus-9999[hal,connman?]
	 >=media-libs/edje-9999
	 >=media-libs/evas-9999[X,opengl?,eet,jpeg,png,safety-checks]"
DEPEND="${RDEPEND}"

src_configure() {
	# NOTE: mixer is plugin-able, but just alsa is provided atm.
	export MY_ECONF="
	  ${MY_ECONF}
	  --disable-install-sysactions
	  $(use_enable pam)
	  $(use_enable alsa mixer)
	  $(use_enable connman)
	  $(use_enable exchange)
	"
	efl_src_configure
}

src_install() {
	efl_src_install
	insinto /etc/enlightenment
	newins "${FILESDIR}/gentoo-sysactions.conf" sysactions.conf
}
