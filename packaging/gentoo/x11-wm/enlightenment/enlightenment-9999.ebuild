# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"
EFL_PKG_IUSE="doc nls"
ESVN_URI_APPEND="e"

inherit efl

DESCRIPTION="Enlightenment DR17 window manager"
HOMEPAGE="http://www.enlightenment.org/"
SRC_URI=""

LICENSE="BSD"
SLOT="0.17"
KEYWORDS=""

IUSE="exchange pam alsa tracker pm-utils debug xinerama xscreensaver opengl bluetooth +hal udev\
	+battery +clock +comp +conf \
	+conf-borders +conf-colors +conf-desklock +conf-desk +conf-desks \
	+conf-dialogs +conf-display +conf-dpms +conf-engine +conf-fonts +conf-imc +conf-intl \
	+conf-menus +conf-mime +conf-mouse +conf-paths +conf-profiles +conf-scale +conf-shelves \
	+conf-startup +conf-theme +conf-winlist \
	connman +cpufreq +dropshadow +everything \
	+everything-files +everything-apps +everything-calc +everything-aspell +everything-settings +everything-windows \
	+fileman +ibar +ibox +illume2 +illume ofono +pager +start +sysactions +syscon +systray \
	+temperature +winlist +wizard"
EVRY_MODS=(everything-files everything-apps everything-calc everything-aspell everything-settings everything-windows)
CONF_MODS=(conf-borders conf-colors conf-desklock conf-desk conf-desks \
	conf-dialogs conf-display conf-dpms conf-engine conf-fonts conf-imc conf-intl \
	conf-menus conf-mime conf-mouse conf-paths conf-profiles conf-scale conf-shelves \
	conf-startup conf-theme conf-winlist)

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
	 >=dev-libs/e_dbus-9999[hal,connman?,ofono?]
	 >=media-libs/edje-9999
	 >=media-libs/evas-9999[X,opengl?,eet,jpeg,png,safety-checks]
	udev? ( dev-libs/eeze )
	everything-aspell? ( app-text/aspell )
	everything-calc? ( sys-devel/bc )
"
DEPEND="${RDEPEND}"

pkg_setup() {
	if ! use everything;then
		for x in ${EVRY_MODS[@]};do
			use $x && die "USE=${x} requires USE=everything"
		done
	fi
	if ! use conf;then
		for x in ${CONF_MODS[@]};do
			use $x && die "USE=${x} requires USE=conf"
		done
	fi
}

src_configure() {
	# NOTE: mixer is plugin-able, but just alsa is provided atm.
	export MY_ECONF="
	  ${MY_ECONF}
	  --disable-install-sysactions
	  $(use_enable hal device-hal)
	  $(use_enable udev device-udev)
	  $(use_enable sysactions install-sysactions)
	  $(use_enable pam)
	  $(use_enable alsa mixer)
	  $(use_enable exchange)
	  $(use_enable bluetooth bluez)
	  $(use_enable battery)
	  $(use_enable clock)
	  $(use_enable comp)
	  $(use_enable conf-borders)
	  $(use_enable conf-colors)
	  $(use_enable conf-desklock)
	  $(use_enable conf-desk)
	  $(use_enable conf-desks)
	  $(use_enable conf-dialogs)
	  $(use_enable conf-display)
	  $(use_enable conf-dpms)
	  $(use_enable conf-engine)
	  $(use_enable conf-fonts)
	  $(use_enable conf-imc)
	  $(use_enable conf-intl)
	  $(use_enable conf-menus)
	  $(use_enable conf-mime)
	  $(use_enable conf-mouse)
	  $(use_enable conf)
	  $(use_enable conf-paths)
	  $(use_enable conf-profiles)
	  $(use_enable conf-scale)
	  $(use_enable conf-shelves)
	  $(use_enable conf-startup)
	  $(use_enable conf-theme)
	  $(use_enable conf-winlist)
	  $(use_enable connman)
	  $(use_enable cpufreq)
	  $(use_enable dropshadow)
	  $(use_enable everything)
	  $(use_enable fileman)
	  $(use_enable ibar)
	  $(use_enable ibox)
	  $(use_enable illume2)
	  $(use_enable illume)
	  $(use_enable ofono)
	  $(use_enable pager)
	  $(use_enable start)
	  $(use_enable syscon)
	  $(use_enable systray)
	  $(use_enable temperature)
	  $(use_enable winlist)
	  $(use_enable wizard)
	  $(use_enable everything-files)
	  $(use_enable everything-apps)
	  $(use_enable everything-calc)
	  $(use_enable everything-aspell)
	  $(use_enable everything-settings)
	  $(use_enable everything-windows)

	"
	efl_src_configure
}

src_install() {
	efl_src_install
	insinto /etc/enlightenment
	newins "${FILESDIR}/gentoo-sysactions.conf" sysactions.conf
}
